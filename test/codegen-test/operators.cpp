#include <opt/option.hpp>
#include <cstdint>
#include <optional>

//'[option_int16_equal]
//'@if $clang
//' mov eax, ecx
//' shr ecx, 16
//' mov r8d, edx
//' shr edx, 16
//' cmp cl, dl
//' sete dl
//' xor cl, 1
//' cmp ax, r8w
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $gcc
//' mov eax, ecx
//' shr ecx, 16
//' mov r8d, ecx
//' cmp ax, dx
//' sete al
//' xor r8d, 1
//' shr edx, 16
//' or eax, r8d
//' cmp dl, cl
//' sete dl
//' and eax, edx
//' ret
//'@if $clang-cl
//' mov r8d, ecx
//' shr r8d, 16
//' mov eax, edx
//' shr eax, 16
//' cmp r8b, al
//' sete r9b
//' xor r8b, 1
//' cmp cx, dx
//' sete al
//' or al, r8b
//' and al, r9b
//' ret
//'@if $msvc
//' mov r8d, ecx
//' shr r8d, 16
//' cmp cx, dx
//' sete al
//' test r8b, r8b
//' sete cl
//' shr edx, 16
//' xor dl, r8b
//' or al, cl
//' xor dl, 1
//' and al, dl
//' ret
bool option_int16_equal(opt::option<int16_t> a, opt::option<int16_t> b) {
    return a == b;
}

//'[std_optional_int16_equal]
//'@if $clang
//' mov r8d, ecx
//' shr r8d, 16
//' mov eax, edx
//' shr eax, 16
//' cmp r8b, al
//' sete al
//' and r8b, al
//' cmp r8b, 1
//' jne .L0
//' cmp cx, dx
//' sete al
//' .L0:
//' ret
//'@if $gcc
//' xor eax, eax
//' mov r8d, ecx
//' mov [rsp + 8], ecx
//' mov ecx, edx
//' shr r8d, 16
//' shr ecx, 16
//' mov [rsp + 16], edx
//' cmp r8b, cl
//' jne .L0
//' mov eax, 1
//' test r8b, r8b
//' je .L0
//' movzx eax, [rsp + 16]
//' cmp [rsp + 8], ax
//' sete al
//' .L0:
//' ret
//'@if $clang-cl
//' mov eax, ecx
//' and eax, 65536
//' test eax, edx
//' je .L0
//' cmp cx, dx
//' sete al
//' ret
//' .L0:
//' test eax, eax
//' setne cl
//' test edx, 65536
//' sete al
//' xor al, cl
//' ret
//'@if $msvc
//' mov r8d, ecx
//' mov eax, edx
//' shr r8d, 16
//' shr eax, 16
//' test r8b, r8b
//' je .L0
//' test al, al
//' je .L0
//' cmp cx, dx
//' sete al
//' ret
//' .L0:
//' cmp r8b, al
//' sete al
//' ret
bool std_optional_int16_equal(std::optional<int16_t> a, std::optional<int16_t> b) {
    return a == b;
}

//'[option_int32_equal]
//'@if $clang
//' mov eax, ecx
//' shr rcx, 32
//' mov r8d, edx
//' shr rdx, 32
//' cmp cl, dl
//' sete dl
//' xor cl, 1
//' cmp eax, r8d
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $gcc
//' mov rax, rcx
//' shr rcx, 32
//' mov r8d, ecx
//' cmp eax, edx
//' sete al
//' xor r8d, 1
//' shr rdx, 32
//' or eax, r8d
//' cmp dl, cl
//' sete dl
//' and eax, edx
//' ret
//'@if $clang-cl
//' mov r8, rcx
//' shr r8, 32
//' mov rax, rdx
//' shr rax, 32
//' cmp r8b, al
//' sete r9b
//' xor r8b, 1
//' cmp ecx, edx
//' sete al
//' or al, r8b
//' and al, r9b
//' ret
//'@if $msvc
//' mov r8, rcx
//' shr r8, 32
//' cmp ecx, edx
//' sete al
//' test r8b, r8b
//' sete cl
//' shr rdx, 32
//' xor dl, r8b
//' or al, cl
//' xor dl, 1
//' and al, dl
//' ret
bool option_int32_equal(opt::option<int32_t> a, opt::option<int32_t> b) {
    return a == b;
}

