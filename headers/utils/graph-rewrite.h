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

namespace std{
    void from_json(const nlohmann::json& src, nlohmann::json& dst){dst=src;}
    void to_json(nlohmann::json& src, const nlohmann::json& dst){src=dst;}
}

namespace smile{
namespace utils{

template<typename A, typename B>
edit_graph<A,B> to_edit_graph(const graph<A,B>& r);

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
        NOP, NEW, ABSOLUTE, LOCAL, VAR, VAR_CON
    }type;

    static bool is_true_node(labels op){return op==decltype(op)::ABSOLUTE or op==decltype(op)::LOCAL or op==decltype(op)::NEW;}

    union{
        void*               absolute;
        void*               new_node;
        items_t             local;
        T*                  var;
        void*               var_con;
    };

    constexpr inline static const char* strings[] = {
        "nop",
        "new",
        "abs","local",
        "var",
        "var_con"
    };

    constexpr inline static uint n = 6;

    friend void from_json(const nlohmann::json& config, edit_node& g){
        {
            auto it=config.find("type");
            if(it==config.end() or !it->is_string())throw StringException("JSONSyntaxError: no type for edit node.");
            g.type=from_string_enum<edit_node>(*it);
        }
        if(g.type!=labels::NEW){
            auto it=config.find("value");
            if(it==config.end())throw StringException("JSONSyntaxError: no value for edit node.");

                 if(g.type==labels::ABSOLUTE)g.absolute=(void*)(uint64_t)(*it);
            else if(g.type==labels::LOCAL)g.local=(*it);
            else if(g.type==labels::VAR){g.var=new T();from_json(*it,*(g.var));}
            else if(g.type==labels::ABSOLUTE)g.var_con=nullptr;

        }
    }

    friend void to_json(nlohmann::json& config, const edit_node& g){
        config["type"]=to_string_enum<edit_node>(g.type);
             if(g.type==labels::ABSOLUTE)config["value"]=(uint64_t)g.absolute;
        else if(g.type==labels::LOCAL)config["value"]=g.local;
        else if(g.type==labels::VAR)to_json(config["value"],*(g.var));
        else if(g.type==labels::NEW)config["value"]=nullptr;
        else if(g.type==labels::VAR_CON)config["value"]=nullptr;
    } 

    ~edit_node(){if(type==labels::VAR)delete var;}
    edit_node(){type=labels::NOP;}
    edit_node(const edit_node& cp){type=cp.type;if(type==labels::VAR)var=new T(*(cp.var));else absolute=cp.absolute;}
    edit_node& operator=(const edit_node& cp){type=cp.type;if(type==labels::VAR)var=new T(*(cp.var));else absolute=cp.absolute;return *this;}
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
        "ld.st",
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
            g.type=from_string_enum<edit_arch>(*it);
        }
        {
            T value;
            auto it=config.find("value");
            if(it!=config.end())from_json(*it,value);
            g.value=value;
        }       
    }

    friend void to_json(nlohmann::json& config, const edit_arch& g){
        config["type"]=to_string_enum<edit_arch>(g.type);
        if(g.value.has_value())to_json(config["value"],g.value.value());
    }

    ~edit_arch(){}
    edit_arch():value(){type=labels::NOP;}
    edit_arch(const edit_arch& cp){type=cp.type;value=cp.value;}
    edit_arch& operator=(const edit_arch& cp){type=cp.type;value=cp.value;return *this;}
};


template<typename NODE, typename ARCH>
struct edit_graph:public graph<edit_node<NODE>,edit_arch<ARCH>>{
    typedef edit_arch<ARCH> earch;
    typedef edit_node<NODE> enode;

    using parent= graph<edit_node<NODE>,edit_arch<ARCH>>;
    using node = typename parent::node;
    using arch = typename parent::arch;

