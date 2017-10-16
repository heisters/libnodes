#include "catch.hpp"
#include "libnodes/Node.h"
#include "libnodes/BundleNode.h"
#include "libnodes/ValueNode.h"

using namespace nodes;
using namespace std;
using namespace Catch;
using namespace nodes::operators;


class BundleReceiver_IONode :
        public Node< Inlets< bundle< float, int > >, Outlets< bundle< float, int > > >
{
public:
    BundleReceiver_IONode( const string &label ) : node_type( label ) {
        in< 0 >().onReceive( [&]( const bundle< float, int > &b ) {
            received.push_back( b );
            this->out< 0 >().update( b );
        } );
    }

    std::vector< bundle< float, int > > received;
};

SCENARIO( "bundling messages", "[nodes]" ) {
    ValueNodef nf( 0.f );
    ValueNodei ni( 0 );
    BundleNode< float, int > nb;
    BundleReceiver_IONode nbr( "receiver" );

    nf >>   nb.in< 0 >();
    ni >>   nb.in< 1 >();
            nb              >> nbr;


    THEN( "it does not update when one element changes" ) {
        nf = 0.5f;

        REQUIRE( nbr.received.size() == 0 );
    }

    THEN( "it does update when all elements changes" ) {
        nf = 0.5f;
        ni = 2;

        REQUIRE( nbr.received.size() == 1 );
        REQUIRE( get< 0 >( nbr.received.at( 0 ) ) == 0.5f );
        REQUIRE( get< 1 >( nbr.received.at( 0 ) ) == 2 );
    }

}