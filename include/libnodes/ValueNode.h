#pragma once
#include "libnodes/Node.h"

namespace nodes {

//! A simple node that holds a primitive value and emits it when the value
//! changes.
template< typename T, typename ...Ts >
class ValueNode : public Node< Inlets< T, Ts... >, Outlets< T, Ts... > >
{
public:
    typedef Node< Inlets< T, Ts... >, Outlets< T, Ts... > > node_type;

    ValueNode( const std::string &label, const T & v ) :
            node_type( label ),
            mValue( v )
    {
        mOldValue = mValue;
        listen();
    }

    ValueNode( const T & v ) :
            node_type(),
            mValue( v )
    {
        mOldValue = mValue;
        listen();
    }

    ValueNode() :
            node_type()
    {
        mOldValue = mValue;
        listen();
    }

    void update()
    {
        if ( mOldValue != mValue ) this->template out< 0 >().update( mValue );
        mOldValue = mValue;
    }

    void set( const T & v ) { mValue = v; update(); }

    ValueNode< T > & operator=( const T & v ) { set( v ); return *this; }

    T & get() { return mValue; }
    const T & get() const { return mValue; }
    T operator()() const { return mValue; }


    bool operator==( const T & rhs ) const { return mValue == rhs; }

protected:
    virtual void listen()
    {
        this->template in< 0 >().onReceive( [&] ( const T & newv ) {
            mValue = newv;
            this->template out< 0 >().update( newv );
        });
    }

private:
    T mValue, mOldValue;
};


typedef ValueNode< float > ValueNodef;
typedef ValueNode< int > ValueNodei;
typedef ValueNode< bool > ValueNodeb;
typedef ValueNode< double > ValueNoded;

}
