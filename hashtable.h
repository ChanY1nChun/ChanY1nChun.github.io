#ifndef _HASHTABLE_H
#define _HASHTABLE_H

#include <vector>
#include <memory>
#include <cstddef>
#include <algorithm>
#include <utility>


using std::vector;
using std::size_t;
using std::pair;


template <class Key> struct hash{ };

inline size_t hash_string( const char *s ) {
    unsigned long h = 0;
    for( ; *s; ++s )
        h = 5 * h + *s;
    return size_t( h );
}

template<> struct hash<char*> {
    size_t operator() ( const char *s ) const { return hash_string(s); }
};
template<> struct hash<const char*> {
    size_t operator() ( const char *s ) const { return hash_string(s); }
};
template<> struct hash<char> {
    size_t operator() ( char x ) const { return x; }
};
template<> struct hash<unsigned char> {
    size_t operator() ( unsigned char x ) const { return x; }
};
template<> struct hash<signed char> {
    size_t operator() ( signed char x ) const { return x; }
};
template<> struct hash<short> {
    size_t operator() ( short x ) const { return x; }
};
template<> struct hash<unsigned short> {
    size_t operator() ( unsigned short x ) const { return x; }
};
template<> struct hash<int> {
    size_t operator() ( int x ) const { return x; }
};
template<> struct hash<unsigned int> {
    size_t operator() ( unsigned int x ) const { return x; }
};
template<> struct hash<long> {
    size_t operator() ( long x ) const { return x; }
};
template<> struct hash<unsigned long> {
    size_t operator() ( unsigned long x ) const { return x; }
};

template<typename T> struct identity {
    const T& operator() ( const T &x ) const { return x; }
};

template<typename T> struct equal_to {
    bool operator() ( const T &x, const T &y ) const { return x == y; }
};

template <typename Value>
struct _hashtable_node {
    _hashtable_node *next;
    Value val;

    _hashtable_node( const Value &x ) : val( x ) { }
    _hashtable_node() = default;
};

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey,class Alloc = std::allocator<_hashtable_node<Value>>>
struct _hashtable_iterator;
//template<class Value, class Key, class HashFcn,
//          class ExtractKey, class EqualKey, class Alloc = std::allocator<_hashtable_node<Value>>>
//class hashtable;



template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey, class Alloc = std::allocator<_hashtable_node<Value>>>
class hashtable {
public:
    typedef HashFcn hasher;
    typedef EqualKey key_equal;
    typedef std::size_t size_type;
    typedef Value value_type;
    typedef Key key_type;
    typedef hashtable<Value, Key, HashFcn,
          ExtractKey, EqualKey, Alloc> self;
    typedef _hashtable_iterator<Value, Key, HashFcn,
          ExtractKey, EqualKey, Alloc> iterator;
    typedef _hashtable_node<Value> node;


    size_type bucket_count() const { return buckets.size(); }
    size_type size() const { return num_elements; }
    hashtable( size_type n, const hasher &hf, const key_equal &eql,
                const ExtractKey &exk)
        : hash( hf ), equals( eql ), get_key( exk ), num_elements( 0 )
        { initialize_buckets( n ); }

    pair<iterator, bool> insert_unique( const value_type &val ) {
        resize(num_elements + 1 );
        return insert_unique_noresize( val );
    }

    iterator insert_equal( const value_type &val ) {
        resize( num_elements + 1 );
        return insert_equal_noresize( val );
    }

    iterator find (const key_type &key ) {
        size_type n = bkt_num_key( key );
        node *first;
        for( first = buckets[n]; first != nullptr && !equals( get_key( first->val ), key ); first = first->next )
        { }

        return iterator ( first, this );
    }

    size_type count ( const key_type &key ) {
        size_type n = bkt_num_key( key );
        size_type result = 0;
        for( node *cur = buckets[n]; cur; cur = cur->next ) {
            if( equals( get_key( cur->val ), key ) )
                ++result;
        }
        return result;
    }

    void clear() {
        for( size_type i = 0; i != buckets.size(); ++i ) {
            node *cur = buckets[i];
            while( cur ) {
                node *next = cur->next;
                delete_node( cur );
                buckets[i] = next;
            }
            buckets[i] = nullptr;
        }
        num_elements = 0;
    }

    void copy_from( const self &org ) {
        clear();
        buckets.reserve( org.buckets.size() );
        buckets.insert( buckets.end(), buckets.size(), nullptr );

        for( size_type i = 0; i != org.buckets.size(); ++i ) {
            if( const node *cur = org.buckets[i]) {
                node *copy = new_node( cur->val );
                buckets[i] = copy;


            for( node *next = cur->next; next; cur = next, next = cur->next ) {
                copy->next = new_node( next->val );
                copy = copy->next;
            }
          }
        }
        num_elements = org.num_elements;
    }
    size_type max_bucket_count() const { return _stl_prime_list[27]; }

    iterator begin() {
        size_type bucket = 0;
        node *cur = buckets[bucket];
        while( cur == nullptr ) cur = buckets[++bucket];
        return iterator( cur, this );
    }

    node* get_vec( const value_type &n ) {
        node *tmp = buckets[n];
        return tmp;
    }


