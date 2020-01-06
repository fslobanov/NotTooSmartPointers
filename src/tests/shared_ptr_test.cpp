#include "gtest/gtest.h"
#include <ntsp/shared_pointer.h>

using namespace ntsp;

TEST( ntsp, shared_ptr_move )
{
    auto s1 = shared_pointer< uint64_t >::make( 42 );
    auto s2 = std::move( s1 );

    ASSERT_TRUE( s1.empty() );
    ASSERT_TRUE( *s2 = 42 );
}
