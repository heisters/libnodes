#include "catch.hpp"
#include "libnodes/Node.h"
#include "libnodes/operators.h"
#include <iostream>
#include "libnodes/ValueNode.h"

using namespace nodes;
using namespace std;
using namespace Catch;
using namespace nodes::operators;

class FloatRecorder_IONode : public Node< Inlets< float >, Outlets< float > >
{
public:
    FloatRecorder_IONode( const string &label ) : node_type( label ) {
        in< 0 >().onReceive( [&]( const float &f ) {
            received.push_back( f );
            this->out< 0 >().update( f );
        } );
    }

    std::vector< float > received;
};

SCENARIO( "With two value nodes", "[nodes]" ) {
    ValueNodef a( 0.f ), b( 0.f );
    FloatRecorder_IONode r( "recorder" );

    a >> b >> r;

    REQUIRE( a == 0.f );
    REQUIRE( b == 0.f );

    THEN( "it passes everything it receives" ) {
        a.in< 0 >().receive( 0.5f );
        a.in< 0 >().receive( 0.5f );
        REQUIRE( r.received.size() == 2 );
        REQUIRE( r.received[0] == 0.5f );
        REQUIRE( r.received[1] == 0.5f );
        REQUIRE( a == 0.5f );
        REQUIRE( b == 0.5f );
    }

    THEN( "it sends changes in value" ) {
        a = 1.f;
        REQUIRE( r.received.size() == 1 );
        REQUIRE( r.received[0] == 1.f );
        a = 1.f;
        REQUIRE( r.received.size() == 1 );
        a = 1.1f;
        REQUIRE( r.received.size() == 2 );
        REQUIRE( r.received[0] == 1.f );
        REQUIRE( r.received[1] == 1.1f );
        REQUIRE( a == 1.1f );
        REQUIRE( b == 1.1f );
    }

    THEN( "it is possible to manually update" ) {
        float * ptr = &a.get();
        *ptr = 1.f;
        REQUIRE( a == 1.f );
        REQUIRE( b == 0.f );
        REQUIRE( r.received.size() == 0 );

        a.update();

        REQUIRE( a == 1.f );
        REQUIRE( b == 1.f );
        REQUIRE( r.received.size() == 1 );
        REQUIRE( r.received[0] == 1.f );
    }
}