#pragma once

#include "traits.h"
#include "reference_counter.h"

namespace ntsp {

template< typename Value >
class weak_pointer_t final
{
    
public:
    using value_type = Value;

public:
    weak_pointer_t() noexcept
            : m_reference_counter( nullptr )
            , m_value( nullptr )
    {

    }

    explicit weak_pointer_t( const shared_pointer_t< value_type > & shared ) noexcept
            : m_reference_counter( shared.m_reference_counter )
            , m_value( shared.m_value )
    {
        assert( m_reference_counter && "Shared was already moved" );
        m_reference_counter->add_weak();
    }

    weak_pointer_t( const weak_pointer_t & other ) noexcept
            : m_reference_counter( other.m_reference_counter )
            , m_value( other.m_value )
    {
        if( m_reference_counter )
        {
            m_reference_counter->add_weak();
        }
    }

    weak_pointer_t & operator =( const weak_pointer_t & other )
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_counter();

        m_reference_counter = other.m_reference_counter;
        if( m_reference_counter )
        {
            m_reference_counter->add_weak();
        }
        m_value = other.m_value;
        return *this;
    }

    weak_pointer_t( weak_pointer_t && other ) noexcept
            : m_reference_counter( other.m_reference_counter ), m_value( other.m_value )
    {
        other.m_reference_counter = nullptr;
        other.m_value = nullptr;
    }

    weak_pointer_t & operator =( weak_pointer_t && other ) noexcept
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_counter();

        m_reference_counter = other.m_reference_counter;
        m_value = other.m_value;

        other.m_reference_counter = nullptr;
        other.m_value = nullptr;

        return *this;
    }

    ~weak_pointer_t()
    {
        delete_counter();
    }

    [[ nodiscard ]] bool expired() const noexcept
    {
        return m_reference_counter->test_strong() == reference_counter_t::state_e::empty;
    }

    shared_pointer_t< value_type > lock() noexcept
    {
        if( m_reference_counter->test_strong() == reference_counter_t::state_e::empty )
        {
            m_value = nullptr;
        }
        return shared_pointer_t< value_type >( m_reference_counter, m_value );
    }

private:
    friend class shared_pointer_t< value_type >;
    friend class enable_shared_from_this_t< value_type >;

private:
    reference_counter_t * m_reference_counter;
    value_type * m_value;

private:
    void delete_counter()
    {
        if( !m_reference_counter )
        {
            return;
        }

        const auto has_weak = m_reference_counter->remove_and_test_weak_empty() == reference_counter_t::state_e::non_empty;
        const auto has_strong = m_reference_counter->test_strong() == reference_counter_t::state_e::non_empty;
        if( has_weak || has_strong )
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
};

}
