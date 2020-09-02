#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

struct soup{
    FRIENDS_LIST

    public:
        struct p_arch_t{
            id_t                left;
            id_t                right;
            native_arch_t       value;
        };

         soup(domain& d);
        ~soup();

    private:

        //Graph interface, here to enable graph-rewrite graphs.
        void*                   add_node(const native_node_t& v);
        void*                   tmp_node(const native_node_t& v);
        void                    rem_node(void* n);
        void                    set_node(void* n, const native_node_t& v);
        native_node_t&          get_node(void* n);
        void                    set_arch(void* n, void* m, const native_arch_t& v=native_arch_t());
        void                    rem_arch(void* n, void* m);
        void                    cpy_conn(void* src, void* dst);
        void                    forget(void* n){throw StringException("Graph abstraction is not supported");};

        domain*                 base;
        
        mapid<arch*>            arches;
        mapid<eqv*>             eqvs;

        set<eqv*>               edit_eqvs;
        map<opt<hash_t>,
            set<eqv*>>          classes={{{},{}}};
};
