#pragma once

#include "./alias-map.h"

namespace smile{
namespace utils{

template<typename Key, typename Cmp = std::less<Key>, typename Scalar=uint64_t>
struct alias_set : protected alias_map<Key,Scalar,Cmp>{
    using parent=alias_map<Key,Scalar,Cmp>;
    private:
        Scalar next_id=1;
        constexpr static unsigned int MAX_ID=100000;
    public:
        using p_it0= typename parent::it0;
        using p_rit0= typename parent::rit0;
        using p_cit0= typename parent::cit0;
        using p_rcit0= typename parent::rcit0;

        struct iterator : public p_it0{
            friend class alias_set;
            private:
                iterator(const p_it0& i):p_it0(i){}
                using p_it0::operator*;
            public:
                const Key& operator*() const{return (p_it0::operator*().first);}
                inline Scalar alias() const{return p_it0::operator*().second;}
        };

        struct reverse_iterator : public p_rit0{
            friend class alias_set;
            private:
                reverse_iterator(const p_rit0& i):p_rit0(i){}
                using p_rit0::operator*;
            public:
                const Key& operator*() const{return (p_rit0::operator*().first);}
                inline Scalar alias() const{return p_rit0::operator*().second;}
        };

        struct const_iterator : public parent::cit0{
            friend class alias_set;
            private:
                const_iterator(const p_cit0& i):p_cit0(i){}
                using p_cit0::operator*;
            public:
                const_iterator(const iterator& i):p_cit0(i){}
                const Key& operator*() const{return (p_cit0::operator*().first);}
                inline Scalar alias() const{return p_cit0::operator*().second;}
        };

        struct const_reverse_iterator : public parent::rcit0{
            friend class alias_set;
            private:
                const_reverse_iterator(const p_rcit0& i):p_rcit0(i){}
                using p_rcit0::operator*;
            public:
                const_reverse_iterator(const iterator& i):p_rcit0(i){}
                const Key& operator*() const{return (p_rcit0::operator*().first);}
                inline Scalar alias() const{return p_rcit0::operator*().second;}
        };

        typedef iterator it0;
        typedef const_iterator cit0;
        typedef reverse_iterator rit0;
        typedef const_reverse_iterator rcit0;

        inline std::pair<iterator,bool> insert(const Key& val){
            assert(next_id<MAX_ID);
            auto [a,b] = parent::insert({val,next_id++});
            return {a,b};
        }

        inline it0  find (const Key& k){return parent::find(k);}
        inline cit0 find (const Key& k) const{return parent::find(k);}

        inline it0  erase (it0 p){return parent::erase(p);}
        inline typename parent::size_type   erase (const Key& k){return parent::erase(k);}

        //Well this is annoying for sure.
        inline it0 begin() noexcept{return parent::begin();}
        inline cit0 begin() const noexcept{return parent::begin();}
        inline cit0 cbegin() const noexcept{return parent::cbegin();}

        inline it0 end() noexcept{return parent::end();}
        inline cit0 end() const noexcept{return parent::end();}
        inline cit0 cend() const noexcept{return parent::cend();}

        inline rit0 rbegin() noexcept{return parent::rbegin();}
        inline rcit0 rbegin() const noexcept{return parent::rbegin();}
        inline rcit0 crbegin() const noexcept{return parent::crbegin();}

        inline rit0 rend() noexcept{return parent::rend();}
        inline rcit0 rend() const noexcept{return parent::rend();}
        inline rcit0 crend() const noexcept{return parent::crend();}

	using parent::size;

        using parent::inv;
	inline void clear(){parent::clear();next_id=0;}
	using parent::at;

};

}}
