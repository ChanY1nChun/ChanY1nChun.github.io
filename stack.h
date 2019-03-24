#ifndef _STACK_H
#define _STACK_H

#include "list.h"

template<typename T, class Seq = list<T>>
class stack {
public:
    typedef typename Seq::value_type value_type;
    typedef typename Seq::size_type size_type;
    typedef typename Seq::reference reference;

    bool empty() { return c.empty(); }
    size_type size() const { return c.size(); }
    reference top() { return c.back(); }

    void push( const value_type &x ) { c.push_back(x); }
    value_type pop() {
        value_type tmp = top();
        c.pop_back();
        return tmp;
    }
    //void pop() { c.pop_back(); }

    stack() = default;

protected:
    Seq c;
};



#endif // _STACK_H

