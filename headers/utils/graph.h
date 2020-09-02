#pragma once

#include <array>
#include <set>
#include <map>
#include <functional>
#include <algorithm>

#include <utils/alias-set.h>
#include <utils/string-exception.h>

/*PUT THIS IN ROTATE*/
#include <stdint.h>   // for uint32_t
#include <limits.h>   // for CHAR_BIT
#include <assert.h>

template<typename T>
inline T rotl (T n, unsigned int c){
  const unsigned int mask = (CHAR_BIT*sizeof(n) - 1);  // assumes width is a power of 2.
  c &= mask;
  return (n<<c) | (n>>( (-c)&mask ));
}

template<typename T>
inline T rotr (T n, unsigned int c){
  const unsigned int mask = (CHAR_BIT*sizeof(n) - 1);
  c &= mask;
  return (n>>c) | (n<<( (-c)&mask ));
}

namespace smile{
namespace utils{

typedef uint64_t items_t;
typedef uint64_t hash_t;

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

            friend bool operator<(const node& l, const node& r){return l.value<r.value;}
            hash_t hash() const{return std::hash<value_t>(value);}   //TODO:continue
        };

        struct arch{
            typedef arch_base value_t;
            value_t                             value;

            ~arch(){}
            arch():value(){}

            friend bool operator<(const arch& l, const arch& r){return l.value<r.value;}
            hash_t hash()const {return std::hash<value_t>(value);}
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

        inline void* resolve_node(items_t i) const{return nodes.at_inv(i);}

