#pragma once

#include "libnodes/Node.h"
#include <memory>

namespace nodes {

template< typename Tfrom, typename Tto >
class ImplicitConversionNode : public Node< Inlets < Tfrom >, Outlets< Tto > >
{
public:
    ImplicitConversionNode( const std::string &label = "" ) :
            Node< Inlets < Tfrom >, Outlets< Tto > >( label )
    {
        this->template in< 0 >().onReceive( [&]( const Tfrom &from ) {
            this->template out< 0 >().update( from );
        });
    }
};


template< typename Tfrom, typename Tto >
class ImplicitConversionNode< std::shared_ptr< Tfrom >, std::shared_ptr< Tto > > :
        public Node< Inlets< std::shared_ptr< Tfrom > >, Outlets< std::shared_ptr< Tto > > >
{
public:
    ImplicitConversionNode( const std::string &label = "" ) :
            Node< Inlets< std::shared_ptr< Tfrom > >, Outlets< std::shared_ptr< Tto > > >( label )
    {
        this->template in< 0 >().onReceive( [&]( const std::shared_ptr< Tfrom > &from ) {
            this->template out< 0 >().update( std::dynamic_pointer_cast< Tto >( from ) );
        });
    }
};


namespace operators {

template< typename Tfrom, typename Tto >
using node_convert = ImplicitConversionNode< Tfrom, Tto >;

}

}