#include <iostream>
#include <cstddef>
#include <memory>


#define VECTOR_ORG 20
using std::allocator;
template <typename T, typename Alloc = std::allocator<T>>
class vector {
public:
    typedef T value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef value_type* iterator;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;




protected:
    Alloc alloc;
    iterator start;
    iterator finish;
    iterator end_of_storage;



public:
    iterator& begin() { return start; }
    iterator& end() { return finish; }
    size_type size() { return size_type( end() - begin()); }
    size_type capacity() { return size_type( end_of_storage - begin() ); }
    bool empty() { return begin() == end(); }
    reference operator[]( size_type n ) { return *(begin() + n); }
    reference front() { return *begin(); }
    reference back() { return *end(); }

    vector() { begin() = alloc.allocate( VECTOR_ORG ); end() = begin(); end_of_storage = VECTOR_ORG ; }
    vector( size_type n, const value_type &value ) { start = alloc.allocate( VECTOR_ORG );
                    finish = start + n; end_of_storage = start + VECTOR_ORG;  std::uninitialized_fill_n( begin(), n, value ); }
//    vector( const vector &orig ) { start = alloc.allocate( 2 * size_type( orig.end() - orig.begin() ));
//                    finish = start + ( orig.end() - orig.begin() ); end_of_storage = 2 * static_cast<size_type>( orig.end() - orig.begin());
//                            std::uninitialized_copy( orig.begin(), orig.end(), begin() ); }

    ~vector() {
        iterator it = start;
        while( it != finish ) {
            alloc.destroy( it );
            ++it;
        }
        alloc.deallocate( start, capacity() );
    }

    void jud_expand() {
        if( end() == end_of_storage )  {    //ÐèÀ©ÈÝ
            expand();
        }
    }

    void expand() {
        iterator new_start = alloc.allocate( 2 * capacity() );
        iterator new_finish = new_start + capacity();
        iterator new_end_of_storage = new_start + 2 * capacity();
        std::uninitialized_copy( begin(), end(), new_start );
        iterator it1 = end();
        while( it1 != begin() )
            alloc.destroy( --it1 );
        alloc.deallocate( begin(), capacity() );

        begin() = new_start;
        end() = new_finish;
        end_of_storage = new_end_of_storage;
    }
    void push_back( const value_type &x ) {
            jud_expand();
            alloc.construct( finish, x );
            ++finish;

    }

    void pop_back() {
        --finish;
        alloc.destroy( finish );
    }

    iterator erase( iterator it ) {
        iterator new_it = it;
        ++new_it;
        while( new_it != finish)
            *it++ = *new_it++;
        --finish;
        alloc.destroy( finish );
        return it;
    }


    iterator erase( iterator first, iterator last ) {
        iterator i = std::copy( last, finish, first);
        while( i != finish ) {
            alloc.destroy( i );
            ++i;
        }

        finish = finish - ( last - first );
        return first;
    }

    void clear() { erase( start, finish ); }

    iterator insert( iterator it, const value_type &x ) {
        if( it == finish ) { push_back( x ); return finish; }
        jud_expand();

        for( iterator it1 = finish; it1 != it; --it1)
            *it1 = *( it1 - 1 );
        ++finish;
        //alloc.destroy( it );
        *it = x;
        //alloc.construct( it, x );
        return it;
    }

    iterator insert( iterator it, size_type n, const value_type &x ) {
        if( n == 0 ) return it;
        if( n >= size_type( end_of_storage - finish )) expand();
        for( iterator it1 = finish - 1; it1 != it - 1; --it1)
            *( it1 + n ) = *( it1 );
        finish += n;
        iterator it2 = it;
        for( size_type i = 0; i < n; ++i ) {
            alloc.destroy( it2 );
            ++it2;
        }
        std::uninitialized_fill_n( it, n, x );

    }


};
