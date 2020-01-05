#pragma once

#include <type_traits>
#include <mutex>

#ifndef NTSP_REFERENCE_COUNTER_TYPE
#define NTSP_REFERENCE_COUNTER_TYPE std::size_t
#endif

namespace NTSP {

template< typename Value >
class WeakPointer;

template< typename Value >
class SharedPointer;

template< typename Value >
class EnableSharedFromThis;

template< typename Value, typename ... Args >
SharedPointer< Value > MakeShared( Args && ... args );

class ReferenceCounter final
{
    using Counter = NTSP_REFERENCE_COUNTER_TYPE;
    static_assert( std::is_integral_v< Counter >, "Not an integral" );

private:
    template< typename Value >
    friend class WeakPointer;

    template< typename Value >
    friend class SharedPointer;

    template< typename Value >
    friend class EnableSharedFromThis;

    template< typename Value, typename ... Args >
    friend SharedPointer< Value > MakeShared( Args && ... args );

private:
    using Mutex = std::mutex;
    using Lock = std::lock_guard< Mutex >;

private:
    explicit ReferenceCounter( bool monotonicAllocated ) noexcept;

    [[ nodiscard ]] bool isMonotonicAllocated() const noexcept;

    void addStrong() noexcept;
    [[ nodiscard ]] bool removeAndTestStrongEmpty() noexcept;
    [[ nodiscard ]] bool hasStrong() const noexcept;

    void addWeak() noexcept;
    [[ nodiscard ]] bool removeAndTestWeakEmpty() noexcept;
    [[ nodiscard ]] bool hasWeak() const noexcept;

private:
    [[ nodiscard ]] bool removeAndTestCounterEmpty( Counter & counter ) noexcept;
    void addReference( Counter & counter ) noexcept;
    [[ nodiscard ]] bool testCounter( const Counter & counter ) const noexcept;

private:
    Counter strong;
    Counter weak;
    const bool monotonicAllocated;
    mutable Mutex mutex;
};

}