#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif


struct eqv{
    FRIENDS_LIST

    public:
        ~eqv();

        items_t     mul()const;
        handle&     sample_and_edit();
        opt<hash_t> hash()const;
        bool        commit();

        bool        uniqueness(bool v);
        bool        set_persistence(word v);

        friend void   to_json(json& j, const eqv& p);
        friend void from_json(const json& j, eqv& p);

        friend bool operator==(const eqv& l, const eqv& r);

    private:
        eqv(soup& b);
        static handle& create_new(soup& b);

        soup*                   base;

        mapid<instance*>        instances;

        word                    unique:1;
        word                    editable:1;
        word                    persistence:sizeof(word)*8-2;

        opt<hash_t>  mutable    _eqv_hash={};
        opt<items_t> mutable    _mul={};
};