#pragma once

#include <cassert>

#include <ntsp/traits.h>
#include <ntsp/reference_counter.h>
#include <memory>

namespace ntsp {
namespace detail {

template< size_t Index, typename... Args >
constexpr decltype( auto ) magic_get_tuple( Args && ... args ) noexcept
{
    return std::get< Index >( std::forward_as_tuple( std::forward< Args >( args )... ) );
}

template< std::size_t N, typename FirstType, typename... Args, std::enable_if_t< N == 0, int >... >
constexpr decltype( auto ) magic_get( FirstType && first, Args && ... as ) noexcept
{
    return std::forward< FirstType >( first );
}

template< std::size_t N, typename FirstType, typename... Args, std::enable_if_t< N != 0, int >... >
constexpr decltype( auto ) magic_get( FirstType && first, Args && ... as ) noexcept
{
    return magic_get< N - 1 >( std::forward< Args >( as )... );
}

template< typename ... Args >
constexpr thread_policy_e get_policy( Args && ... args )
{
    if constexpr ( sizeof ... ( args ) > 0 )
    {
        const auto first_value = detail::magic_get< 0 >( std::forward< Args >( args )... );
        if constexpr ( std::is_same_v< thread_policy_e, decltype( first_value ) > )
        {
            return first_value;
        }
        else
        {
            return thread_policy_e::safe;
        }
    }
    else
    {
        return thread_policy_e::safe;
    }
}
}

template< typename T >
struct shared_pointer_default_config final
{
    using value_type = T;
    using deleter = std::default_delete< value_type >;
    using allocator = std::allocator< value_type >;
    constexpr static thread_policy_e thread_policy = thread_policy_e::safe;
};

template< class From, class To >
concept convertible_to = std::is_convertible_v< From, To > &&
                         requires( From from ) {
                             static_cast< To >( from );
                         };

template< typename SharedPointerConfig >
concept shared_pointer_config =
requires {
    typename SharedPointerConfig::value_type;
    SharedPointerConfig::thread_policy -> convertible_to< thread_policy_e >;
};

template< typename Value, thread_policy_e Policy = thread_policy_e::safe >
class shared_pointer final
{

public:
    using value_type = Value;
    constexpr static thread_policy_e thread_policy = Policy;

public:
    template< typename ... Args >
    static decltype( auto ) make( Args && ... args )
    {
        constexpr static std::size_t reference_counter_size = sizeof( std::aligned_storage_t< sizeof( reference_counter_t ), alignof( reference_counter_t ) > );
        constexpr static std::size_t storage_size = sizeof( std::aligned_storage_t< sizeof( value_type ), alignof( value_type ) > );

        const auto memory = static_cast< char * >( std::malloc( reference_counter_size + storage_size ) );
        if( ! memory )
        {
            throw std::bad_alloc();
        }

        const auto counter = new( memory ) reference_counter_t( true );
        const auto value = new( memory + reference_counter_size ) value_type( std::forward< Args ... >( args ... ) );
        return shared_pointer< value_type, thread_policy >( counter, value );
    }

public:
    shared_pointer()
            : m_reference_counter( new reference_counter_t( false ) )
            , m_storage( nullptr )
    {
        process_shared_from_this( shared_pointer::m_storage, this );
    }

    explicit shared_pointer( value_type * value )
            : m_reference_counter( new reference_counter_t( false ) )
            , m_storage( reinterpret_cast< storage_t * >( value ) )
    {
        m_reference_counter->add_strong();
    }

    ~shared_pointer()
    {
        if( ! m_reference_counter )
        {
            return;
        }

        delete_counter_and_storage();
    }

    shared_pointer( const shared_pointer & other )
            : m_reference_counter( other.m_reference_counter )
            , m_storage( other.m_storage )
    {
        if( m_reference_counter )
        {
            m_reference_counter->add_strong();
        }
    }

    shared_pointer & operator =( const shared_pointer & other )
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_counter_and_storage();

        m_reference_counter = other.m_reference_counter;
        if( m_reference_counter )
        {
            m_reference_counter->add_strong();
        }
        m_storage = other.m_storage;
        return *this;
    }