    size_type next_size( size_type n ) { return _stl_next_prime( n ); }

    size_type bkt_num( const value_type &val, size_type n ) const {
        return bkt_num_key( get_key( val ), n );
    }

    size_type bkt_num( const value_type &val ) const {
        return bkt_num_key( get_key( val ) );
    }

    size_type bkt_num_key( const key_type &key ) const {
        return bkt_num_key( key, buckets.size() );
    }

    size_type bkt_num_key( const key_type &key, size_type n ) const {
        return hash( key ) % n;
    }

private:
    hasher hash;
    key_equal equals;
    ExtractKey get_key;
    Alloc alloc;



    std::vector<node*> buckets;
    size_type num_elements;

    //const int _stl_num_primes = 28;
    size_type _stl_prime_list[28] =
    {
        53,97,193,389,769,
        1543,3079,6151,12289,24593,
        49157,98317,196613,393241,786433,
        1572869,3145739,6291469,12582917,25165843,
        50331653,100663319,201326611,402653189,805306457,
        1610612741,3221225473ul,4294967291ul
    };



    size_type _stl_next_prime( unsigned long n ) {
        const size_type *first = _stl_prime_list;
        const size_type *last = _stl_prime_list + 28;
        const size_type *pos = std::lower_bound( first, last, n);

        return pos == last ? *( last - 1 ) : *pos;
    }

    node* new_node( const value_type &val ) {
        node *n = alloc.allocate( 1 );
        n->next = nullptr;
        alloc.construct( n, val );
        return n;
    }

    void delete_node( node *n ) {
        alloc.destroy( n );
        alloc.deallocate( n, 1 );
    }



    void initialize_buckets( size_type n ) {
        const size_type n_buckets = next_size( n );
        buckets.reserve( n_buckets );
        buckets.insert( buckets.end(), n_buckets, nullptr );
        num_elements = 0;
    }



    void resize( size_type num_elements_hint ) {
        const size_type old_n = buckets.size();
        if( num_elements_hint > old_n ) {
            const size_type n = next_size( num_elements_hint );
            if( n > old_n ) {
                vector<node*> tmp( n, nullptr );
                for( size_type bucket = 0; bucket != old_n; ++bucket ) {
                    node *first = buckets[bucket];
                    while( first ) {
                        size_type new_bucket = bkt_num( first->val, n );
                        buckets[bucket] = first->next;
                        first->next = tmp[new_bucket];
                        tmp[new_bucket] = first;
                        first = buckets[bucket];
                    }
                }
                buckets.swap( tmp );
            }
        }
    }



    pair<iterator, bool> insert_unique_noresize( const value_type &val ) {
        const size_type n = bkt_num( val );
        node *first = buckets[n];

        for( node *cur = first; cur; cur = cur->next )
            if( equals( get_key( cur->val ), get_key( val ) ) )
                return { iterator( cur, this ), false };

        node *tmp = new_node( val );
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return { iterator( tmp, this ), true };
    }


    iterator insert_equal_noresize( const value_type &val ) {
        const size_type n = bkt_num( val );
        node *first = buckets[n];

        for( node *cur = first; cur; cur = cur->next )
        if( equals( get_key( cur->val ), get_key( val ) ) ) {
            node *tmp = new_node( val );
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elements;
            return iterator( tmp, this );
        }

        node *tmp = new_node( val );
        tmp->next = first;
        buckets[n] = tmp;
        ++num_elements;
        return iterator( tmp, this );
    }


};

template <class Value, class Key, class HashFcn,
          class ExtractKey, class EqualKey,class Alloc>
struct _hashtable_iterator {
    //typedef hashtable<Value, Key, HashFcn,
     //     ExtractKey, EqualKey, std::allocator<_hashtable_node<Value>>> hashtable;

    typedef _hashtable_iterator<Value, Key, HashFcn,
          ExtractKey, EqualKey, Alloc> iterator;

    typedef _hashtable_node<Value> node;

    typedef std::forward_iterator_tag iterator_category;
    typedef Value value_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::size_t size_type;
    typedef Value& reference;
    typedef Value* pointer;

    node *cur;
    hashtable<Value, Key, HashFcn,
          ExtractKey, EqualKey, std::allocator<_hashtable_node<Value>>>  *ht;

    _hashtable_iterator(node *n, hashtable<Value, Key, HashFcn,
          ExtractKey, EqualKey, std::allocator<_hashtable_node<Value>>> *tab) : cur( n ) , ht( tab ) { }
    _hashtable_iterator() = default;

    reference operator*() const { return cur->val; }
    pointer operator->() const { return &(operator*()); }

    bool operator!=( const iterator &it ) const { return cur != it.cur; }
    bool operator==( const iterator &it ) const { return cur == it.cur; }

    iterator& operator++() {
        const node* old = cur;
        cur = cur->next;
        if( !cur ) {
            size_type i = ht->bkt_num( old->val );
            while( !cur && ++i < ht->bucket_count() )
                cur = ht->get_vec( i );
        }
        return *this;
    }

    iterator operator++(int) {
        iterator tmp = *this;
        ++*this;
        return tmp;
    }


};

#endif // _HASHTABLE_H
