#pragma once

#include "WeakPointer.h"

namespace NTSP {

template< typename Value >
class EnableSharedFromThis
{
    template< typename V >
    friend class WeakPointer;

    template< typename V >
    friend class SharedPointer;

public:
    [[ nodiscard ]] SharedPointer< Value > sharedFromThis() noexcept
    {
        assert( self.referenceCounter );
        return self.lock();
    }

    [[ nodiscard ]] WeakPointer< Value > weakFromThis() noexcept
    {
        return self;
    }

private:
    WeakPointer< Value > self;
};


}