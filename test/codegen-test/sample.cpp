#include <opt/option.hpp>
#include <optional>
#include <utility>
#include <cstdint>
#include <array>

struct int64x2 {
    int64_t x;
    int64_t y;
};

//'[third_fn]
//'@if $linux & $clang
//' movsxd rax, edx
//' mov dword ptr [rdi + rax*4 + 8], 6
//' ret
//'@if $windows & $gcc
//' movsxd r8, r8d
//' mov dword ptr [rcx + r8*4 + 8], 6
//' ret
//'@if $windows & ($msvc | $clang-cl | $clang)
//' movsxd rax, r8d
//' mov dword ptr [rcx + rax*4 + 8], 6
//' ret
void third_fn(int* x, int y, int z) {
    x[z + 2] = 6;
}

//'[option_make_from_int32_sum]
//'@if $msvc
//' lea eax, [rdx + r8]
//' mov byte ptr [rcx + 4], 1
//' mov [rcx], eax
//' mov rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' add edx, r8d
//' mov [rcx], edx
//' mov byte ptr [rcx + 4], 1
//' ret
//'@if $linux & $clang[16..]
//' lea ecx, [rdi + rsi]
//' mov rax, 4294967296
//' or rax, rcx
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' add esi, edx
//' mov [rdi], esi
//' mov byte ptr [rdi + 4], 1
//' ret
//'@if $windows & $gcc
//' lea eax, [rcx + rdx]
//' bts rax, 32
//' ret
//'@if $windows & $clang
//' add ecx, edx
//' mov rax, 4294967296
//' or rax, rcx
//' ret
opt::option<int32_t> option_make_from_int32_sum(int32_t x, int32_t y) {
    return x + y;
}

//'[option_make_from_int64_sum]
//'@if $msvc
//' lea rax, [rdx + r8]
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rax
//' mov rax, rcx
//' ret
//'@if $linux & $clang[16..]
//' lea rax, [rdi + rsi]
//' mov dl, 1
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' add rsi, rdx
//' mov [rdi], rsi
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $windows & $gcc
//' mov rax, rcx
//' add rdx, r8
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rdx
//' ret
//'@if $windows & ($clang-cl | $clang)
//' mov rax, rcx
//' add rdx, r8
//' mov [rcx], rdx
//' mov byte ptr [rcx + 8], 1
//' ret
opt::option<int64_t> option_make_from_int64_sum(int64_t x, int64_t y) {
    return x + y;
}

//'[std_optional_make_from_int32_sum]
//'@if $msvc
//' lea eax, [rdx + r8]
//' mov byte ptr [rcx + 4], 1
//' mov [rcx], eax
//' mov rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' add edx, r8d
//' mov [rcx], edx
//' mov byte ptr [rcx + 4], 1
//' ret
//'@if $linux & $clang
//' lea ecx, [rdi + rsi]
//' mov rax, 4294967296
//' or rax, rcx
//' ret
//'@if $windows & $gcc
//' sub rsp, 24
//' add ecx, edx
//' mov byte ptr [rsp + 12], 1
//' mov [rsp + 8], ecx
//' mov rax, [rsp + 8]
//' add rsp, 24
//' ret
//'@if $windows & $clang
//' add ecx, edx
//' mov rax, 4294967296
//' or rax, rcx
//' ret
std::optional<int32_t> std_optional_make_from_int32_sum(int32_t x, int32_t y) {
    return x + y;
}

//'[std_optional_make_from_int64_sum]
//'@if $msvc
//' lea rax, [rdx + r8]
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rax
//' mov rax, rcx
//' ret
//'@if $linux & $clang
//' lea rax, [rdi + rsi]
//' mov dl, 1
//' ret
//'@if $windows & $gcc
//' mov rax, rcx
//' add rdx, r8
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rdx
//' ret
//'@if $windows & ($clang-cl | $clang)
//' mov rax, rcx
//' add rdx, r8
//' mov [rcx], rdx
//' mov byte ptr [rcx + 8], 1
//' ret
std::optional<int64_t> std_optional_make_from_int64_sum(int64_t x, int64_t y) {
    return x + y;
}

//'[option_bool_empty]
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $clang[16..] | $gcc
//' xor eax, eax
//' ret
//'@if $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 1], 0
//' ret
opt::option<bool> option_bool_empty() {
    return opt::none;
}

//'[option_int32_empty]
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $clang[16..] | $gcc
//' xor eax, eax
//' ret
//'@if $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 4], 0
//' ret
opt::option<int32_t> option_int32_empty() {
    return opt::none;
}

//'[option_int64_empty]
//'@if $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $linux & $clang[16..]
//' xor edx, edx
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $windows & ($clang-cl | $clang | $gcc)
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
opt::option<int64_t> option_int64_empty() {
    return opt::none;
}

//'[std_optional_bool_empty]
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $clang | $gcc
//' xor eax, eax
//' ret
std::optional<bool> std_optional_bool_empty() {
    return std::nullopt;
}

//'[std_optional_int32_empty]
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $gcc
//' sub rsp, 24
//' xor eax, eax
//' add rsp, 24
//' ret
//'@if $clang
//' xor eax, eax
//' ret
std::optional<int32_t> std_optional_int32_empty() {
    return std::nullopt;
}

//'[std_optional_int64_empty]
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $linux & $clang
//' xor edx, edx
//' ret
//'@if $windows & $gcc
//' pxor xmm0, xmm0
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $windows & $clang
//' mov rax, rcx
//' mov byte ptr [rcx], 0
//' mov byte ptr [rcx + 8], 0
//' ret
std::optional<int64_t> std_optional_int64_empty() {
    return std::nullopt;
}

//'[option_bool_default]
//'@if $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $clang[16..] | $gcc
//' xor eax, eax
//' ret
//'@if $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 1], 0
//' ret
opt::option<bool> option_bool_default() {
    return {};
}

//'[std_optional_bool_default]
//'@if $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $clang | $gcc
//' xor eax, eax
//' ret
std::optional<bool> std_optional_bool_default() {
    return {};
}