    shared_pointer( shared_pointer && other ) noexcept
            : m_reference_counter( other.m_reference_counter )
            , m_storage( other.m_storage )
    {
        other.m_reference_counter = nullptr;
        other.m_storage = nullptr;
    }

    shared_pointer & operator =( shared_pointer && other ) noexcept
    {
        if( &other == this || other.m_reference_counter == m_reference_counter )
        {
            return *this;
        }

        delete_counter_and_storage();

        m_reference_counter = other.m_reference_counter;
        m_storage = other.m_storage;

        other.m_reference_counter = nullptr;
        other.m_storage = nullptr;

        return *this;
    }

public:
    [[nodiscard]] inline value_type * get() const noexcept
    {
        return reinterpret_cast< value_type * >( m_storage );
    }

    [[nodiscard]] inline bool empty() const noexcept
    {
        return nullptr == m_storage;
    }

    explicit inline operator bool() const noexcept
    {
        return empty();
    }

    inline value_type * operator ->() const noexcept
    {
        return get();
    }

    inline value_type & operator *() const noexcept
    {
        assert( m_storage && "value_type == nullptr" );
        return *get();
    }

public:
    [[nodiscard]] bool operator ==( const shared_pointer & rhs ) const noexcept
    {
        return m_reference_counter == rhs.m_reference_counter;
    }

    [[nodiscard]] bool operator !=( const shared_pointer & rhs ) const noexcept
    {
        return ! ( rhs == *this );
    }

    [[nodiscard]] bool operator <( const shared_pointer & rhs ) const noexcept
    {
        return m_reference_counter < rhs.m_reference_counter;
    }

private:
    friend class weak_pointer< value_type, thread_policy >;

    friend class enable_shared_from_this< value_type, thread_policy >;

    template< typename V, typename ... Args >
    friend decltype( auto ) make_shared( Args && ... args );

private:
    using reference_counter_t = reference_counter< thread_policy >;
    reference_counter_t * m_reference_counter;

    using storage_t = std::aligned_storage_t< sizeof( value_type ), alignof( value_type ) >;
    storage_t * m_storage;

private:
    explicit shared_pointer( reference_counter_t * reference_counter, value_type * value ) noexcept
            : m_reference_counter( reference_counter )
            , m_storage( reinterpret_cast< storage_t * >( value ) )
    {
        m_reference_counter->add_strong();
        process_shared_from_this( get(), this );
    }

private:
    void delete_counter_and_storage()
    {
        assert( m_reference_counter && "Already moved" );
        if( m_reference_counter->remove_and_test_strong_empty() == reference_counter_t::state_e::non_empty )
        {
            return;
        }

        if( m_reference_counter->is_monotonic_allocated() )
        {
            get()->~value_type();
        }
        else
        {
            delete m_storage;
        }
        m_storage = nullptr;

        if( m_reference_counter->test_weak() == reference_counter_t::state_e::non_empty )
        {
            return;
        }

        if( m_reference_counter->is_monotonic_allocated() )
        {
            m_reference_counter->~reference_counter();
            std::free( m_reference_counter );
        }
        else
        {
            delete m_reference_counter;
        }
        m_reference_counter = nullptr;
    }

    static void process_shared_from_this( value_type * value, shared_pointer< value_type, thread_policy > * self )
    {
        if constexpr( is_enable_shared_from_this_v< value_type, thread_policy > )
        {
            const auto shared_from_this = reinterpret_cast< enable_shared_from_this< value_type, thread_policy > * >( value );
            shared_from_this->m_reference_counter = self->m_reference_counter;
        }
    }
};

template< typename Value, thread_policy_e Policy, typename ... Args >
decltype( auto ) make_shared( Args && ... args )
{
    return shared_pointer< Value, Policy >::make( std::forward< Args ... >( args ... ) );
}

template< typename Value, typename ... Args >
decltype( auto ) make_shared( Args && ... args )
{
    return make_shared< Value, thread_policy_e::safe >( std::forward< Args ... >( args ... ) );
}

}