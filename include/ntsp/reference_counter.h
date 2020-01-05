#pragma once

#include <type_traits>
#include <mutex>

#ifndef NTSP_REFERENCE_COUNTER_TYPE
#define NTSP_REFERENCE_COUNTER_TYPE std::size_t
#endif

namespace ntsp {

template< typename Value >
class weak_pointer_t;

template< typename Value >
class shared_pointer_t;

template< typename Value >
class enable_shared_from_this_t;

template< typename Value, typename ... Args >
shared_pointer_t< Value > make_shared( Args && ... args );

class reference_counter_t final
{
    using counter = NTSP_REFERENCE_COUNTER_TYPE;
    static_assert( std::is_integral_v< counter >, "Not an integral" );

private:
    template< typename Value >
    friend class weak_pointer_t;

    template< typename Value >
    friend class shared_pointer_t;

    template< typename Value >
    friend class enable_shared_from_this_t;

    template< typename Value, typename ... Args >
    friend shared_pointer_t< Value > make_shared( Args && ... args );

private:
    using bottleneck = std::mutex;
    using lock = std::lock_guard< bottleneck >;

private:
    explicit reference_counter_t( bool monotonicAllocated ) noexcept;

    [[ nodiscard ]] bool is_monotonic_allocated() const noexcept;

private:
    enum class state_e : bool { empty, non_empty };

    void add_strong() noexcept;
    [[ nodiscard ]] state_e remove_and_test_strong_empty() noexcept;
    [[ nodiscard ]] state_e test_strong() const noexcept;

    void add_weak() noexcept;
    [[ nodiscard ]] state_e remove_and_test_weak_empty() noexcept;
    [[ nodiscard ]] state_e test_weak() const noexcept;

private:
    [[ nodiscard ]] state_e remove_and_test_empty( counter & counter ) noexcept;
    void add( counter & counter ) noexcept;
    [[ nodiscard ]] state_e test( const counter & counter ) const noexcept;

private:
    counter strong;
    counter weak;
    const bool monotonicAllocated;
    mutable bottleneck mutex;
};

}