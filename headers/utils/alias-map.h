#pragma once

#include <map>

namespace smile{
namespace utils{
    template<uint i, typename T>
    class derive:public T{};


    template<typename Key2, typename Key1, typename Cmp2 = std::less<Key2>, typename Alloc2 = std::allocator<std::pair<const Key2, const Key1>>, typename Cmp1 = std::less<Key1>, typename Alloc1 = std::allocator<std::pair<const Key1, const Key2> >>
    class alias_map_inv;

    template<typename Key1, typename Key2, typename Cmp1 = std::less<Key1>, typename Alloc1 = std::allocator<std::pair<const Key1, const Key2>>, typename Cmp2 = std::less<Key2>, typename Alloc2 = std::allocator<std::pair<const Key2, const Key1> >>
    class alias_map : private derive<0,std::map<Key1,const Key2,Cmp1,Alloc1>>, private derive<1,std::map<Key2,const Key1,Cmp2,Alloc2>>{
        friend class alias_map_inv<Key1,Key2,Cmp1,Alloc1,Cmp2,Alloc2>;
        using d0 = derive<0,std::map<Key1,const Key2,Cmp1,Alloc1>>;
        using d1 = derive<1,std::map<Key2,const Key1,Cmp2,Alloc2>>;

        protected:
            using it0=  typename d0::iterator;
            using it1=  typename d1::iterator;
            using cit0= typename d0::const_iterator;
            using cit1= typename d1::const_iterator;

            using rit0=  typename d0::reverse_iterator;
            using rit1=  typename d1::reverse_iterator;
            using rcit0= typename d0::const_reverse_iterator;
            using rcit1= typename d1::const_reverse_iterator;
            
        public:
            using size_type = typename d0::size_type;

            //class iterator:private derive<0,typename std::map<Key1,Key2,Cmp1,Alloc1>::iterator>, private derive<1,typename std::map<Key2,Key1,Cmp2,Alloc2>::iterator>;
            //class const_iterator:private derive<0,typename std::map<Key1,Key2,Cmp1,Alloc1>::const_iterator>, private derive<1,typename std::map<Key2,Key1,Cmp2,Alloc2>::const_iterator>;
            /*class iterator{
                friend class alias_map;
                
                it0 l;
                it1 r;

                inline iterator(const it0& al, const it1& ar):l(al),r(ar){}
                inline iterator(it0&& al, it1&& ar):l(al),r(ar){}

                public:
                    inline const it0& left()  const{return l;}
                    inline const it1& right() const{return r;}

                    inline friend bool operator==(const iterator& a, const iterator& b){return a.l==b.l && a.r==b.r;}
                    inline friend bool operator!=(const iterator& a, const iterator& b){return !operator==(a,b);}
            };*/


            inline void clear() noexcept{d0::clear();d1::clear();}
            inline size_type size() const noexcept{return d0::size();}
            inline size_type max_size() const noexcept{return d0::max_size();}
            inline bool empty() const noexcept{return d0::empty();}
            inline size_type count (const typename d0::key_type& k) const{return d0::count(k);}

            //Todo add implace.
            inline std::pair<it0,bool> insert(const typename d0::value_type& val){
                typename d1::value_type inv_val={val.second,val.first};
                auto t1=d1::insert(inv_val);
                auto t2=d0::insert(val);
                return {t2.first,t1.second==t2.second?t1.second:false};
            }

            inline std::pair<it1,bool> insert_inv(const typename d1::value_type& val){
                typename d0::value_type inv_val={val.second,val.first};
                auto t1=d0::insert(inv_val);
                auto t2=d1::insert(val);
                return {t2.first,t1.second==t2.second?t1.second:false};
            }

            
            inline it0  find (const Key1& k){return d0::find(k);}
            inline cit0 find (const Key1& k) const{return d0::find(k);}
            inline it1  find_inv (const Key2& k){return d1::find(k);}
            inline cit1 find_inv (const Key2& k) const{return d1::find(k);}
            
