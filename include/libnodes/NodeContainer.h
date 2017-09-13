#pragma once

#include <cstddef>
#include <functional>
#include <algorithm>
#include <memory>
#include <vector>
#include <set>

namespace nodes {
template< class T >
using ref = std::shared_ptr< T >;

struct reference_wrapper_compare {
    template< typename A, typename B >
    bool operator()( const std::reference_wrapper< A > &lhs, const std::reference_wrapper< B > &rhs ) const {
        return lhs.get() < rhs.get();
    }
};


template< typename V >
class connection_container {
public:
    typedef reference_wrapper_compare compare;
    typedef std::reference_wrapper< V > value_type;
    typedef std::vector< value_type > vector_type;
    typedef std::set< value_type, compare > set_type;

    bool insert( const value_type &member ) {
        if ( mSet.insert( member ).second ) {
            mVector.push_back( member );
            return true;
        }
        return false;
    }

    bool erase( const value_type &member ) {
        if ( mSet.erase( member )) {
            mVector.erase( std::remove_if( mVector.begin(), mVector.end(), [&]( const value_type &m ) {
                return m.get() == member.get();
            } ), mVector.end());
            return true;
        }
        return false;
    }

    typename vector_type::iterator begin() { return mVector.begin(); }
    typename vector_type::iterator end() { return mVector.end(); }
    typename vector_type::const_iterator cbegin() const { return mVector.cbegin(); }
    typename vector_type::const_iterator cend() const { return mVector.cend(); }

    bool empty() const { return mSet.empty(); }

    std::size_t size() const { return mSet.size(); }

    void clear() {
        mVector.clear();
        mSet.clear();
    }

private:
    vector_type mVector;
    set_type mSet;
};

namespace algorithms {
///////////////////////////////////////////////////////////////////////////
// connection_container algorithms
///////////////////////////////////////////////////////////////////////////
template< class value_t, class return_t, class ref_t = ref< value_t > >
void call( connection_container< value_t > &container,
           return_t( ref_t::element_type::*fn )( void )) {
    for ( auto &c : container ) (( *c ).*fn )();
}

template<
        class a_t,
        class b_t,
        class a_ref_t = ref< a_t >,
        class b_ref_t = const ref< b_t >
>
void call( connection_container< a_t > &a,
           const connection_container< b_t > &b,
           void ( a_t::*fn )( const b_t & )) {
    auto a_it = a.begin();
    auto a_end = a.end();
    auto b_it = b.begin();
    auto b_end = b.end();

    while ( a_it != a_end && b_it != b_end ) {
        (( **a_it ).*fn )( *b_it );
        ++a_it;
        ++b_it;
    }
}


template<
        class a_t,
        class b_t,
        class a_ref_t = ref< a_t >,
        class b_ref_t = const ref< b_t >
>
void call( connection_container< a_t > &a,
           const b_ref_t &b,
           void ( a_t::*fn )( const b_t & )) {
    for ( auto &aa : a ) (( *aa ).*fn )( b );
}


template< class T, typename ref_t = ref< T > >
void update( connection_container< T > &connections ) {
    call( connections, &ref_t::element_type::update );
}

///////////////////////////////////////////////////////////////////////////
// std::tuple algorithms
///////////////////////////////////////////////////////////////////////////


// call a function on each member of a tuple
template<
        typename F,
        std::size_t I = 0,
        typename... Tp
>
inline typename std::enable_if< I >= sizeof...( Tp ) >::type
call( std::tuple< Tp... > &, F &, std::integral_constant< std::size_t, I > ) // Unused arguments are given no names.
{}

template<
        typename F,
        std::size_t I = 0,
        typename... Tp
>
inline typename std::enable_if< I < sizeof...( Tp ) >::type
call( std::tuple< Tp... > &t, F & fn, std::integral_constant< std::size_t, I > i )
{
    fn( std::get< I >( t ) );
    call( t, fn, std::integral_constant< std::size_t, I + 1 >{} );
}


// call a function on each member of a tuple, passing the member index as the second
// parameter


template<
        typename F,
        std::size_t I = 0,
        typename... Tp
>
inline typename std::enable_if< I >= sizeof...( Tp ) >::type
call_with_index( std::tuple< Tp... > &, F &, std::integral_constant< std::size_t, I > ) // Unused arguments are given no names.
{}

template<
        typename F,
        std::size_t I = 0,
        typename... Tp
>
inline typename std::enable_if< I < sizeof...( Tp ) >::type
call_with_index( std::tuple< Tp... > &t, F & fn, std::integral_constant< std::size_t, I > i )
{
    fn( std::get< I >( t ), i );
    call_with_index( t, fn, std::integral_constant< std::size_t, I + 1 >{} );
}

///////////////////////////////////////////////////////////////////////////
// std::array algorithms
///////////////////////////////////////////////////////////////////////////

template<
        typename F,
        typename V,
        std::size_t N,
        std::size_t I = 0
>
inline typename std::enable_if< I >= N >::type
call( std::array< V, N > &, F &, std::integral_constant< std::size_t, I > )
{}

template<
        typename F,
        typename V,
        std::size_t N,
        std::size_t I = 0
>
inline typename std::enable_if< I < N >::type
call( std::array< V, N > &a, F & fn, std::integral_constant< std::size_t, I > i )
{
    fn( a.at( i ) );
    call( a, fn, std::integral_constant< std::size_t, I + 1 >{} );
}


template<
        typename F,
        typename V,
        std::size_t N,
        std::size_t I = 0
>
inline typename std::enable_if< I >= N >::type
call_with_index( std::array< V, N > &, F &, std::integral_constant< std::size_t, I > )
{}

template<
        typename F,
        typename V,
        std::size_t N,
        std::size_t I = 0
>
inline typename std::enable_if< I < N >::type
call_with_index( std::array< V, N > &a, F & fn, std::integral_constant< std::size_t, I > i )
{
    fn( a.at( i ), i );
    call_with_index( a, fn, std::integral_constant< std::size_t, I + 1 >{} );
}

}
}

