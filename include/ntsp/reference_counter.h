#pragma once

#include <type_traits>
#include <mutex>
#include <atomic>

#include <ntsp/types.h>

#ifndef NTSP_REFERENCE_COUNTER_TYPE
#define NTSP_REFERENCE_COUNTER_TYPE std::size_t
#endif

namespace ntsp {

template< typename Value, thread_policy_e Policy >
class weak_pointer;

template< typename Value, thread_policy_e Policy  >
class shared_pointer;

template< typename Value, thread_policy_e Policy  >
class enable_shared_from_this;

template< typename Value, typename ... Args >
decltype( auto ) make_shared( Args && ... args );


namespace detail {

template< thread_policy_e Policy >
struct reference_counter_thread_guard;

template<>
struct reference_counter_thread_guard< thread_policy_e::safe >
{
public:
    void lock() noexcept
    {
        while( spinlock.test_and_set( std::memory_order_acquire ) )
        {

        }
    }

    void unlock() noexcept
    {
        spinlock.clear( std::memory_order_release );
    }

private:
    std::atomic_flag spinlock = ATOMIC_FLAG_INIT;
};

template<>
struct reference_counter_thread_guard< thread_policy_e::unsafe >
{
    void lock() noexcept
    {

    }

    void unlock() noexcept
    {

    }
};

}

template< thread_policy_e Policy >
class reference_counter final
{
    using counter = NTSP_REFERENCE_COUNTER_TYPE;
    static constexpr auto thread_policy = Policy;
    static_assert( std::is_integral_v< counter >, "Not an integral" );

    enum class state_e : bool
    {
        empty, non_empty
    };

private:
    using thread_guard = detail::reference_counter_thread_guard< thread_policy >;
    using lock = std::lock_guard< thread_guard >;

private:
    explicit reference_counter( bool monotonicAllocated ) noexcept
            : m_monotonic_allocated( monotonicAllocated )
            , m_strong( 0 )
            , m_weak( 0 )
    {

    }

    [[ nodiscard ]] bool is_monotonic_allocated() const noexcept
    {
        return m_monotonic_allocated;
    }

private:
    void add_strong() noexcept
    {
        add( m_strong );
    }
    [[ nodiscard ]] state_e remove_and_test_strong_empty() noexcept
    {
        return remove_and_test_empty( m_strong );
    }
    [[ nodiscard ]] state_e test_strong() const noexcept
    {
        return test( m_strong );
    }

    void add_weak() noexcept
    {
        add( m_weak );
    }
    [[ nodiscard ]] state_e remove_and_test_weak_empty() noexcept
    {
        return remove_and_test_empty( m_weak );
    }
    [[ nodiscard ]] state_e test_weak() const noexcept
    {
        return test( m_weak );
    }

private:
    template< typename V, thread_policy_e P >
    friend class weak_pointer;

    template< typename V, thread_policy_e P >
    friend class shared_pointer;

    template< typename V, thread_policy_e P >
    friend class enable_shared_from_this;

    template< typename V, typename ... Args >
    friend decltype( auto ) make_shared( Args && ... args );

private:
    counter m_strong;
    counter m_weak;
    const bool m_monotonic_allocated;
    mutable thread_guard m_thread_guard;

private:
    [[ nodiscard ]] state_e remove_and_test_empty( counter & counter ) noexcept
    {
        lock lock( m_thread_guard );

        switch ( counter )
        {
            case 0:
                return state_e::empty;

            case 1:
                --counter;
                return state_e::empty;

            default:
                --counter;
                return state_e::non_empty;
        }
    }
    void add( counter & counter ) noexcept
    {
        lock lock( m_thread_guard );
        ++counter;
    }
    [[ nodiscard ]] state_e test( const counter & counter ) const noexcept
    {
        lock lock( m_thread_guard );
        return 0 == counter ? state_e::empty : state_e::non_empty;
    }
};

}