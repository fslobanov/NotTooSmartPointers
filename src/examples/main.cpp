#include <iostream>

#include <ntsp/shared_pointer.h>
#include <ntsp/weak_pointer.h>
#include <ntsp/enable_shared_from_this.h>

int main()
{
    for( auto index = 0; index < 1; ++index )
    {
        using namespace ntsp;

        {
            auto s1 = shared_pointer< uint64_t >::make( 42 );
            auto s2 = std::move( s1 );
            auto w1 = weak_pointer< uint64_t >( s2 );
            auto s3 = w1.lock();

            std::clog << "#### " << *s3 << " , " << *s2.get() << std::endl;
        }

        {
            auto w1 = []() noexcept
            {
                auto s1 = shared_pointer< int >( new int( 666 ) );
                auto w1 = weak_pointer< int >( s1 );
                return w1;
            }();

            std::clog << std::boolalpha << "#### " << w1.expired() << std::endl;
        }

        {
            auto s1 = shared_pointer< int >::make( 1488 );
            auto s2 = shared_pointer< int >::make( 666 );
            s2 = s1;

            auto s3 = shared_pointer< int >( new int( 42 ) );
            s3 = std::move( s2 );

            std::clog << "#### " << *s3 << std::endl;
        }

        {
            auto s1 = make_shared< int >( 12 );
            std::clog << "#### " << *s1 << std::endl;
        }

        {
            struct Foo : public enable_shared_from_this< Foo >
            {
                explicit Foo( int value ) noexcept : value( value )
                {
                    std::clog << "foo ctor" << std::endl;
                }

                ~Foo()
                {
                    std::clog << "foo dtor" << std::endl;
                }

                int value;
            };

            auto s1 = make_shared< Foo >( 42 );
            auto s2 = shared_pointer< Foo >( new Foo( 666 ) );
            auto s3 = s1->shared_from_this();
            auto w1 = s1->weak_from_this();

            std::clog << "#### " << s2->value << " , " << w1.lock()->value << std::endl;
        }

        {
            make_shared< double >( 666 );
        }
    }

    return 0;
}