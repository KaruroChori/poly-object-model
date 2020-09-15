#pragma once
#include <map>
#include <set>
#include <iostream>

namespace smile{
namespace utils{

template <typename T>
struct eq_pool{
    eq_pool(const eq_pool& base){
        class_id=base.class_id;

        auto& pref=pool.at(class_id);
        auto& uref=in_use.at(class_id);
        
        //LOAD ME! Check if there is any object in the pool to be reused.
        if(pref.items.size()!=0){
            auto it=pref.items.begin();
            data=*it;
            pref.items.erase(it);
        }
        else{
            //std::cout<<"こんばんは\n";
            data=new T(*(base.data));
        }
        uref.insert(this);
    }
    eq_pool(const T& base, items_t max=3){
        next_id++;
        class_id=next_id;
        pool[class_id]={{},max}; //Just to reserve the spot.
        in_use[class_id].insert(this);

        data=new T(base);
    }
    ~eq_pool(){
        auto& dst=pool.at(class_id);
        auto& src=in_use.at(class_id);
        
        src.erase(this);
        //SAVE ME.
        if(src.size()!=0 && dst.items.size()+1<=dst.max_copies){
            dst.items.insert(data);
        }
        //ONLY MONICA!
        else if(src.size()!=0){
            delete data;
        }
        //Not even a copy, I cannot recover the class. Trash everything.
        else{
            _clear();
            pool.erase(class_id);
            in_use.erase(class_id);
            //Take back the id in case this was just a quick temporary class.
            if(class_id==next_id)next_id--;
            delete data;
        }

        class_id=0;
        data=nullptr;
    }

    template<bool SAFE>
    eq_pool& merge(eq_pool& dst){
        if constexpr(SAFE){
            if(*dst.data!=*data)return *this;
        }
        in_use.at(class_id).erase(this);
        //Cleanup if I was the last object fully represented in the class.
        if(in_use.size()==0){
            _clear();
            pool.erase(class_id);
            in_use.erase(class_id);
        }

        class_id=dst.class_id;
        in_use.at(class_id).insert(this);
        return dst;
    }

    eq_pool& split(){
        if(in_use.at(class_id).size()==1){}
        else{
            in_use.erase(class_id);

            auto max=pool.at(class_id).max_copies;
            next_id++;
            class_id=next_id;
            pool[class_id]={{},max}; //Just to reserve the spot.
            in_use[class_id].insert(this);
        }
        return *this;
    }

    inline T& operator *(){split();return *data;}
    inline const T& operator *() const{return *data;}
    inline operator T&(){split();return *data;}
    inline operator const T&() const{return *data;}

    void _debug(){
        std::cout<<"Not yet implemented.\n";
    }

    void _max_copies(items_t t){
        auto& dst=pool.at(class_id);
        if(dst.max_copies<t)
            for(items_t i=dst.max_copies;i<t;i++)dst.items.erase(dst.items.begin());
        dst.max_copies=t;
    }
    void _clear(){
        auto& dst=pool.at(class_id);
        for(auto i:dst.items)delete i;
        dst.items.erase(dst.items.begin(),dst.items.end());
    }
    void _clear_all(){
        for(auto& [i,j]:pool)for(auto& [a,b]:j){
            for(auto w:b.items)delete w;
            b.items.erase(b.items.begin(),b.items.end());
        }
    }
    
    private:
        uint64_t class_id=0;
        T*       data;

        struct pool_item{
            std::set<T*> items;
            items_t max_copies;
        };

        typedef std::set<eq_pool*> in_use_item;

        static inline std::map<uint64_t,pool_item> pool;
        static inline std::map<uint64_t,in_use_item> in_use;
        static inline uint64_t next_id=0;
};

}}