#pragma once

#include <array>
#include <set>
#include <map>
#include <functional>

#include <utils/alias-set.h>
#include <utils/string-exception.h>


namespace smile{
namespace utils{

typedef uint64_t items_t;

template<typename A, uint B>
using array=std::array<A,B>;

template<typename A>
using set=std::set<A>;

template<typename A, typename B>
using map=std::map<A,B>;

template<typename A>
using mapid=smile::utils::alias_set<A,std::less<A>,items_t>;

//using namespace std;
using namespace nlohmann;

template<typename NODE, typename ARCH>
struct edit_graph;

template<typename NODE, typename ARCH>
struct graph{
    typedef NODE node_base;
    typedef ARCH arch_base;

    friend struct edit_graph<NODE,ARCH>;

    protected:
        struct node;
        struct arch;

        struct node{
            typedef node_base value_t;
            map<node*,arch>                     links;
            set<node*>                          inv_links;

            value_t                             value;

            ~node(){}
            node():value(){}
            //node(const node& cp){value=cp.value;}
            //node& operator=(const node& cp){value=cp.value;return *this;}
        };

        struct arch{
            typedef arch_base value_t;
            value_t                             value;

            ~arch(){}
            arch():value(){}
            //arch(const arch& cp){value=cp.value;}
            //arch& operator=(const arch& cp){value=cp.value;return *this;}
        };

    protected:

        virtual void*                           add_node(const typename node::value_t& v);
        virtual void*                           tmp_node(const typename node::value_t& v);
        virtual void                            rem_node(void* n);
        virtual void                            set_node(void* n, const typename node::value_t& v);
        virtual typename node::value_t&         get_node(void* n);
        virtual void                            set_arch(void* n, void* m, const typename arch::value_t& v=typename arch::value_t());
        virtual void                            rem_arch(void* n, void* m);
        virtual void                            cpy_conn(void* src, void* dst);
        virtual void                            forget(void* n){if(!editable)throw StringException("Not editable graph");throw StringException("Graph abstraction is not supported");};

        mapid<node*>                            nodes;
        bool                                    editable=true;

    public:

        inline void* resolve_node(items_t i){return nodes.inv().at(i);}

        ~graph(){
            for(auto& i:nodes){
                delete i;
            }
        }

        friend void from_json(const nlohmann::json& config, graph& g){
            //Re:Zero
            for(auto& i:g.nodes){
                delete i;
            }
            g.nodes.clear();

            std::map<std::string,void*> mnodes;

            //Get Nodes
            {
                auto it=config.find("nodes");
                if(it==config.end() or !it->is_object())throw StringException("JSONSyntaxError");
                else{
                    for(auto& [i,j]:it->items()){
                        typename node::value_t val;
                        from_json(j,val);

                        node* tmp=(node*)g.add_node(val);
                        mnodes.insert({i,tmp});
                    }
                }
            }
            //Make links
            {
                auto it=config.find("arches");
                if(it==config.end() or !it->is_array())throw StringException("JSONSyntaxError");
                else{
                    for(auto& i:*it){
                        if(!i.is_array() or i.size()!=3)throw StringException("JSONSyntaxError");
                        arch_base tmp;
                        from_json(i[2],tmp);
                        g.set_arch(mnodes[i[0]],mnodes[i[1]],tmp);
                    }
                }
            }
        }

        friend void to_json(nlohmann::json& config, const graph& g){
            for(auto i: g.nodes){
                auto j=g.nodes.at(i);
                to_json(config["nodes"][std::to_string(j)],i->value);

                for(auto [a,b]:i->links){
                    nlohmann::json tmp, tmp2;
                    to_json(tmp2,b.value);
                    tmp.push_back(j);
                    tmp.push_back(g.nodes.at(a));
                    tmp.push_back(tmp2);

                    config["arches"].push_back(tmp);
                }
            }
        }

        //Deleted for now, I may decide to introduce it back later.
        friend void to_dot(std::ostream& out, const graph& g){}

        inline friend std::ostream& operator<<(std::ostream& out, const graph& g){json tmp;to_json(tmp,g);out<<tmp;return out;}

};

template<typename N, typename A>
void* graph<N,A>::tmp_node(const typename node::value_t& v){
    if(!editable)throw StringException("Not editable graph");
    node* tmp=new node();
    tmp->value=v;
    return tmp;
}

template<typename N, typename A>
void* graph<N,A>::add_node(const typename node::value_t& v){
    if(!editable)throw StringException("Not editable graph");
    node* tmp=new node();
    nodes.insert(tmp);
    tmp->value=v;
    return tmp;
}

template<typename N, typename A>
void graph<N,A>::rem_node(void* n){
    if(!editable)throw StringException("Not editable graph");
    node* nn=(node*)n;
    auto it=nodes.find(nn);
    if(it!=nodes.end()){
        for(auto i:nn->inv_links){i->links.erase(i->links.find(nn));}
        nodes.erase(it);
        delete nn;
    }
}

template<typename N, typename A>
void graph<N,A>::set_node(void* n, const typename node::value_t& v){
    if(!editable)throw StringException("Not editable graph");
    node* nn=(node*)n;
    nn->value=v;
}

template<typename N, typename A>
typename graph<N,A>::node::value_t& graph<N,A>::get_node(void* n){
    if(!editable)throw StringException("Not editable graph");
    node* nn=(node*)n;
    return nn->value;
}

template<typename N, typename A>
void graph<N,A>::set_arch(void* n, void* m, const typename arch::value_t& v){
    if(!editable)throw StringException("Not editable graph");
    node *nn=(node*)n, *mm=(node*)m;
    nn->links[mm].value=v;
    mm->inv_links.insert(nn);
}

template<typename N, typename A>
void graph<N,A>::rem_arch(void* n, void* m){
    if(!editable)throw StringException("Not editable graph");
    node *nn=(node*)n, *mm=(node*)m;
    auto it=nn->links.find(mm);
    if(it!=nn->links.end()){
        nn->links.erase(it);
        mm->inv_links.erase(mm->inv_links.find(nn));
        return;
    }
}

template<typename N, typename A>
void graph<N,A>::cpy_conn(void* src, void* dst){
    if(!editable)throw StringException("Not editable graph");
}

}
}