//'[option_int32_default]
//'@if $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $clang[16..] | $gcc
//' xor eax, eax
//' ret
//'@if $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 4], 0
//' ret
opt::option<int32_t> option_int32_default() {
    return {};
}

//'[std_optional_int32_default]
//'@if $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $clang
//' xor eax, eax
//' ret
//'@if $gcc
//' sub rsp, 24
//' xor eax, eax
//' add rsp, 24
//' ret
std::optional<int32_t> std_optional_int32_default() {
    return {};
}

//'[option_int64_default]
//'@if $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $linux & $clang[16..]
//' xor edx, edx
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl)
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
opt::option<int64_t> option_int64_default() {
    return {};
}

//'[std_optional_int64_default]
//'@if $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $linux & $clang
//' xor edx, edx
//' ret
//'@if $windows & $clang
//' mov rax, rcx
//' mov byte ptr [rcx], 0
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $windows & $gcc
//' pxor xmm0, xmm0
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
std::optional<int64_t> std_optional_int64_default() {
    return {};
}

//'[option_uint64_reset]
//'@if $linux & $clang
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $windows & ($clang | $clang-cl | $gcc | $msvc)
//' mov byte ptr [rcx + 8], 0
//' ret
void option_uint64_reset(opt::option<uint64_t>& a) {
    a.reset();
}

//'[std_optional_uint64_reset]
//'@if $linux & $clang[17..]
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $linux & $clang[..17]
//' cmp byte ptr [rdi + 8], 0
//' je .L0
//' mov byte ptr [rdi + 8], 0
//' .L0:
//' ret
//'@if $windows & $clang
//' cmp byte ptr [rcx + 8], 1
//' jne .L0
//' mov byte ptr [rcx + 8], 0
//' .L0:
//' ret
//'@if $windows & ($gcc | $msvc | $clang-cl)
//' mov byte ptr [rcx + 8], 0
//' ret
void std_optional_uint64_reset(std::optional<uint64_t>& a) {
    a.reset();
}

//'[option_int64x2_reset]
//'@if $linux & $clang
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $windows & ($clang | $clang-cl | $msvc | $gcc)
//' mov byte ptr [rcx + 16], 0
//' ret
void option_int64x2_reset(opt::option<int64x2>& a) {
    a.reset();
}

//'[std_optional_int64x2_reset]
//'@if $linux & $clang[17..]
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $linux & $clang[..17]
//' cmp byte ptr [rdi + 16], 0
//' je .L0
//' mov byte ptr [rdi + 16], 0
//' .L0:
//' ret
//'@if $windows & $clang
//' cmp byte ptr [rcx + 16], 1
//' jne .L0
//' mov byte ptr [rcx + 16], 0
//' .L0:
//' ret
//'@if $windows & ($clang-cl | $msvc | $gcc)
//' mov byte ptr [rcx + 16], 0
//' ret
void std_optional_int64x2_reset(std::optional<int64x2>& a) {
    a.reset();
}

//'[option_uint64_has_value]
//'@if $linux & $clang
//' movzx eax, [rdi + 8]
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 8]
//' ret
bool option_uint64_has_value(const opt::option<uint64_t>& a) {
    return a.has_value();
}

//'[std_optional_uint64_has_value]
//'@if $linux & $clang
//' movzx eax, [rdi + 8]
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 8]
//' ret
bool std_optional_uint64_has_value(const std::optional<uint64_t>& a) {
    return a.has_value();
}

//'[option_int64x2_has_value]
//'@if $linux & $clang
//' movzx eax, [rdi + 16]
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 16]
//' ret
bool option_int64x2_has_value(const opt::option<int64x2>& a) {
    return a.has_value();
}

//'[std_optional_int64x2_has_value]
//'@if $linux & $clang
//' movzx eax, [rdi + 16]
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 16]
//' ret
bool std_optional_int64x2_has_value(const std::optional<int64x2>& a) {
    return a.has_value();
}

//'[option_int64_assign_from_value]
//'@if $linux & $clang
//' mov qword ptr [rdi], 10
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $windows & ($clang | $clang-cl | $msvc | $gcc)
//' mov qword ptr [rcx], 10
//' mov byte ptr [rcx + 8], 1
//' ret
void option_int64_assign_from_value(opt::option<int64_t>& a) {
    a = 10;
}

//'[std_optional_int64_assign_from_value]
//'@if $linux & $clang[19..]
//' cmp byte ptr [rdi + 8], 0
//' jne .L0
//' mov byte ptr [rdi + 8], 1
//' .L0:
//' mov qword ptr [rdi], 10
//' ret
//'@if $linux & $clang[16..19]
//' cmp byte ptr [rdi + 8], 0
//' je .L0
//' mov qword ptr [rdi], 10
//' ret
//' .L0:
//' mov byte ptr [rdi + 8], 1
//' mov qword ptr [rdi], 10
//' ret
//'@if $linux & $clang[..16]
//' cmp byte ptr [rdi + 8], 0
//' mov qword ptr [rdi], 10
//' je .L0
//' ret
//' .L0:
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl)
//' cmp byte ptr [rcx + 8], 0
//' jne .L0
//' mov byte ptr [rcx + 8], 1
//' .L0:
//' mov qword ptr [rcx], 10
//' ret
//'@if $msvc
//' mov qword ptr [rcx], 10
//' cmp byte ptr [rcx + 8], 0
//' jne .L0
//' mov byte ptr [rcx + 8], 1
//' .L0:
//' ret
void std_optional_int64_assign_from_value(std::optional<int64_t>& a) {
    a = 10;
}

//'[option_int64_assign_from_other]
//'@if $linux & $clang
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $windows & ($clang | $clang-cl | $msvc)
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
void option_int64_assign_from_other(opt::option<int64_t>& a, const opt::option<int64_t>& b) {
    a = b;
}

//'[std_optional_int64_assign_from_other]
//'@if $linux & $clang
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $windows & $clang
//' movzx eax, [rdx + 8]
//' mov [rcx + 8], al
//' mov rax, [rdx]
//' mov [rcx], rax
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $clang-cl | $msvc
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
void std_optional_int64_assign_from_other(std::optional<int64_t>& a, const std::optional<int64_t>& b) {
    a = b;
}

