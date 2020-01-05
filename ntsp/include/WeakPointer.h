#pragma once

#include "Traits.h"
#include "ReferenceCounter.h"

namespace NTSP {

template< typename Value >
class WeakPointer final
{
public:
    using ValueType = Value;

    friend class SharedPointer< Value >;

    friend class EnableSharedFromThis< Value >;

public:
    WeakPointer() noexcept
            : referenceCounter( nullptr ), value( nullptr )
    {

    }

    explicit WeakPointer( const SharedPointer< Value > & shared ) noexcept
            : referenceCounter( shared.referenceCounter ), value( shared.value )
    {
        assert( referenceCounter && "Shared was already moved" );
        referenceCounter->addWeak();
    }

    WeakPointer( const WeakPointer & other ) noexcept
            : referenceCounter( other.referenceCounter ), value( other.value )
    {
        if( referenceCounter )
        {
            referenceCounter->addWeak();
        }
    }

    WeakPointer & operator =( const WeakPointer & other )
    {
        if( &other == this || other.referenceCounter == referenceCounter )
        {
            return *this;
        }

        doDelete();

        referenceCounter = other.referenceCounter;
        if( referenceCounter )
        {
            referenceCounter->addWeak();
        }
        value = other.value;
        return *this;
    }

    WeakPointer( WeakPointer && other ) noexcept
            : referenceCounter( other.referenceCounter ), value( other.value )
    {
        other.referenceCounter = nullptr;
        other.value = nullptr;
    }

    WeakPointer & operator =( WeakPointer && other ) noexcept
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

    ~WeakPointer()
    {
        doDelete();
    }

    [[ nodiscard ]] bool expired() const noexcept
    {
        return !referenceCounter->hasStrong();
    }

    SharedPointer< Value > lock() noexcept
    {
        if( !referenceCounter->hasStrong() )
        {
            value = nullptr;
        }
        return SharedPointer< Value >( referenceCounter, value );
    }

private:
    ReferenceCounter * referenceCounter;
    Value * value;

private:
    void doDelete()
    {
        if( !referenceCounter )
        {
            return;
        }

        const auto hasWeak = !referenceCounter->removeAndTestWeakEmpty();
        if( hasWeak || referenceCounter->hasStrong() )
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
};

}
