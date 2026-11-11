#include <opt/option.hpp>

namespace {

struct non_destructible {
    ~non_destructible() = delete;
};

[[maybe_unused]] void test() {
    {
        // error[gcc]: use of deleted function '{anonymous}::non_destructible::~non_destructible()'
        opt::option<non_destructible> o1; // error: instantiation of opt::option with a non-destructible type is ill-formed
        // value_or is declared as returning remove_cv_t<T>, which is an array type for T = char[20]
        // error[gcc]x2: function returning an array
        // error[msvc]x2: function returns array
        opt::option<char[20]> o2; // error: instantiation of opt::option with an array type is ill-formed
    }
    {
        opt::option<std::in_place_t> o1; // error: instantiation of opt::option with std::in_place_t is ill-formed
        opt::option<const std::in_place_t> o2; // error^: instantiation of opt::option with std::in_place_t is ill-formed
        opt::option<volatile std::in_place_t> o3; // error^: instantiation of opt::option with std::in_place_t is ill-formed
        opt::option<const volatile std::in_place_t> o4; // error^: instantiation of opt::option with std::in_place_t is ill-formed
    }
    {
        opt::option<opt::none_t> o1; // error: instantiation of opt::option with opt::none_t is ill-formed
        opt::option<const opt::none_t> o2; // error^: instantiation of opt::option with opt::none_t is ill-formed
        opt::option<volatile opt::none_t> o3; // error^: instantiation of opt::option with opt::none_t is ill-formed
        opt::option<const volatile opt::none_t> o4; // error^: instantiation of opt::option with opt::none_t is ill-formed
    }
    {
        // error[clang]: no viable constructor or deduction guide for deduction of template arguments of '{{(opt::)?}}option'
        // error[gcc]: class template argument deduction failed
        // error[gcc]: no matching function for call to 'option()'
        // error[msvc]: cannot deduce template arguments for 'opt::option'
        // error[msvc]: 'opt::option<T,Traits> opt::option(void) noexcept': could not deduce template argument for 'T'
        // error[msvc]: 'opt::option<T,Traits> opt::option(opt::option<T,Traits>)': expects 1 arguments - 0 provided
        opt::option o1;
        // error[clang]: no matching constructor for initialization of 'opt::option<{{(opt::)?}}none_t>'
        opt::option o2(opt::none);
    }
    {
        // error[gcc]: data member 'opt::impl::option_storage<void()>::<unnamed union>::value' invalidly declared function type
        // error[gcc]: static assertion failed: instantiation of opt::option with a non-destructible type is ill-formed
        // error[gcc]x2: function returning a function
        opt::option<void()> o1;
    }
    {
        // error[gcc,clang]: instantiation of opt::option with a non-destructible type is ill-formed
        // error[gcc]: 'opt::impl::option_storage<T>::<unnamed union>::value' has incomplete type
        // error[gcc]: invalid use of 'void'
        // error[gcc]x20: forming reference to void
        // error[clang]: data member instantiated with function type
        // error[clang]: field has incomplete type
        // error[msvc]: 'opt::impl::option_storage<T>::value': a member of a class template cannot acquire a function type
        // error[msvc]x2: member function not allowed in anonymous union
        // error[msvc]: 'value': this use of 'void' is not valid
        // error[msvc]x18: you cannot create a reference to 'void'
        // error[msvc]: function returns function
        opt::option<void> o1;
        // error[clang]x2: instantiation of opt::option with a non-destructible type is ill-formed
        // error[gcc]: instantiation of opt::option with a non-destructible type is ill-formed
        // error[gcc]: 'opt::impl::option_storage<T>::<unnamed union>::value' has incomplete type
        // error[gcc]: invalid use of 'void'
        // error[gcc]x16: forming reference to void
        // error[clang]: field has incomplete type
        // error[msvc]: function returns function
        opt::option<const void> o2;
    }
}

}