//'[option_int64x2_assign_from_other]
//'@if $linux & $clang
//' mov rax, [rsi + 16]
//' mov [rdi + 16], rax
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' ret
void option_int64x2_assign_from_other(opt::option<int64x2>& a, const opt::option<int64x2>& b) {
    a = b;
}

//'[std_optional_int64x2_assign_from_other]
//'@if $linux & $clang
//' mov rax, [rsi + 16]
//' mov [rdi + 16], rax
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $windows & $clang
//' movzx eax, [rdx + 16]
//' mov [rcx + 16], al
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' ret
//'@if $clang-cl
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' ret
void std_optional_int64x2_assign_from_other(std::optional<int64x2>& a, const std::optional<int64x2>& b) {
    a = b;
}

//'[option_int64_copy]
//'@if $linux & $clang[16..]
//' mov rax, [rdi]
//' movzx edx, [rdi + 8]
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
opt::option<int64_t> option_int64_copy(const opt::option<int64_t>& a) {
    return a;
}

//'[std_optional_int64_copy]
//'@if $linux & $clang
//' mov rax, [rdi]
//' movzx edx, [rdi + 8]
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
std::optional<int64_t> std_optional_int64_copy(const std::optional<int64_t>& a) {
    return a;
}

//'[option_int64x2_copy]
//'@if $linux & $clang
//' mov rax, rdi
//' mov rcx, [rsi + 16]
//' mov [rdi + 16], rcx
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' mov rcx, [rdx + 16]
//' mov [rax + 16], rcx
//' movups xmm0, [rdx]
//' movups [rax], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rdx, [rdx + 16]
//' mov rax, rcx
//' mov [rcx + 16], rdx
//' movups [rcx], xmm0
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movsd xmm1, [rdx + 16]
//' movups [rcx], xmm0
//' movsd [rcx + 16], xmm1
//' ret
opt::option<int64x2> option_int64x2_copy(const opt::option<int64x2>& a) {
    return a;
}

//'[std_optional_int64x2_copy]
//'@if $linux & $clang
//' mov rax, rdi
//' mov rcx, [rsi + 16]
//' mov [rdi + 16], rcx
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' mov rcx, [rdx + 16]
//' mov [rax + 16], rcx
//' movups xmm0, [rdx]
//' movups [rax], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rdx, [rdx + 16]
//' mov rax, rcx
//' mov [rcx + 16], rdx
//' movups [rcx], xmm0
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movsd xmm1, [rdx + 16]
//' movups [rcx], xmm0
//' movsd [rcx + 16], xmm1
//' ret
std::optional<int64x2> std_optional_int64x2_copy(const std::optional<int64x2>& a) {
    return a;
}

//'[option_uint8x64_copy]
//'@if $linux & $clang
//' mov rax, rdi
//' movzx ecx, [rsi + 64]
//' mov [rdi + 64], cl
//' movups xmm0, [rsi]
//' movups xmm1, [rsi + 16]
//' movups xmm2, [rsi + 32]
//' movups xmm3, [rsi + 48]
//' movups [rdi + 48], xmm3
//' movups [rdi + 32], xmm2
//' movups [rdi + 16], xmm1
//' movups [rdi], xmm0
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' movzx ecx, [rdx + 64]
//' mov [rax + 64], cl
//' movups xmm0, [rdx]
//' movups xmm1, [rdx + 16]
//' movups xmm2, [rdx + 32]
//' movups xmm3, [rdx + 48]
//' movups [rax + 48], xmm3
//' movups [rax + 32], xmm2
//' movups [rax + 16], xmm1
//' movups [rax], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' movdqu xmm0, [rdx + 16]
//' mov rax, rcx
//' movups [rcx + 16], xmm0
//' movdqu xmm0, [rdx + 32]
//' movups [rcx + 32], xmm0
//' movdqu xmm0, [rdx + 48]
//' movzx edx, [rdx + 64]
//' movups [rcx + 48], xmm0
//' mov [rcx + 64], dl
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' movzx eax, [rdx + 64]
//' movups xmm1, [rdx + 16]
//' movups [rcx], xmm0
//' movups xmm0, [rdx + 32]
//' movups [rcx + 16], xmm1
//' movups xmm1, [rdx + 48]
//' movups [rcx + 32], xmm0
//' movups [rcx + 48], xmm1
//' mov [rcx + 64], al
//' mov rax, rcx
//' ret
opt::option<std::array<uint8_t, 64>> option_uint8x64_copy(const opt::option<std::array<uint8_t, 64>>& a) {
    return a;
}

//'[std_optional_uint8x64_copy]
//'@if $linux & $clang
//' mov rax, rdi
//' movzx ecx, [rsi + 64]
//' mov [rdi + 64], cl
//' movups xmm0, [rsi]
//' movups xmm1, [rsi + 16]
//' movups xmm2, [rsi + 32]
//' movups xmm3, [rsi + 48]
//' movups [rdi + 48], xmm3
//' movups [rdi + 32], xmm2
//' movups [rdi + 16], xmm1
//' movups [rdi], xmm0
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' movzx ecx, [rdx + 64]
//' mov [rax + 64], cl
//' movups xmm0, [rdx]
//' movups xmm1, [rdx + 16]
//' movups xmm2, [rdx + 32]
//' movups xmm3, [rdx + 48]
//' movups [rax + 48], xmm3
//' movups [rax + 32], xmm2
//' movups [rax + 16], xmm1
//' movups [rax], xmm0
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' movdqu xmm0, [rdx + 16]
//' mov rax, rcx
//' movups [rcx + 16], xmm0
//' movdqu xmm0, [rdx + 32]
//' movups [rcx + 32], xmm0
//' movdqu xmm0, [rdx + 48]
//' movzx edx, [rdx + 64]
//' movups [rcx + 48], xmm0
//' mov [rcx + 64], dl
//' ret
//'@if $msvc
//' movups xmm0, [rdx]
//' movzx eax, [rdx + 64]
//' movups xmm1, [rdx + 16]
//' movups [rcx], xmm0
//' movups xmm0, [rdx + 32]
//' movups [rcx + 16], xmm1
//' movups xmm1, [rdx + 48]
//' movups [rcx + 32], xmm0
//' movups [rcx + 48], xmm1
//' mov [rcx + 64], al
//' mov rax, rcx
//' ret
std::optional<std::array<uint8_t, 64>> std_optional_uint8x64_copy(const std::optional<std::array<uint8_t, 64>>& a) {
    return a;
}

