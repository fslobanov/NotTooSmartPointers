#include <ntsp/reference_counter.h>

namespace ntsp {

reference_counter::reference_counter( bool monotonicAllocated ) noexcept
        : monotonicAllocated( monotonicAllocated ), strong( 0 ), weak( 0 )
{

}

bool reference_counter::is_monotonic_allocated() const noexcept
{
    return monotonicAllocated;
}

void reference_counter::add_strong() noexcept
{
    add( strong );
}

reference_counter::state_e reference_counter::remove_and_test_strong_empty() noexcept
{
    return remove_and_test_empty( strong );
}

void reference_counter::add_weak() noexcept
{
    add( weak );
}

reference_counter::state_e reference_counter::remove_and_test_weak_empty() noexcept
{
    return remove_and_test_empty( weak );
}

reference_counter::state_e reference_counter::test_weak() const noexcept
{
    return test( weak );
}

reference_counter::state_e reference_counter::test_strong() const noexcept
{
    return test( strong );
}

reference_counter::state_e reference_counter::remove_and_test_empty( reference_counter::counter & counter ) noexcept
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

void reference_counter::add( reference_counter::counter & counter ) noexcept
{
    lock lock( mutex );
    ++counter;
}

reference_counter::state_e reference_counter::test( const reference_counter::counter & counter ) const noexcept
{
    lock lock( mutex );
    return 0 == counter ? state_e::empty : state_e::non_empty;
}

}