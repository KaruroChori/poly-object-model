#include <iostream>
#include <fstream>
#include <poly-soups.h>
#include <utils/graph-rewrite.h>
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

        next_t*                  select_and_edit(){
            //if(edit_ka==true)return this;
        }

        //next_t*                         sample() const;
        next_t*                         add(){
            auto tmp=next_t::make_new();
            tmp->edit_ka=true;
            editable.insert(tmp);
            return tmp;
        }

        void                            rem(){
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

using namespace smile::utils;
using namespace std;
using namespace nlohmann;

int main(int argc, const char* argv[]){
    decompose<test_t,3> hello;
    hello.self.a;
    hello.next.self.b="ciao";
    get<0>(hello);
    get<1>(hello);
    get<2>(hello);
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
