#pragma once
#include "connection_container.h"

namespace nodes {

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