            inline it0         erase (it0 p){d1::erase(p->second);return d0::erase(p);}
            inline size_type   erase (const Key1& k){auto it=d0::find(k);if(it!=d0::end()){auto ret=d1::erase(it->second);d0::erase(it);return ret;}else return 0;}
            inline it1         erase_inv (it1 p){d0::erase(p->second);return d1::erase(p);}
            inline size_type   erase_inv (const Key2& k){auto it=d1::find(k);if(it!=d1::end()){auto ret=d0::erase(it->second);d1::erase(it);return ret;}else return 0;}

            //Well this is annoying for sure.
            inline it0 begin() noexcept{return d0::begin();}
            inline cit0 begin() const noexcept{return d0::begin();}
            inline cit0 cbegin() const noexcept{return d0::cbegin();}

            inline it0 end() noexcept{return d0::end();}
            inline cit0 end() const noexcept{return d0::end();}
            inline cit0 cend() const noexcept{return d0::cend();}

            inline it1 begin_inv() noexcept{return d1::begin();}
            inline cit1 begin_inv() const noexcept{return d1::begin();}
            inline cit1 cbegin_inv() const noexcept{return d1::cbegin();}

            inline it1 end_inv() noexcept{return d1::end();}
            inline cit1 end_inv() const noexcept{return d1::end();}
            inline cit1 cend_inv() const noexcept{return d1::cend();}

            inline rit0 rbegin() noexcept{return d0::rbegin();}
            inline rcit0 rbegin() const noexcept{return d0::rbegin();}
            inline rcit0 crbegin() const noexcept{return d0::crbegin();}

            inline rit0 rend() noexcept{return d0::rend();}
            inline rcit0 rend() const noexcept{return d0::rend();}
            inline rcit0 crend() const noexcept{return d0::crend();}

            inline rit1 rbegin_inv() noexcept{return d1::rbegin();}
            inline rcit1 rbegin_inv() const noexcept{return d1::rbegin();}
            inline rcit1 crbegin_inv() const noexcept{return d1::crbegin();}

            inline rit1 rend_inv() noexcept{return d1::rend();}
            inline rcit1 rend_inv() const noexcept{return d1::rend();}
            inline rcit1 crend_inv() const noexcept{return d1::crend();}

            inline const Key2& at (const Key1& k) const{return d0::at(k);}
            inline const Key1& at_inv (const Key2& k) const{return d1::at(k);}

            //REMOVED STUFF
            //const Key2& operator[] (const Key1& k);
            //mapped_type& operator[] (key_type&& k);
            //mapped_type& at (const key_type& k);
            //template <class P> pair<iterator,bool> insert (P&& val);
            //iterator insert (const_iterator position, const value_type& val);
            //template <class P> iterator insert (const_iterator position, P&& val);
            //iterator  erase (const_iterator first, const_iterator last);

            inline alias_map_inv<Key1,Key2,Cmp1,Alloc1,Cmp2,Alloc2> inv(){return alias_map_inv<Key1,Key2,Cmp1,Alloc1,Cmp2,Alloc2>(*this);}

    };

    template<typename Key2, typename Key1, typename Cmp2, typename Alloc2, typename Cmp1, typename Alloc1>
    class alias_map_inv{
        using base=alias_map<Key2,Key1,Cmp2,Alloc2,Cmp1,Alloc1>;
        using d0 = typename base::d1;
        using d1 = typename base::d0;

        using it0=  typename d0::iterator;
        using it1=  typename d1::iterator;
        using cit0= typename d0::const_iterator;
        using cit1= typename d1::const_iterator;

        using rit0=  typename d0::reverse_iterator;
        using rit1=  typename d1::reverse_iterator;
        using rcit0= typename d0::const_reverse_iterator;
        using rcit1= typename d1::const_reverse_iterator;

        base& ref;

        public:
            using size_type = typename d0::size_type;

