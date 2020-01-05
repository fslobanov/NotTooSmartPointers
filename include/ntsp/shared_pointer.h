#pragma once

#include <cassert>

#include "traits.h"
#include "reference_counter.h"

namespace ntsp {

template< typename Value >
class shared_pointer_t final
{

public:
    using value_type = Value;

public:
    template< typename ... Args >
    static shared_pointer_t< value_type > make( Args && ... args )
    {
        constexpr static auto memory_size = sizeof( reference_counter_t ) + sizeof( value_type );
        const auto memory = static_cast< char * >( std::malloc( memory_size ) );
        if( !memory )
        {
            throw std::bad_alloc();
        }

        const auto block = new( memory ) reference_counter_t( true );
        const auto value = new( memory + sizeof( reference_counter_t ) ) value_type( std::forward< Args ... >( args ... ) );
        return shared_pointer_t< value_type >( block, value );
    }

private:
    explicit shared_pointer_t( reference_counter_t * reference_counter, value_type * value ) noexcept
            : m_reference_counter( reference_counter ), m_value( value )
    {
        reference_counter->add_strong();
        process_shared_from_this( shared_pointer_t::m_value, this );
    }

public:
    shared_pointer_t()
            : m_reference_counter( new reference_counter_t( false ) ), m_value( nullptr )
    {
        process_shared_from_this( shared_pointer_t::m_value, this );
    }

    explicit shared_pointer_t( value_type * value )
            : m_reference_counter( new reference_counter_t( false ) ), m_value( value )
    {
        m_reference_counter->add_strong();
    }

    ~shared_pointer_t()
    {
        if( !m_reference_counter )
        {
            return;
        }

        delete_counter_and_value();
    }

    shared_pointer_t( const shared_pointer_t & other )
            : m_reference_counter( other.m_reference_counter ), m_value( other.m_value )
    {
        if( m_reference_counter )
        {
            m_reference_counter->add_strong();
        }
    }

    shared_pointer_t & operator =( const shared_pointer_t & other )
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_counter_and_value();

        m_reference_counter = other.m_reference_counter;
        if( m_reference_counter )
        {
            m_reference_counter->add_strong();
        }
        m_value = other.m_value;
        return *this;
    }

    shared_pointer_t( shared_pointer_t && other ) noexcept
            : m_reference_counter( other.m_reference_counter ), m_value( other.m_value )
    {
        other.m_reference_counter = nullptr;
        other.m_value = nullptr;
    }

    shared_pointer_t & operator =( shared_pointer_t && other ) noexcept
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_counter_and_value();

        m_reference_counter = other.m_reference_counter;
        m_value = other.m_value;

        other.m_reference_counter = nullptr;
        other.m_value = nullptr;

        return *this;
    }

public:
    [[nodiscard]] value_type * get() const noexcept
    {
        return m_value;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return nullptr == m_value;
    }

    explicit operator bool() const noexcept
    {
        return empty();
    }

    value_type * operator ->() noexcept
    {
        return m_value;
    }

    value_type & operator *() noexcept
    {
        assert( m_value && "value_type == nullptr" );
        return *m_value;
    }

private:
    reference_counter_t * m_reference_counter;
    value_type * m_value;

private:
    friend class weak_pointer_t< value_type >;

    friend class enable_shared_from_this_t< value_type >;

    template< typename V, typename ... Args >
    friend shared_pointer_t< V > make_shared( Args && ... args );

private:
    void delete_counter_and_value()
    {
        assert( m_reference_counter && "Already moved" );
        if( m_reference_counter->remove_and_test_strong_empty() == reference_counter_t::state_e::non_empty )
        {
            return;
        }

        if( m_reference_counter->is_monotonic_allocated() )
        {
            m_value->~Value();
        }
        else
        {
            delete m_value;
        }
        m_value = nullptr;

        if( m_reference_counter->test_weak() == reference_counter_t::state_e::non_empty )
        {
            return;
        }

        if( m_reference_counter->is_monotonic_allocated() )
        {
            m_reference_counter->~reference_counter_t();
            std::free( m_reference_counter );
        }
        else
        {
            delete m_reference_counter;
        }
        m_reference_counter = nullptr;
    }

    static void process_shared_from_this( value_type * value, shared_pointer_t< value_type > * self )
    {
        if constexpr( is_enable_shared_from_this_v< value_type > )
        {
            const auto shared_from_this = reinterpret_cast< enable_shared_from_this_t< value_type > * >( value );
            //shared_from_this->self = weak_pointer_t< value_type >( *self );
            shared_from_this->m_reference_counter = self->m_reference_counter;
        }
    }
};

template< typename value_type, typename... Args >
shared_pointer_t< value_type > make_shared( Args && ... args )
{
    return shared_pointer_t< value_type >::make( std::forward< Args ... >( args ... ) );
}

}