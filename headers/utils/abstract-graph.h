#pragma once

#include "graph.h"

namespace smile{
namespace utils{

template<typename NODE, typename ARCH>
struct abstract_graph{
    typedef NODE node_base;
    typedef ARCH arch_base;

    struct node;
    struct arch;
    struct regio;

    struct node{
        set<arch*> connect;
        set<arch*> inv_connect;

        node_base value;
    };

    struct arch{
        union{
            node*   n;
            regio*  r;
        }left;
        bool left_type;

        union{
            node*   n;
            regio*  r;
        }right;
        bool right_type;

        arch_base value;
    };

    struct regio{
        set<arch*> connect;
        set<arch*> inv_connect;
    };

    public:
        abstract_graph(const graph<node_base,arch_base>& r);
        abstract_graph(const abstract_graph& r);

        regio* cut(node*);                            //Cut a hole covering a node.
        regio* boundary_cut(node*);                   //Cut a hole touching a node.
        regio* merge(regio*, regio*);                 //Merge two holes.
        regio* resolve_region(node*);                 //Resolve which is the current region covering a hole.

        template<typename T>
        set<map<items_t,set<items_t>>> match(const T& base); 
    
    protected:

        mapid<node*>                            nodes;
        mapid<regio*>                           regiones;
        map<node*,set<regio*>>                  original_allocation;
        mapid<arch*>                            arches;

    public:
        friend void from_json(const nlohmann::json& config, graph& g){
            /*
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
            */
        }

        friend void to_json(nlohmann::json& config, const graph& g){
            /*
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
            */
        }

        //Deleted for now, I may decide to introduce it back later.
        friend void to_dot(std::ostream& out, const graph& g){}

        inline friend std::ostream& operator<<(std::ostream& out, const graph& g){json tmp;to_json(tmp,g);out<<tmp;return out;}
};

}
}