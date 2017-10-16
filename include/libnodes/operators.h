#pragma once

#include "libnodes/Node.h"

namespace nodes {
namespace operators {

//! Connect an outlet to an inlet
template<
        typename To,
        typename Ti,
        typename I = typename Ti::type,
        typename O = typename To::type
>
inline const Ti & operator>>( To &outlet, Ti &inlet )
{
    outlet.connect( inlet );
    return inlet;
}

//! Connect a node ref's first outlet to another node ref's first inlet
template<
        typename Ni,
        typename No,
        std::size_t Ii = 0,
        std::size_t Io = 0,
        typename To = typename Ni::template outlet_type< Ii >::type,
        typename Ti = typename No::template inlet_type< Io >::type
>
inline const ref< No > & operator>>( const ref< Ni > &input, const ref< No > &output )
{
    input->template out< Ii >() >> output->template in< Io >();
    return output;
}

//! Connect a node's first outlet to another node's first inlet
template<
        typename Ni,
        typename No,
        std::size_t Ii = 0,
        std::size_t Io = 0,
        typename To = typename Ni::template outlet_type< Ii >::type,
        typename Ti = typename No::template inlet_type< Io >::type
>
inline No & operator>>( Ni &input, No &output )
{
    input.template out< Ii >() >> output.template in< Io >();
    return output;
}

//! Connect a node's first outlet to an inlet
template<
        typename Ni,
        typename Ti,
        std::size_t Ii = 0,
        typename To = typename Ni::template outlet_type< Ii >::type,
        typename I = typename Ti::type
>
inline Ti & operator>>( Ni &input, Ti &inlet )
{
    input.template out< Ii >() >> inlet;
    return inlet;
}

}
}