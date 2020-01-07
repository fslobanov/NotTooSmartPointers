#include <ntsp/reference_counter.h>

namespace ntsp {

reference_counter::reference_counter( bool monotonicAllocated ) noexcept
        : m_monotonic_allocated( monotonicAllocated ), m_strong( 0 ), m_weak( 0 )
{

}

bool reference_counter::is_monotonic_allocated() const noexcept
{
    return m_monotonic_allocated;
}

void reference_counter::add_strong() noexcept
{
    add( m_strong );
}

reference_counter::state_e reference_counter::remove_and_test_strong_empty() noexcept
{
    return remove_and_test_empty( m_strong );
}

void reference_counter::add_weak() noexcept
{
    add( m_weak );
}

reference_counter::state_e reference_counter::remove_and_test_weak_empty() noexcept
{
    return remove_and_test_empty( m_weak );
}

reference_counter::state_e reference_counter::test_weak() const noexcept
{
    return test( m_weak );
}

reference_counter::state_e reference_counter::test_strong() const noexcept
{
    return test( m_strong );
}

reference_counter::state_e reference_counter::remove_and_test_empty( reference_counter::counter & counter ) noexcept
{
    lock lock( m_bottleneck );

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
    lock lock( m_bottleneck );
    ++counter;
}

reference_counter::state_e reference_counter::test( const reference_counter::counter & counter ) const noexcept
{
    lock lock( m_bottleneck );
    return 0 == counter ? state_e::empty : state_e::non_empty;
}

}