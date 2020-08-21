#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

/**
 * @brief Wrapper class for a native_node_t object to integrate nodes in the soup.
 */

struct node{
    public:
        ~node();

        items_t mul() const;

        friend hash_t    hash(const node& r);
        friend bool operator<(const node& a, const node& b);

        friend void   to_json(json& j, const node& p);
        friend void from_json(const json& j, node& p);


    private:
        node(domain& d, const native_node_t& v);
        node(domain& d, handle& h);

        struct partial_t{
            set<capture*>   part_of;
            capture*        main_part_of = nullptr;
            set<arch*>      connected_to;
        };

        domain*                 base;
        word mutable            visitando:1             = false;
        word                    native_ka;
        word                    res:sizeof(word)*8-2;

        map<soup*,partial_t>    deps;   
        
        native_node_t           native_value;
        handle*                 derived_h_value         = nullptr;
        capture*                derived_c_value         = nullptr;   
};
