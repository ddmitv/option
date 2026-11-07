#include <opt/option.hpp>
#include <utility>

namespace {

struct from_tag {};

// not convertible to int
struct not_convertible_lvalue {};
struct not_convertible_rvalue {};

// constructible from from_tag, but not copyable
struct no_copy {
    no_copy() = default;
    no_copy(const no_copy&) = delete;
    no_copy(no_copy&&) = delete;
    no_copy(from_tag) {}
};

// constructible from from_tag, but not movable
struct no_move {
    no_move() = default;
    no_move(const no_move&) = delete;
    no_move(no_move&&) = delete;
    no_move(from_tag) {}
};

[[maybe_unused]] void test() {
    {
        opt::option<int> opt;
        // error: opt::option::value_or: U must be convertible to remove_cv_t<T>
        // error[clang]: no viable conversion from returned value of type {{.*}}not_convertible_lvalue{{.*}}
        // error[gcc]: cannot convert {{.*}}not_convertible_lvalue{{.*}} in return
        // error[msvc]: {{.*}}return{{.*}}not_convertible_lvalue
        (void)opt.value_or(not_convertible_lvalue{});
    }
    {
        opt::option<no_copy> opt;
        // error: opt::option::value_or: const T& must be convertible to remove_cv_t<T>
        // error[clang]: call to deleted constructor of {{.*}}no_copy
        // error[gcc]: use of deleted function {{.*}}no_copy::no_copy
        (void)opt.value_or(from_tag{});
    }
    {
        opt::option<int> opt;
        // error: opt::option::value_or: U must be convertible to remove_cv_t<T>
        // error[clang]: no viable conversion from returned value of type {{.*}}not_convertible_rvalue{{.*}}
        // error[gcc]: cannot convert {{.*}}not_convertible_rvalue{{.*}} in return
        // error[msvc]: {{.*}}return{{.*}}not_convertible_rvalue
        (void)std::move(opt).value_or(not_convertible_rvalue{});
    }
    {
        opt::option<no_move> opt;
        // error: opt::option::value_or: T must be convertible to remove_cv_t<T>
        // error[clang]: call to deleted constructor of {{.*}}no_move
        // error[gcc]: use of deleted function {{.*}}no_move::no_move
        (void)std::move(opt).value_or(from_tag{});
    }
}

} // namespace
