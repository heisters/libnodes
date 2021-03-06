#pragma once

#include <cstddef>
#include <tuple>
#include <array>
#include <type_traits>
#include <iostream>
#include <string>
#include "libnodes/nod_signal.h"
#include "libnodes/connection_container.h"
#include "libnodes/xlet_iterator.h"

namespace nodes {

//-----------------------------------------------------------------------------
// Forward declarations

template< class node_t >
using ref = std::shared_ptr< node_t >;

template< class o_node_t >
class INode;
template< class o_node_t >
using INodeRef = ref< INode< o_node_t > >;

template< class T > using signal = nod::signal< T >;
typedef nod::connection connection;

template< typename out_t >
class Outlet;
template< typename in_t >
class Inlet;

class AnyNode;
class NodeBase;
class OutletBase;
class InletBase;

//-----------------------------------------------------------------------------
// Utility base classes

//! makes it illegal to copy a derived class
struct Noncopyable
{
protected:
    Noncopyable() = default;

    ~Noncopyable() = default;

    Noncopyable( const Noncopyable & ) = delete;

    Noncopyable &operator=( const Noncopyable & ) = delete;
};

//! provides derived classes with automatically assigned, globally unique
//! numeric identifiers
class HasId
{
public:
    HasId() : mId( sId++ ) {}

    uint64_t id() const { return mId; }

protected:
    static uint64_t sId;
    uint64_t mId;
};


//! Declares an interface for anything that acts like a node
class NodeConcept
{
public:
    virtual ~NodeConcept() = default;


    virtual uint64_t id() const = 0;

    virtual void setLabel( const std::string &label ) = 0;
    virtual std::string getLabel() const = 0;
    virtual const std::string &label() const = 0;
    virtual std::string &label() = 0;

    virtual std::size_t num_inlets() const = 0;
    virtual std::size_t num_outlets() const = 0;
};

//-----------------------------------------------------------------------------
// Visitors and Visitables

class VisitableBase
{
public:
};

class VisitorBase
{
public:
    virtual ~VisitorBase() = default;
};

template< typename... T >
class Visitor;

template< typename T >
class Visitor< T > : virtual public VisitorBase
{
public:
    virtual void visit( T & ) = 0;
    virtual void visit( OutletBase &, InletBase & ) {}
};

template< typename T, typename... Ts >
class Visitor< T, Ts... > : public Visitor< T >, public Visitor< Ts... >
{
public:
    using Visitor< T >::visit;
    using Visitor< Ts... >::visit;

    virtual void visit( T & ) = 0;
};

//! Base class for custom node visitors. T... is a list of node classes it can visit.
template< class ... T >
class NodeVisitor : public Visitor< T... >
{
public:
};

//! Makes it possible to pass and call methods upon nodes without knowing their types.
class AnyNode : virtual public NodeConcept
{
    struct Concept : virtual public NodeConcept
    {
        virtual ~Concept() = default;

        virtual NodeBase& get() = 0;
        virtual const NodeBase& get() const = 0;

        template< typename V >
        void accept( V & visitor )
        {
            acceptDispatch( &visitor );
        }

        virtual void acceptDispatch( VisitorBase * ) = 0;
    };

    template< typename T >
    struct Model : public Concept
    {
        Model( T &n ) : node( n ) {}

        NodeBase& get() override { return node; }
        const NodeBase& get() const override { return node; }


        void acceptDispatch( VisitorBase * v ) override
        {
            auto typedVisitor = dynamic_cast< Visitor< T >* >( v );
            if ( typedVisitor ) {
                node.accept( *typedVisitor );
            } else {
                auto genericVisitor = dynamic_cast< Visitor< NodeBase >* >( v );
                if ( genericVisitor ) {
                    node.accept( *genericVisitor );
                }
            }
        }

        uint64_t id() const override { return node.id(); }

        void setLabel( const std::string &label ) override { return node.setLabel( label ); }
        std::string getLabel() const override { return node.getLabel(); }
        const std::string &label() const override { return node.label(); }
        std::string &label() override { return node.label(); }
        std::size_t num_inlets() const override { return node.num_inlets(); };
        std::size_t num_outlets() const override { return node.num_outlets(); };

    private:
        T &node;
    };

    std::unique_ptr< Concept > mConcept;
public:

    template< typename T >
    AnyNode( T &node ) :
            mConcept( new Model< T >( node )) {}

    operator NodeBase& () { return mConcept->get(); }
    operator const NodeBase& () const { return mConcept->get(); }


    template< typename V >
    void accept( V & visitor )
    {
        mConcept->accept( visitor );
    }

    uint64_t id() const override { return mConcept->id(); }

