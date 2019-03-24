#ifndef _QUEUE_H
#define _QUEUE_H

#include "list.h"

template <typename T, class Seq = list<T>>
class queue {
public:
    typedef typename Seq::value_type value_type;
    typedef typename Seq::reference reference;
    typedef typename Seq::size_type size_type;

    queue() = default;
    bool empty() const { return c.empty(); }
    size_type size() const { return c.size(); }
    reference front()  { return c.front(); }
    reference back()  { return c.back(); }
    void push( const value_type &val ) { c.push_back( val ); }
    value_type pop() {
        value_type tmp = front();
        c.pop_front();
        return tmp;
    }

protected:
    Seq c;
};

#endif // _QUEUE_H