//'[option_uint8x64_default]
//'@if $linux & $clang
//' mov rax, rdi
//' mov byte ptr [rdi + 64], 0
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl)
//' mov rax, rcx
//' mov byte ptr [rcx + 64], 0
//' ret
//'@if $msvc
//' mov byte ptr [rcx + 64], 0
//' mov rax, rcx
//' ret
opt::option<std::array<uint8_t, 64>> option_uint8x64_default() {
    return {};
}

//'[std_optional_uint8x64_default]
//'@if $linux & $clang
//' mov rax, rdi
//' mov byte ptr [rdi + 64], 0
//' ret
//'@if $windows & $clang
//' mov rax, rcx
//' mov byte ptr [rcx], 0
//' mov byte ptr [rcx + 64], 0
//' ret
//'@if $windows & $gcc
//' pxor xmm0, xmm0
//' mov rax, rcx
//' mov byte ptr [rcx + 64], 0
//' movups [rcx], xmm0
//' movups [rcx + 16], xmm0
//' movups [rcx + 32], xmm0
//' movups [rcx + 48], xmm0
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 64], 0
//' ret
//'@if $msvc
//' mov byte ptr [rcx + 64], 0
//' mov rax, rcx
//' ret
std::optional<std::array<uint8_t, 64>> std_optional_uint8x64_default() {
    return {};
}

//'[option_int32_to_uint32_ctor]
//'@if $linux & $clang[16..]
//' mov rax, 1099511627775
//' and rax, rdi
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' mov ecx, [rsp + 8]
//' mov [rdi], ecx
//' movzx ecx, [rsp + 12]
//' mov [rdi + 4], cl
//' ret
//'@if $windows & ($clang | $gcc)
//' mov rax, 1099511627775
//' and rax, rcx
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov [rcx], edx
//' shr rdx, 32
//' mov [rcx + 4], dl
//' ret
//'@if $msvc
//' mov [rcx], edx
//' mov rax, rcx
//' shr rdx, 32
//' mov [rcx + 4], dl
//' ret
opt::option<uint32_t> option_int32_to_uint32_ctor(const opt::option<int32_t> a) {
    return a;
}

//'[std_optional_int32_to_uint32_ctor]
//'@if $linux & $clang[19..]
//' mov rax, 8589934591
//' and rax, rdi
//' ret
//'@if $linux & $clang[..19]
//' mov eax, edi
//' shr rdi, 32
//' xor ecx, ecx
//' test dil, dil
//' setne cl
//' shl rcx, 32
//' or rax, rcx
//' ret
//'@if $windows & $clang
//' mov rdx, 4294967296
//' and rdx, rcx
//' mov rax, rcx
//' shl rax, 31
//' sar rax, 63
//' and eax, ecx
//' or rax, rdx
//' ret
//'@if $windows & $gcc
//' sub rsp, 24
//' xor eax, eax
//' mov rdx, rcx
//' mov [rsp + 32], rcx
//' shr rdx, 32
//' test dl, dl
//' je .L0
//' mov r8d, [rsp + 32]
//' mov eax, 1
//' .L0:
//' mov [rsp + 8], r8d
//' mov [rsp + 12], al
//' mov rax, [rsp + 8]
//' add rsp, 24
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' bt rdx, 32
//' jae .L0
//' mov [rax], edx
//' mov byte ptr [rax + 4], 1
//' .L0:
//' ret
//'@if $msvc
//' mov rax, rdx
//' mov byte ptr [rcx + 4], 0
//' shr rax, 32
//' test al, al
//' mov rax, rcx
//' je .L0
//' mov [rcx], edx
//' mov byte ptr [rcx + 4], 1
//' .L0:
//' ret
std::optional<uint32_t> std_optional_int32_to_uint32_ctor(const std::optional<int32_t> a) {
    return a;
}

//'[option_int32_to_uint64_ctor]
//'@if $linux & $clang[16..]
//' movsxd rax, edi
//' mov rdx, rdi
//' shr rdx, 32
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' movsxd rcx, [rsp + 8]
//' mov [rdi], rcx
//' movzx ecx, [rsp + 12]
//' mov [rdi + 8], cl
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl)
//' mov rax, rcx
//' movsxd rcx, edx
//' shr rdx, 32
//' mov [rax], rcx
//' mov [rax + 8], dl
//' ret
//'@if $msvc
//' movsxd rax, edx
//' mov [rcx], rax
//' mov rax, rcx
//' shr rdx, 32
//' mov [rcx + 8], dl
//' ret
opt::option<uint64_t> option_int32_to_uint64_ctor(const opt::option<int32_t> a) {
    return a;
}

//'[option_int32_assign_from_uint32]
//'@if $linux & $clang
//' mov eax, [rsi]
//' mov [rdi], eax
//' movzx eax, [rsi + 4]
//' mov [rdi + 4], al
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov eax, [rdx]
//' mov [rcx], eax
//' movzx eax, [rdx + 4]
//' mov [rcx + 4], al
//' ret
//'@if $windows & $gcc
//' mov eax, [rdx]
//' mov [rcx], eax
//' movzx eax, [rdx + 4]
//' mov [rcx + 4], al
//' ret
//'@if $msvc
//' mov eax, [rdx]
//' mov [rcx], eax
//' movzx eax, [rdx + 4]
//' mov [rcx + 4], al
//' ret
void option_int32_assign_from_uint32(opt::option<int32_t>& a, const opt::option<uint32_t>& b) {
    a = b;
}

