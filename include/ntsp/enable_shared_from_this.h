#pragma once

#include "weak_pointer.h"

namespace ntsp {

template< typename Value >
class enable_shared_from_this_t
{
    template< typename V >
    friend class weak_pointer_t;

    template< typename V >
    friend class shared_pointer_t;

public:
    virtual ~enable_shared_from_this_t() = default;

public:
    [[ nodiscard ]] shared_pointer_t< Value > shared_from_this() noexcept
    {
        assert( m_reference_counter && "Was not shared" );
        return shared_pointer_t< Value >( m_reference_counter, reinterpret_cast< Value * >( this ) );
    }

    [[ nodiscard ]] weak_pointer_t< Value > weak_from_this() noexcept
    {
        return weak_pointer_t< Value >( shared_from_this() );
    }

private:
    reference_counter_t * m_reference_counter = nullptr;
};

}