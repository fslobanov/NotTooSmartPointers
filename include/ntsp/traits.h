#pragma once

#include <type_traits>

#include <ntsp/types.h>

namespace ntsp {

template< typename Value, thread_policy_e Policy >
class shared_pointer;

template< typename Value, thread_policy_e Policy >
class weak_pointer;

template< typename Value, thread_policy_e Policy >
class enable_shared_from_this;

template< typename All,  thread_policy_e Policy  >
struct is_shared_pointer final : public std::false_type
{
};

template< typename Value, thread_policy_e Policy >
struct is_shared_pointer< shared_pointer< Value, Policy >, Policy > final : std::true_type
{
};

template < typename Value, thread_policy_e Policy >
constexpr auto is_shared_pointer_v = is_shared_pointer< Value, Policy >::value;

template< typename All, thread_policy_e Policy >
struct is_weak_pointer final : public std::false_type
{
};

template< typename Value, thread_policy_e Policy >
struct is_weak_pointer< weak_pointer< Value, Policy >, Policy > final : std::true_type
{
};

template < typename Value, thread_policy_e Policy >
constexpr auto is_weak_pointer_v = is_weak_pointer< Value, Policy  >::value;

template< typename Value, thread_policy_e Policy >
struct is_enable_shared_from_this final : public std::is_base_of< enable_shared_from_this < Value, Policy >, Value >
{
};

template < typename Value, thread_policy_e Policy >
constexpr bool is_enable_shared_from_this_v = is_enable_shared_from_this< Value, Policy >::value;

}