//'[option_int64_assign_from_uint32]
//'@if $linux & $clang
//' mov eax, [rsi]
//' mov [rdi], rax
//' movzx eax, [rsi + 4]
//' mov [rdi + 8], al
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov eax, [rdx]
//' mov [rcx], rax
//' movzx eax, [rdx + 4]
//' mov [rcx + 8], al
//' ret
//'@if $windows & $gcc
//' mov eax, [rdx]
//' mov [rcx], rax
//' movzx eax, [rdx + 4]
//' mov [rcx + 8], al
//' ret
//'@if $msvc
//' mov eax, [rdx]
//' mov [rcx], rax
//' movzx eax, [rdx + 4]
//' mov [rcx + 8], al
//' ret
void option_int64_assign_from_uint32(opt::option<int64_t>& a, const opt::option<uint32_t>& b) {
    a = b;
}

//'[option_int32_emplace]
//'@if $linux & $clang
//' mov dword ptr [rdi], 100
//' mov byte ptr [rdi + 4], 1
//' ret
//'@if $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov dword ptr [rcx], 100
//' mov byte ptr [rcx + 4], 1
//' ret
void option_int32_emplace(opt::option<int32_t>& a) {
    a.emplace(100);
}

//'[std_optional_int32_emplace]
//'@if $linux & $clang
//' mov dword ptr [rdi], 100
//' mov byte ptr [rdi + 4], 1
//' ret
//'@if $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov dword ptr [rcx], 100
//' mov byte ptr [rcx + 4], 1
//' ret
void std_optional_int32_emplace(std::optional<int32_t>& a) {
    a.emplace(100);
}

//'[option_int64_emplace]
//'@if $linux & $clang
//' mov qword ptr [rdi], 100
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov qword ptr [rcx], 100
//' mov byte ptr [rcx + 8], 1
//' ret
void option_int64_emplace(opt::option<int64_t>& a) {
    a.emplace(100);
}

//'[std_optional_int64_emplace]
//'@if $linux & $clang
//' mov qword ptr [rdi], 100
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov qword ptr [rcx], 100
//' mov byte ptr [rcx + 8], 1
//' ret
void std_optional_int64_emplace(std::optional<int64_t>& a) {
    a.emplace(100);
}

//'[option_int32_swap]
//'@if $linux & $clang
//' mov rax, [rdi]
//' mov rcx, [rsi]
//' mov [rdi], rcx
//' mov [rsi], rax
//' ret
//'@if $windows & ($clang | $gcc | $clang-cl)
//' mov rax, [rcx]
//' mov r8, [rdx]
//' mov [rcx], r8
//' mov [rdx], rax
//' ret
//'@if $msvc
//' mov rax, [rdx]
//' mov r8, [rcx]
//' mov [rcx], rax
//' mov [rdx], r8
//' ret
void option_int32_swap(opt::option<int32_t>& a, opt::option<int32_t>& b) {
    a.swap(b);
}

//'[std_optional_int32_swap]
//'@if $linux & $clang[19..]
//' movzx ecx, [rsi + 4]
//' cmp byte ptr [rdi + 4], 1
//' jne .L0
//' mov eax, [rdi]
//' test cl, cl
//' je .L1
//' mov ecx, [rsi]
//' mov [rdi], ecx
//' mov [rsi], eax
//' ret
//' .L0:
//' test cl, cl
//' je .L2
//' mov eax, [rsi]
//' mov [rdi], eax
//' mov byte ptr [rdi + 4], 1
//' mov byte ptr [rsi + 4], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], eax
//' mov byte ptr [rsi + 4], 1
//' mov byte ptr [rdi + 4], 0
//' ret
//'@if $linux & $clang[..19]
//' cmp byte ptr [rdi + 4], 0
//' movzx ecx, [rsi + 4]
//' je .L0
//' mov eax, [rdi]
//' test cl, cl
//' je .L1
//' mov ecx, [rsi]
//' mov [rdi], ecx
//' mov [rsi], eax
//' ret
//' .L0:
//' test cl, cl
//' je .L2
//' mov eax, [rsi]
//' mov [rdi], eax
//' mov byte ptr [rdi + 4], 1
//' mov byte ptr [rsi + 4], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], eax
//' mov byte ptr [rsi + 4], 1
//' mov byte ptr [rdi + 4], 0
//' ret
//'@if $windows & $clang
//' movzx eax, [rcx + 4]
//' cmp al, [rdx + 4]
//' jne .L0
//' test al, al
//' je .L1
//' mov eax, [rcx]
//' mov r8d, [rdx]
//' mov [rcx], r8d
//' mov [rdx], eax
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov eax, [rcx]
//' mov [rdx], eax
//' mov byte ptr [rdx + 4], 1
//' cmp byte ptr [rcx + 4], 1
//' jne .L1
//' mov byte ptr [rcx + 4], 0
//' ret
//' .L2:
//' mov eax, [rdx]
//' mov [rcx], eax
//' mov byte ptr [rcx + 4], 1
//' cmp byte ptr [rdx + 4], 1
//' jne .L1
//' mov byte ptr [rdx + 4], 0
//' .L1:
//' ret
//'@if $windows & $gcc
//' movzx eax, [rdx + 4]
//' cmp byte ptr [rcx + 4], 0
//' je .L0
//' mov r8d, [rcx]
//' test al, al
//' je .L1
//' mov eax, [rdx]
//' mov [rcx], eax
//' mov [rdx], r8d
//' .L2:
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov eax, [rdx]
//' mov byte ptr [rcx + 4], 1
//' mov [rcx], eax
//' mov byte ptr [rdx + 4], 0
//' ret
//' .L1:
//' mov [rdx], r8d
//' mov byte ptr [rdx + 4], 1
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $clang-cl
//' mov rax, [rcx]
//' mov r8, [rdx]
//' mov [rcx], r8
//' mov [rdx], rax
//' ret
//'@if $msvc
//' mov rax, [rdx]
//' mov r8, [rcx]
//' mov [rcx], rax
//' mov [rdx], r8
//' ret
void std_optional_int32_swap(std::optional<int32_t>& a, std::optional<int32_t>& b) {
    a.swap(b);
}

