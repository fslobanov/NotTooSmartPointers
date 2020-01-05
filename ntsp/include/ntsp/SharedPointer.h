#pragma once

#include <cassert>

#include "Traits.h"
#include "ReferenceCounter.h"

namespace NTSP {

template< typename Value >
class SharedPointer final
{
    friend class WeakPointer< Value >;

    friend class EnableSharedFromThis< Value >;

    template< typename V, typename ... Args >
    friend SharedPointer< V > MakeShared( Args && ... args );

public:
    using ValueType = Value;

public:
    template< typename ... Args >
    static SharedPointer< Value > make( Args && ... args )
    {
        constexpr static auto memorySize = sizeof( ReferenceCounter ) + sizeof( Value );
        const auto memory = static_cast< char * >( std::malloc( memorySize ) );
        if( !memory )
        {
            throw std::bad_alloc();
        }

        const auto block = new( memory ) ReferenceCounter( true );
        const auto value = new( memory + sizeof( ReferenceCounter ) ) Value( std::forward< Args ... >( args ... ) );
        return SharedPointer< Value >( block, value );
    }

private:
    explicit SharedPointer( ReferenceCounter * referenceCounter, Value * value ) noexcept
            : referenceCounter( referenceCounter ), value( value )
    {
        referenceCounter->addStrong();
        processSharedFromThis( SharedPointer::value, this );
    }

public:
    SharedPointer()
            : referenceCounter( new ReferenceCounter( false ) ), value( nullptr )
    {
        processSharedFromThis( SharedPointer::value, this );
    }

    explicit SharedPointer( Value * value )
            : referenceCounter( new ReferenceCounter( false ) ), value( value )
    {
        referenceCounter->addStrong();
    }

    ~SharedPointer()
    {
        if( !referenceCounter )
        {
            return;
        }

        doDelete();
    }

    SharedPointer( const SharedPointer & other )
            : referenceCounter( other.referenceCounter ), value( other.value )
    {
        if( referenceCounter )
        {
            referenceCounter->addStrong();
        }
    }

    SharedPointer & operator =( const SharedPointer & other )
    {
        if( &other == this || other.referenceCounter == referenceCounter )
        {
            return *this;
        }

        doDelete();

        referenceCounter = other.referenceCounter;
        if( referenceCounter )
        {
            referenceCounter->addStrong();
        }
        value = other.value;
        return *this;
    }

    SharedPointer( SharedPointer && other ) noexcept
            : referenceCounter( other.referenceCounter ), value( other.value )
    {
        other.referenceCounter = nullptr;
        other.value = nullptr;
    }

    SharedPointer & operator =( SharedPointer && other ) noexcept
    {
        if( &other == this || other.referenceCounter == referenceCounter )
        {
            return *this;
        }

        doDelete();

        referenceCounter = other.referenceCounter;
        value = other.value;

        other.referenceCounter = nullptr;
        other.value = nullptr;

        return *this;
    }

public:
    [[ nodiscard ]] Value * get() const noexcept
    {
        return value;
    }

    [[ nodiscard ]] bool isEmpty() const noexcept
    {
        return nullptr == value;
    }

    explicit operator bool() const noexcept
    {
        return isEmpty();
    }

    Value * operator ->() noexcept
    {
        return value;
    }

    Value & operator *() noexcept
    {
        assert( value && "Value == nullptr" );
        return *value;
    }

private:
    ReferenceCounter * referenceCounter;
    Value * value;

private:
    void doDelete()
    {
        assert( referenceCounter && "Already moved" );
        if( !referenceCounter->removeAndTestStrongEmpty() )
        {
            return;
        }

        if( referenceCounter->isMonotonicAllocated() )
        {
            value->~Value();
        }
        else
        {
            delete value;
        }
        value = nullptr;

        if( referenceCounter->hasWeak() )
        {
            return;
        }

        if( referenceCounter->isMonotonicAllocated() )
        {
            referenceCounter->~ReferenceCounter();
            std::free( referenceCounter );
        }
        else
        {
            delete referenceCounter;
        }
        referenceCounter = nullptr;
    }

    static void processSharedFromThis( Value * value, SharedPointer< ValueType > * self )
    {
        if( IsEnableSharedFromThis< Value >::value )
        {
            const auto sharedFromThis = reinterpret_cast< EnableSharedFromThis< Value > * >( value );
            sharedFromThis->self = WeakPointer< Value >( *self );
        }
    }
};

template< typename Value, typename... Args >
SharedPointer< Value > MakeShared( Args && ... args )
{
    return SharedPointer< Value >::make( std::forward< Args ... >( args ... ) );
}

}