    void setLabel( const std::string &label ) override { return mConcept->setLabel( label ); }
    std::string getLabel() const override { return mConcept->getLabel(); }
    const std::string &label() const override { return mConcept->label(); }
    std::string &label() override { return mConcept->label(); }
    std::size_t num_inlets() const override { return mConcept->num_inlets(); };
    std::size_t num_outlets() const override { return mConcept->num_outlets(); };
};



//! Base class for nodes that can accept visitors. V is the class of the node itself.
template< class V >
class VisitableNode : public VisitableBase
{
    template< typename T >
    struct outlet_visitor
    {
        T &visitor;
        outlet_visitor( T &v ) : visitor( v ) {}

        template< typename To, typename Ti >
        void visitConnection( To & o, Ti & i, long )
        {
        };

        template< typename To, typename Ti >
        auto visitConnection( To & o, Ti & i, int )
        -> decltype( visitor.visit( o, i ), void() )
        {
            visitor.visit( o, i );
        };


        template< typename To >
        void operator()( Outlet< To > &outlet )
        {
            for ( auto &inlet : outlet.connections() ) {
                visitConnection( outlet, inlet.get(), 0 );

                auto n = inlet.get().node();
                if ( n != nullptr ) {
                    n->accept( visitor );
                }
            }
        }
    };

public:
    typedef V visitable_type;

    VisitableNode()
    {
        auto &_this = static_cast< V & >( *this );

        _this.outlets().each( [&]( auto &o ) {
            o.setNode( _this );
        } );

        _this.inlets().each( [&]( auto &i ) {
            i.setNode( _this );
        } );
    }

    template< typename T >
    void accept( T &visitor )
    {
        auto &_this = static_cast< V & >( *this );

        visitor.visit( _this );

        outlet_visitor< T > ov( visitor );
        _this.outlets().each( ov );
    }
};

//-----------------------------------------------------------------------------
// Inlets and Outlets




//! Abstract base class for all inlets.
//! Abstract base class for all inlets.
class Xlet : private Noncopyable, public HasId
{
public:
    bool operator<( const Xlet &b ) { return mId < b.mId; }
    bool operator==( const Xlet &b ) { return mId == b.mId; }

    const AnyNode *node() const { return mNode; }
    AnyNode *node() { return mNode; }

    std::size_t getIndex() const { return mIndex; }
    const std::size_t & index() const { return mIndex; }

protected:
    template< typename T >
    void setNode( T &node ) { mNode = new AnyNode( node ); }

    void setIndex( std::size_t i ) { mIndex = i; }

    template< typename Ti, typename To >
    friend class Node;
    template< typename V >
    friend class VisitableNode;


    AnyNode *mNode = nullptr;
    std::size_t mIndex = 0;
};


class InletBase : public Xlet
{
};

class OutletBase : public Xlet
{
};

//! An Inlet accepts \a in_data_ts to its receive method, and returns
//! \a read_t from its read method.
template< typename in_t >
class Inlet : public InletBase
{
public:
    typedef in_t type;
    typedef signal< void( const in_t & ) > receive_signal;
    typedef Outlet< type > outlet_type;
    friend outlet_type;

    virtual void receive( const in_t &data ) { mReceiveSignal( data ); }

    template< class T >
    connection onReceive( T &&fn )
    {
        return mReceiveSignal.connect( fn );
    }

protected:
    bool connect( outlet_type &out ) { return mConnections.insert( out ); }
    bool disconnect( outlet_type &out ) { return mConnections.erase( out ); }
    void disconnect() { mConnections.clear(); }

public:
    bool isConnected() const { return !mConnections.empty(); }
    bool isConnectedTo( const outlet_type &out ) const { return mConnections.contains( out ); }

    std::size_t numConnections() const { return mConnections.size(); }

private:
    receive_signal mReceiveSignal;
    connection_container< outlet_type > mConnections;
};

//! An Outlet connects to an \a out_data_ts Inlet, and is updated with
//! \a update_t.
template< typename out_t >
class Outlet : public OutletBase
{
public:
    typedef out_t type;
    typedef Inlet< type > inlet_type;

    virtual void update( const out_t &in )
    {
        for ( auto &c : mConnections ) {
            c.get().receive( in );
        }
    }

    bool connect( inlet_type &in )
    {
        in.connect( *this );
        return mConnections.insert( in );
    }

    bool disconnect( inlet_type &in )
    {
        in.disconnect( *this );
        return mConnections.erase( in );
    }

    void disconnect()
    {
        for ( auto &i : mConnections ) i.get().disconnect( *this );
        mConnections.clear();
    }

    bool isConnected() const { return !mConnections.empty(); }

    std::size_t numConnections() const { return mConnections.size(); }

