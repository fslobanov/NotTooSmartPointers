#include <ntsp/reference_counter.h>

namespace ntsp {

reference_counter_t::reference_counter_t( bool monotonicAllocated ) noexcept
        : monotonicAllocated( monotonicAllocated ), strong( 0 ), weak( 0 )
{

}

bool reference_counter_t::is_monotonic_allocated() const noexcept
{
    return monotonicAllocated;
}

void reference_counter_t::add_strong() noexcept
{
    add( strong );
}

reference_counter_t::state_e reference_counter_t::remove_and_test_strong_empty() noexcept
{
    return remove_and_test_empty( strong );
}

void reference_counter_t::add_weak() noexcept
{
    add( weak );
}

reference_counter_t::state_e reference_counter_t::remove_and_test_weak_empty() noexcept
{
    return remove_and_test_empty( weak );
}

reference_counter_t::state_e reference_counter_t::test_weak() const noexcept
{
    return test( weak );
}

reference_counter_t::state_e reference_counter_t::test_strong() const noexcept
{
    return test( strong );
}

reference_counter_t::state_e reference_counter_t::remove_and_test_empty( reference_counter_t::counter & counter ) noexcept
{
    lock lock( mutex );

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

void reference_counter_t::add( reference_counter_t::counter & counter ) noexcept
{
    lock lock( mutex );
    ++counter;
}

reference_counter_t::state_e reference_counter_t::test( const reference_counter_t::counter & counter ) const noexcept
{
    lock lock( mutex );
    return 0 == counter ? state_e::empty : state_e::non_empty;
}

}