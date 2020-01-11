#pragma once

#include <cassert>

#include <ntsp/traits.h>
#include <ntsp/reference_counter.h>

namespace ntsp {

template< typename Value, thread_policy_e Policy = thread_policy_e::safe >
class shared_pointer final
{

public:
    using value_type = Value;
    constexpr static thread_policy_e thread_policy = Policy;

public:
    template< typename ... Args >
    static shared_pointer< value_type > make( Args && ... args )
    {
        constexpr static auto memory_size = sizeof( reference_counter< thread_policy > ) + sizeof( value_type );
        const auto memory = static_cast< char * >( std::malloc( memory_size ) );
        if( !memory )
        {
            throw std::bad_alloc();
        }

        const auto counter = new( memory ) reference_counter< thread_policy >( true );
        const auto value = new( memory + sizeof( reference_counter< thread_policy > ) ) value_type( std::forward< Args ... >( args ... ) );
        return shared_pointer< value_type >( counter, value );
    }

public:
    shared_pointer()
            : m_reference_counter( new reference_counter< thread_policy >( false ) )
            , m_value( nullptr )
    {
        process_shared_from_this( shared_pointer::m_value, this );
    }

    explicit shared_pointer( value_type * value )
            : m_reference_counter( new reference_counter< thread_policy >( false ) )
            , m_value( value )
    {
        m_reference_counter->add_strong();
    }

    ~shared_pointer()
    {
        if( !m_reference_counter )
        {
            return;
        }

        delete_counter_and_value();
    }

    shared_pointer( const shared_pointer & other )
            : m_reference_counter( other.m_reference_counter )
            , m_value( other.m_value )
    {
        if( m_reference_counter )
        {
            m_reference_counter->add_strong();
        }
    }

    shared_pointer & operator =( const shared_pointer & other )
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

    shared_pointer( shared_pointer && other ) noexcept
            : m_reference_counter( other.m_reference_counter )
            , m_value( other.m_value )
    {
        other.m_reference_counter = nullptr;
        other.m_value = nullptr;
    }

    shared_pointer & operator =( shared_pointer && other ) noexcept
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
    [[ nodiscard ]] value_type * get() const noexcept
    {
        return m_value;
    }

    [[ nodiscard ]] bool empty() const noexcept
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

public:
    [[ nodiscard ]] bool operator ==( const shared_pointer & rhs ) const noexcept
    {
        return m_reference_counter == rhs.m_reference_counter;
    }

    [[ nodiscard ]] bool operator !=( const shared_pointer & rhs ) const noexcept
    {
        return !( rhs == *this );
    }

    [[ nodiscard ]] bool operator <( const shared_pointer & rhs ) const noexcept
    {
        return m_reference_counter < rhs.m_reference_counter;
    }

private:
    friend class weak_pointer< value_type, thread_policy >;

    friend class enable_shared_from_this< value_type, thread_policy >;

    template< typename V, thread_policy_e P,  typename ... Args >
    friend shared_pointer< V, P > make_shared( Args && ... args );

private:
    reference_counter< thread_policy > * m_reference_counter;
    value_type * m_value;

private:
    explicit shared_pointer( reference_counter< thread_policy > * reference_counter, value_type * value ) noexcept
            : m_reference_counter( reference_counter )
            , m_value( value )
    {
        m_reference_counter->add_strong();
        process_shared_from_this( m_value, this );
    }

private:
    void delete_counter_and_value()
    {
        assert( m_reference_counter && "Already moved" );
        if( m_reference_counter->remove_and_test_strong_empty() == reference_counter< thread_policy >::state_e::non_empty )
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

        if( m_reference_counter->test_weak() == reference_counter< thread_policy >::state_e::non_empty )
        {
            return;
        }

        if( m_reference_counter->is_monotonic_allocated() )
        {
            m_reference_counter->~reference_counter();
            std::free( m_reference_counter );
        }
        else
        {
            delete m_reference_counter;
        }
        m_reference_counter = nullptr;
    }

    static void process_shared_from_this( value_type * value, shared_pointer< value_type, thread_policy > * self )
    {
        if constexpr( is_enable_shared_from_this_v< value_type, thread_policy > )
        {
            const auto shared_from_this = reinterpret_cast< enable_shared_from_this< value_type, thread_policy > * >( value );
            shared_from_this->m_reference_counter = self->m_reference_counter;
        }
    }
};

template< typename Value, thread_policy_e Policy = thread_policy_e::safe, typename... Args >
shared_pointer< Value > make_shared( Args && ... args )
{
    return shared_pointer< Value, Policy >::make( std::forward< Args ... >( args ... ) );
}

}