//'[std_optional_int32_equal]
//'@if $clang
//' mov r8, rcx
//' shr r8, 32
//' mov rax, rdx
//' shr rax, 32
//' cmp r8b, al
//' sete al
//' and r8b, al
//' cmp r8b, 1
//' jne .L0
//' cmp ecx, edx
//' sete al
//' .L0:
//' ret
//'@if $gcc
//' xor eax, eax
//' mov r8, rcx
//' mov [rsp + 8], rcx
//' mov rcx, rdx
//' shr r8, 32
//' shr rcx, 32
//' mov [rsp + 16], rdx
//' cmp r8b, cl
//' jne .L0
//' mov eax, 1
//' test r8b, r8b
//' je .L0
//' mov eax, [rsp + 16]
//' cmp [rsp + 8], eax
//' sete al
//' .L0:
//' ret
//'@if $clang-cl
//' mov rax, 4294967296
//' mov r8, rcx
//' and r8, rax
//' test r8, rdx
//' je .L0
//' cmp ecx, edx
//' sete al
//' ret
//' .L0:
//' test r8, r8
//' setne cl
//' test rdx, rax
//' sete al
//' xor al, cl
//' ret
//'@if $msvc
//' mov r8, rcx
//' mov rax, rdx
//' shr r8, 32
//' shr rax, 32
//' test r8b, r8b
//' je .L0
//' test al, al
//' je .L0
//' cmp ecx, edx
//' sete al
//' ret
//' .L0:
//' cmp r8b, al
//' sete al
//' ret
bool std_optional_int32_equal(std::optional<int32_t> a, std::optional<int32_t> b) {
    return a == b;
}

//'[opt_option_int64_equal]
//'@if $clang | $clang-cl
//' movzx r8d, [rcx + 8]
//' cmp r8b, [rdx + 8]
//' sete r9b
//' xor r8b, 1
//' mov rax, [rcx]
//' cmp rax, [rdx]
//' sete al
//' or al, r8b
//' and al, r9b
//' ret
//'@if $gcc
//' mov rax, rcx
//' mov rcx, rdx
//' movzx edx, [rax + 8]
//' mov r9, [rcx]
//' cmp [rax], r9
//' mov r8d, edx
//' sete al
//' xor r8d, 1
//' or eax, r8d
//' cmp [rcx + 8], dl
//' sete dl
//' and eax, edx
//' ret
//'@if $msvc
//' movzx r8d, [rcx + 8]
//' mov rax, [rdx]
//' cmp [rcx], rax
//' sete al
//' test r8b, r8b
//' sete cl
//' xor r8b, [rdx + 8]
//' xor r8b, 1
//' or al, cl
//' and al, r8b
//' ret
bool opt_option_int64_equal(const opt::option<int64_t>& a, const opt::option<int64_t>& b) {
    return a == b;
}

//'[std_optional_int64_equal]
//'@if $clang
//' movzx r8d, [rcx + 8]
//' cmp r8b, [rdx + 8]
//' sete al
//' and r8b, al
//' cmp r8b, 1
//' jne .L0
//' mov rax, [rcx]
//' cmp rax, [rdx]
//' sete al
//' .L0:
//' ret
//'@if $gcc
//' xor eax, eax
//' mov r8, rcx
//' mov rcx, rdx
//' movzx edx, [r8 + 8]
//' cmp dl, [rcx + 8]
//' jne .L0
//' mov eax, 1
//' test dl, dl
//' je .L0
//' mov rax, [rcx]
//' cmp [r8], rax
//' sete al
//' .L0:
//' ret
//'@if $clang-cl
//' movzx eax, [rcx + 8]
//' movzx r8d, [rdx + 8]
//' mov r9d, eax
//' and r9b, r8b
//' cmp r9b, 1
//' jne .L0
//' mov rax, [rcx]
//' cmp rax, [rdx]
//' sete al
//' ret
//' .L0:
//' xor al, r8b
//' xor al, 1
//' ret
//'@if $msvc
//' movzx r8d, [rcx + 8]
//' movzx eax, [rdx + 8]
//' test r8b, r8b
//' je .L0
//' test al, al
//' je .L0
//' mov rax, [rdx]
//' cmp [rcx], rax
//' sete al
//' ret
//' .L0:
//' cmp r8b, al
//' sete al
//' ret
bool std_optional_int64_equal(const std::optional<int64_t>& a, const std::optional<int64_t>& b) {
    return a == b;
}

