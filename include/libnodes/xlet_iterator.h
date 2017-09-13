#pragma once

#include "libnodes/algorithms.h"

namespace nodes {

template< std::size_t I >
using index_constant = typename std::integral_constant< std::size_t, I >;


template< typename T >
class xlet_iterator
{
    T & mXlets;

public:
    xlet_iterator( T & xlets ) : mXlets( xlets ) {};


    //! the underlying container for Xlets, either a std::tuple or std::array
    typedef T container_type;

    //! the number of Xlets
    static constexpr std::size_t size = std::tuple_size< container_type >::value;

    //! get the type of an Xlet at index \a I
    template< std::size_t I >
    using type = typename std::tuple_element< I, container_type >::type;

    //! returns a reference to the Xlet at index \a I
    template< std::size_t I >
    type< I > & get() { return std::get< I >( mXlets ); }

    //! returns a const reference to the Xlet at index \a I
    template< std::size_t I >
    type< I > const & get() const { return std::get< I >( mXlets ); }

    //! iterate over all xlets, starting at index \a i
    template< typename F, std::size_t I = 0 >
    void each( F &fn, index_constant< I > i = index_constant< 0 >{} )
    { return algorithms::call( mXlets, fn, i ); }

    //! iterate over all xlets, starting at index \a i
    template< typename F, std::size_t I = 0 >
    void each( F &&fn, index_constant< I > i = index_constant< 0 >{} )
    { return algorithms::call( mXlets, fn, i ); }

    //! iterate over all xlets, passing the index of the index, starting at
    //! index \a i
    template< typename F, std::size_t I = 0 >
    void each_with_index( F &fn, index_constant< I > i = index_constant< 0 >{} )
    { return algorithms::call_with_index( mXlets, fn, i ); }

    //! iterate over all xlets, passing the index of the index, starting at
    //! index \a i
    template< typename F, std::size_t I = 0 >
    void each_with_index( F &&fn, index_constant< I > i = index_constant< 0 >{} )
    { return algorithms::call_with_index( mXlets, fn, i ); }
};

}