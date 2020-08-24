#pragma once

#include <array>
#include <set>
#include <map>
#include <functional>

#include <utils/alias-set.h>
#include <utils/string-exception.h>
#include <utils/graph.h>

//using namespace std;
using namespace nlohmann;

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

namespace smile{
namespace utils{

template<typename T>
std::string to_string_enum(typename T::labels o){
    return T::strings[((int)o<T::n)?(int)o:0];
}

template<typename T>
typename T::labels from_string_enum(const std::string& str){
    for(uint i=0;i<T::n;i++){
        if(str==T::strings[i])return (typename T::labels)i;
    }
    return (typename T::labels)0;
}

template<typename T>
struct edit_node{
    enum class labels{
        NOP, NEW, ABSOLUTE, LOCAL, VAR
    }type;

    static bool is_true_node(labels op){return op==decltype(op)::ABSOLUTE or op==decltype(op)::LOCAL or op==decltype(op)::NEW;}

    union{
        void*               absolute;
        void*               new_node;
        items_t             local;
        T                   var;
    };

    constexpr inline static const char* strings[] = {
        "nop",
        "new",
        "abs","local",
        "var"
    };

    constexpr inline static uint n = 5;

    friend void from_json(const nlohmann::json& config, edit_node& g){
        {
            auto it=config.find("type");
            if(it==config.end() or !it->is_string())throw StringException("JSONSyntaxError");
            g.type=to_string_enum<edit_node>(*it);
        }
        {
            auto it=config.find("value");
            if(it==config.end() or !it->is_string())throw StringException("JSONSyntaxError");

                 if(g.type==labels::ABSOLUTE)g.absolute=(void*)(uint64_t)(*it);
            else if(g.type==labels::LOCAL)g.local=(*it);
            else if(g.type==labels::VAR)from_json(*it,g.var);
        }
    }

    friend void to_json(nlohmann::json& config, const edit_node& g){
        config["type"]=to_string_enum<edit_node>(g.type);
             if(g.type==labels::ABSOLUTE)config["value"]=(uint64_t)g.absolute;
        else if(g.type==labels::LOCAL)config["value"]=g.local;
        else if(g.type==labels::VAR)to_json(config["value"],g.var);
        else if(g.type==labels::NEW)config["value"]=nullptr;
    } 
};

template<typename T>
struct edit_arch{
    enum class labels{
        NOP,
        LOAD_ST,
        STORE_ST, ARCH_SET, ARCH_DEL,
        LOAD_CON,
        STORE_CON,
        FORGET, DELETE
    }type;

    std::optional<T>   value;

    constexpr inline static const char* strings[] = {
        "nop",
        "ld.s.cp",
        "sto.st","arch.set","arch.del",
        "ld.con",
        "sto.con",
        "forget","del"
    };

    constexpr inline static int level[] = {
        0,
        1,
        2,2,2,
        3,
        4,
        5,5
    };

    constexpr inline static uint n = 9;

    friend void from_json(const nlohmann::json& config, edit_arch& g){
        {
            auto it=config.find("type");
            if(it==config.end() or !it->is_string())throw StringException("JSONSyntaxError");
            g.type=to_string_enum<edit_arch>(*it);
        }
        {
            auto it=config.find("value");
            if(it!=config.end())from_json(*it,g.value);
        }       
    }

    friend void to_json(nlohmann::json& config, const edit_arch& g){
        config["type"]=to_string_enum<edit_arch>(g.type);
        if(g.value.has_value())to_json(config["value"],g.value.value());
    }
};


template<typename NODE, typename ARCH>
struct edit_graph:public graph<edit_node<NODE>,edit_arch<ARCH>>{
    typedef edit_arch<ARCH> earch;
    typedef edit_node<NODE> enode;

    using parent= graph<edit_node<NODE>,edit_arch<ARCH>>;
    using node = typename parent::node;
    using arch = typename parent::arch;

    constexpr inline static uint LAYERS_N = 6;

    ~edit_graph(){}

    void compile(){
        //if(!editable)throw StringException("Not editable graph");
        for(auto& i:pieces)i.clear();

        for(auto& i:this->nodes){
            for(auto& [j,z]:i->links){
                pieces[edit_arch<ARCH>::level[(uint64_t)z.value.type]].insert({i,j,&z});
            }
        }
        this->editable=false;
    }

