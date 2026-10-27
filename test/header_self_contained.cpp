#include <opt/option.hpp>

#include <string>
#include <string_view>
#include <vector>

OPTION_CLANG_SUPPRESS_WARNING("-Wc++98-compat")
OPTION_CLANG_SUPPRESS_WARNING("-Wc++98-compat-pedantic")
OPTION_CLANG_SUPPRESS_WARNING("-Wpre-c++20-compat")

// clang versions <= 18 had weaker -Wconsumed analysis and they are producing false positives.
#if OPTION_CLANG
    #if __clang_major__ <= 18
        OPTION_CLANG_SUPPRESS_WARNING("-Wconsumed")
    #endif
#endif

namespace {

template<typename T>
void test_comparisons() {
    opt::option<T> o;
    opt::option<T> o2 = o;
    (void)(o == o2);
    (void)(o != o2);
    (void)(o <  o2);
    (void)(o <= o2);
    (void)(o >  o2);
    (void)(o >= o2);
    (void)(o <=> o2);
}

template<typename T>
void test_basic() {
    opt::option<T> o;
    o.emplace();
    (void)o.has_value();
    (void)o.get();
    (void)o.get_unchecked();
    (void)*o;
    (void)o.operator->();

    o.emplace(T{});
    (void)o.value();

    opt::option<T> o2 = o;
    for (auto it = o.begin(); it != o.end(); ++it) {
        (void)*it;
    }
    for (auto it = static_cast<const opt::option<T>&>(o).begin();
         it != static_cast<const opt::option<T>&>(o).end(); ++it) {
        (void)*it;
         }

    swap(o, o2);
    o.reset();
}

template<typename T>
void test_in_place() {
    opt::option<T> o{std::in_place};
    (void)o.get();
}

[[maybe_unused]] void instantiate() {
    test_comparisons<int>();
    test_comparisons<unsigned>();
    test_comparisons<char>();
    test_comparisons<std::string>();

    test_basic<int>();
    test_basic<unsigned>();
    test_basic<bool>();
    test_basic<char>();
    test_basic<double>();          // <-- double: basic ops only
    test_basic<std::string>();
    test_basic<std::string_view>();
    test_basic<std::vector<int>>();

    test_in_place<int>();
    test_in_place<std::string>();
}

}
