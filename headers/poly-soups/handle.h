#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

struct handle{
    FRIENDS_LIST

    public:
        ~handle();

        items_t     mul()const;
        hash_t      hash()const;

        friend void to_json(json& j, const handle& p);
        friend void from_json(const json& j, handle& p);

        //Full-equivalence
        friend bool operator==(const handle& l, const handle& r);

    private:
        instance*               base;

        opt<hash_t>  mutable    _hash={};
        items_t                 _mul;
};