            alias_map_inv(base& r):ref(r){}

            //Todo add implace.
            inline std::pair<it0,bool> insert(const typename d0::value_type& val){return ref.insert_inv(val);}
            inline std::pair<it1,bool> insert_inv(const typename d1::value_type& val){return ref.insert(val);}

            inline it0  find (const Key1& k){return ref.find_inv(k);}
            inline cit0 find (const Key1& k) const{return ref.find_inv(k);}
            inline it1  find_inv (const Key2& k){return ref.find(k);}
            inline cit1 find_inv (const Key2& k) const{return ref.find(k);}
            
            inline it0         erase (it0 p){return ref.erase_inv(p);}
            inline size_type   erase (const Key1& k){return ref.erase_inv(k);}
            inline it1         erase_inv (it1 p){return ref.erase(p);}
            inline size_type   erase_inv (const Key2& k){return ref.erase(k);}

            //Well this is annoying for sure.
            inline it0 begin() noexcept{return ref.begin_inv();}
            inline cit0 begin() const noexcept{return ref.begin_inv();}
            inline cit0 cbegin() const noexcept{return ref.cbegin_inv();}

            inline it0 end() noexcept{return ref.end_inv();}
            inline cit0 end() const noexcept{return ref.end_inv();}
            inline cit0 cend() const noexcept{return ref.cend_inv();}

            inline it1 begin_inv() noexcept{return ref.begin();}
            inline cit1 begin_inv() const noexcept{return ref.begin();}
            inline cit1 cbegin_inv() const noexcept{return ref.cbegin();}

            inline it1 end_inv() noexcept{return ref.end();}
            inline cit1 end_inv() const noexcept{return ref.end();}
            inline cit1 cend_inv() const noexcept{return ref.cend();}

            inline rit0 rbegin() noexcept{return ref.rbegin_inv();}
            inline rcit0 rbegin() const noexcept{return ref.rbegin_inv();}
            inline rcit0 crbegin() const noexcept{return ref.crbegin_inv();}

            inline rit0 rend() noexcept{return ref.rend_inv();}
            inline rcit0 rend() const noexcept{return ref.rend_inv();}
            inline rcit0 crend() const noexcept{return ref.crend_inv();}

            inline rit1 rbegin_inv() noexcept{return ref.rbegin();}
            inline rcit1 rbegin_inv() const noexcept{return ref.rbegin();}
            inline rcit1 crbegin_inv() const noexcept{return ref.crbegin();}

            inline rit1 rend_inv() noexcept{return ref.rend();}
            inline rcit1 rend_inv() const noexcept{return ref.rend();}
            inline rcit1 crend_inv() const noexcept{return ref.crend();}

            inline const Key2& at (const Key1& k) const{return ref.at_inv(k);}
            inline const Key1& at_inv (const Key2& k) const{return ref.at(k);}

            inline base inv(){return ref;}

    };

    //Used somewhere else in the library so it does make sense to keep it saved somewhere.

    template<typename Key1, typename Key2, typename Cmp1 = std::less<Key1>, typename Alloc1 = std::allocator<std::pair<const Key1, const Key2>>, typename Cmp2 = std::less<Key2>, typename Alloc2 = std::allocator<std::pair<const Key2, const Key1> >>
	inline const Key1& inv_at(alias_map<Key1,Key2,Cmp1,Alloc1,Cmp2,Alloc2>& src, const Key2& v){
		return src.at_inv(v);
	}

    template<typename Key1, typename Key2, typename Cmp1 = std::less<Key1>, typename Alloc1 = std::allocator<std::pair<const Key1, const Key2>>, typename Cmp2 = std::less<Key2>, typename Alloc2 = std::allocator<std::pair<const Key2, const Key1> >>
	inline const Key1& inv_at(alias_map_inv<Key1,Key2,Cmp1,Alloc1,Cmp2,Alloc2>& src, const Key2& v){
		return src.at_inv(v);
	}
}}
