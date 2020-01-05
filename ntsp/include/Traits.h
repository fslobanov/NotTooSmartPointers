#pragma once

#include <type_traits>

namespace NTSP {

template< typename Value >
class WeakPointer;

template< typename Value >
class SharedPointer;

template< typename Value >
class EnableSharedFromThis;

template< typename All >
struct IsSharedPointer final : public std::false_type
{
};

template< typename Value >
struct IsSharedPointer< SharedPointer< Value > > final : std::true_type
{
};

template< typename All >
struct IsWeakPointer final : public std::false_type
{
};

template< typename Value >
struct IsWeakPointer< IsWeakPointer< Value > > final : std::true_type
{
};

template< typename Value >
struct IsEnableSharedFromThis  final : public std::is_base_of< EnableSharedFromThis< Value >, Value >
{
};

}
