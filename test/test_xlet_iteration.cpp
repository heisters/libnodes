#include "catch.hpp"
#include "libnodes/Node.h"
#include "libnodes/operators.h"
#include <iostream>

using namespace nodes;
using namespace std;
using namespace Catch;
using namespace nodes::operators;


class TwoInts_IONode : public Node< Inlets< int, int >, Outlets< int, int > > {
public:
    TwoInts_IONode( const string &label ) : node_type( label ) {
        inlets().each_with_index( [&]( auto & inlet, size_t idx ) {
            inlet.onReceive( [&, idx]( const int & i ) {
                received.push_back( i );
            });
        });
    }

    std::vector< int > received;
};

class UniformTwoInts_IONode : public Node< UniformInlets< int, 2 >, UniformOutlets< int, 2 > > {
public:
    UniformTwoInts_IONode( const string &label ) : node_type( label ) {
        inlets().each_with_index( [&]( auto & inlet, size_t idx ) {
            inlet.onReceive( [&, idx]( const int & i ) {
                received.push_back( i );
            });
        });
    }

    std::vector< int > received;
};


struct callable {
    TwoInts_IONode & n;

    template< std::int64_t I >
    void operator()( Inlet< int > & inlet, algorithms::index_constant< I > i )
    {
        inlet.onReceive( [&]( const int & received ) {
            n.out< I >().update( received );
        });
    }
};


SCENARIO( "With a single node with multiple inlets and outlets", "[nodes]" )
{
    TwoInts_IONode n( "label" );

    THEN( "it is possible to iterate inlets" ) {
        int i = 0;
        n.inlets().each( [&]( Inlet< int > &inlet ) {
            i++;
        } );

        REQUIRE( i == 2 );
    }

    THEN( "it is possible to iterate outlets" ) {
        int i = 0;
        n.outlets().each( [&]( Outlet< int > &outlet ) {
            i++;
        } );

        REQUIRE( i == 2 );
    }

    THEN( "it is possible to iterate inlets with indices" ) {
        int i = 0;
        vector< size_t > indices;
        n.inlets().each_with_index( [&]( Inlet< int > &inlet, size_t j ) {
            i++;
            indices.push_back( j );
        } );

        REQUIRE( i == 2 );
        vector< size_t > expected{ 0, 1 };
        REQUIRE( indices == expected );
    }

    THEN( "it is possible to iterate outlets with indices" ) {
        int i = 0;
        vector< size_t > indices;
        n.outlets().each_with_index( [&]( Outlet< int > &outlet, size_t j ) {
            i++;
            indices.push_back( j );
        } );

        REQUIRE( i == 2 );
        vector< size_t > expected{ 0, 1 };
        REQUIRE( indices == expected );
    }

    THEN( "it is possible to iterate inlets and outlets together" ) {
        n.inlets().each_with_index( callable{ n } );

        TwoInts_IONode n2( "two" );

        n >> n2;
        n.out< 1 >() >> n2.in< 1 >();

        n.in< 0 >().receive( 1 );
        n.in< 1 >().receive( 2 );

        REQUIRE( n2.received.size() == 2 );
        REQUIRE( n2.received[ 0 ] == 1 );
        REQUIRE( n2.received[ 1 ] == 2 );
    }

    THEN( "it is possible to skip an inlet" ) {
        int i = 0;
        n.inlets()[ from< 1 >{} ].each( [&]( Inlet< int > &inlet ) {
            i++;
        } );

        REQUIRE( i == 1 );
    }

    THEN( "it is possible to skip an outlet" ) {
        int i = 0;
        n.outlets()[ from< 1 >{} ].each( [&]( Outlet< int > &outlet ) {
            i++;
        } );

        REQUIRE( i == 1 );
    }


    THEN( "it is possible to skip the last inlet" ) {
        int i = 0;
        n.inlets()[ from< 0 >::to < -2 > {} ].each( [&]( Inlet< int > &inlet ) {
            i++;
        } );

        REQUIRE( i == 1 );
    }

    THEN( "it is possible to skip the last outlet" ) {
        int i = 0;
        n.outlets()[ from< 0 >::to < -2 > {} ].each( [&]( Outlet< int > &outlet ) {
            i++;
        } );

        REQUIRE( i == 1 );
    }
}

SCENARIO( "With a single node with uniform inlets and outlets", "[nodes]" ) {
    UniformTwoInts_IONode un( "label" );

    THEN( "it is possible to iterate uniform inlets with a range-based for loop" ) {
        int i = 0;
        for ( auto & inlet : un.inlets() ) {
            i++;
        }

        REQUIRE( i == 2 );
    }

    THEN( "it is possible to access an inlet at runtime" ) {
        auto & i = un.inlets().at( 0 );
        REQUIRE( typeid( i ) == typeid( Inlet< int > )  );
    }
}