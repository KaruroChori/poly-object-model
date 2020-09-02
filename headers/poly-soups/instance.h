#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

struct instance{
    FRIENDS_LIST

    public:
        ~instance();

        items_t     mul()const;
        hash_t      hash()const;

        friend void to_json(json& j, const instance& p);
        friend void from_json(const json& j, instance& p);

        //Down-equivalence
        friend bool operator==(const instance& l, const instance& r);

    private:
        instance(eqv& base);

        eqv*                    base;

        opt<hash_t>  mutable    _hash={};
        opt<items_t> mutable    _mul=0;
        items_t                 not_assigned=0;

        set<handle*>            handles;
        //It would be possible to create a second hash-map, however the number of elements is assumed to be small enough to make it a waste of space.
        //A linear search is going to be fast enough in any realistic scenario.

        structure*              struct_model;
        mapid<capture*>         captures;
};