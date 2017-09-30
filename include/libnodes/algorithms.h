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

template< std::int64_t I >
using index_constant = typename std::integral_constant< std::int64_t, I >;

template< std::int64_t i, std::int64_t end, std::size_t size >
struct is_iterable
{
    static constexpr bool test = i <= ( end < 0 ? (std::int64_t)size + end : end );
};


// call a function on each member of a tuple
template<
        typename F,
        std::int64_t I = 0,
        std::int64_t End = -1,
        typename... Tp
>
inline typename std::enable_if< ! is_iterable< I, End, sizeof...( Tp ) >::test >::type
call( std::tuple< Tp... > &, F &, index_constant< I >, index_constant< End > ) // Unused arguments are given no names.
{}

template<
        typename F,
        std::int64_t I = 0,
        std::int64_t End = -1,
        typename... Tp
>
inline typename std::enable_if< is_iterable< I, End, sizeof...( Tp ) >::test >::type
call( std::tuple< Tp... > &t, F & fn, index_constant< I > i, index_constant< End > e )
{
    fn( std::get< I >( t ) );
    call( t, fn, index_constant< I + 1 >{}, e );
}

// call a function on each member of a tuple, passing the member index as the second
// parameter


template<
        typename F,
        std::int64_t I = 0,
        std::int64_t End = -1,
        typename... Tp
>
inline typename std::enable_if< ! is_iterable< I, End, sizeof...( Tp ) >::test >::type
call_with_index( std::tuple< Tp... > &, F &, index_constant< I >, index_constant< End > ) // Unused arguments are given no names.
{}

template<
        typename F,
        std::int64_t I = 0,
        std::int64_t End = -1,
        typename... Tp
>
inline typename std::enable_if< is_iterable< I, End, sizeof...( Tp ) >::test >::type
call_with_index( std::tuple< Tp... > &t, F & fn, index_constant< I > i, index_constant< End > e )
{
    fn( std::get< I >( t ), i );
    call_with_index( t, fn, index_constant< I + 1 >{}, e );
}

///////////////////////////////////////////////////////////////////////////
// std::array algorithms
///////////////////////////////////////////////////////////////////////////

template<
        typename F,
        typename V,
        std::size_t N,
        std::int64_t I = 0,
        std::int64_t End = -1
>
inline typename std::enable_if< ! is_iterable< I, End, N >::test >::type
call( std::array< V, N > &, F &, index_constant< I >, index_constant< End > )
{}

template<
        typename F,
        typename V,
        std::size_t N,
        std::int64_t I = 0,
        std::int64_t End = -1
>
inline typename std::enable_if< is_iterable< I, End, N >::test >::type
call( std::array< V, N > &a, F & fn, index_constant< I > i, index_constant< End > e )
{
    fn( a.at( i ) );
    call( a, fn, index_constant< I + 1 >{}, e );
}


template<
        typename F,
        typename V,
        std::size_t N,
        std::int64_t I = 0,
        std::int64_t End = -1
>
inline typename std::enable_if< ! is_iterable< I, End, N >::test >::type
call_with_index( std::array< V, N > &, F &, index_constant< I >, index_constant< End > )
{}

template<
        typename F,
        typename V,
        std::size_t N,
        std::int64_t I = 0,
        std::int64_t End = -1
>
inline typename std::enable_if< is_iterable< I, End, N >::test >::type
call_with_index( std::array< V, N > &a, F & fn, index_constant< I > i, index_constant< End > e )
{
    fn( a.at( i ), i );
    call_with_index( a, fn, index_constant< I + 1 >{}, e );
}

}

}