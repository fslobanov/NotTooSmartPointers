#include "gtest/gtest.h"
#include <ntsp/shared_pointer.h>
#include <ntsp/weak_pointer.h>

using namespace ntsp;

TEST( ntsp, weak_ptr )
{
    auto expired{ false };
    auto w1 = [ &expired ]() noexcept -> weak_pointer< int >
    {
        auto s1 = make_shared< int >( 42 );
        auto w1 = weak_pointer< int >( s1 );

        expired = w1.expired();

        return w1;
    }();

    ASSERT_TRUE( !expired );
    ASSERT_TRUE( w1.expired() );
}

TEST( ntsp, copy_assign )
{
    auto s1 = make_shared< int >( 42 );
    auto w1 = weak_pointer< int >( s1 );

    auto s2 = make_shared< int >( 666 );
    auto w2 = weak_pointer< int >( s2 );

    w1 = w2;
}
