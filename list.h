#ifndef _LIST_H
#define _LIST_H

#include <memory>
#include <cstddef>
#include <algorithm>

template <typename T>
struct _list_node {
    typedef _list_node<T>* pointer;
    pointer prev;
    pointer next;
    T data;
    _list_node( const T &x ) : data( x ) { }
};

template <typename T>
struct _list_iterator {
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef _list_node<T>* link_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    typedef _list_iterator<T> self;


    link_type node;
    _list_iterator( link_type p ) : node( p ) { }
    _list_iterator( const _list_iterator<T> &rhs ) : node( rhs.node ) { }

    bool operator== ( const self &x ) const { return node == x.node; }
    bool operator!= ( const self &x ) const { return node != x.node; }

    reference operator*() const { return node->data; }
    pointer operator->() const { return &(operator* ()); }

    self& operator++() {
        node = node->next;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        node = node->prev;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

};



template <typename T, class Alloc = std::allocator<_list_node<T>>>
class list {
public:
    typedef _list_node<T> list_node;
    typedef list_node* link_type;
    typedef _list_iterator<T> iterator;
    typedef size_t size_type;
    typedef T value_type;
    typedef T& reference;

    list() { empty_initialize(); _size = 0; }
    ~list() { clear(); }
    iterator begin() { return iterator((*node).next); }
    iterator end() { return iterator(node); }
    bool empty() const { return node->next == node; }
    size_type size() const { return _size; }

    reference front() { return *begin(); }
    reference back() { return *(--end()); }

    iterator insert( iterator position, const T &x ) {
        link_type tmp = create_node( x );
        tmp->next = position.node;
        tmp->prev = position.node->prev;
        (position.node)->prev->next = tmp;
        position.node->prev = tmp;
        ++_size;
        return tmp;
    }

    iterator erase( iterator pos ) {
        link_type next_node = pos.node->next;
        pos.node->prev->next = pos.node->next;
        pos.node->next->prev = pos.node->prev;
        destroy_node( pos.node );
        --_size;
        return iterator(next_node);
    }
    void push_back( const T &x ) { insert( end(), x ); }
    void push_front( const T &x ) { insert( begin(), x ); }


    void pop_front() { erase( begin() ); }
    void pop_back() { iterator tmp = end(); erase( --tmp );  }

    void clear() {
        iterator cur = begin();
        while( cur != end() )
            cur = erase( cur );
        node->next = node;
        node->prev = node;
        _size = 0;
    }

    void remove( const T &x ) {
        iterator first = begin();
        iterator last = end();
        while( first != last ) {
            if( *first == x ) {
                first = erase( first );
                --_size;
            }

            else
                ++first;
        }
    }

    void unique() {
        iterator first = begin();
        iterator last = end();
        if( first == last ) return;
        iterator next = first;
        ++next;
        while( next != last ) {
            if( *first == *next ) {
                next = erase( next );
                --_size;
            }

            else {
                first = next;
                ++next;
            }
        }
    }

    void reverse() {
        iterator pos = begin();
        ++pos;
        while( pos != end() ) {
            link_type tmp = pos.node;
            pos = erase( pos );
            insert( begin(), tmp->data );
        }
    }


protected:

    link_type node;
    Alloc alloc;
    size_type _size;
    link_type get_node() { return alloc.allocate( 2 ); }
    void put_node( link_type p ) { alloc.deallocate( p, 1 ); }

    link_type create_node( const T &x ) {
        link_type p = get_node();
        alloc.construct( p, x );
        return p;
    }

    void destroy_node( link_type p ) {
        alloc.destroy( p );
        put_node( p );
    }

    void empty_initialize() {
        node = get_node();
        node->next = node;
        node->prev = node;
    }
};

#endif //  _LIST_H
