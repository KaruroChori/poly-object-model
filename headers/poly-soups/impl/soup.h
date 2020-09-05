#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

IMPL_PREFIX
IMPL_NAME soup::soup(domain& d){}

IMPL_PREFIX
IMPL_NAME soup::~soup(){}

IMPL_PREFIX
void* IMPL_NAME soup::add_node(const native_node_t& v){}

IMPL_PREFIX
void* IMPL_NAME soup::tmp_node(const native_node_t& v){}

IMPL_PREFIX
void  IMPL_NAME soup::rem_node(void* n){}

IMPL_PREFIX
void  IMPL_NAME soup::set_node(void* n, const native_node_t& v){}

IMPL_PREFIX
IMPL_NAME native_node_t&  IMPL_NAME soup::get_node(void* n){}

IMPL_PREFIX
void  IMPL_NAME soup::set_arch(void* n, void* m, const native_arch_t& v){}

IMPL_PREFIX
void  IMPL_NAME soup::rem_arch(void* n, void* m){}

IMPL_PREFIX
void  IMPL_NAME soup::cpy_conn(void* n, void* m){}
