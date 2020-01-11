#pragma once

#include <ntsp/shared_pointer.h>
#include <ntsp/weak_pointer.h>

namespace ntsp {

template< typename Value, thread_policy_e Policy = thread_policy_e::safe >
class enable_shared_from_this
{
public:
    using value_type = Value;
    constexpr static auto thread_policy = Policy;

public:
    virtual ~enable_shared_from_this() = default;

public:
    [[ nodiscard ]] shared_pointer< value_type, thread_policy > shared_from_this() noexcept
    {
        assert( m_reference_counter && "Was not shared" );
        return shared_pointer< value_type, thread_policy >( m_reference_counter, reinterpret_cast< Value * >( this ) );
    }

    [[ nodiscard ]] weak_pointer< value_type, thread_policy > weak_from_this() noexcept
    {
        return weak_pointer< value_type, thread_policy  >( shared_from_this() );
    }

private:
    template< typename V, thread_policy_e P >
    friend class weak_pointer;

    template< typename V, thread_policy_e P >
    friend class shared_pointer;

private:
    reference_counter< thread_policy > * m_reference_counter = nullptr;
};

}