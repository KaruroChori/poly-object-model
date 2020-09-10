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

        node(const node_base& v=node_base()):value(v){}
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

        arch(node& a, node& b, const arch_base& v):value(v){left.n=&a;right.n=&b;left_type=0;right_type=0;}
    };

    struct regio{
        set<node*> boundary;
        set<arch*> connect;
        set<arch*> inv_connect;
    };

    struct normal_t{
        map<node*,items_t> nodes;
        map<arch*,items_t> arches;
        map<regio*,items_t> regiones;

        //TODO: JSON
    };

    public:
        abstract_graph(const graph<node_base,arch_base>& r);
        abstract_graph(const abstract_graph& r);
        ~abstract_graph();

        regio* cut(node*);                            //Cut a hole covering a node.
        regio* boundary_cut(node*);                   //Cut a hole touching a node.
        regio* merge(regio*, regio*);                 //Merge two holes.
        regio* resolve_region(node*);                 //Resolve which is the current region covering a hole.

        template<typename T>
        set<map<items_t,set<items_t>>> match(const T& base); 
    
    protected:

        mapid<node*>                            nodes;
        mapid<regio*>                           regiones;
        map<node*,set<regio*>>                  original_allocation;    //Where forgotten nodes are going to be mapped.
        mapid<arch*>                            arches;

        std::optional<normal_t>                 normal;
        std::optional<hash_t>                   hash_value;

    public:
        normal_t normalize() const;
        hash_t hash(const normal_t&) const;

        friend void from_json(const nlohmann::json& config, abstract_graph& g){
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

        //TODO: Complete
        friend void to_json(nlohmann::json& config, const abstract_graph& g){
            for(auto i: g.nodes){
                auto j=g.nodes.at(i);
                to_json(config["nodes"][std::to_string(j)],i->value);
            }

            for(auto i: g.regiones){
                auto j=g.regiones.at(i);

            }

            for(auto i: g.arches){
                auto j=g.arches.at(i);
            }
        }

        //Deleted for now, I may decide to introduce it back later.
        friend void to_dot(std::ostream& out, const abstract_graph& g){}

        inline friend std::ostream& operator<<(std::ostream& out, const abstract_graph& g){json tmp;to_json(tmp,g);out<<tmp;return out;}
};

template<typename NODE, typename ARCH>
abstract_graph<NODE,ARCH>::abstract_graph(const graph<NODE,ARCH>& r){
    std::map<const typename graph<NODE,ARCH>::node*,node*> map_nodes;
    std::map<std::pair<const typename graph<NODE,ARCH>::node*,const typename graph<NODE,ARCH>::node*>,const typename graph<NODE,ARCH>::arch::value_t&> tmp_arches;

    r.for_each([&](auto& i){
        auto t=new node(i.value);
        nodes.insert(t);
        map_nodes.insert({&i,t});

        i.for_each([&](auto& j, auto& v){
            tmp_arches.insert({{&i,&j},v.value});
        });
    });

    //Spawn arches.
    for(auto [i,j]: tmp_arches){
        auto ln=map_nodes.at(i.first);
        auto rn=map_nodes.at(i.second);
        auto t=new arch(*ln,*rn,j);
        arches.insert(t);
        ln->connect.insert(t);
        rn->inv_connect.insert(t);
    }
}


template<typename NODE, typename ARCH>
abstract_graph<NODE,ARCH>::~abstract_graph(){
    for(auto i:nodes)delete i;
    for(auto i:arches)delete i;
    for(auto i:regiones)delete i;
}

template<typename NODE, typename ARCH>
typename abstract_graph<NODE,ARCH>::regio* abstract_graph<NODE,ARCH>::cut(node* nt){
    //System call! Tranfer connectivity: node to region.
    auto ret=new regio();
    //Self connectivity delenda est.
    for(auto i:nt->connect){if(i->left.n==i->right.n)continue;i->left.r=ret;i->left_type=1;ret->connect.insert(i);}
    for(auto i:nt->inv_connect){if(i->left.n==i->right.n){delete i;continue;};i->right.r=ret;i->right_type=1;ret->inv_connect.insert(i);}
    //Enhance armament!
    regiones.insert(ret);
    original_allocation[nt].insert(ret);    //TODO:May be removed later if there is no need for it.
    //Release recollection!
    nodes.remove(nt);
    delete nt;
    //And hopefully you are done and you'll not come back 10 times to cheer up your mate.
    return ret;
}

template<typename NODE, typename ARCH>
typename abstract_graph<NODE,ARCH>::regio* abstract_graph<NODE,ARCH>::boundary_cut(node* nt){
    auto ret=new regio();
    //No connectivity to shift here.
    ret->boundary.insert(nt);
    regiones.insert(ret);
    return ret;
}

template<typename NODE, typename ARCH>
typename abstract_graph<NODE,ARCH>::regio* abstract_graph<NODE,ARCH>::merge(regio* ra, regio* rb){
    if(ra==rb)return;
    //Merge right to left. No difference but performance can be affected by this choice.

    //Check if there is connectivity to be removed.
    for(auto i:ra->connect){
        if(i->right_type==1 && i->right.r==rb){/*delendo*/delete i;}
        else ra->connect.insert(i);
    }
    for(auto i:ra->inv_connect){
        if(i->left_type==1 && i->left.r==rb){/*delendo*/delete i;}
        else ra->inv_connect.insert(i);
    }
    for(auto i:rb->boundary)ra->boundary.insert(i);
    regiones.erase(rb);
    delete rb;
}

//TODO: may be removed.
template<typename NODE, typename ARCH>
typename abstract_graph<NODE,ARCH>::regio* abstract_graph<NODE,ARCH>::resolve_region(node*){throw "NotImplemented";}

}
}