//'[option_int64_swap]
//'@if $linux & $clang
//' movups xmm0, [rdi]
//' movaps [rsp - 24], xmm0
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' movaps xmm0, [rsp - 24]
//' movups [rsi], xmm0
//' ret
//'@if $windows & $clang
//' sub rsp, 24
//' movups xmm0, [rcx]
//' movaps [rsp], xmm0
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movaps xmm0, [rsp]
//' movups [rdx], xmm0
//' add rsp, 24
//' ret
//'@if $windows & $gcc
//' movdqu xmm0, [rcx]
//' movdqu xmm1, [rdx]
//' movups [rcx], xmm1
//' movups [rdx], xmm0
//' ret
//'@if $clang-cl
//' sub rsp, 56
//' mov rax, [rip + __security_cookie]
//' xor rax, rsp
//' mov [rsp + 48], rax
//' movups xmm0, [rcx]
//' movaps [rsp + 32], xmm0
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movaps xmm0, [rsp + 32]
//' movups [rdx], xmm0
//' mov rcx, [rsp + 48]
//' xor rcx, rsp
//' cmp rcx, [rip + __security_cookie]
//' jne .L0
//' add rsp, 56
//' ret
//' .L0:
//' call __security_check_cookie
//' int3
//'@if $msvc
//' movups xmm0, [rdx]
//' movups xmm1, [rcx]
//' movups [rcx], xmm0
//' movups [rdx], xmm1
//' ret
void option_int64_swap(opt::option<int64_t>& a, opt::option<int64_t>& b) {
    a.swap(b);
}

//'[std_optional_int64_swap]
//'@if $linux & $clang[19..]
//' movzx ecx, [rsi + 8]
//' cmp byte ptr [rdi + 8], 1
//' jne .L0
//' mov rax, [rdi]
//' test cl, cl
//' je .L1
//' mov rcx, [rsi]
//' mov [rdi], rcx
//' mov [rsi], rax
//' ret
//' .L0:
//' test cl, cl
//' je .L2
//' mov rax, [rsi]
//' mov [rdi], rax
//' mov byte ptr [rdi + 8], 1
//' mov byte ptr [rsi + 8], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], rax
//' mov byte ptr [rsi + 8], 1
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $linux & $clang[..19]
//' cmp byte ptr [rdi + 8], 0
//' movzx ecx, [rsi + 8]
//' je .L0
//' mov rax, [rdi]
//' test cl, cl
//' je .L1
//' mov rcx, [rsi]
//' mov [rdi], rcx
//' mov [rsi], rax
//' ret
//' .L0:
//' test cl, cl
//' je .L2
//' mov rax, [rsi]
//' mov [rdi], rax
//' mov byte ptr [rdi + 8], 1
//' mov byte ptr [rsi + 8], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], rax
//' mov byte ptr [rsi + 8], 1
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $windows & $clang
//' movzx eax, [rcx + 8]
//' cmp al, [rdx + 8]
//' jne .L0
//' test al, al
//' je .L1
//' mov rax, [rcx]
//' mov r8, [rdx]
//' mov [rcx], r8
//' mov [rdx], rax
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov rax, [rcx]
//' mov [rdx], rax
//' mov byte ptr [rdx + 8], 1
//' cmp byte ptr [rcx + 8], 1
//' jne .L1
//' mov byte ptr [rcx + 8], 0
//' ret
//' .L2:
//' mov rax, [rdx]
//' mov [rcx], rax
//' mov byte ptr [rcx + 8], 1
//' cmp byte ptr [rdx + 8], 1
//' jne .L1
//' mov byte ptr [rdx + 8], 0
//' .L1:
//' ret
//'@if $windows & $gcc
//' movzx eax, [rdx + 8]
//' cmp byte ptr [rcx + 8], 0
//' je .L0
//' mov r8, [rcx]
//' test al, al
//' je .L1
//' mov rax, [rdx]
//' mov [rcx], rax
//' mov [rdx], r8
//' .L2:
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov rax, [rdx]
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rax
//' mov byte ptr [rdx + 8], 0
//' ret
//' .L1:
//' mov [rdx], r8
//' mov byte ptr [rdx + 8], 1
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $clang-cl
//' sub rsp, 56
//' mov rax, [rip + __security_cookie]
//' xor rax, rsp
//' mov [rsp + 48], rax
//' movups xmm0, [rcx]
//' movaps [rsp + 32], xmm0
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movaps xmm0, [rsp + 32]
//' movups [rdx], xmm0
//' mov rcx, [rsp + 48]
//' xor rcx, rsp
//' cmp rcx, [rip + __security_cookie]
//' jne .L0
//' add rsp, 56
//' ret
//' .L0:
//' call __security_check_cookie
//' int3
//'@if $msvc
//' movups xmm0, [rdx]
//' movups xmm1, [rcx]
//' movups [rcx], xmm0
//' movups [rdx], xmm1
//' ret
void std_optional_int64_swap(std::optional<int64_t>& a, std::optional<int64_t>& b) {
    a.swap(b);
}

//'[option_int64x2_swap]
//'@if $linux & $clang
//' mov rax, [rdi + 16]
//' mov [rsp - 8], rax
//' movups xmm0, [rdi]
//' movaps [rsp - 24], xmm0
//' mov rax, [rsi + 16]
//' mov [rdi + 16], rax
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' mov rax, [rsp - 8]
//' mov [rsi + 16], rax
//' movaps xmm0, [rsp - 24]
//' movups [rsi], xmm0
//' ret
//'@if $windows & $clang
//' sub rsp, 24
//' mov rax, [rcx + 16]
//' mov [rsp + 16], rax
//' movups xmm0, [rcx]
//' movaps [rsp], xmm0
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' mov rax, [rsp + 16]
//' mov [rdx + 16], rax
//' movaps xmm0, [rsp]
//' movups [rdx], xmm0
//' add rsp, 24
//' ret
//'@if $windows & $gcc
//' sub rsp, 40
//' movdqu xmm1, [rdx]
//' movdqu xmm0, [rcx]
//' mov rax, [rcx + 16]
//' movups [rcx], xmm1
//' mov r8, [rdx + 16]
//' mov [rcx + 16], r8
//' mov [rdx + 16], rax
//' movups [rdx], xmm0
//' add rsp, 40
//' ret
//'@if $clang-cl
//' sub rsp, 72
//' mov rax, [rip + __security_cookie]
//' xor rax, rsp
//' mov [rsp + 64], rax
//' mov rax, [rcx + 16]
//' mov [rsp + 48], rax
//' movups xmm0, [rcx]
//' movaps [rsp + 32], xmm0
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' mov rax, [rsp + 48]
//' mov [rdx + 16], rax
//' movaps xmm0, [rsp + 32]
//' movups [rdx], xmm0
//' mov rcx, [rsp + 64]
//' xor rcx, rsp
//' cmp rcx, [rip + __security_cookie]
//' jne .L0
//' add rsp, 72
//' ret
//' .L0:
//' call __security_check_cookie
//' int3
//'@if $msvc
//' movups xmm0, [rdx]
//' movups xmm3, [rcx]
//' movsd xmm2, [rcx + 16]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' movups [rdx], xmm3
//' movsd [rdx + 16], xmm2
//' ret
void option_int64x2_swap(opt::option<int64x2>& a, opt::option<int64x2>& b) {
    a.swap(b);
}

