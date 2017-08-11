libnodes
========

Makes it easy to construct trees of nodes that pass arbitrary data on update.

Features:
* type safe
* allows tree traversal using `VisitableNode<T>`
* provides syntactic sugar and algorithms for common operations like connecting
  nodes and iterating over them. 

Examples
--------

Declare a node type that sends and receives integers:

```c++
class Int_IONode :
public Node<                        // descend from abstract Node class
    Inlets< int >,                  // It has one inlet that takes ints
    Outlets< int >                  // And one outlet that emits ints
>
{
public:
    // overload the default constructor to provide an onReceive listener
    Int_IONode( const string &label ) : Node< Inlets< int >, Outlets< int > >( label ) {
        // listen to the first inlet for ints
        in< 0 >().onReceive( [&]( const int &i ) {
            // store the received int in a vector
            received.push_back( i );
            // send the int to the first outlet
            this->out< 0 >().update( i );
        } );
    }

    std::vector< int > received;
};
```

Construct two nodes and connect them:

```c++
Int_IONode n1( "label 1" );
Int_IONode n2( "label 2" );
n1 >> n2;
```

Send the root node an integer, and it propagates through the tree:

```c++
n1.in< 0 >().receive( 1 );
// n2.received[0] == 1

```

See the [tests](test/test_nodes.cpp) for more examples.