#pragma once

#include <ntsp/shared_pointer.h>

namespace ntsp {

template< typename Value, thread_policy_e Policy = thread_policy_e::safe >
class weak_pointer final
{
    
public:
    using value_type = Value;
    constexpr static thread_policy_e thread_policy = Policy;

public:
    weak_pointer() noexcept
            : m_reference_counter( nullptr )
            , m_value( nullptr )
    {

    }

    explicit weak_pointer( const shared_pointer< value_type, thread_policy > & shared ) noexcept
            : m_reference_counter( shared.m_reference_counter )
            , m_value( shared.get() )
    {
        assert( m_reference_counter && "Shared was already moved" );
        m_reference_counter->add_weak();
    }

    weak_pointer( const weak_pointer & other ) noexcept
            : m_reference_counter( other.m_reference_counter )
            , m_value( other.m_value )
    {
        if( m_reference_counter )
        {
            m_reference_counter->add_weak();
        }
    }

    weak_pointer & operator =( const weak_pointer & other )
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_reference_counter();

        m_reference_counter = other.m_reference_counter;
        if( m_reference_counter )
        {
            m_reference_counter->add_weak();
        }
        m_value = other.m_value;
        return *this;
    }

    weak_pointer( weak_pointer && other ) noexcept
            : m_reference_counter( other.m_reference_counter )
            , m_value( other.m_value )
    {
        other.m_reference_counter = nullptr;
        other.m_value = nullptr;
    }

    weak_pointer & operator =( weak_pointer && other ) noexcept
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_reference_counter();

        m_reference_counter = other.m_reference_counter;
        m_value = other.m_value;

        other.m_reference_counter = nullptr;
        other.m_value = nullptr;

        return *this;
    }

    ~weak_pointer()
    {
        delete_reference_counter();
    }

    [[ nodiscard ]] bool expired() const noexcept
    {
        return m_reference_counter->test_strong() == reference_counter< thread_policy >::state_e::empty;
    }

    shared_pointer< value_type, thread_policy > lock() noexcept
    {
        if( m_reference_counter->test_strong() == reference_counter< thread_policy >::state_e::empty )
        {
            m_value = nullptr;
        }
        return shared_pointer< value_type, thread_policy >( m_reference_counter, m_value );
    }

private:
    friend class shared_pointer< value_type, thread_policy >;
    friend class enable_shared_from_this< value_type, thread_policy >;

private:
    reference_counter< thread_policy >  * m_reference_counter;
    value_type * m_value;

private:
    void delete_reference_counter()
    {
        if( !m_reference_counter )
        {
            return;
        }

        const auto has_weak = m_reference_counter->remove_and_test_weak_empty() == reference_counter< thread_policy >::state_e::non_empty;
        const auto has_strong = m_reference_counter->test_strong() == reference_counter< thread_policy >::state_e::non_empty;
        if( has_weak || has_strong )
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
};

}