//'[option_int32_equal_with_value]
//'@if $clang
//' mov eax, ecx
//' shr rcx, 32
//' and ecx, 1
//' cmp edx, eax
//' sete al
//' and al, cl
//' ret
//'@if $gcc
//' cmp edx, ecx
//' sete al
//' shr rcx, 32
//' and eax, ecx
//' ret
//'@if $clang-cl
//' mov r8, rcx
//' shr r8, 32
//' and r8d, 1
//' cmp ecx, edx
//' sete al
//' and al, r8b
//' ret
//'@if $msvc
//' cmp ecx, edx
//' sete al
//' shr rcx, 32
//' and al, cl
//' ret
bool option_int32_equal_with_value(opt::option<int32_t> a, int32_t b) {
    return a == b;
}

//'[std_optional_int32_equal_with_value]
//'@if $clang
//' mov eax, ecx
//' shr rcx, 32
//' cmp edx, eax
//' sete al
//' and al, cl
//' ret
//'@if $gcc
//' mov rax, rcx
//' mov [rsp + 8], rcx
//' shr rax, 32
//' test al, al
//' je .L0
//' cmp [rsp + 8], edx
//' sete al
//' .L0:
//' ret
//'@if $clang-cl
//' mov r8, rcx
//' shr r8, 32
//' cmp ecx, edx
//' sete al
//' and al, r8b
//' ret
//'@if $msvc
//' mov rax, rcx
//' shr rax, 32
//' test al, al
//' je .L0
//' cmp ecx, edx
//' sete al
//' ret
//' .L0:
//' xor al, al
//' ret
bool std_optional_int32_equal_with_value(std::optional<int32_t> a, int32_t b) {
    return a == b;
}

//'[option_int64_equal_with_value]
//'@if $clang | $clang-cl | $gcc | $msvc
//' cmp [rcx], rdx
//' sete al
//' and al, [rcx + 8]
//' ret
bool option_int64_equal_with_value(const opt::option<int64_t>& a, int64_t b) {
    return a == b;
}

//'[std_optional_int64_equal_with_value]
//'@if $clang | $clang-cl
//' cmp [rcx], rdx
//' sete al
//' and al, [rcx + 8]
//' ret
//'@if $gcc
//' movzx eax, [rcx + 8]
//' test al, al
//' je .L0
//' cmp [rcx], rdx
//' sete al
//' .L0:
//' ret
//'@if $msvc
//' cmp byte ptr [rcx + 8], 0
//' je .L0
//' cmp [rcx], rdx
//' sete al
//' ret
//' .L0:
//' xor al, al
//' ret
bool std_optional_int64_equal_with_value(const std::optional<int64_t>& a, int64_t b) {
    return a == b;
}

//'[option_int16_less]
//'@if $clang
//' test edx, 65536
//' jne .L0
//' xor eax, eax
//' ret
//' .L0:
//' test ecx, 65536
//' sete r8b
//' cmp cx, dx
//' setl al
//' or al, r8b
//' ret
//'@if $gcc
//' mov eax, edx
//' mov r8d, ecx
//' mov ecx, edx
//' shr eax, 16
//' mov edx, eax
//' test al, al
//' je .L0
//' test r8d, 16711680
//' je .L0
//' cmp r8w, cx
//' setl dl
//' .L0:
//' mov eax, edx
//' ret
//'@if $clang-cl
//' test edx, 65536
//' jne .L0
//' xor eax, eax
//' ret
//' .L0:
//' test ecx, 65536
//' sete r8b
//' cmp cx, dx
//' setl al
//' or al, r8b
//' ret
//'@if $msvc
//' mov eax, edx
//' shr eax, 16
//' test al, al
//' jne .L0
//' ret
//' .L0:
//' mov eax, ecx
//' shr eax, 16
//' test al, al
//' jne .L1
//' mov al, 1
//' ret
//' .L1:
//' cmp cx, dx
//' setl al
//' ret
bool option_int16_less(opt::option<int16_t> a, opt::option<int16_t> b) {
    return a < b;
}