//'[std_optional_int64x2_swap]
//'@if $linux & $clang[23..]
//' movzx eax, [rsi + 16]
//' cmp byte ptr [rdi + 16], 1
//' jne .L0
//' test al, al
//' je .L1
//' movups xmm0, [rdi]
//' movups xmm1, [rsi]
//' movups [rdi], xmm1
//' movups [rsi], xmm0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' mov byte ptr [rdi + 16], 1
//' mov byte ptr [rsi + 16], 0
//' .L2:
//' ret
//' .L1:
//' movups xmm0, [rdi]
//' movups [rsi], xmm0
//' mov byte ptr [rsi + 16], 1
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $linux & $clang[19..23]
//' movzx eax, [rsi + 16]
//' cmp byte ptr [rdi + 16], 1
//' jne .L0
//' test al, al
//' je .L1
//' movups xmm0, [rdi]
//' movaps [rsp - 24], xmm0
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' movaps xmm0, [rsp - 24]
//' movups [rsi], xmm0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' mov byte ptr [rdi + 16], 1
//' mov byte ptr [rsi + 16], 0
//' .L2:
//' ret
//' .L1:
//' movups xmm0, [rdi]
//' movups [rsi], xmm0
//' mov byte ptr [rsi + 16], 1
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $linux & $clang[..19]
//' cmp byte ptr [rdi + 16], 0
//' movzx eax, [rsi + 16]
//' je .L0
//' test al, al
//' je .L1
//' movups xmm0, [rdi]
//' movaps [rsp - 24], xmm0
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' movaps xmm0, [rsp - 24]
//' movups [rsi], xmm0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' mov byte ptr [rdi + 16], 1
//' mov byte ptr [rsi + 16], 0
//' .L2:
//' ret
//' .L1:
//' movups xmm0, [rdi]
//' movups [rsi], xmm0
//' mov byte ptr [rsi + 16], 1
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $windows & $clang
//' sub rsp, 24
//' movzx eax, [rcx + 16]
//' cmp al, [rdx + 16]
//' jne .L0
//' test al, al
//' je .L1
//' movups xmm0, [rcx]
//' movaps [rsp], xmm0
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movaps xmm0, [rsp]
//' movups [rdx], xmm0
//' add rsp, 24
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movups xmm0, [rcx]
//' movups [rdx], xmm0
//' mov byte ptr [rdx + 16], 1
//' cmp byte ptr [rcx + 16], 1
//' jne .L1
//' mov byte ptr [rcx + 16], 0
//' add rsp, 24
//' ret
//' .L2:
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' mov byte ptr [rcx + 16], 1
//' cmp byte ptr [rdx + 16], 1
//' jne .L1
//' mov byte ptr [rdx + 16], 0
//' .L1:
//' add rsp, 24
//' ret
//'@if $windows & $gcc
//' movzx eax, [rdx + 16]
//' cmp byte ptr [rcx + 16], 0
//' je .L0
//' test al, al
//' je .L1
//' movdqu xmm0, [rcx]
//' movdqu xmm1, [rdx]
//' movups [rcx], xmm1
//' movups [rdx], xmm0
//' .L2:
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movdqu xmm0, [rdx]
//' mov byte ptr [rcx + 16], 1
//' movups [rcx], xmm0
//' mov byte ptr [rdx + 16], 0
//' ret
//' .L1:
//' movdqu xmm0, [rcx]
//' mov byte ptr [rdx + 16], 1
//' movups [rdx], xmm0
//' mov byte ptr [rcx + 16], 0
//' ret
//'@if $clang-cl
//' sub rsp, 72
//' mov rax, [rip + __security_cookie]
//' xor rax, rsp
//' mov [rsp + 64], rax
//' mov rax, [rcx + 16]
//' mov [rsp + 48], rax
//' movups xmm0, [rcx]
//' movaps [rsp + 32], xmm0
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' mov rax, [rsp + 48]
//' mov [rdx + 16], rax
//' movaps xmm0, [rsp + 32]
//' movups [rdx], xmm0
//' mov rcx, [rsp + 64]
//' xor rcx, rsp
//' cmp rcx, [rip + __security_cookie]
//' jne .L0
//' add rsp, 72
//' ret
//' .L0:
//' call __security_check_cookie
//' int3
//'@if $msvc
//' movups xmm0, [rdx]
//' movups xmm3, [rcx]
//' movsd xmm2, [rcx + 16]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' movups [rdx], xmm3
//' movsd [rdx + 16], xmm2
//' ret
void std_optional_int64x2_swap(std::optional<int64x2>& a, std::optional<int64x2>& b) {
    a.swap(b);
}

