#include <iostream>
#include <fstream>
#include <poly-soups.h>
#include <utils/graph-rewrite.h>
#include <utils/eq-pool.h>
#include <regex>

typedef uint64_t hash_t;
typedef uint64_t items_t;

struct test_t{
    /*struct naked_t{
        uint& a;
        std::string& b;
        uint& c;
    };*/

    struct self_t{
        uint a;
        int cmp(const test_t& a, const test_t& b);
        hash_t hash(const test_t& a);
        static inline constexpr bool support_hash=true;
    }self;
    struct{
        struct self_t{
            std::string b;
            int cmp(const test_t& a, const test_t& b);
            hash_t hash(const test_t& a);
            static inline constexpr bool support_hash=false;
        }self;
        struct{
            struct self_t{
                uint c;
                int cmp(const test_t& a, const test_t& b);
                hash_t hash(const test_t& a);
                static inline constexpr bool support_hash=true;
            }self;
        }next;
    }next;
};

template<bool LINEAR_SEARCH, typename T, uint N, typename PARENT=void>
struct eqv_decompose{
    typedef eqv_decompose<T::next::support_hash,decltype(T::next),N-1,eqv_decompose> next_t;
    decltype(T::self)                   common;
    mutable std::optional<items_t>      _count;
    PARENT*                             parent=nullptr;

    std::map<std::optional<hash_t>,std::set<next_t*>>  children;
    std::set<next_t*>                   editable;

    bool                                unique_ka;
    bool                                edit_ka;

    void    invalidate_count() const{if constexpr(is_root())parent->invalidate_count();_count={};}
    void    delta_count(int t) const{if constexpr(is_root())parent->delta_count(t);_count.value()+=t;}

    public:
        static bool constexpr           is_root(){return std::is_same<PARENT,void>::value;}
        static bool constexpr           has_hash(){return !LINEAR_SEARCH;}
        static bool constexpr           is_leaf(){return false;}

        std::optional<hash_t>           hash() const;

        items_t                         count() const{
            if(_count.has_value())return _count.value();
            else{
                items_t tmp=0;
                for(auto& [i,j]:children){
                    for(auto z:j){
                        tmp+=z->count();
                    }
                }
                _count={tmp};
                return tmp;
            }
        }

        void uniqueness(bool u){
            //The same as now: return
            if(u==unique_ka)return;
            if(unique_ka){
                //unique -> not unique
            }
            else{
                //not unique -> unique
            }
        }

        next_t*                  select_and_edit(){
            //if(edit_ka==true)return this;
        }

        //next_t*                         sample() const;
        next_t*                         add(){
            auto tmp=next_t::make_new(this);
            tmp->edit_ka=true;
            editable.insert(tmp);
            return tmp;
        }

        void                            rem(){
        }

        inline void commit_recursive(){
            for(auto& [i,j]:children)for(auto z:j)z->commit_recursive();
            for(auto z:editable)z->commit();
        }

        void                            commit(){
            if(!edit_ka)return;
            //I cannot commit a root.
            if constexpr(is_root())return;
            edit_ka=false;
            parent->editable.erase(this);
            parent->delta_count(count());
            if constexpr(PARENT::has_hash()){
                parent->children.insert({hash(),this});
            }
            else{
                parent->children.insert({{},this});
            }
        }

        void for_each(const std::function<void(typename T::naked_t,items_t)>& fn){
            if(edit_ka)throw "EditableContainer";
            for(auto& [i,j]:children){
                for(auto z:i){
                    z->for_each(fn);
                }
            }
        }

        void for_each_restrict(const std::function<void(typename T::naked_t,items_t)>& fn, uint k=0){
            if(edit_ka)throw "EditableContainer";
            if(k==0){
                //I should not split anymore :)
                for(auto& [i,j]:children){
                    for(auto z:i){
                        fn(/**/ z->count());
                    }
                }  
            }
            else{
                for(auto& [i,j]:children){
                    for(auto z:i){
                        z->for_each(fn,k-1);
                    }
                } 
            }          
        }