    friend edit_graph to_edit_graph<NODE,ARCH>(const graph<NODE,ARCH>& r);

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
    inline T& apply_on(T& target, const std::function<void*(items_t)>& pi) const{
        auto& eg=*this;

        set<typename T::node*> tmp_nodes;

        if(eg.editable)throw StringException("Graph must be compiled");

        auto w_node = [&](node* ptr)->typename T::node*{
            if(ptr->value.type==enode::labels::ABSOLUTE)return (typename T::node*)ptr->value.absolute;
            else if(ptr->value.type==enode::labels::NEW)return (typename T::node*)ptr->value.new_node;
            else if(ptr->value.type==enode::labels::LOCAL)return (typename T::node*)target.resolve_node(ptr->value.local);
            else{
                throw StringException("Wrong addressing method");
            }
        };

        //Generate the new nodes which are not there yet.
        for(auto& i:eg.nodes){
            if(i->value.type==enode::labels::NEW){
                i->value.new_node=(void*)target.add_node({});
            }
        }

        for(auto& j:eg.pieces[1]){
            if(std::get<2>(j)->value.type==earch::labels::LOAD_ST){
                *(std::get<1>(j)->value.var)=std::max(w_node(std::get<0>(j))->value,*(std::get<1>(j)->value.var));
            }
            else throw StringException("Graph state is broken.");
        }

        for(auto& j:eg.pieces[2]){
            if(std::get<2>(j)->value.type==earch::labels::STORE_ST){
                w_node(std::get<1>(j))->value=*(std::get<0>(j)->value.var);
            }
            else if(std::get<2>(j)->value.type==earch::labels::ARCH_SET){
                if(std::get<2>(j)->value.value.has_value())target.set_arch(w_node(std::get<0>(j)),w_node(std::get<1>(j)),std::get<2>(j)->value.value.value());
                else target.set_arch(w_node(std::get<0>(j)),w_node(std::get<1>(j)),{});
            }
            else if(std::get<2>(j)->value.type==earch::labels::ARCH_DEL){
                target.rem_arch(w_node(std::get<0>(j)),w_node(std::get<1>(j)));
            }
            else throw StringException("Graph state is broken.");            
        }
        for(auto& j:eg.pieces[3]){
            if(std::get<2>(j)->value.type==earch::labels::LOAD_CON){
                auto* t=target.tmp_node({});
                std::get<1>(j)->value.var_con=t;
                tmp_nodes.insert((typename T::node*)t);
                //Copy connectivity here.
                target.cpy_conn(w_node(std::get<0>(j)),t);     
            }
            else throw StringException("Graph state is broken.");
        }

        for(auto& j:eg.pieces[4]){
            if(std::get<2>(j)->value.type==earch::labels::STORE_CON){
                //Copy connectivity from here.
                target.cpy_conn(std::get<0>(j)->value.var_con,w_node(std::get<1>(j)));
            }
            else{
                for(auto& i:tmp_nodes)delete i;
                throw StringException("Graph state is broken.");
            }
        }

        //Clean up temporary nodes :)
        for(auto& i:tmp_nodes)delete i;

        for(auto& j:eg.pieces[5]){
            if(std::get<2>(j)->value.type==earch::labels::FORGET){
                target.rem_node(w_node(std::get<0>(j)));
            }
            else if(std::get<2>(j)->value.type==earch::labels::DELETE){
                target.forget(w_node(std::get<0>(j)));
            }
            else throw StringException("Graph state is broken.");              
        }

        return target;
    }


    template<typename T>
    inline T& apply_on(T& target) const{return apply_on(target,[&](items_t i)->void*{return target.resolve_node(i);});}

    /*
    template<typename T>
    friend T& operator*=(T& target, std::pair<const edit_graph&,const std::function<void*(items_t)>&> eg){return target;}


    template<typename T>
    inline friend T& operator*=(T& target, const edit_graph& eg){target*={eg,[&](items_t i)->void*{return target.resolve_node(i);}};}

    template<typename T>
    inline friend T operator*(const T& target, std::pair<const edit_graph&,const std::function<void*(items_t)>&> eg){auto ntarget=target;return ntarget*=eg;}

    template<typename T>
    inline friend T operator*(const T& target, const edit_graph& eg){auto ntarget=target;return ntarget*=eg;}
    */
    protected:
        array<set<std::tuple<node*,node*,arch*>>,LAYERS_N> pieces;

        static bool is_legal(typename earch::labels mnemonic, typename enode::labels op){
            if((mnemonic == decltype(mnemonic)::FORGET || mnemonic == decltype(mnemonic)::DELETE) && enode::is_true_node(op))return true;
            return false;
        }
        static bool is_legal(typename earch::labels mnemonic, typename enode::labels op1, typename enode::labels op2){
            if(mnemonic == decltype(mnemonic)::LOAD_CON  &&  enode::is_true_node(op1) && op2==decltype(op2)::VAR_CON)return true;
            else if(mnemonic == decltype(mnemonic)::LOAD_ST  &&  enode::is_true_node(op1) && op2==decltype(op2)::VAR)return true;
            else if(mnemonic == decltype(mnemonic)::STORE_CON && op1==decltype(op1)::VAR_CON && enode::is_true_node(op2))return true;
            else if(mnemonic == decltype(mnemonic)::STORE_ST && op1==decltype(op1)::VAR && enode::is_true_node(op2))return true;
            else if((mnemonic == decltype(mnemonic)::ARCH_DEL or mnemonic == decltype(mnemonic)::ARCH_SET) && enode::is_true_node(op1) && enode::is_true_node(op2))return true;
            return false;
        }

        virtual void set_arch(void* n, void* m, const typename arch::value_t& v=typename arch::value_t()){
            node* nn=(node*)n;
            node* mm=(node*)m;

            if(!is_legal(v.type,nn->value.type,mm->value.type) && !(nn==mm && is_legal(v.type,nn->value.type))){
                throw StringException("Illegal graph instruction.");
            }
            
            parent::set_arch(n,m,v);
        }

};

//Generate the edit graph from the original structure.
template<typename A, typename B>
edit_graph<A,B> to_edit_graph(const graph<A,B>& r){
    edit_graph<A,B> tmp;
    std::map<const typename graph<A,B>::node*,typename edit_graph<A,B>::enode*> map_nodes;
    //Nodes
    r.for_each([&](auto& i){
        //typename edit_graph<A,B>::enode t{.type=typename edit_graph<A,B>::enode::labels::NEW};
        auto n=(typename edit_graph<A,B>::enode*)tmp.add_node({});
        n->type=edit_graph<A,B>::enode::labels::NEW;

        map_nodes.insert({&i,n});
    });

    //Add arches
    r.for_each([&](auto& i){
        i.for_each([&](auto& j, auto& v){
            typename edit_graph<A,B>::earch q;
            q.type=edit_graph<A,B>::earch::labels::ARCH_SET;
            q.value=v.value;
            tmp.set_arch(map_nodes.at(&i),map_nodes.at(&j),q);
        });
    });

    return tmp;
}

}
}
