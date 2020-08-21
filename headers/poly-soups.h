#pragma once

#include <set>

#include <nlohmann/json.hpp>
#include <utils/alias-set.h>
#include <utils/alias-map.h>
#include <utils/string-exception.h>

//This class will be converted in a template one later on to ensure type portabilty.
//However for the time being it easier to debug it in this form.

//Temporary types
typedef uint64_t NODUS_T;
typedef uint64_t ARCUS_T;

//It is used as a namespace
struct poly{

typedef NODUS_T native_node_t;
typedef ARCUS_T native_arch_t;
typedef uint64_t id_t;
typedef uint64_t items_t;
typedef uint64_t hash_t;
typedef uint64_t word;

template<typename A>
using mapid=smile::utils::alias_set<A,std::less<A>,items_t>;

template<typename B>
using set=std::set<B>;

template<typename A, typename B>
using bimap=smile::utils::alias_map<A,B>;

template<typename T>
using opt=std::optional<T>;

using json=nlohmann::json;

template<typename A, typename B>
using map=std::map<A,B>;

template<typename A, typename B>
using pair=std::pair<A,B>;

struct node;
struct arch;
struct domain;
struct soup;
struct eqv;
struct instance;
struct handle;
struct capture;
struct structure;

#define FRAGMENTS
#define FRIENDS_LIST    friend struct node;     \
                        friend struct arcus;    \
                        friend struct domain;   \
                        friend struct soup;     \
                        friend struct eqv;      \
                        friend struct instnce;  \
                        friend struct handle;   \
                        friend struct capture;  \
                        friend struct structure;

//All the definitions
#include "./poly-soups/nodus.h"
#include "./poly-soups/arcus.h"
#include "./poly-soups/domain.h"
#include "./poly-soups/soup.h"
#include "./poly-soups/eqv.h"
#include "./poly-soups/instance.h"
#include "./poly-soups/handle.h"
#include "./poly-soups/capture.h"
#include "./poly-soups/structure.h"

};

#define IMPL_PREFIX
#define IMPL_NAME   poly::

//All the implementations
#include "./poly-soups/impl/nodus.h"
#include "./poly-soups/impl/arcus.h"
#include "./poly-soups/impl/domain.h"
#include "./poly-soups/impl/soup.h"
#include "./poly-soups/impl/eqv.h"
#include "./poly-soups/impl/instance.h"
#include "./poly-soups/impl/handle.h"
#include "./poly-soups/impl/capture.h"
#include "./poly-soups/impl/structure.h"


#undef FRAGMENTS
#undef IMPL_PREFIX
#undef IMPL_NAME
#undef FRIENDS_LIST