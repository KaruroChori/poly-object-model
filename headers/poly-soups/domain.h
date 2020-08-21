#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

struct domain{
    public:
         domain();
        ~domain();

        id_t add(pair<handle&,capture&> p);
        id_t add(const native_node_t& s);

        bool rem(id_t id);

        friend void   to_json(json& j, const domain& p);
        friend void from_json(const json& j, domain& p);

        inline friend domain& operator+=(domain& d, pair<handle&,capture&> p){if(!d.add(p))throw StringException("FailedNodeInsertion");return d;}
        inline friend domain& operator+=(domain& d, const native_node_t& s){if(!d.add(s))throw "FailedNodeInsertion";return d;}
        inline friend domain& operator-=(domain& d, id_t id){if(!d.rem(id))throw "FailedNodeRemoval";return d;}

    private:

        mapid<node*>            nodes;
        set<soup*>              soups;
};