    void reset(){
        this->nodes.clear();
        for(auto& i:pieces)i.clear();
        this->editable=true;
    }

    template<typename T>
    friend T& operator*=(T& target, std::pair<const edit_graph&,const std::function<void*(items_t)>&> eg){
        if(eg.first.editable)throw StringException("Graph must be compiled");

        //Generate the new nodes which are not there yet.
        for(auto i:eg.first.nodes){
            if(i->value.type==enode::labels::NEW){
                i->value.new_node=(void*)target.add_node_ptr();
            }
        }

        for(auto j:eg.first.pieces[1]){
            if(std::get<2>(j)->value.type==earch::labels::LOAD_ST){

            }
            else throw StringException("Graph state is broken.");
        }

        for(auto j:eg.first.pieces[2]){
            if(std::get<2>(j)->value.type==earch::labels::STORE_ST){

            }
            else if(std::get<2>(j)->value.type==earch::labels::ARCH_SET){
                target.set_arch(std::get<0>(j),std::get<1>(j),std::get<2>(j)->value.value);
            }
            else if(std::get<2>(j)->value.type==earch::labels::ARCH_DEL){
                target.rem_arch(std::get<0>(j),std::get<1>(j));
            }
            else throw StringException("Graph state is broken.");            
        }
        for(auto j:eg.first.pieces[3]){
            if(std::get<2>(j)->value.type==earch::labels::LOAD_CON){

            }
            else throw StringException("Graph state is broken.");
        }

        for(auto j:eg.first.pieces[4]){
            if(std::get<2>(j)->value.type==earch::labels::STORE_CON){

            }
            else throw StringException("Graph state is broken.");
        }

        for(auto j:eg.first.pieces[5]){
            if(std::get<2>(j)->value.type==earch::labels::FORGET){
                target.rem_node(std::get<0>(j));
            }
            else if(std::get<2>(j)->value.type==earch::labels::DELETE){
                target.forget(std::get<0>(j));
            }
            else throw StringException("Graph state is broken.");              
        }
    }

    template<typename T>
    inline friend T& operator*=(T& target, const edit_graph& eg){target*={eg,[&](items_t i)->void*{return target.resolve_node(i);}};}

    template<typename T>
    inline friend T operator*(const T& target, std::pair<const edit_graph&,const std::function<void*(items_t)>&> eg){auto ntarget=target;return ntarget*=eg;}

    template<typename T>
    inline friend T operator*(const T& target, const edit_graph& eg){auto ntarget=target;return ntarget*=eg;}
    
    protected:
        array<set<std::tuple<node*,node*,arch*>>,LAYERS_N> pieces;

        static bool is_legal(typename earch::labels mnemonic, typename enode::labels op){
            if((mnemonic == decltype(mnemonic)::FORGET || mnemonic == decltype(mnemonic)::DELETE) && enode::is_true_node(op))return true;
            return false;
        }
        static bool is_legal(typename earch::labels mnemonic, typename enode::labels op1, typename enode::labels op2){
            if((mnemonic == decltype(mnemonic)::LOAD_CON or mnemonic == decltype(mnemonic)::LOAD_ST) &&
                enode::is_true_node(op1) && op2==decltype(op2)::VAR)return true;
            else if((mnemonic == decltype(mnemonic)::STORE_CON or mnemonic == decltype(mnemonic)::STORE_ST) && op2==decltype(op1)::VAR && enode::is_true_node(op2))return true;
            else if((mnemonic == decltype(mnemonic)::ARCH_DEL or mnemonic == decltype(mnemonic)::ARCH_SET) && enode::is_true_node(op1) && enode::is_true_node(op2))return true;
            return false;
        }

        virtual void set_arch(void* n, void* m, const typename arch::value_t& v=typename arch::value_t()){
            node* nn=(node*)n;
            node* mm=(node*)m;

            if(nn==mm && !is_legal(v.type,nn->value.type)){
                throw StringException("Illegal graph instruction.");
            }
            else if(!is_legal(v.type,nn->value.type,mm->value.type)){
                throw StringException("Illegal graph instruction.");
            }

            parent::set_arch(n,m,v);
        }

};

}
}
