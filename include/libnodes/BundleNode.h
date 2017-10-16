#pragma once
#include "libnodes/Node.h"
#include <tuple>
#include <bitset>

namespace nodes {

//! Convenience type for bundling multiple heterogenous message types
template< typename ...Ts >
using bundle = std::tuple< Ts... >;


//! Node for easily combining multiple messages into one bundle
template< typename ...Ts >
class BundleNode : public Node< Inlets< Ts... >, Outlets< bundle< Ts... > > >
{
public:
    typedef bundle< Ts... > bundle_type;
    static constexpr std::size_t bundle_size = std::tuple_size< bundle_type >::value;

    BundleNode( const std::string & label = "" ) :
            Node< Inlets< Ts... >, Outlets< bundle< Ts... > > >( label )
    {
        reset();

        this->inlets().each_with_index( [&]( auto & inlet, auto i ) {
            inlet.onReceive( [&]( const auto & received ) {
                std::get< i >( mBundle ) = received;
                mElementUpdated[ i ] = true;

                update();
            });
        });
    }

    void reset()
    {
        mElementUpdated.reset();
    }

    void update()
    {
        if ( ! mElementUpdated.all() ) return;

        this->template out< 0 >().update( mBundle );
        reset();
    }

private:

    bundle_type mBundle;
    std::bitset< bundle_size > mElementUpdated;
};

}