#pragma once

#include <cstddef>
#include <functional>
#include <algorithm>
#include <memory>
#include <vector>
#include <set>

namespace nodes {
template< class T >
using ref = std::shared_ptr< T >;

struct reference_wrapper_compare {
    template< typename A, typename B >
    bool operator()( const std::reference_wrapper< A > &lhs, const std::reference_wrapper< B > &rhs ) const {
        return lhs.get() < rhs.get();
    }
};


template< typename V >
class connection_container {
public:
    typedef reference_wrapper_compare compare;
    typedef std::reference_wrapper< V > value_type;
    typedef std::vector< value_type > vector_type;
    typedef std::set< value_type, compare > set_type;

    bool insert( const value_type &member ) {
        if ( mSet.insert( member ).second ) {
            mVector.push_back( member );
            return true;
        }
        return false;
    }

    bool erase( const value_type &member ) {
        if ( mSet.erase( member )) {
            mVector.erase( std::remove_if( mVector.begin(), mVector.end(), [&]( const value_type &m ) {
                return m.get() == member.get();
            } ), mVector.end());
            return true;
        }
        return false;
    }

    typename vector_type::iterator begin() { return mVector.begin(); }
    typename vector_type::iterator end() { return mVector.end(); }
    typename vector_type::const_iterator cbegin() const { return mVector.cbegin(); }
    typename vector_type::const_iterator cend() const { return mVector.cend(); }

    bool empty() const { return mSet.empty(); }

    std::size_t size() const { return mSet.size(); }

    void clear() {
        mVector.clear();
        mSet.clear();
    }

private:
    vector_type mVector;
    set_type mSet;
};

}

