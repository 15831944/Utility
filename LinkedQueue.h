/*
 * File:   LinkedQueue.h
 * Author: Andy Huang
 *
 * Created on 2020年7月30日, 下午 12:15
 */

#ifndef UTILITY_LINKEDQUEUE_H
#define UTILITY_LINKEDQUEUE_H

#include <cstddef>

namespace KGI_TW_Der_Utility {

template<typename T>
struct LinkedQueue {
private:
    struct element {
        T content;
        element* next;

        element( const T& in ) : content( in ), next( NULL ) {}
    };
public:
    LinkedQueue() : head( NULL ), tail( NULL ) {}

    bool pop( T& out ) {
        try {
            if ( head == NULL || tail == NULL ) {
                return false;
            }
            else {
                out = head->content;
                element* next = head->next;
                delete head;
                if ( next == NULL ) {
                    head = tail = NULL;
                }
                else {
                    head = next;
                }
                return true;
            }
        } catch ( ... ) {
            return false;
        }
    }

    bool push( const T& in ) {
        try {
            element* eIn = new element( in );
            if ( head == NULL && tail == NULL ) {
                head = tail = eIn;
                return true;
            }
            else if ( head == NULL || tail == NULL ) {
                delete eIn;
                return false;
            }
            else {
                tail->next = eIn;
                tail = eIn;
                return true;
            }
        }
        catch ( ... ) {
            return false;
        }
    }

    bool empty() {
        return ( head == NULL && tail == NULL );
    }
private:
    element* head;
    element* tail;
};

}

#endif /* UTILITY_LINKEDQUEUE_H */