        map<node*,items_t> normalize() const;
        hash_t hash(const map<node*,items_t>&) const;

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
void graph<N,A>::cpy_conn(void* vsrc, void* vdst){
    if(!editable)throw StringException("Not editable graph");
    node *src=(node*)vsrc, *dst=(node*)vdst;
    for(auto& [i,j]:src->links){
        if(dst->links.find(i)!=dst->links.end()){throw StringException("Resolution model not provided. Collision among arches.");}
        set_arch(dst,i,j.value);
    }
    for(auto& i:src->inv_links){
        if(i==src){/*Avoid self-loops as they have been already represented on the previous step*/}
        else{
            if(dst->inv_links.find(i)!=dst->inv_links.end()){throw StringException("Resolution model not provided. Collision among inverse arches.");}
            set_arch(i,dst,src->links.at(dst).value);
        }
    }

}

template<typename N, typename A>
map<typename graph<N,A>::node*,items_t> graph<N,A>::normalize() const{
    //TODO: Rewrite this condition to be more consistnet
    if(nodes.size()<=1)throw "Stupid case";

    std::vector<node*> ord_seq;
    std::map<node*,items_t> ord_seq_inv;

    auto cmp_classes=[](const std::pair<items_t,items_t>& a, const std::pair<items_t,items_t>& b)->bool{
        if(a.first<b.first)return true;
        else if(a.first>b.first)return false;
        if(a.second<b.second)return true;
        else if(a.second>b.second)return false;
        return false;
    };

    std::set<std::pair<items_t,items_t>,decltype(cmp_classes)> classes(cmp_classes);

    //Compare two nodes based on their class
    auto cmp_seq=[&](items_t a, items_t b)->bool{
        if(a==b)return true;
        //TODO replace 1000 with big number.
        auto ia=classes.lower_bound({a,100000});
        auto ib=classes.lower_bound({b,100000});
        //They are in unique classes, and so they are not represented here anymore
        if(a<ia->first+ia->second && b<ib->first+ib->second)return true;
        return false;
    };

    /*
    //Full node comparison
    auto cmp_nodes=[&](node* a, node* b)->uint{
        if(*a<*b)return -1;
        else if(*b<*a)return 1;
        else{
            if(a->links.size()<b->links.size())return -1;
            if(a->links.size()>b->links.size())return 1;

            if(a->inv_links.size()<b->inv_links.size())return -1;
            if(a->inv_links.size()>b->inv_links.size())return 1;

            for(auto i=a->links.begin(),j=b->links.begin();i!=a->links.end();i++,j++){
                if(i->second<j->second)return -1;
                if(j->second<i->second)return 1;
                auto t=cmp_seq(ord_seq_inv.at(i->first),ord_seq_inv.at(j->first));
                if(t!=0)return t;
            }

            for(auto i=a->inv_links.begin(),j=b->inv_links.begin();i!=a->inv_links.end();i++,j++){
                auto t=cmp_seq(ord_seq_inv.at(i),ord_seq_inv.at(j));
                if(t!=0)return t;
            }

            return 0;
        }
    };
    */

    //First node comparison
    auto cmp_nodes_1=[&](node* a, node* b)->uint{
        if(*a<*b)return -1;
        else if(*b<*a)return 1;
        else{
            if(a->links.size()<b->links.size())return -1;
            if(a->links.size()>b->links.size())return 1;

            if(a->inv_links.size()<b->inv_links.size())return -1;
            if(a->inv_links.size()>b->inv_links.size())return 1;

            for(auto i=a->links.begin(),j=b->links.begin();i!=a->links.end();i++,j++){
                if(i->second<j->second)return -1;
                if(j->second<i->second)return 1;
            }

            //for(auto i=a->inv_links.begin(),j=b->inv_links.begin();i!=a->inv_links.end();i++,j++){
            //}

            return 0;
        }
    };

    //Partial node comparison, assuming the other tests were already performed.
    auto cmp_nodes_2=[&](node* a, node* b)->uint{
        for(auto i=a->links.begin(),j=b->links.begin();i!=a->links.end();i++,j++){
            auto t=cmp_seq(ord_seq_inv.at(i->first),ord_seq_inv.at(j->first));
            if(t!=0)return t;
        }

        for(auto i=a->inv_links.begin(),j=b->inv_links.begin();i!=a->inv_links.end();i++,j++){
            auto t=cmp_seq(ord_seq_inv.at(*i),ord_seq_inv.at(*j));
            if(t!=0)return t;
        }
        return 0;
    };

    ord_seq.reserve(nodes.size());
    {
        items_t c=0;
        for(auto i:nodes){ord_seq.push_back(i);ord_seq_inv.insert({i,c});c++;}
    }
    classes.insert({0,nodes.size()});

    for(;classes.size()!=0;){
        bool delendo=false;
        for(auto tmp_i=classes.begin();tmp_i!=classes.end();tmp_i++){
            auto tmp=*tmp_i;
            //TODO: IMPLEMENT THE PROPER SORT HERE!
            std::sort(ord_seq.begin()+tmp.first,ord_seq.begin()+tmp.first+tmp.second);

            items_t cases=0;
            items_t last_change=tmp.first;
            for(auto i=tmp.first;i<tmp.first+tmp.second-1;i++){
                auto a=ord_seq.begin()+i;
                auto b=a+1;
                //They are different
                if((*a<*b)==true){
                    if(i+1-last_change==1){
                        //This class is unitary, there is no reason to actually generate it.
                    }
                    else classes.insert({last_change,i+1-last_change});
                    last_change=i+1;
                    cases++;
                }
            }
            if(cases!=0){
                //Yay I got a reduction event. Remove the base and add the last which is surely missing.
                classes.erase(tmp);
                classes.insert({last_change,tmp.first+tmp.second-last_change});
                delendo=true;
                break;
            }
            else{
                //No split was possible, go to the next one please.
            }

        }

        if(delendo==true){
            //Ok go on to the next stage plz.
            continue;
        }
        else{
            //I need to force split something
            auto tmp=*classes.begin();
            classes.erase(classes.begin());
            classes.insert({tmp.first,1});
            classes.insert({tmp.first+1,tmp.second-1});
        }
    }

    std::map<node*,items_t> ret_map;
    {
        items_t c=0;
        for(auto i:ord_seq){ret_map[i]=c;c++;}
    }

    return ret_map;
}

template<typename N, typename A>
hash_t graph<N,A>::hash(const map<node*,items_t>& m) const{
    hash_t tmp=0;
    uint k=0;
    for(auto [i,j]:m){
        tmp|=rotl<hash_t>(i->hash(),k%(sizeof(hash_t)*8)*(sizeof(hash_t)*8/m.size()+1));
        k++;
    }
    return 0;
}

}
}