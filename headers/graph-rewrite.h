#pragma once

#include <array>
#include <set>
#include <map>
#include <functional>

#include <utils/alias-set.h>


/*
# SUPPORTED FUNCTIONS

## Stage 0
* Allocation
* Load status (copy)
* Load status (swap)

## Stage 1
* Store status
* Arch editing
* Arch deletion

## Stage 2
* Load connectivity (copy)
* Load connectivity (move)

## Stage 3
* Store connectivity

## Stage 4
* Forget
* Delete

*/

//template<typename NODE, typename ARCH>
typedef uint64_t NODE;
typedef uint64_t ARCH;

//This should be translated to concepts I guess.

template<typename T>
std::string to_string(typename T::labels o){
    return T::strings[((int)o<T::n)?(int)o:0];
}

template<typename T>
typename T::labels from_string(const std::string& str){
    for(uint i=0;i<T::n;i++){
        if(str==T::strings[i])return (typename T::labels)i;
    }
    return (typename T::labels)0;
}

struct edit_graph{
    typedef NODE node_i;
    typedef ARCH arch_i;

    typedef uint64_t items_t;

    template<typename A, uint B>
    using array=std::array<A,B>;

    template<typename A>
    using set=std::set<A>;

    template<typename A, typename B>
    using map=std::map<A,B>;

    template<typename A>
    using mapid=smile::utils::alias_set<A,std::less<A>,items_t>;

    struct OP_LABEL{
        typedef OP_LABEL self_type;

        enum class labels{
            NOP,
            LOAD_ST_CP, LOAD_ST_SWP,
            STORE_ST, ARCH_SET, ARCH_DEL,
            LOAD_CON_CP, LOAD_CON_MV,
            STORE_CON,
            FORGET, DELETE
        };

        constexpr inline static const char* strings[] = {
            "nop",
            "ld.s.cp","ld.s.swp",
            "sto.st","arch.set","arch.del",
            "ld.con.cp","ld.con.mv",
            "sto.con",
            "forget","del"
        };

        constexpr inline static uint n = 11;

        friend std::string to_string<self_type>(labels o);
        friend labels from_string<self_type>(const std::string& str);

        constexpr inline static int level[] = {
            0,
            1,1,
            2,2,2,
            3,3,
            4,
            5,5
        };
    };

    struct NODE_TYPE{
        typedef NODE_TYPE self_type;

        enum class labels{
            NOP, NEW, ABSOLUTE, LOCAL, VAR
        };

        constexpr inline static const char* strings[] = {
            "nop",
            "new",
            "abs","local",
            "var"
        };

        constexpr inline static uint n = 5;

        friend std::string to_string<self_type>(labels o);
        friend labels from_string<self_type>(const std::string& str);
    };

    static bool is_true_node(NODE_TYPE::labels op){return op==decltype(op)::ABSOLUTE or op==decltype(op)::LOCAL or op==decltype(op)::NEW;}

    static bool is_legal(OP_LABEL::labels mnemonic, NODE_TYPE::labels op){
        /*
            alloc NEW/VAR
            forget/delete NEW/ABSOLUTE/LOCAL
        */
        if((mnemonic == decltype(mnemonic)::FORGET || mnemonic == decltype(mnemonic)::DELETE) && is_true_node(op))return true;
        return false;
    }
    static bool is_legal(OP_LABEL::labels mnemonic, NODE_TYPE::labels op1, NODE_TYPE::labels op2){
        if((mnemonic == decltype(mnemonic)::LOAD_CON_CP or mnemonic == decltype(mnemonic)::LOAD_CON_MV or mnemonic == decltype(mnemonic)::LOAD_ST_CP or mnemonic == decltype(mnemonic)::LOAD_ST_SWP) &&
            is_true_node(op1) && op2==decltype(op2)::VAR)return true;
        else if((mnemonic == decltype(mnemonic)::STORE_CON or mnemonic == decltype(mnemonic)::STORE_ST) && op2==decltype(op1)::VAR && is_true_node(op2))return true;
        else if((mnemonic == decltype(mnemonic)::ARCH_DEL or mnemonic == decltype(mnemonic)::ARCH_SET) && is_true_node(op1) && is_true_node(op2))return true;
        return false;
    }

    constexpr inline static uint LAYERS_N = 6;

    private:
        struct node_base;
        struct arch_base;
        struct node;
        struct arch;

        struct node_base{
            NODE_TYPE               node_type;
            union{
                void*               absolute;
                void*               new_node;
                items_t             local;
                node_i              var;

            };
        };

        struct arch_base{
            OP_LABEL                arch_type;
            std::optional<arch_i>   value;
        };

        struct node{
            typedef node_base value_t;
            array<map<node*,arch>,LAYERS_N>     links;
            array<set<node*>,LAYERS_N>          inv_links;

            value_t                             value;
        };

        struct arch{
            typedef arch_base value_t;
            value_t                             value;
        };

        mapid<node*> nodes;

        void*                    add_node(const typename node::value_t& v);
        void                     rem_node(void* n);
        void                     set_node(void* n, const typename node::value_t& v);
        typename node::value_t&  get_node(void* n);
        void                     set_arch(void* n, void* m, const typename arch::value_t& v=typename arch::value_t());
        void                     rem_arch(void* n, void* m);
        void                     cpy_conn(void* src, void* dst);
        void                     mv_conn(void* src, void* dst);
        void                     forget(void* n);

    public:

        ~edit_graph(){
            for(auto& i:nodes){
                delete i;
            }
        }

        template<typename T>
        friend T& operator*=(T& target, const edit_graph& eg);

        template<typename T>
        friend T& operator*=(T& target, std::pair<const edit_graph&,const std::function<void*(items_t)>&> eg);

        template<typename T>
        inline friend T operator*(const T& target, const edit_graph& eg);

        template<typename T>
        inline friend T operator*(const T& target, std::pair<const edit_graph&,const std::function<void*(items_t)>&> eg);

        
};