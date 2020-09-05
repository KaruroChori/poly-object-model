#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

IMPL_PREFIX
IMPL_NAME domain::domain(){}

IMPL_PREFIX
IMPL_NAME domain::~domain(){}

IMPL_PREFIX
IMPL_NAME id_t IMPL_NAME domain::add(pair<handle&,capture&> p){}

IMPL_PREFIX
IMPL_NAME id_t IMPL_NAME domain::add(const native_node_t& s){}

//I need to change this namespace.
namespace smile{
    IMPL_PREFIX
    void to_json(IMPL_NAME json& i, const IMPL_NAME domain& p){}

    IMPL_PREFIX
    void from_json(const IMPL_NAME json& j,IMPL_NAME domain& p){}
}