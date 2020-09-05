#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

IMPL_PREFIX
IMPL_NAME node::~node(){}

IMPL_PREFIX
IMPL_NAME items_t IMPL_NAME node::mul() const{}

IMPL_PREFIX
IMPL_NAME node::node(domain& d, const native_node_t& v){}

IMPL_PREFIX
IMPL_NAME node::node(domain& d, handle& h){}

        /*friend hash_t    hash(const node& r);
        friend bool operator<(const node& a, const node& b);*/

        /*friend void   to_json(json& j, const node& p);
        friend void from_json(const json& j, node& p);*/


