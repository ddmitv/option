// https://github.com/llvm/llvm-project/blob/main/libcxx/test/support/archetypes.ipp

#ifndef ARCHETYPE_BASE
    #define ARCHETYPE_BASE(Name) ::archetype_bases::null_base
#endif
#ifndef SPEC_EXPLICIT
    #define SPEC_EXPLICIT
#endif
#ifndef SPEC_NOEXCEPT
    #define SPEC_NOEXCEPT
#endif
#ifndef SPEC_CONSTEXPR
    #define SPEC_CONSTEXPR constexpr
#endif
#ifndef SPEC_ASSIGN_CONSTEXPR
    #define SPEC_ASSIGN_CONSTEXPR SPEC_CONSTEXPR
#endif
#ifndef CTOR_BODY
    #define CTOR_BODY = default;
#endif
#ifndef DEFAULT_CTOR_BODY
    #define DEFAULT_CTOR_BODY CTOR_BODY
#endif
#ifndef ASSIGN_BODY
    #define ASSIGN_BODY = default;
#endif
#ifndef DTOR_BODY
    #define DTOR_BODY(Name)
#endif

struct all_ctors : ARCHETYPE_BASE(all_ctors) {
    using base = ARCHETYPE_BASE(all_ctors);
    using base::base;
    using base::operator=;
    SPEC_EXPLICIT SPEC_CONSTEXPR all_ctors() SPEC_NOEXCEPT DEFAULT_CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR all_ctors(const all_ctors&) SPEC_NOEXCEPT CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR all_ctors(all_ctors&&) SPEC_NOEXCEPT CTOR_BODY
    SPEC_ASSIGN_CONSTEXPR all_ctors& operator=(const all_ctors&) SPEC_NOEXCEPT ASSIGN_BODY
    SPEC_ASSIGN_CONSTEXPR all_ctors& operator=(all_ctors&&) SPEC_NOEXCEPT ASSIGN_BODY
    DTOR_BODY(all_ctors)
};

struct no_ctors : ARCHETYPE_BASE(no_ctors) {
    SPEC_EXPLICIT no_ctors() SPEC_NOEXCEPT = delete;
    SPEC_EXPLICIT no_ctors(const no_ctors&) SPEC_NOEXCEPT = delete;
    no_ctors& operator=(const no_ctors&) SPEC_NOEXCEPT = delete;
    DTOR_BODY(no_ctors)
};

struct no_default : ARCHETYPE_BASE(no_default) {
    using base = ARCHETYPE_BASE(no_default);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR no_default() SPEC_NOEXCEPT = delete;
    DTOR_BODY(no_default)
};

struct default_only : ARCHETYPE_BASE(default_only) {
    using base = ARCHETYPE_BASE(default_only);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR default_only() SPEC_NOEXCEPT DEFAULT_CTOR_BODY
    default_only(const default_only&) SPEC_NOEXCEPT = delete;
    default_only& operator=(const default_only&) SPEC_NOEXCEPT = delete;
    DTOR_BODY(default_only)
};

struct copyable : ARCHETYPE_BASE(copyable) {
    using base = ARCHETYPE_BASE(copyable);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR copyable() SPEC_NOEXCEPT DEFAULT_CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR copyable(const copyable&) SPEC_NOEXCEPT CTOR_BODY
    copyable& operator=(const copyable&) SPEC_NOEXCEPT ASSIGN_BODY
    DTOR_BODY(copyable)
};

struct copy_only : ARCHETYPE_BASE(copy_only) {
    using base = ARCHETYPE_BASE(copy_only);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR copy_only() SPEC_NOEXCEPT DEFAULT_CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR copy_only(const copy_only&) SPEC_NOEXCEPT CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR copy_only(copy_only&&) SPEC_NOEXCEPT = delete;
    copy_only& operator=(const copy_only&) SPEC_NOEXCEPT ASSIGN_BODY
    copy_only& operator=(copy_only&&) SPEC_NOEXCEPT = delete;
    DTOR_BODY(copy_only)
};