//'[std_optional_int16_less]
//'@if $clang
//' test edx, 65536
//' jne .L0
//' xor eax, eax
//' ret
//' .L0:
//' test ecx, 65536
//' sete r8b
//' cmp cx, dx
//' setl al
//' or al, r8b
//' ret
//'@if $gcc
//' mov eax, edx
//' mov [rsp + 8], ecx
//' shr eax, 16
//' and ecx, 16711680
//' mov [rsp + 16], edx
//' je .L0
//' test al, al
//' je .L0
//' movzx eax, [rsp + 8]
//' cmp [rsp + 16], ax
//' setg al
//' .L0:
//' ret
//'@if $clang-cl
//' mov eax, ecx
//' and eax, 65536
//' test eax, edx
//' je .L0
//' cmp cx, dx
//' setl al
//' ret
//' .L0:
//' shr edx, 16
//' and edx, 1
//' test eax, eax
//' sete al
//' and al, dl
//' ret
//'@if $msvc
//' mov r8d, ecx
//' mov eax, edx
//' shr r8d, 16
//' shr eax, 16
//' test r8b, r8b
//' je .L0
//' test al, al
//' je .L0
//' cmp cx, dx
//' setl al
//' ret
//' .L0:
//' cmp r8b, al
//' setb al
//' ret
bool std_optional_int16_less(std::optional<int16_t> a, std::optional<int16_t> b) {
    return a < b;
}

//'[option_int32_less]
//'@if $clang
//' bt rdx, 32
//' jb .L0
//' xor eax, eax
//' ret
//' .L0:
//' mov rax, 4294967296
//' test rcx, rax
//' sete r8b
//' cmp ecx, edx
//' setl al
//' or al, r8b
//' ret
//'@if $gcc
//' mov rax, rdx
//' mov r8, rcx
//' mov rcx, rdx
//' shr rax, 32
//' mov edx, eax
//' test al, al
//' je .L0
//' mov rax, r8
//' shr rax, 32
//' test al, al
//' je .L0
//' cmp r8d, ecx
//' setl dl
//' .L0:
//' mov eax, edx
//' ret
//'@if $clang-cl
//' bt rdx, 32
//' jb .L0
//' xor eax, eax
//' ret
//' .L0:
//' mov rax, 4294967296
//' test rcx, rax
//' sete r8b
//' cmp ecx, edx
//' setl al
//' or al, r8b
//' ret
//'@if $msvc
//' mov rax, rdx
//' shr rax, 32
//' test al, al
//' jne .L0
//' ret
//' .L0:
//' mov rax, rcx
//' shr rax, 32
//' test al, al
//' jne .L1
//' mov al, 1
//' ret
//' .L1:
//' cmp ecx, edx
//' setl al
//' ret
bool option_int32_less(opt::option<int32_t> a, opt::option<int32_t> b) {
    return a < b;
}

//'[std_optional_int32_less]
//'@if $clang
//' bt rdx, 32
//' jb .L0
//' xor eax, eax
//' ret
//' .L0:
//' mov rax, 4294967296
//' test rcx, rax
//' sete r8b
//' cmp ecx, edx
//' setl al
//' or al, r8b
//' ret
//'@if $gcc
//' mov r8, rcx
//' mov rax, rdx
//' mov [rsp + 8], rcx
//' shr r8, 32
//' mov [rsp + 16], rdx
//' shr rax, 32
//' test r8b, r8b
//' je .L0
//' test al, al
//' je .L0
//' mov eax, [rsp + 16]
//' cmp [rsp + 8], eax
//' setl al
//' .L0:
//' ret
//'@if $clang-cl
//' mov r8, 4294967296
//' mov rax, rcx
//' and rax, r8
//' test rax, rdx
//' je .L0
//' cmp ecx, edx
//' setl al
//' ret
//' .L0:
//' test rdx, r8
//' setne cl
//' test rax, rax
//' sete al
//' and al, cl
//' ret
//'@if $msvc
//' mov r8, rcx
//' mov rax, rdx
//' shr r8, 32
//' shr rax, 32
//' test r8b, r8b
//' je .L0
//' test al, al
//' je .L0
//' cmp ecx, edx
//' setl al
//' ret
//' .L0:
//' cmp r8b, al
//' setb al
//' ret
bool std_optional_int32_less(std::optional<int32_t> a, std::optional<int32_t> b) {
    return a < b;
}