    connection_container< inlet_type > &connections() { return mConnections; }
    const connection_container< inlet_type > &connections() const { return mConnections; }

private:
    connection_container< inlet_type > mConnections;
};

//! Provides a common interface for hetero- and homo-geneous inlets.
template< typename T, typename Ti = xlet_iterator< T > >
class AbstractInlets
{
public:
    //! the underlying container for inlets, either a std::tuple or std::array
    typedef T inlets_container_type;

    //! an iterator type for accessing the inlets
    typedef Ti inlets_iterator_type;

    //! the number of inlets
    static constexpr std::size_t in_size = inlets_iterator_type::size;

    //! get the type of the inlet at index \a I
    template< std::size_t I >
    using inlet_type = typename inlets_iterator_type::template type< I >;

    //! returns an iterator wrapping the inlets
    inline inlets_iterator_type inlets() { return inlets_iterator_type{ mInlets }; }

    //! returns a const iterator wrapping the inlets
    inline const inlets_iterator_type inlets() const { return inlets_iterator_type{ mInlets }; }

    //! returns the number of inlets at runtime
    inline std::size_t num_inlets() const { return in_size; }

    //! returns a reference to the inlet at index \a I
    template< std::size_t I >
    inlet_type< I > & in() { return inlets().template get< I >(); }

    //! returns a const reference to the inlet at index \a I
    template< std::size_t I >
    inlet_type< I > const & in() const { return inlets().template get< I >(); }
protected:
    inlets_container_type mInlets;
};

//! Provides a common interface for hetero- and homo-geneous outlets.
template< typename T, typename Ti = xlet_iterator< T > >
class AbstractOutlets
{
public:
    //! the underlying container for outlets, either a std::tuple or std::array
    typedef T outlets_container_type;

    //! an iterator type for accessing the outlets
    typedef Ti outlets_iterator_type;

    //! the number of outlets
    static constexpr std::size_t out_size = outlets_iterator_type::size;

    //! get the type of the outlet at index \a I
    template< std::size_t I >
    using outlet_type = typename outlets_iterator_type::template type< I >;

    //! returns an iterator wrapping the outlets
    inline outlets_iterator_type outlets() { return outlets_iterator_type{ mOutlets }; }

    //! returns a const iterator wrapping the outlets
    inline const outlets_iterator_type outlets() const { return outlets_iterator_type{ mOutlets }; }

    //! returns the number of outlets at runtime
    inline std::size_t num_outlets() const { return out_size; }

    //! returns a reference to the outlet at index \a I
    template< std::size_t I >
    outlet_type< I > & out() { return outlets().template get< I >(); }

    //! returns a const reference to the outlet at index \a I
    template< std::size_t I >
    outlet_type< I > const & out() const { return outlets().template get< I >(); }
protected:
    outlets_container_type mOutlets;
};

//! A collection of heterogeneous Inlets
template< typename ... T >
class Inlets : public AbstractInlets< std::tuple< Inlet< T >... > >
{
public:

};

//! A collection of homogeneous Inlets
template< typename T, std::size_t I, typename A = std::array< Inlet< T >, I > >
class UniformInlets : public AbstractInlets< A, uniform_xlet_iterator< A > >
{
public:

};

//! A collection of heterogeneous Outlets
template< typename ... T >
class Outlets : public AbstractOutlets< std::tuple< Outlet< T >... > >
{
public:
};

//! A collection of homogeneous Outlets
template< typename T, std::size_t I, typename A = std::array< Outlet< T >, I > >
class UniformOutlets : public AbstractOutlets< A, uniform_xlet_iterator< A > >
{
public:
};

class NodeBase : private Noncopyable, public HasId, virtual public NodeConcept
{
public:
    NodeBase( const std::string &label = "" )
    {
        if ( label == "" ) {
            setLabel( "node " + std::to_string( mId ) );
        } else {
            setLabel( label );
        }
    }

    uint64_t id() const override { return HasId::id(); }

    void setLabel( const std::string &label ) override { mLabel = label; }
    std::string getLabel() const override { return mLabel; }
    const std::string &label() const override { return mLabel; }
    std::string &label() override { return mLabel; }

private:
    std::string mLabel;
};

//! A node has inlets and outlets, specified by its template arguments
template< typename Ti, typename To >
class Node : public NodeBase, public Ti, public To, public VisitableNode< Node< Ti, To > >
{
public:
    typedef Node< Ti, To > node_type;

    Node( const std::string &label = "" ) : NodeBase( label )
    {
        size_t i = 0;
        this->inlets().each( [&]( auto & in ) {
            in.setIndex( i++ );
        } );
        i = 0;

        this->outlets().each( [&]( auto & out ) {
            out.setIndex( i++ );
        } );
    }

    std::size_t num_inlets() const override { return Ti::num_inlets(); };
    std::size_t num_outlets() const override { return To::num_outlets(); };
};

}