struct non_copyable : ARCHETYPE_BASE(non_copyable) {
    using base = ARCHETYPE_BASE(non_copyable);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR non_copyable() SPEC_NOEXCEPT DEFAULT_CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR non_copyable(const non_copyable&) SPEC_NOEXCEPT = delete;
    non_copyable& operator=(const non_copyable&) SPEC_NOEXCEPT = delete;
    DTOR_BODY(non_copyable)
};

struct move_only : ARCHETYPE_BASE(move_only) {
    using base = ARCHETYPE_BASE(move_only);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR move_only() SPEC_NOEXCEPT DEFAULT_CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR move_only(move_only&&) SPEC_NOEXCEPT CTOR_BODY
    move_only& operator=(move_only&&) SPEC_NOEXCEPT ASSIGN_BODY
    DTOR_BODY(move_only)
};

struct copy_assignable : ARCHETYPE_BASE(copy_assignable) {
    using base = ARCHETYPE_BASE(copy_assignable);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR copy_assignable() SPEC_NOEXCEPT = delete;
    copy_assignable& operator=(const copy_assignable&) SPEC_NOEXCEPT ASSIGN_BODY
    DTOR_BODY(copy_assignable)
};

struct copy_assign_only : ARCHETYPE_BASE(copy_assign_only) {
    using base = ARCHETYPE_BASE(copy_assign_only);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR copy_assign_only() SPEC_NOEXCEPT = delete;
    copy_assign_only& operator=(const copy_assign_only&) SPEC_NOEXCEPT ASSIGN_BODY
    copy_assign_only& operator=(copy_assign_only&&) SPEC_NOEXCEPT = delete;
    DTOR_BODY(copy_assign_only)
};

struct move_assign_only : ARCHETYPE_BASE(move_assign_only) {
    using base = ARCHETYPE_BASE(move_assign_only);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR move_assign_only() SPEC_NOEXCEPT = delete;
    move_assign_only& operator=(const move_assign_only&) SPEC_NOEXCEPT = delete;
    move_assign_only& operator=(move_assign_only&&) SPEC_NOEXCEPT ASSIGN_BODY
    DTOR_BODY(move_assign_only)
};

struct converting_type : ARCHETYPE_BASE(converting_type) {
    using base = ARCHETYPE_BASE(converting_type);
    using base::base;
    SPEC_EXPLICIT SPEC_CONSTEXPR converting_type() SPEC_NOEXCEPT DEFAULT_CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR converting_type(const converting_type&) SPEC_NOEXCEPT CTOR_BODY
    SPEC_EXPLICIT SPEC_CONSTEXPR converting_type(converting_type&&) SPEC_NOEXCEPT CTOR_BODY
    converting_type& operator=(const converting_type&) SPEC_NOEXCEPT ASSIGN_BODY
    converting_type& operator=(converting_type&&) SPEC_NOEXCEPT ASSIGN_BODY
    template <class... Args>
    SPEC_EXPLICIT SPEC_CONSTEXPR converting_type(Args&&...) SPEC_NOEXCEPT {}
    template <class Arg>
    converting_type& operator=(Arg&&) SPEC_NOEXCEPT { return *this; }
    DTOR_BODY(converting_type)
};

template<template <typename...> typename List>
using apply_types = List<all_ctors, no_ctors, no_default, default_only, copyable, copy_only, non_copyable, move_only, copy_assignable, copy_assign_only, move_assign_only, converting_type>;

#undef ARCHETYPE_BASE
#undef SPEC_EXPLICIT
#undef SPEC_NOEXCEPT
#undef SPEC_CONSTEXPR
#undef SPEC_ASSIGN_CONSTEXPR
#undef CTOR_BODY
#undef DEFAULT_CTOR_BODY
#undef ASSIGN_BODY
#undef DTOR_BODY
