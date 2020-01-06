#pragma once

#include "weak_pointer.h"

namespace ntsp {

template< typename Value >
class enable_shared_from_this
{
    template< typename V >
    friend class weak_pointer;

    template< typename V >
    friend class shared_pointer;

public:
    virtual ~enable_shared_from_this() = default;

public:
    [[ nodiscard ]] shared_pointer< Value > shared_from_this() noexcept
    {
        assert( m_reference_counter && "Was not shared" );
        return shared_pointer< Value >( m_reference_counter, reinterpret_cast< Value * >( this ) );
    }

    [[ nodiscard ]] weak_pointer< Value > weak_from_this() noexcept
    {
        return weak_pointer< Value >( shared_from_this() );
    }

private:
    reference_counter * m_reference_counter = nullptr;
};

}