#ifndef _FORWARD_H
#define _FORWARD_H

#include <memory>
#include <cstddef>
template <typename T>
struct _forward_list_node {
    typedef _forward_list_node<T>* pointer;
    typedef _forward_list_node<T> self;
    pointer next;
    T data;

    _forward_list_node() : next( nullptr ) { }
    _forward_list_node( const T &x ) : data( x ) { }
    _forward_list_node( const self &x ) : data( x.data ), next( x.next ) { }
   // _forward_list_node( const pointer &x ) : data( x->data ), next( x->next ) { }
};

template <typename T>
struct _forward_list_iterator {
    typedef T value_type;
    typedef T& reference;
    typedef T* pointer;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;

    typedef _forward_list_iterator<T> self;
    typedef _forward_list_node<T>* list_node;
    list_node node;

    _forward_list_iterator( const list_node &x ) : node( x ) { }
    _forward_list_iterator( const self &x ) : node( x.node ) { }

    bool operator==( const self &x ) { return node == x.node; }
    bool operator!=( const self &x ) { return node == x.node; }

    reference operator*() { return node->data; }
    pointer operator->() { return &(operator*()); }

    self& operator++() { node = node->next; return *this; }
    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

};

template <typename T, class Alloc = std::allocator<_forward_list_node<T>>>
class forward_list {
public:
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef _forward_list_iterator<T> iterator;

    forward_list() { head.next = nullptr; _size = 0; }
   // ~forward_list() { _size = 0; }

    iterator begin() { return iterator( head.next ); } //head.nextΪ_forward_list_node<T>*
    iterator end() { return iterator( nullptr ); }
    size_type size() { return _size; }
    bool empty() { return head.next == nullptr; }

    reference front() { return head.next->data; }
    void push_front( const value_type &x ) { insertA( iterator( &head ), x ); }

    void insertA( iterator pos, const value_type &x ) {

        Node tmp = pos.node->next;
        pos.node->next = create_node( x );
        ++pos;
        pos.node->next = tmp;

        //pos.node->next = create_node( x );

        //(pos.node->next).node->next = tmp;
        ++_size;
    }

    value_type pop_front() { eraseA( iterator( &head ) ); }
    void eraseA( iterator pos ) {
        Node tmp = pos.node;
        ++pos;
        Node del = pos.node;
        ++pos;
        Node next = pos.node;
        tmp->next = next;
        del->next = nullptr;
        destroy_node( del );

        /*Node tmp = (pos.node->next).node->next;
        Node next = pos.node->next;
        pos.node->next = tmp;
        next->next = nullptr;
        destroy_node( next );*/
        --_size;
    }

private:
    typedef _forward_list_node<T> list_node;
    typedef list_node* Node;
    size_type _size;
    Alloc alloc;
    list_node head;
    Node create_node( const value_type &x ) {
        Node node = alloc.allocate( 1 );
        alloc.construct( node, x );
        return node;
    }

    void destroy_node( Node node ) {
        alloc.destroy( node );
        alloc.deallocate( node, 1 );
    }
};

#endif // _FORWARD_H