        ~eqv_decompose(){
            for(auto& [i,j]:children){
                for(auto z:j){
                    delete z;
                }
            }
            for(auto z:editable){
                delete z;
            }
        }
};

template<bool LINEAR_SEARCH, typename T, typename PARENT>
struct eqv_decompose<LINEAR_SEARCH,T,0,PARENT>{
    decltype(T::self)                   common;
    mutable std::optional<items_t>      _count;
    PARENT*                             parent;

    void    invalidate_count() const{if constexpr(is_root())parent->invalidate_count();_count={};}
    void    delta_count(int t) const{if constexpr(is_root())parent->delta_count(t);_count.value()+=t;}
    
    public:
        static bool constexpr           is_root(){return std::is_same<PARENT,void>::value;}
        static bool constexpr           has_hash(){return !LINEAR_SEARCH;}
        static bool constexpr           is_leaf(){return true;}

        items_t                         count() const;
        std::optional<hash_t>           hash() const;

        void for_each(const std::function<void(typename T::naked_t,items_t)>& fn){
            fn(/**/ count());
        }

        void for_each_restrict(const std::function<void(typename T::naked_t,items_t)>& fn, uint k=0){
            fn(/**/ count());
        }
};

//////////////////////////////////////////////////////////////////////////////////

template<typename T, uint N>
struct decompose{
    decltype(T::self) self;
    decompose<decltype(T::next),N-1> next;

    template<uint M>
    struct _get;

    template<uint M>
    struct _get_const;

    template<uint M>
    inline static _get<M> get;

    template<uint M>
    inline static _get_const<M> get_const;

    template<uint M>
    struct _get{auto& operator()(decompose& base){return decltype(base.next)::template get<M-1>(base.next);}};

    template<>
    struct _get<0>{auto& operator()(decompose& base){return base.self;}};

    template<uint M>
    struct _get_const{auto& operator()(const decompose& base){return decltype(base.next)::template get_const<M-1>(base.next);}};

    template<>
    struct _get_const<0>{auto& operator()(const decompose& base){return base.self;}};
};

template<typename T>
struct decompose<T,1>{
    decltype(T::self) self;

    template<uint M>
    struct _get;

    template<uint M>
    struct _get_const;

    template<uint M>
    inline static _get<M> get;

    template<uint M>
    inline static _get_const<M> get_const;

    template<uint M>
    struct _get{auto& operator()(decompose& base){throw "";}};

    template<>
    struct _get<0>{auto& operator()(decompose& base){return base.self;}};

    template<uint M>
    struct _get_const{auto& operator()(const decompose& base){throw "";}};

    template<>
    struct _get_const<0>{auto& operator()(const decompose& base){return base.self;}};
};

template<uint N,typename T>
auto& get(T& base){return T::template get<N>(base);}

template<uint N,typename T>
const auto& get(const T& base){return T::template get_const<N>(base);}


///////////////
// AUTOCLASS //
///////////////

using namespace smile::utils;
using namespace std;
using namespace nlohmann;

int main(int argc, const char* argv[]){
    {
    eq_pool<int> c1(1), c2(2), c3(4), c4(5);
    for(uint i=0;i<100;i++)eq_pool<int> b(c1),c(c2),d(c3);
    std::cout<<c1+5;
    }
    /*decompose<test_t,3> hello;
    hello.self.a;
    hello.next.self.b="ciao";
    get<0>(hello);
    get<1>(hello);
    get<2>(hello);*/
    return 0;
    
    std::map<string,graph<string,string>> graphs;
    for(;;){
        string buffer;
        getline(cin,buffer,'\n');

        to_edit_graph(graphs["banana"]);
        if(buffer=="exit")break;
        cout<<buffer;
    }


    try{
    }
    catch(std::exception& e){
        std::cout<<e.what();
    }
    return 0;
}
