#include "gtest/gtest.h"
#include <ntsp/shared_pointer.h>
#include <ntsp/enable_shared_from_this.h>

using namespace ntsp;

TEST( ntsp, enable_shared_from_this )
{
    struct Foo : public enable_shared_from_this< Foo >
    {
        explicit Foo( int value ) noexcept : value( value )
        {
            //std::clog << "foo ctor" << std::endl;
        }

        ~Foo()
        {
            //std::clog << "foo dtor" << std::endl;
        }

        int value;
    };

    auto s1 = make_shared< Foo >( 42 );
    auto s2 = s1->shared_from_this();

    ASSERT_TRUE( s1->value == s2->value );
    ASSERT_TRUE( s1.get() == s2.get() );
}
