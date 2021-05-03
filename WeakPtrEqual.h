/*
 * File:   WeakPtrEqual.h
 * Author: 0010080
 *
 * Created on 2017年12月11日, 下午 12:10
 */

#ifndef UTILITY_WEAKPTREQUAL_H
#define UTILITY_WEAKPTREQUAL_H

#include <memory>

namespace KGI_TW_Der_Utility {

template<class T>
class WeakPtrEqual {
public:
    WeakPtrEqual( const std::weak_ptr<T>& ptr ) : _ptr( ptr ) {}
    WeakPtrEqual( const std::shared_ptr<T>& ptr ) : _ptr( ptr ) {}

    bool operator()( const std::weak_ptr<T>& ptr ) {
        return ( !ptr.owner_before( _ptr ) && !_ptr.owner_before( ptr ) );
    }
    bool operator()( const std::shared_ptr<T>& ptr ) {
        return ( !ptr.owner_before( _ptr ) && !_ptr.owner_before( ptr ) );
    }
protected:
    std::weak_ptr<T> _ptr;
};

}

#endif /* UTILITY_WEAKPTREQUAL_H */