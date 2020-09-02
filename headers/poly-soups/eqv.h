#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

struct eqv{
    FRIENDS_LIST

    public:
        ~eqv();

        items_t     mul()const;
        hash_t      hash()const;

        handle&     sample_and_edit();
        bool        commit();

        bool        set_uniqueness(bool v);
        bool        set_persistence(word v);

        friend void   to_json(json& j, const eqv& p);
        friend void from_json(const json& j, eqv& p);

        //Weak equivalence
        friend bool operator==(const eqv& l, const eqv& r);

    private:
        eqv(soup& b);
        static handle& make(soup& b);

        soup*                   base;

        mapid<instance*>        instances;
        //It would be possible to create a second hash-map, however the number of elements is assumed to be small enough to make it a waste of space.
        //A linear search is going to be fast enough in any realistic scenario.

        word                    unique:1;
        word                    editable:1;
        word                    persistence:sizeof(word)*8-2;

        opt<hash_t>  mutable    _eqv_hash={};
        opt<items_t> mutable    _mul={};
};