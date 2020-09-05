#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

IMPL_PREFIX
IMPL_NAME arch::~arch(){}

IMPL_PREFIX
IMPL_NAME items_t IMPL_NAME arch::mul() const {}

IMPL_PREFIX
IMPL_NAME arch::arch(soup& s, node* l, node* r, const native_arch_t& value){}

/*
        friend hash_t    hash(const arch& r);
        friend bool operator<(const arch& a, const arch& b);

        friend void   to_json(json& j, const arch& p);
        friend void from_json(const json& j, arch& p);
*/