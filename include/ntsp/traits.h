#pragma once

#include <type_traits>

namespace ntsp {

template< typename Value >
class weak_pointer_t;

template< typename Value >
class shared_pointer_t;

template< typename Value >
class enable_shared_from_this_t;

template< typename All >
struct is_shared_pointer final : public std::false_type
{
};

template< typename Value >
struct is_shared_pointer< shared_pointer_t< Value > > final : std::true_type
{
};

template < typename Value >
constexpr auto is_shared_pointer_v = is_shared_pointer< Value >::value;

template< typename All >
struct is_weak_pointer final : public std::false_type
{
};

template< typename Value >
struct is_weak_pointer< weak_pointer_t< Value > > final : std::true_type
{
};

template < typename Value >
constexpr auto is_weak_pointer_v = is_weak_pointer< Value >::value;

template< typename Value >
struct is_enable_shared_from_this final : public std::is_base_of< enable_shared_from_this_t < Value >, Value >
{
};

template < typename Value >
constexpr bool is_enable_shared_from_this_v = is_enable_shared_from_this< Value >::value;

}