//'[option_find_index_of_value]
//'@if $linux & $clang[16..]
//' xor eax, eax
//' test rsi, rsi
//' je .L0
//' .L1:
//' cmp [rdi + rax*4], edx
//' je .L2
//' inc rax
//' cmp rsi, rax
//' jne .L1
//' xor edx, edx
//' mov rax, rsi
//' ret
//' .L0:
//' xor edx, edx
//' ret
//' .L2:
//' mov dl, 1
//' ret
//'@if $linux & $clang[..16]
//' mov rax, rdi
//' test rdx, rdx
//' je .L0
//' xor edi, edi
//' .L1:
//' cmp [rsi + rdi*4], ecx
//' je .L2
//' inc rdi
//' cmp rdx, rdi
//' jne .L1
//' .L0:
//' xor ecx, ecx
//' mov [rax + 8], cl
//' ret
//' .L2:
//' mov [rax], rdi
//' mov cl, 1
//' mov [rax + 8], cl
//' ret
//'@if $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' test r8, r8
//' je .L0
//' xor ecx, ecx
//' .L1:
//' cmp [rdx + rcx*4], r9d
//' je .L2
//' inc rcx
//' cmp r8, rcx
//' jne .L1
//' .L0:
//' xor ecx, ecx
//' mov [rax + 8], cl
//' ret
//' .L2:
//' mov [rax], rcx
//' mov cl, 1
//' mov [rax + 8], cl
//' ret
//'@if $windows & $gcc
//' mov rax, rcx
//' test r8, r8
//' je .L0
//' xor r10d, r10d
//' jmp .L1
//' .L2:
//' add r10, 1
//' cmp r10, r8
//' je .L0
//' .L1:
//' cmp [rdx + r10*4], r9d
//' jne .L2
//' mov [rax], r10
//' mov byte ptr [rax + 8], 1
//' ret
//' .L0:
//' mov byte ptr [rax + 8], 0
//' ret
//'@if $msvc
//' xor eax, eax
//' test r8, r8
//' je .L0
//' .L1:
//' cmp [rdx + rax*4], r9d
//' je .L2
//' inc rax
//' cmp rax, r8
//' jb .L1
//' .L0:
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//' .L2:
//' mov [rcx], rax
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 1
//' ret
opt::option<size_t> option_find_index_of_value(const int* array, size_t size, int value) {
    for (size_t i = 0; i < size; ++i) {
        if (array[i] == value) {
            return i;
        }
    }
    return opt::none;
}

//'[std_optional_find_index_of_value]
//'@if $linux & $clang
//' xor eax, eax
//' test rsi, rsi
//' je .L0
//' .L1:
//' cmp [rdi + rax*4], edx
//' je .L2
//' inc rax
//' cmp rsi, rax
//' jne .L1
//' xor edx, edx
//' mov rax, rsi
//' ret
//' .L0:
//' xor edx, edx
//' ret
//' .L2:
//' mov dl, 1
//' ret
//'@if $windows & $clang
//' mov rax, rcx
//' test r8, r8
//' je .L0
//' xor ecx, ecx
//' .L1:
//' cmp [rdx + rcx*4], r9d
//' je .L2
//' inc rcx
//' cmp r8, rcx
//' jne .L1
//' .L0:
//' mov byte ptr [rax], 0
//' xor ecx, ecx
//' mov [rax + 8], cl
//' ret
//' .L2:
//' mov [rax], rcx
//' mov cl, 1
//' mov [rax + 8], cl
//' ret
//'@if $clang-cl
//' mov rax, rcx
//' test r8, r8
//' je .L0
//' xor ecx, ecx
//' .L1:
//' cmp [rdx + rcx*4], r9d
//' je .L2
//' inc rcx
//' cmp r8, rcx
//' jne .L1
//' .L0:
//' xor ecx, ecx
//' mov [rax + 8], cl
//' ret
//' .L2:
//' mov [rax], rcx
//' mov cl, 1
//' mov [rax + 8], cl
//' ret
//'@if $windows & $gcc
//' mov rax, rcx
//' test r8, r8
//' je .L0
//' xor r10d, r10d
//' jmp .L1
//' .L2:
//' add r10, 1
//' cmp r10, r8
//' je .L0
//' .L1:
//' cmp [rdx + r10*4], r9d
//' jne .L2
//' mov [rax], r10
//' mov byte ptr [rax + 8], 1
//' ret
//' .L0:
//' pxor xmm0, xmm0
//' movups [rax], xmm0
//' ret
//'@if $msvc
//' xor eax, eax
//' test r8, r8
//' je .L0
//' .L1:
//' cmp [rdx + rax*4], r9d
//' je .L2
//' inc rax
//' cmp rax, r8
//' jb .L1
//' .L0:
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//' .L2:
//' mov [rcx], rax
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 1
//' ret
std::optional<size_t> std_optional_find_index_of_value(const int* array, size_t size, int value) {
    for (size_t i = 0; i < size; ++i) {
        if (array[i] == value) {
            return i;
        }
    }
    return std::nullopt;
}

//'[opt_option_int32_value]
//'@if $linux & $clang[17..]
//' cmp byte ptr [rdi + 4], 0
//' je .L0
//' mov eax, [rdi]
//' ret
//' .L0:
//' push rax
//' call _ZN3opt4impl16throw_bad_optionEv
//'@if $linux & $clang[..17]
//' push rax
//' cmp byte ptr [rdi + 4], 0
//' je .L0
//' mov eax, [rdi]
//' pop rcx
//' ret
//' .L0:
//' call _ZN3opt4impl16throw_bad_optionEv
//'@if $windows & $clang
//' sub rsp, 40
//' cmp byte ptr [rcx + 4], 0
//' je .L0
//' mov eax, [rcx]
//' add rsp, 40
//' ret
//' .L0:
//' call _ZN3opt4impl16throw_bad_optionEv
//' int3
//'@if $windows & $gcc
//' sub rsp, 40
//' cmp byte ptr [rcx + 4], 0
//' je .L0
//' mov eax, [rcx]
//' add rsp, 40
//' ret
//' .L0:
//' call _ZN3opt4impl16throw_bad_optionEv
//' nop
//'@if $msvc | $clang-cl
//' sub rsp, 40
//' cmp byte ptr [rcx + 4], 0
//' je .L0
//' mov eax, [rcx]
//' add rsp, 40
//' ret
//' .L0:
//' call ?throw_bad_option@impl@opt@@YAXXZ
//' int3
int32_t opt_option_int32_value(opt::option<int32_t>& o) {
    return o.value();
}
