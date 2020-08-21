#pragma once
#ifndef FRAGMENTS
#error This file should only be included as a fragment.
#endif

/**
 * @brief Wrapper class for a native_arch_t object to integrate arches in the soup.
 */

struct arch{
    FRIENDS_LIST

    public:
        ~arch();

        items_t mul() const;

        friend hash_t    hash(const arch& r);
        friend bool operator<(const arch& a, const arch& b);

        friend void   to_json(json& j, const arch& p);
        friend void from_json(const json& j, arch& p);

    private:
        arch(soup& b, node* l, node* r, const native_arch_t& value);

        soup*                   base;

        node*                   left;
        node*                   right;

        set<capture*>           part_of;
        structure*              main_part_of;

        native_arch_t           value;
};