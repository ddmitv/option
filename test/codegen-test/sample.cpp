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
//'@if $x64 & $linux & $clang
//' movsxd rax, edx
//' mov dword ptr [rdi + rax*4 + 8], 6
//' ret
//'@if $x64 & $linux & $gcc[14..]
//' movsxd rdx, edx
//' mov dword ptr [rdi + rdx*4 + 8], 6
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' movsx rdx, edx
//' mov dword ptr [rdi + rdx*4 + 8], 6
//' ret
//'@if $x64 & $windows & $gcc
//' movsxd r8, r8d
//' mov dword ptr [rcx + r8*4 + 8], 6
//' ret
//'@if $x64 & $windows & ($msvc | $clang-cl | $clang)
//' movsxd rax, r8d
//' mov dword ptr [rcx + rax*4 + 8], 6
//' ret
//'@if $arm64 & $gcc
//' add x2, x0, w2, sxtw 2
//' mov w1, 6
//' str w1, [x2, 8]
//' ret
//'@if $arm64 & $clang[18..]
//' add x8, x0, w2, sxtw 2
//' mov w9, 6
//' str w9, [x8, 8]
//' ret
//'@if $arm64 & $clang[..18]
//' add w8, w2, 2
//' mov w9, 6
//' str w9, [x0, w8, sxtw 2]
//' ret
void third_fn(int* x, int y, int z) {
    x[z + 2] = 6;
}

//'[option_make_from_int32_sum]
//'@if $x64 & $msvc
//' lea eax, [rdx + r8]
//' mov byte ptr [rcx + 4], 1
//' mov [rcx], eax
//' mov rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' add edx, r8d
//' mov [rcx], edx
//' mov byte ptr [rcx + 4], 1
//' ret
//'@if $x64 & $linux & $clang[16..]
//' lea ecx, [rdi + rsi]
//' mov rax, 4294967296
//' or rax, rcx
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov rax, rdi
//' add esi, edx
//' mov [rdi], esi
//' mov byte ptr [rdi + 4], 1
//' ret
//'@if $x64 & $linux & $gcc
//' lea eax, [rdi + rsi]
//' bts rax, 32
//' ret
//'@if $x64 & $windows & $gcc
//' lea eax, [rcx + rdx]
//' bts rax, 32
//' ret
//'@if $x64 & $windows & $clang
//' add ecx, edx
//' mov rax, 4294967296
//' or rax, rcx
//' ret
//'@if $arm64 & $gcc[14..]
//' add w0, w0, w1
//' mov x2, 0
//' mov x3, 1
//' bfi x2, x0, 0, 32
//' bfi x2, x3, 32, 8
//' mov x0, x2
//' ret
//'@if $arm64 & $gcc[..14]
//' add w1, w0, w1
//' mov x0, 0
//' mov x2, 1
//' bfi x0, x1, 0, 32
//' bfi x0, x2, 32, 8
//' ret
//'@if $arm64 & $clang
//' add w8, w1, w0
//' orr x0, x8, 0x100000000
//' ret
opt::option<int32_t> option_make_from_int32_sum(int32_t x, int32_t y) {
    return x + y;
}

//'[option_make_from_int64_sum]
//'@if $x64 & $msvc
//' lea rax, [rdx + r8]
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rax
//' mov rax, rcx
//' ret
//'@if $x64 & $linux & $clang[16..]
//' lea rax, [rdi + rsi]
//' mov dl, 1
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov rax, rdi
//' add rsi, rdx
//' mov [rdi], rsi
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $x64 & $linux & $gcc[14..]
//' xor edx, edx
//' lea rax, [rdi + rsi]
//' mov dl, 1
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' lea rax, [rdi + rsi]
//' mov edx, 1
//' ret
//'@if $x64 & $windows & $gcc
//' mov rax, rcx
//' add rdx, r8
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rdx
//' ret
//'@if $x64 & $windows & ($clang-cl | $clang)
//' mov rax, rcx
//' add rdx, r8
//' mov [rcx], rdx
//' mov byte ptr [rcx + 8], 1
//' ret
//'@if $arm64 & $gcc
//' add x0, x0, x1
//' mov x1, 1
//' ret
//'@if $arm64 & $clang
//' add x0, x1, x0
//' mov w1, 1
//' ret
opt::option<int64_t> option_make_from_int64_sum(int64_t x, int64_t y) {
    return x + y;
}

//'[std_optional_make_from_int32_sum]
//'@if $x64 & $msvc
//' lea eax, [rdx + r8]
//' mov byte ptr [rcx + 4], 1
//' mov [rcx], eax
//' mov rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' add edx, r8d
//' mov [rcx], edx
//' mov byte ptr [rcx + 4], 1
//' ret
//'@if $x64 & $linux & $clang
//' lea ecx, [rdi + rsi]
//' mov rax, 4294967296
//' or rax, rcx
//' ret
//'@if $x64 & $linux & $gcc
//' add edi, esi
//' mov byte ptr [rsp - 4], 1
//' mov [rsp - 8], edi
//' mov rax, [rsp - 8]
//' ret
//'@if $x64 & $windows & $gcc
//' sub rsp, 24
//' add ecx, edx
//' mov byte ptr [rsp + 12], 1
//' mov [rsp + 8], ecx
//' mov rax, [rsp + 8]
//' add rsp, 24
//' ret
//'@if $x64 & $windows & $clang
//' add ecx, edx
//' mov rax, 4294967296
//' or rax, rcx
//' ret
//'@if $arm64 & $gcc
//' sub sp, sp, 16
//' add w0, w0, w1
//' mov w1, 1
//' str w0, [sp, 8]
//' strb w1, [sp, 12]
//' ldr x0, [sp, 8]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $clang
//' add w8, w1, w0
//' orr x0, x8, 0x100000000
//' ret
std::optional<int32_t> std_optional_make_from_int32_sum(int32_t x, int32_t y) {
    return x + y;
}

//'[std_optional_make_from_int64_sum]
//'@if $x64 & $msvc
//' lea rax, [rdx + r8]
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rax
//' mov rax, rcx
//' ret
//'@if $x64 & $linux & $clang
//' lea rax, [rdi + rsi]
//' mov dl, 1
//' ret
//'@if $x64 & $linux & $gcc
//' add rdi, rsi
//' mov byte ptr [rsp - 16], 1
//' mov rdx, [rsp - 16]
//' mov [rsp - 24], rdi
//' mov rax, [rsp - 24]
//' ret
//'@if $x64 & $windows & $gcc
//' mov rax, rcx
//' add rdx, r8
//' mov byte ptr [rcx + 8], 1
//' mov [rcx], rdx
//' ret
//'@if $x64 & $windows & ($clang-cl | $clang)
//' mov rax, rcx
//' add rdx, r8
//' mov [rcx], rdx
//' mov byte ptr [rcx + 8], 1
//' ret
//'@if $arm64 & $gcc
//' sub sp, sp, 16
//' mov w2, 1
//' add x0, x0, x1
//' strb w2, [sp, 8]
//' ldr x1, [sp, 8]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $clang
//' add x0, x1, x0
//' mov w1, 1
//' ret
std::optional<int64_t> std_optional_make_from_int64_sum(int64_t x, int64_t y) {
    return x + y;
}

//'[option_bool_empty]
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $x64 & ($clang[16..] | $gcc)
//' xor eax, eax
//' ret
//'@if $x64 & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 1], 0
//' ret
//'@if $arm64 & $gcc
//' mov w0, 0
//' ret
//'@if $arm64 & $clang
//' mov w0, wzr
//' ret
opt::option<bool> option_bool_empty() {
    return opt::none;
}

//'[option_int32_empty]
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $x64 & ($clang[16..] | $gcc)
//' xor eax, eax
//' ret
//'@if $x64 & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 4], 0
//' ret
//'@if $arm64 & $gcc
//' mov x0, 0
//' ret
//'@if $arm64 & $clang
//' mov x0, xzr
//' ret
opt::option<int32_t> option_int32_empty() {
    return opt::none;
}

//'[option_int64_empty]
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $linux & $clang[16..]
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $x64 & $linux & $gcc[15..]
//' xor eax, eax
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' xor edx, edx
//' xor eax, eax
//' ret
//'@if $x64 & $windows & ($clang-cl | $clang | $gcc)
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $arm64 & $gcc
//' mov x0, 0
//' mov x1, 0
//' ret
//'@if $arm64 & $clang
//' mov x1, xzr
//' ret
opt::option<int64_t> option_int64_empty() {
    return opt::none;
}

//'[std_optional_bool_empty]
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $x64 & ($clang | $gcc)
//' xor eax, eax
//' ret
//'@if $arm64 & $gcc
//' mov w0, 0
//' ret
//'@if $arm64 & $clang
//' mov w0, wzr
//' ret
std::optional<bool> std_optional_bool_empty() {
    return std::nullopt;
}

//'[std_optional_int32_empty]
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $linux & $gcc[15..]
//' xor eax, eax
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' mov byte ptr [rsp - 4], 0
//' mov rax, [rsp - 8]
//' ret
//'@if $x64 & $windows & $gcc
//' sub rsp, 24
//' xor eax, eax
//' add rsp, 24
//' ret
//'@if $x64 & $clang
//' xor eax, eax
//' ret
//'@if $arm64 & $gcc[15..]
//' sub sp, sp, 16
//' mov x0, 0
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..15]
//' sub sp, sp, 16
//' strb wzr, [sp, 12]
//' ldr x0, [sp, 8]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $clang
//' mov x0, xzr
//' ret
std::optional<int32_t> std_optional_int32_empty() {
    return std::nullopt;
}

//'[std_optional_int64_empty]
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $linux & $libcxx & $clang
//' xor eax, eax
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $gcc[15..]
//' xor eax, eax
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' mov byte ptr [rsp - 16], 0
//' mov rax, [rsp - 24]
//' mov rdx, [rsp - 16]
//' ret
//'@if $x64 & $windows & $gcc
//' pxor xmm0, xmm0
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $windows & $clang
//' mov rax, rcx
//' mov byte ptr [rcx], 0
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $arm64 & $gcc[15..]
//' sub sp, sp, 16
//' mov x0, 0
//' mov x1, 0
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..15]
//' sub sp, sp, 16
//' strb wzr, [sp, 8]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libcxx & $clang
//' mov x0, xzr
//' mov x1, xzr
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' mov x1, xzr
//' ret
std::optional<int64_t> std_optional_int64_empty() {
    return std::nullopt;
}

//'[option_bool_default]
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $x64 & ($clang[16..] | $gcc)
//' xor eax, eax
//' ret
//'@if $x64 & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 1], 0
//' ret
//'@if $arm64 & $gcc
//' mov w0, 0
//' ret
//'@if $arm64 & $clang
//' mov w0, wzr
//' ret
opt::option<bool> option_bool_default() {
    return {};
}

//'[std_optional_bool_default]
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 1], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 1], 0
//' ret
//'@if $x64 & ($clang | $gcc)
//' xor eax, eax
//' ret
//'@if $arm64 & $gcc
//' mov w0, 0
//' ret
//'@if $arm64 & $clang
//' mov w0, wzr
//' ret
std::optional<bool> std_optional_bool_default() {
    return {};
}

//'[option_int32_default]
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $x64 & ($clang[16..] | $gcc)
//' xor eax, eax
//' ret
//'@if $x64 & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 4], 0
//' ret
//'@if $arm64 & $gcc
//' mov x0, 0
//' ret
//'@if $arm64 & $clang
//' mov x0, xzr
//' ret
opt::option<int32_t> option_int32_default() {
    return {};
}

//'[std_optional_int32_default]
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 4], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' ret
//'@if $x64 & $clang
//' xor eax, eax
//' ret
//'@if $x64 & $linux & $gcc[15..]
//' xor eax, eax
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' mov byte ptr [rsp - 4], 0
//' mov rax, [rsp - 8]
//' ret
//'@if $x64 & $windows & $gcc
//' sub rsp, 24
//' xor eax, eax
//' add rsp, 24
//' ret
//'@if $arm64 & $gcc[15..]
//' sub sp, sp, 16
//' mov x0, 0
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..15]
//' sub sp, sp, 16
//' strb wzr, [sp, 12]
//' ldr x0, [sp, 8]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $clang
//' mov x0, xzr
//' ret
std::optional<int32_t> std_optional_int32_default() {
    return {};
}

//'[option_int64_default]
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $linux & $clang[16..]
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov rax, rdi
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $x64 & $linux & $gcc[15..]
//' xor eax, eax
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' xor edx, edx
//' xor eax, eax
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl)
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $arm64 & $gcc
//' mov x0, 0
//' mov x1, 0
//' ret
//'@if $arm64 & $clang
//' mov x1, xzr
//' ret
opt::option<int64_t> option_int64_default() {
    return {};
}

//'[std_optional_int64_default]
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 8], 0
//' mov rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $x64 & $linux & $libcxx & $clang
//' xor eax, eax
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang
//' xor edx, edx
//' ret
//'@if $x64 & $windows & $clang
//' mov rax, rcx
//' mov byte ptr [rcx], 0
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $x64 & $linux & $gcc[15..]
//' xor eax, eax
//' xor edx, edx
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' mov byte ptr [rsp - 16], 0
//' mov rax, [rsp - 24]
//' mov rdx, [rsp - 16]
//' ret
//'@if $x64 & $windows & $gcc
//' pxor xmm0, xmm0
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $arm64 & $gcc[15..]
//' sub sp, sp, 16
//' mov x0, 0
//' mov x1, 0
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..15]
//' sub sp, sp, 16
//' strb wzr, [sp, 8]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libcxx & $clang
//' mov x0, xzr
//' mov x1, xzr
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' mov x1, xzr
//' ret
std::optional<int64_t> std_optional_int64_default() {
    return {};
}

//'[option_uint64_reset]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl | $gcc | $msvc)
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $arm64
//' strb wzr, [x0, 8]
//' ret
void option_uint64_reset(opt::option<uint64_t>& a) {
    a.reset();
}

//'[std_optional_uint64_reset]
//'@if $x64 & $linux & $libcxx & $clang[19..]
//' cmp byte ptr [rdi + 8], 1
//' jne .L0
//' mov byte ptr [rdi + 8], 0
//' .L0:
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..19]
//' cmp byte ptr [rdi + 8], 0
//' je .L0
//' mov byte ptr [rdi + 8], 0
//' .L0:
//' ret
//'@if $x64 & $linux & $libstdcxx & ($clang[17..] | $gcc[13..])
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $x64 & $linux & $libstdcxx & ($clang[..17] | $gcc[..13])
//' cmp byte ptr [rdi + 8], 0
//' je .L0
//' mov byte ptr [rdi + 8], 0
//' .L0:
//' ret
//'@if $x64 & $windows & $clang
//' cmp byte ptr [rcx + 8], 1
//' jne .L0
//' mov byte ptr [rcx + 8], 0
//' .L0:
//' ret
//'@if $x64 & $windows & ($gcc | $msvc | $clang-cl)
//' mov byte ptr [rcx + 8], 0
//' ret
//'@if $arm64 & $gcc[13..]
//' strb wzr, [x0, 8]
//' ret
//'@if $arm64 & $gcc[..13]
//' ldrb w1, [x0, 8]
//' cbz w1, .L0
//' strb wzr, [x0, 8]
//' .L0:
//' ret
//'@if $arm64 & $libcxx & $clang[23..]
//' ldrb w8, [x0, 8]
//' cbz w8, .L0
//' strb wzr, [x0, 8]
//' .L0:
//' ret
//'@if $arm64 & $libcxx & $clang[19..23]
//' ldrb w8, [x0, 8]
//' cmp w8, 1
//' b.ne .L0
//' strb wzr, [x0, 8]
//' .L0:
//' ret
//'@if $arm64 & $libcxx & $clang[..19]
//' ldrb w8, [x0, 8]
//' cbz w8, .L0
//' strb wzr, [x0, 8]
//' .L0:
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' strb wzr, [x0, 8]
//' ret
void std_optional_uint64_reset(std::optional<uint64_t>& a) {
    a.reset();
}

//'[option_int64x2_reset]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl | $msvc | $gcc)
//' mov byte ptr [rcx + 16], 0
//' ret
//'@if $arm64
//' strb wzr, [x0, 16]
//' ret
void option_int64x2_reset(opt::option<int64x2>& a) {
    a.reset();
}

//'[std_optional_int64x2_reset]
//'@if $x64 & $linux & $libcxx & $clang[19..]
//' cmp byte ptr [rdi + 16], 1
//' jne .L0
//' mov byte ptr [rdi + 16], 0
//' .L0:
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..19]
//' cmp byte ptr [rdi + 16], 0
//' je .L0
//' mov byte ptr [rdi + 16], 0
//' .L0:
//' ret
//'@if $x64 & $linux & $libstdcxx & ($clang[17..] | $gcc[13..])
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $x64 & $linux & $libstdcxx & ($clang[..17] | $gcc[..13])
//' cmp byte ptr [rdi + 16], 0
//' je .L0
//' mov byte ptr [rdi + 16], 0
//' .L0:
//' ret
//'@if $x64 & $windows & $clang
//' cmp byte ptr [rcx + 16], 1
//' jne .L0
//' mov byte ptr [rcx + 16], 0
//' .L0:
//' ret
//'@if $x64 & $windows & ($clang-cl | $msvc | $gcc)
//' mov byte ptr [rcx + 16], 0
//' ret
//'@if $arm64 & $gcc[13..]
//' strb wzr, [x0, 16]
//' ret
//'@if $arm64 & $gcc[..13]
//' ldrb w1, [x0, 16]
//' cbz w1, .L0
//' strb wzr, [x0, 16]
//' .L0:
//' ret
//'@if $arm64 & $libcxx & $clang[23..]
//' ldrb w8, [x0, 16]
//' cbz w8, .L0
//' strb wzr, [x0, 16]
//' .L0:
//' ret
//'@if $arm64 & $libcxx & $clang[19..23]
//' ldrb w8, [x0, 16]
//' cmp w8, 1
//' b.ne .L0
//' strb wzr, [x0, 16]
//' .L0:
//' ret
//'@if $arm64 & $libcxx & $clang[..19]
//' ldrb w8, [x0, 16]
//' cbz w8, .L0
//' strb wzr, [x0, 16]
//' .L0:
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' strb wzr, [x0, 16]
//' ret
void std_optional_int64x2_reset(std::optional<int64x2>& a) {
    a.reset();
}

//'[option_uint64_has_value]
//'@if $x64 & $linux & ($clang | $gcc)
//' movzx eax, [rdi + 8]
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 8]
//' ret
//'@if $arm64
//' ldrb w0, [x0, 8]
//' ret
bool option_uint64_has_value(const opt::option<uint64_t>& a) {
    return a.has_value();
}

//'[std_optional_uint64_has_value]
//'@if $x64 & $linux & ($clang | $gcc)
//' movzx eax, [rdi + 8]
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 8]
//' ret
//'@if $arm64
//' ldrb w0, [x0, 8]
//' ret
bool std_optional_uint64_has_value(const std::optional<uint64_t>& a) {
    return a.has_value();
}

//'[option_int64x2_has_value]
//'@if $x64 & $linux & ($clang | $gcc)
//' movzx eax, [rdi + 16]
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 16]
//' ret
//'@if $arm64
//' ldrb w0, [x0, 16]
//' ret
bool option_int64x2_has_value(const opt::option<int64x2>& a) {
    return a.has_value();
}

//'[std_optional_int64x2_has_value]
//'@if $x64 & $linux & ($clang | $gcc)
//' movzx eax, [rdi + 16]
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl | $msvc)
//' movzx eax, [rcx + 16]
//' ret
//'@if $arm64
//' ldrb w0, [x0, 16]
//' ret
bool std_optional_int64x2_has_value(const std::optional<int64x2>& a) {
    return a.has_value();
}

//'[option_int64_assign_from_value]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov qword ptr [rdi], 10
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl | $msvc | $gcc)
//' mov qword ptr [rcx], 10
//' mov byte ptr [rcx + 8], 1
//' ret
//'@if $arm64 & $gcc
//' mov x2, 10
//' mov w1, 1
//' str x2, [x0]
//' strb w1, [x0, 8]
//' ret
//'@if $arm64 & $clang
//' mov w8, 10
//' mov w9, 1
//' str x8, [x0]
//' strb w9, [x0, 8]
//' ret
void option_int64_assign_from_value(opt::option<int64_t>& a) {
    a = 10;
}

//'[std_optional_int64_assign_from_value]
//'@if $x64 & $linux & ($clang[19..] | $gcc)
//' cmp byte ptr [rdi + 8], 0
//' jne .L0
//' mov byte ptr [rdi + 8], 1
//' .L0:
//' mov qword ptr [rdi], 10
//' ret
//'@if $x64 & $linux & $clang[16..19]
//' cmp byte ptr [rdi + 8], 0
//' je .L0
//' mov qword ptr [rdi], 10
//' ret
//' .L0:
//' mov byte ptr [rdi + 8], 1
//' mov qword ptr [rdi], 10
//' ret
//'@if $x64 & $linux & $clang[..16]
//' cmp byte ptr [rdi + 8], 0
//' mov qword ptr [rdi], 10
//' je .L0
//' ret
//' .L0:
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl)
//' cmp byte ptr [rcx + 8], 0
//' jne .L0
//' mov byte ptr [rcx + 8], 1
//' .L0:
//' mov qword ptr [rcx], 10
//' ret
//'@if $x64 & $msvc
//' mov qword ptr [rcx], 10
//' cmp byte ptr [rcx + 8], 0
//' jne .L0
//' mov byte ptr [rcx + 8], 1
//' .L0:
//' ret
//'@if $arm64 & $gcc[13..]
//' ldrb w1, [x0, 8]
//' tbnz x1, 0, .L0
//' mov w1, 1
//' strb w1, [x0, 8]
//' .L0:
//' mov x1, 10
//' str x1, [x0]
//' ret
//'@if $arm64 & $gcc[..13]
//' ldrb w1, [x0, 8]
//' cbnz w1, .L0
//' mov w1, 1
//' strb w1, [x0, 8]
//' .L0:
//' mov x1, 10
//' str x1, [x0]
//' ret
//'@if $arm64 & $clang[19..]
//' ldrb w8, [x0, 8]
//' tbnz w8, 0, .L0
//' mov w8, 1
//' strb w8, [x0, 8]
//' .L0:
//' mov w8, 10
//' str x8, [x0]
//' ret
//'@if $arm64 & $clang[..19]
//' ldrb w8, [x0, 8]
//' cbz w8, .L0
//' mov w8, 10
//' str x8, [x0]
//' ret
//' .L0:
//' mov w8, 1
//' strb w8, [x0, 8]
//' mov w8, 10
//' str x8, [x0]
//' ret
void std_optional_int64_assign_from_value(std::optional<int64_t>& a) {
    a = 10;
}

//'[option_int64_assign_from_other]
//'@if $x64 & $linux & $clang
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $linux & $gcc
//' movdqu xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl | $msvc)
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $arm64 & $gcc
//' ldp x2, x3, [x1]
//' stp x2, x3, [x0]
//' ret
//'@if $arm64 & $clang
//' ldr q0, [x1]
//' str q0, [x0]
//' ret
void option_int64_assign_from_other(opt::option<int64_t>& a, const opt::option<int64_t>& b) {
    a = b;
}

//'[std_optional_int64_assign_from_other]
//'@if $x64 & $linux & $libcxx & $clang
//' movzx eax, [rsi + 8]
//' mov [rdi + 8], al
//' mov rax, [rsi]
//' mov [rdi], rax
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & $clang
//' movzx eax, [rdx + 8]
//' mov [rcx + 8], al
//' mov rax, [rdx]
//' mov [rcx], rax
//' ret
//'@if $x64 & $linux & $gcc
//' movdqu xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $clang-cl | $msvc
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $arm64 & $gcc
//' ldp x2, x3, [x1]
//' stp x2, x3, [x0]
//' ret
//'@if $arm64 & $libcxx & $clang
//' ldrb w8, [x1, 8]
//' ldr x9, [x1]
//' strb w8, [x0, 8]
//' str x9, [x0]
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' ldr q0, [x1]
//' str q0, [x0]
//' ret
void std_optional_int64_assign_from_other(std::optional<int64_t>& a, const std::optional<int64_t>& b) {
    a = b;
}

//'[option_int64x2_assign_from_other]
//'@if $x64 & $linux & $clang
//' mov rax, [rsi + 16]
//' mov [rdi + 16], rax
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $linux & $gcc
//' movdqu xmm0, [rsi]
//' movups [rdi], xmm0
//' mov rax, [rsi + 16]
//' mov [rdi + 16], rax
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' ret
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' ret
//'@if $arm64 & $gcc[14..]
//' ldp x2, x3, [x1]
//' ldr x1, [x1, 16]
//' stp x2, x3, [x0]
//' str x1, [x0, 16]
//' ret
//'@if $arm64 & $gcc[..14]
//' ldp x2, x3, [x1]
//' stp x2, x3, [x0]
//' ldr x1, [x1, 16]
//' str x1, [x0, 16]
//' ret
//'@if $arm64 & $clang
//' ldr x8, [x1, 16]
//' ldr q0, [x1]
//' str x8, [x0, 16]
//' str q0, [x0]
//' ret
void option_int64x2_assign_from_other(opt::option<int64x2>& a, const opt::option<int64x2>& b) {
    a = b;
}

//'[std_optional_int64x2_assign_from_other]
//'@if $x64 & $linux & $libcxx & $clang
//' movzx eax, [rsi + 16]
//' mov [rdi + 16], al
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang
//' mov rax, [rsi + 16]
//' mov [rdi + 16], rax
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & $clang
//' movzx eax, [rdx + 16]
//' mov [rcx + 16], al
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $linux & $gcc
//' movdqu xmm0, [rsi]
//' movups [rdi], xmm0
//' mov rax, [rsi + 16]
//' mov [rdi + 16], rax
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' movups [rcx], xmm0
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' ret
//'@if $x64 & $clang-cl
//' mov rax, [rdx + 16]
//' mov [rcx + 16], rax
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' ret
//'@if $arm64 & $gcc[14..]
//' ldp x2, x3, [x1]
//' ldr x1, [x1, 16]
//' stp x2, x3, [x0]
//' str x1, [x0, 16]
//' ret
//'@if $arm64 & $gcc[..14]
//' ldp x2, x3, [x1]
//' stp x2, x3, [x0]
//' ldr x1, [x1, 16]
//' str x1, [x0, 16]
//' ret
//'@if $arm64 & $libcxx & $clang
//' ldrb w8, [x1, 16]
//' ldr q0, [x1]
//' strb w8, [x0, 16]
//' str q0, [x0]
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' ldr x8, [x1, 16]
//' ldr q0, [x1]
//' str x8, [x0, 16]
//' str q0, [x0]
//' ret
void std_optional_int64x2_assign_from_other(std::optional<int64x2>& a, const std::optional<int64x2>& b) {
    a = b;
}

//'[option_int64_copy]
//'@if $x64 & $linux & $clang[16..]
//' mov rax, [rdi]
//' movzx edx, [rdi + 8]
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov rax, rdi
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $linux & $gcc
//' mov rax, [rdi]
//' mov rdx, [rdi + 8]
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $arm64 & $gcc
//' ldp x0, x1, [x0]
//' ret
//'@if $arm64 & $clang
//' ldp x8, x1, [x0]
//' mov x0, x8
//' ret
opt::option<int64_t> option_int64_copy(const opt::option<int64_t>& a) {
    return a;
}

//'[std_optional_int64_copy]
//'@if $x64 & $linux & $clang
//' mov rax, [rdi]
//' movzx edx, [rdi + 8]
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $linux & $gcc
//' mov rax, [rdi]
//' mov rdx, [rdi + 8]
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movups [rcx], xmm0
//' ret
//'@if $arm64 & $gcc[14..]
//' sub sp, sp, 16
//' ldp x0, x1, [x0]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..14]
//' ldp x2, x1, [x0]
//' sub sp, sp, 16
//' add sp, sp, 16
//' mov x0, x2
//' ret
//'@if $arm64 & $clang
//' ldp x8, x1, [x0]
//' mov x0, x8
//' ret
std::optional<int64_t> std_optional_int64_copy(const std::optional<int64_t>& a) {
    return a;
}

//'[option_int64x2_copy]
//'@if $x64 & $linux & $clang
//' mov rax, rdi
//' mov rcx, [rsi + 16]
//' mov [rdi + 16], rcx
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' mov rcx, [rdx + 16]
//' mov [rax + 16], rcx
//' movups xmm0, [rdx]
//' movups [rax], xmm0
//' ret
//'@if $x64 & $linux & $gcc
//' movdqu xmm0, [rsi]
//' mov rdx, [rsi + 16]
//' mov rax, rdi
//' mov [rdi + 16], rdx
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rdx, [rdx + 16]
//' mov rax, rcx
//' mov [rcx + 16], rdx
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movsd xmm1, [rdx + 16]
//' movups [rcx], xmm0
//' movsd [rcx + 16], xmm1
//' ret
//'@if $arm64 & $gcc
//' ldp x2, x3, [x0]
//' stp x2, x3, [x8]
//' ldr x0, [x0, 16]
//' str x0, [x8, 16]
//' ret
//'@if $arm64 & $clang
//' ldr q0, [x0]
//' ldr x9, [x0, 16]
//' str q0, [x8]
//' str x9, [x8, 16]
//' ret
opt::option<int64x2> option_int64x2_copy(const opt::option<int64x2>& a) {
    return a;
}

//'[std_optional_int64x2_copy]
//'@if $x64 & $linux & $clang
//' mov rax, rdi
//' mov rcx, [rsi + 16]
//' mov [rdi + 16], rcx
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' mov rax, rcx
//' mov rcx, [rdx + 16]
//' mov [rax + 16], rcx
//' movups xmm0, [rdx]
//' movups [rax], xmm0
//' ret
//'@if $x64 & $linux & $gcc
//' movdqu xmm0, [rsi]
//' mov rdx, [rsi + 16]
//' mov rax, rdi
//' mov [rdi + 16], rdx
//' movups [rdi], xmm0
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rdx]
//' mov rdx, [rdx + 16]
//' mov rax, rcx
//' mov [rcx + 16], rdx
//' movups [rcx], xmm0
//' ret
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' mov rax, rcx
//' movsd xmm1, [rdx + 16]
//' movups [rcx], xmm0
//' movsd [rcx + 16], xmm1
//' ret
//'@if $arm64 & $gcc
//' ldp x2, x3, [x0]
//' stp x2, x3, [x8]
//' ldr x0, [x0, 16]
//' str x0, [x8, 16]
//' ret
//'@if $arm64 & $clang
//' ldr q0, [x0]
//' ldr x9, [x0, 16]
//' str q0, [x8]
//' str x9, [x8, 16]
//' ret
std::optional<int64x2> std_optional_int64x2_copy(const std::optional<int64x2>& a) {
    return a;
}

//'[option_uint8x64_copy]
//'@if $x64 & $linux & $clang
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
//'@if $x64 & $windows & ($clang | $clang-cl)
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
//'@if $x64 & $linux & $gcc[14..]
//' movdqu xmm0, [rsi]
//' movzx edx, [rsi + 64]
//' mov rax, rdi
//' movups [rdi], xmm0
//' movdqu xmm0, [rsi + 16]
//' mov [rdi + 64], dl
//' movups [rdi + 16], xmm0
//' movdqu xmm0, [rsi + 32]
//' movups [rdi + 32], xmm0
//' movdqu xmm0, [rsi + 48]
//' movups [rdi + 48], xmm0
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' movdqu xmm0, [rsi]
//' movdqu xmm1, [rsi + 16]
//' mov rax, rdi
//' movdqu xmm2, [rsi + 32]
//' movdqu xmm3, [rsi + 48]
//' movzx edx, [rsi + 64]
//' movups [rdi], xmm0
//' movups [rdi + 16], xmm1
//' mov [rdi + 64], dl
//' movups [rdi + 32], xmm2
//' movups [rdi + 48], xmm3
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[14..]
//' ldp q30, q29, [x0]
//' ldr q31, [x0, 32]
//' stp q30, q29, [x8]
//' ldrb w1, [x0, 64]
//' ldr q30, [x0, 48]
//' strb w1, [x8, 64]
//' stp q31, q30, [x8, 32]
//' ret
//'@if $arm64 & $gcc[..14]
//' ldp q2, q3, [x0]
//' ldp q0, q1, [x0, 32]
//' ldrb w0, [x0, 64]
//' stp q2, q3, [x8]
//' stp q0, q1, [x8, 32]
//' strb w0, [x8, 64]
//' ret
//'@if $arm64 & $clang[23..]
//' ldp q0, q1, [x0]
//' ldrb w9, [x0, 64]
//' strb w9, [x8, 64]
//' stp q0, q1, [x8]
//' ldp q0, q2, [x0, 32]
//' stp q0, q2, [x8, 32]
//' ret
//'@if $arm64 & $clang[18..23]
//' ldp q0, q1, [x0, 32]
//' ldrb w9, [x0, 64]
//' strb w9, [x8, 64]
//' stp q0, q1, [x8, 32]
//' ldp q1, q0, [x0]
//' stp q1, q0, [x8]
//' ret
//'@if $arm64 & $clang[..18]
//' ldp q0, q1, [x0, 32]
//' stp q0, q1, [x8, 32]
//' ldrb w9, [x0, 64]
//' ldp q2, q0, [x0]
//' strb w9, [x8, 64]
//' stp q2, q0, [x8]
//' ret
opt::option<std::array<uint8_t, 64>> option_uint8x64_copy(const opt::option<std::array<uint8_t, 64>>& a) {
    return a;
}

//'[std_optional_uint8x64_copy]
//'@if $x64 & $linux & $clang
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
//'@if $x64 & $windows & ($clang | $clang-cl)
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
//'@if $x64 & $linux & $gcc[14..]
//' movdqu xmm0, [rsi]
//' movzx edx, [rsi + 64]
//' mov rax, rdi
//' movups [rdi], xmm0
//' movdqu xmm0, [rsi + 16]
//' mov [rdi + 64], dl
//' movups [rdi + 16], xmm0
//' movdqu xmm0, [rsi + 32]
//' movups [rdi + 32], xmm0
//' movdqu xmm0, [rsi + 48]
//' movups [rdi + 48], xmm0
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' movdqu xmm0, [rsi]
//' movdqu xmm1, [rsi + 16]
//' mov rax, rdi
//' movdqu xmm2, [rsi + 32]
//' movdqu xmm3, [rsi + 48]
//' movzx edx, [rsi + 64]
//' movups [rdi], xmm0
//' movups [rdi + 16], xmm1
//' mov [rdi + 64], dl
//' movups [rdi + 32], xmm2
//' movups [rdi + 48], xmm3
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[14..]
//' ldp q30, q29, [x0]
//' ldr q31, [x0, 32]
//' stp q30, q29, [x8]
//' ldrb w1, [x0, 64]
//' ldr q30, [x0, 48]
//' strb w1, [x8, 64]
//' stp q31, q30, [x8, 32]
//' ret
//'@if $arm64 & $gcc[..14]
//' ldp q2, q3, [x0]
//' ldp q0, q1, [x0, 32]
//' ldrb w0, [x0, 64]
//' stp q2, q3, [x8]
//' stp q0, q1, [x8, 32]
//' strb w0, [x8, 64]
//' ret
//'@if $arm64 & $clang[23..]
//' ldp q0, q1, [x0]
//' ldrb w9, [x0, 64]
//' strb w9, [x8, 64]
//' stp q0, q1, [x8]
//' ldp q0, q2, [x0, 32]
//' stp q0, q2, [x8, 32]
//' ret
//'@if $arm64 & $clang[18..23]
//' ldp q0, q1, [x0, 32]
//' ldrb w9, [x0, 64]
//' strb w9, [x8, 64]
//' stp q0, q1, [x8, 32]
//' ldp q1, q0, [x0]
//' stp q1, q0, [x8]
//' ret
//'@if $arm64 & $clang[..18]
//' ldp q0, q1, [x0, 32]
//' stp q0, q1, [x8, 32]
//' ldrb w9, [x0, 64]
//' ldp q2, q0, [x0]
//' strb w9, [x8, 64]
//' stp q2, q0, [x8]
//' ret
std::optional<std::array<uint8_t, 64>> std_optional_uint8x64_copy(const std::optional<std::array<uint8_t, 64>>& a) {
    return a;
}

//'[option_uint8x64_default]
//'@if $x64 & $linux & $clang
//' mov rax, rdi
//' mov byte ptr [rdi + 64], 0
//' ret
//'@if $x64 & $linux & $gcc
//' mov byte ptr [rdi + 64], 0
//' mov rax, rdi
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl)
//' mov rax, rcx
//' mov byte ptr [rcx + 64], 0
//' ret
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 64], 0
//' mov rax, rcx
//' ret
//'@if $arm64
//' strb wzr, [x8, 64]
//' ret
opt::option<std::array<uint8_t, 64>> option_uint8x64_default() {
    return {};
}

//'[std_optional_uint8x64_default]
//'@if $x64 & $linux & $libcxx & $clang
//' mov rax, rdi
//' mov byte ptr [rdi], 0
//' mov byte ptr [rdi + 64], 0
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang
//' mov rax, rdi
//' mov byte ptr [rdi + 64], 0
//' ret
//'@if $x64 & $windows & $clang
//' mov rax, rcx
//' mov byte ptr [rcx], 0
//' mov byte ptr [rcx + 64], 0
//' ret
//'@if $x64 & $linux & $gcc[15..]
//' pxor xmm0, xmm0
//' mov byte ptr [rdi + 64], 0
//' mov rax, rdi
//' movups [rdi], xmm0
//' movups [rdi + 16], xmm0
//' movups [rdi + 32], xmm0
//' movups [rdi + 48], xmm0
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' mov byte ptr [rdi + 64], 0
//' mov rax, rdi
//' ret
//'@if $x64 & $windows & $gcc
//' pxor xmm0, xmm0
//' mov rax, rcx
//' mov byte ptr [rcx + 64], 0
//' movups [rcx], xmm0
//' movups [rcx + 16], xmm0
//' movups [rcx + 32], xmm0
//' movups [rcx + 48], xmm0
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 64], 0
//' ret
//'@if $x64 & $msvc
//' mov byte ptr [rcx + 64], 0
//' mov rax, rcx
//' ret
//'@if $arm64 & $gcc[15..]
//' movi v31.4s, 0
//' strb wzr, [x8, 64]
//' stp q31, q31, [x8]
//' stp q31, q31, [x8, 32]
//' ret
//'@if $arm64 & $gcc[..15]
//' strb wzr, [x8, 64]
//' ret
//'@if $arm64 & $libcxx & $clang
//' strb wzr, [x8]
//' strb wzr, [x8, 64]
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' strb wzr, [x8, 64]
//' ret
std::optional<std::array<uint8_t, 64>> std_optional_uint8x64_default() {
    return {};
}

//'[option_int32_to_uint32_ctor]
//'@if $x64 & $linux & ($clang[16..] | $gcc)
//' mov rax, 1099511627775
//' and rax, rdi
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov rax, rdi
//' mov ecx, [rsp + 8]
//' mov [rdi], ecx
//' movzx ecx, [rsp + 12]
//' mov [rdi + 4], cl
//' ret
//'@if $x64 & $windows & ($clang | $gcc)
//' mov rax, 1099511627775
//' and rax, rcx
//' ret
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov [rcx], edx
//' shr rdx, 32
//' mov [rcx + 4], dl
//' ret
//'@if $x64 & $msvc
//' mov [rcx], edx
//' mov rax, rcx
//' shr rdx, 32
//' mov [rcx + 4], dl
//' ret
//'@if $arm64 & $gcc[14..]
//' mov x1, 0
//' ubfx x2, x0, 32, 8
//' bfi x1, x0, 0, 32
//' bfi x1, x2, 32, 8
//' mov x0, x1
//' ret
//'@if $arm64 & $gcc[..14]
//' mov x1, x0
//' mov x0, 0
//' bfi x0, x1, 0, 32
//' ubfx x1, x1, 32, 8
//' bfi x0, x1, 32, 8
//' ret
//'@if $arm64 & $clang
//' and x0, x0, 0xffffffffff
//' ret
opt::option<uint32_t> option_int32_to_uint32_ctor(const opt::option<int32_t> a) {
    return a;
}

//'[std_optional_int32_to_uint32_ctor]
//'@if $x64 & $linux & $libcxx & $clang[19..]
//' mov rcx, 4294967296
//' and rcx, rdi
//' mov rax, rdi
//' shl rax, 31
//' sar rax, 63
//' and eax, edi
//' or rax, rcx
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..19]
//' mov rcx, 1095216660480
//' and rcx, rdi
//' mov edx, edi
//' mov rax, 4294967296
//' or rax, rdx
//' test rcx, rcx
//' cmove rax, rcx
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[19..]
//' mov rax, 8589934591
//' and rax, rdi
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[..19]
//' mov eax, edi
//' shr rdi, 32
//' xor ecx, ecx
//' test dil, dil
//' setne cl
//' shl rcx, 32
//' or rax, rcx
//' ret
//'@if $x64 & $windows & $clang
//' mov rdx, 4294967296
//' and rdx, rcx
//' mov rax, rcx
//' shl rax, 31
//' sar rax, 63
//' and eax, ecx
//' or rax, rdx
//' ret
//'@if $x64 & $linux & $gcc
//' mov rax, rdi
//' mov [rsp - 24], rdi
//' xor edx, edx
//' shr rax, 32
//' test al, al
//' je .L0
//' mov ecx, [rsp - 24]
//' mov edx, 1
//' .L0:
//' mov [rsp - 8], ecx
//' mov [rsp - 4], dl
//' mov rax, [rsp - 8]
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
//' mov rax, rcx
//' mov byte ptr [rcx + 4], 0
//' bt rdx, 32
//' jae .L0
//' mov [rax], edx
//' mov byte ptr [rax + 4], 1
//' .L0:
//' ret
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[13..]
//' sub sp, sp, 32
//' mov w1, 0
//' str x0, [sp, 8]
//' tbz x0, 32, .L0
//' ldr w2, [sp, 8]
//' mov w1, 1
//' .L0:
//' str w2, [sp, 24]
//' strb w1, [sp, 28]
//' ldr x0, [sp, 24]
//' add sp, sp, 32
//' ret
//'@if $arm64 & $gcc[..13]
//' sub sp, sp, 32
//' mov w1, 0
//' str x0, [sp, 8]
//' tst x0, 1095216660480
//' beq .L0
//' ldr w2, [sp, 8]
//' mov w1, 1
//' .L0:
//' str w2, [sp, 24]
//' strb w1, [sp, 28]
//' ldr x0, [sp, 24]
//' add sp, sp, 32
//' ret
//'@if $arm64 & $libcxx & $clang[19..]
//' sbfx x8, x0, 32, 1
//' and x9, x0, 0x100000000
//' and w8, w0, w8
//' orr x0, x8, x9
//' ret
//'@if $arm64 & $libcxx & $clang[..19]
//' mov x8, 4294967296
//' tst x0, 0xff00000000
//' bfxil x8, x0, 0, 32
//' csel x0, xzr, x8, eq
//' ret
//'@if $arm64 & $libstdcxx & $clang[19..]
//' and x0, x0, 0x1ffffffff
//' ret
//'@if $arm64 & $libstdcxx & $clang[..19]
//' tst x0, 0xff00000000
//' and x9, x0, 0xffffffff
//' cset w8, ne
//' orr x0, x9, x8, lsl 32
//' ret
std::optional<uint32_t> std_optional_int32_to_uint32_ctor(const std::optional<int32_t> a) {
    return a;
}

//'[option_int32_to_uint64_ctor]
//'@if $x64 & $linux & $clang[16..]
//' movsxd rax, edi
//' mov rdx, rdi
//' shr rdx, 32
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov rax, rdi
//' movsxd rcx, [rsp + 8]
//' mov [rdi], rcx
//' movzx ecx, [rsp + 12]
//' mov [rdi + 8], cl
//' ret
//'@if $x64 & $linux & $gcc[14..]
//' mov rcx, rdi
//' movsxd rax, edi
//' shr rcx, 32
//' movzx edx, cl
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' mov rcx, rdi
//' movsx rax, edi
//' shr rcx, 32
//' movzx edx, cl
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl)
//' mov rax, rcx
//' movsxd rcx, edx
//' shr rdx, 32
//' mov [rax], rcx
//' mov [rax + 8], dl
//' ret
//'@if $x64 & $msvc
//' movsxd rax, edx
//' mov [rcx], rax
//' mov rax, rcx
//' shr rdx, 32
//' mov [rcx + 8], dl
//' ret
//'@if $arm64 & $gcc
//' mov x1, x0
//' sxtw x0, w0
//' ubfx x1, x1, 32, 8
//' ret
//'@if $arm64 & $clang
//' sxtw x8, w0
//' ubfx x1, x0, 32, 8
//' mov x0, x8
//' ret
opt::option<uint64_t> option_int32_to_uint64_ctor(const opt::option<int32_t> a) {
    return a;
}

//'[option_int32_assign_from_uint32]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov eax, [rsi]
//' mov [rdi], eax
//' movzx eax, [rsi + 4]
//' mov [rdi + 4], al
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' mov eax, [rdx]
//' mov [rcx], eax
//' movzx eax, [rdx + 4]
//' mov [rcx + 4], al
//' ret
//'@if $x64 & $windows & $gcc
//' mov eax, [rdx]
//' mov [rcx], eax
//' movzx eax, [rdx + 4]
//' mov [rcx + 4], al
//' ret
//'@if $x64 & $msvc
//' mov eax, [rdx]
//' mov [rcx], eax
//' movzx eax, [rdx + 4]
//' mov [rcx + 4], al
//' ret
//'@if $arm64 & $gcc
//' ldr w2, [x1]
//' ldrb w1, [x1, 4]
//' str w2, [x0]
//' strb w1, [x0, 4]
//' ret
//'@if $arm64 & $clang
//' ldr w8, [x1]
//' str w8, [x0]
//' ldrb w8, [x1, 4]
//' strb w8, [x0, 4]
//' ret
void option_int32_assign_from_uint32(opt::option<int32_t>& a, const opt::option<uint32_t>& b) {
    a = b;
}

//'[option_int64_assign_from_uint32]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov eax, [rsi]
//' mov [rdi], rax
//' movzx eax, [rsi + 4]
//' mov [rdi + 8], al
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' mov eax, [rdx]
//' mov [rcx], rax
//' movzx eax, [rdx + 4]
//' mov [rcx + 8], al
//' ret
//'@if $x64 & $windows & $gcc
//' mov eax, [rdx]
//' mov [rcx], rax
//' movzx eax, [rdx + 4]
//' mov [rcx + 8], al
//' ret
//'@if $x64 & $msvc
//' mov eax, [rdx]
//' mov [rcx], rax
//' movzx eax, [rdx + 4]
//' mov [rcx + 8], al
//' ret
//'@if $arm64 & $gcc
//' ldrb w2, [x1, 4]
//' ldr w1, [x1]
//' str x1, [x0]
//' strb w2, [x0, 8]
//' ret
//'@if $arm64 & $clang
//' ldr w8, [x1]
//' str x8, [x0]
//' ldrb w8, [x1, 4]
//' strb w8, [x0, 8]
//' ret
void option_int64_assign_from_uint32(opt::option<int64_t>& a, const opt::option<uint32_t>& b) {
    a = b;
}

//'[option_int32_emplace]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov dword ptr [rdi], 100
//' mov byte ptr [rdi + 4], 1
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov dword ptr [rcx], 100
//' mov byte ptr [rcx + 4], 1
//' ret
//'@if $arm64 & $gcc
//' mov w2, 100
//' mov w1, 1
//' str w2, [x0]
//' strb w1, [x0, 4]
//' ret
//'@if $arm64 & $clang
//' mov w8, 100
//' mov w9, 1
//' str w8, [x0]
//' strb w9, [x0, 4]
//' ret
void option_int32_emplace(opt::option<int32_t>& a) {
    a.emplace(100);
}

//'[std_optional_int32_emplace]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov dword ptr [rdi], 100
//' mov byte ptr [rdi + 4], 1
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov dword ptr [rcx], 100
//' mov byte ptr [rcx + 4], 1
//' ret
//'@if $arm64 & $gcc
//' mov w2, 100
//' mov w1, 1
//' str w2, [x0]
//' strb w1, [x0, 4]
//' ret
//'@if $arm64 & $clang
//' mov w8, 100
//' mov w9, 1
//' str w8, [x0]
//' strb w9, [x0, 4]
//' ret
void std_optional_int32_emplace(std::optional<int32_t>& a) {
    a.emplace(100);
}

//'[option_int64_emplace]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov qword ptr [rdi], 100
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov qword ptr [rcx], 100
//' mov byte ptr [rcx + 8], 1
//' ret
//'@if $arm64 & $gcc
//' mov x2, 100
//' mov w1, 1
//' str x2, [x0]
//' strb w1, [x0, 8]
//' ret
//'@if $arm64 & $clang
//' mov w8, 100
//' mov w9, 1
//' str x8, [x0]
//' strb w9, [x0, 8]
//' ret
void option_int64_emplace(opt::option<int64_t>& a) {
    a.emplace(100);
}

//'[std_optional_int64_emplace]
//'@if $x64 & $linux & ($clang | $gcc)
//' mov qword ptr [rdi], 100
//' mov byte ptr [rdi + 8], 1
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $msvc | $clang-cl)
//' mov qword ptr [rcx], 100
//' mov byte ptr [rcx + 8], 1
//' ret
//'@if $arm64 & $gcc
//' mov x2, 100
//' mov w1, 1
//' str x2, [x0]
//' strb w1, [x0, 8]
//' ret
//'@if $arm64 & $clang
//' mov w8, 100
//' mov w9, 1
//' str x8, [x0]
//' strb w9, [x0, 8]
//' ret
void std_optional_int64_emplace(std::optional<int64_t>& a) {
    a.emplace(100);
}

//'[option_int32_swap]
//'@if $x64 & $linux & $clang
//' mov rax, [rdi]
//' mov rcx, [rsi]
//' mov [rdi], rcx
//' mov [rsi], rax
//' ret
//'@if $x64 & $linux & $gcc
//' mov rax, [rdi]
//' mov rdx, [rsi]
//' mov [rdi], rdx
//' mov [rsi], rax
//' ret
//'@if $x64 & $windows & ($clang | $gcc | $clang-cl)
//' mov rax, [rcx]
//' mov r8, [rdx]
//' mov [rcx], r8
//' mov [rdx], rax
//' ret
//'@if $x64 & $msvc
//' mov rax, [rdx]
//' mov r8, [rcx]
//' mov [rcx], rax
//' mov [rdx], r8
//' ret
//'@if $arm64 & $gcc
//' ldr x3, [x1]
//' ldr x2, [x0]
//' str x3, [x0]
//' str x2, [x1]
//' ret
//'@if $arm64 & $clang
//' ldr x8, [x1]
//' ldr x9, [x0]
//' str x8, [x0]
//' str x9, [x1]
//' ret
void option_int32_swap(opt::option<int32_t>& a, opt::option<int32_t>& b) {
    a.swap(b);
}

//'[std_optional_int32_swap]
//'@if $x64 & $linux & $libcxx & $clang[19..]
//' movzx eax, [rdi + 4]
//' cmp al, [rsi + 4]
//' jne .L0
//' test al, al
//' je .L1
//' mov eax, [rdi]
//' mov ecx, [rsi]
//' mov [rdi], ecx
//' mov [rsi], eax
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov eax, [rdi]
//' mov [rsi], eax
//' mov byte ptr [rsi + 4], 1
//' cmp byte ptr [rdi + 4], 1
//' jne .L1
//' mov byte ptr [rdi + 4], 0
//' ret
//' .L2:
//' mov eax, [rsi]
//' mov [rdi], eax
//' mov byte ptr [rdi + 4], 1
//' cmp byte ptr [rsi + 4], 1
//' jne .L1
//' mov byte ptr [rsi + 4], 0
//' .L1:
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..19]
//' movzx eax, [rdi + 4]
//' test al, al
//' setne cl
//' cmp byte ptr [rsi + 4], 0
//' setne dl
//' xor dl, cl
//' je .L0
//' test al, al
//' je .L1
//' mov eax, [rdi]
//' mov [rsi], eax
//' mov byte ptr [rsi + 4], 1
//' cmp byte ptr [rdi + 4], 0
//' je .L2
//' mov byte ptr [rdi + 4], 0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov eax, [rdi]
//' mov ecx, [rsi]
//' mov [rdi], ecx
//' mov [rsi], eax
//' ret
//' .L1:
//' mov eax, [rsi]
//' mov [rdi], eax
//' mov byte ptr [rdi + 4], 1
//' cmp byte ptr [rsi + 4], 0
//' je .L2
//' mov byte ptr [rsi + 4], 0
//' .L2:
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[19..]
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
//'@if $x64 & $linux & $libstdcxx & $clang[..19]
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
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[15..]
//' movzx eax, [rsi + 4]
//' cmp byte ptr [rdi + 4], 0
//' je .L0
//' mov edx, [rdi]
//' test al, al
//' je .L1
//' mov eax, [rsi]
//' mov [rdi], eax
//' mov [rsi], edx
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov eax, [rsi]
//' mov byte ptr [rdi + 4], 1
//' mov [rdi], eax
//' mov byte ptr [rsi + 4], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], edx
//' mov byte ptr [rsi + 4], 1
//' mov byte ptr [rdi + 4], 0
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' cmp byte ptr [rdi + 4], 0
//' movzx eax, [rsi + 4]
//' je .L0
//' mov edx, [rdi]
//' test al, al
//' je .L1
//' mov eax, [rsi]
//' mov [rdi], eax
//' mov [rsi], edx
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov eax, [rsi]
//' mov byte ptr [rdi + 4], 1
//' mov [rdi], eax
//' mov byte ptr [rsi + 4], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], edx
//' mov byte ptr [rsi + 4], 1
//' mov byte ptr [rdi + 4], 0
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
//' mov rax, [rcx]
//' mov r8, [rdx]
//' mov [rcx], r8
//' mov [rdx], rax
//' ret
//'@if $x64 & $msvc
//' mov rax, [rdx]
//' mov r8, [rcx]
//' mov [rcx], rax
//' mov [rdx], r8
//' ret
//'@if $arm64 & $gcc[13..]
//' ldrb w3, [x0, 4]
//' ldrb w2, [x1, 4]
//' tbz x3, 0, .L0
//' ldr w3, [x0]
//' tbz x2, 0, .L1
//' ldr w2, [x1]
//' str w2, [x0]
//' str w3, [x1]
//' .L2:
//' ret
//' .L0:
//' tbz x2, 0, .L2
//' ldr w3, [x1]
//' mov w2, 1
//' str w3, [x0]
//' strb w2, [x0, 4]
//' strb wzr, [x1, 4]
//' ret
//' .L1:
//' mov w2, 1
//' str w3, [x1]
//' strb w2, [x1, 4]
//' strb wzr, [x0, 4]
//' ret
//'@if $arm64 & $gcc[..13]
//' ldrb w3, [x0, 4]
//' ldrb w2, [x1, 4]
//' cbz w3, .L0
//' ldr w3, [x0]
//' cbz w2, .L1
//' ldr w2, [x1]
//' str w2, [x0]
//' str w3, [x1]
//' .L2:
//' ret
//' .L0:
//' cbz w2, .L2
//' ldr w3, [x1]
//' mov w2, 1
//' str w3, [x0]
//' strb w2, [x0, 4]
//' strb wzr, [x1, 4]
//' ret
//' .L1:
//' mov w2, 1
//' str w3, [x1]
//' strb w2, [x1, 4]
//' strb wzr, [x0, 4]
//' ret
//'@if $arm64 & $libcxx & $clang[23..]
//' ldrb w8, [x0, 4]
//' ldrb w9, [x1, 4]
//' cmp w8, w9
//' b.ne .L0
//' cbz w8, .L1
//' ldr w8, [x1]
//' ldr w9, [x0]
//' str w8, [x0]
//' str w9, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' mov w8, 1
//' strb w8, [x1, 4]
//' ldr w8, [x0]
//' ldrb w9, [x0, 4]
//' str w8, [x1]
//' cbz w9, .L1
//' strb wzr, [x0, 4]
//' ret
//' .L2:
//' mov w8, 1
//' strb w8, [x0, 4]
//' ldr w8, [x1]
//' ldrb w9, [x1, 4]
//' str w8, [x0]
//' cbz w9, .L1
//' strb wzr, [x1, 4]
//' .L1:
//' ret
//'@if $arm64 & $libcxx & $clang[19..23]
//' ldrb w8, [x0, 4]
//' ldrb w9, [x1, 4]
//' cmp w8, w9
//' b.ne .L0
//' cbz w8, .L1
//' ldr w8, [x1]
//' ldr w9, [x0]
//' str w8, [x0]
//' str w9, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' mov w8, 1
//' ldr w9, [x0]
//' strb w8, [x1, 4]
//' ldrb w8, [x0, 4]
//' str w9, [x1]
//' cmp w8, 1
//' b.ne .L1
//' strb wzr, [x0, 4]
//' ret
//' .L2:
//' mov w8, 1
//' ldr w9, [x1]
//' strb w8, [x0, 4]
//' ldrb w8, [x1, 4]
//' str w9, [x0]
//' cmp w8, 1
//' b.ne .L1
//' strb wzr, [x1, 4]
//' .L1:
//' ret
//'@if $arm64 & $libcxx & $clang[18..19]
//' ldrb w8, [x0, 4]
//' ldrb w9, [x1, 4]
//' cmp w8, 0
//' cset w10, ne
//' cmp w9, 0
//' cset w9, ne
//' eor w9, w10, w9
//' tbz w9, 0, .L0
//' cbz w8, .L1
//' mov w8, 1
//' strb w8, [x1, 4]
//' ldr w8, [x0]
//' ldrb w9, [x0, 4]
//' str w8, [x1]
//' cbz w9, .L2
//' strb wzr, [x0, 4]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr w8, [x1]
//' ldr w9, [x0]
//' str w8, [x0]
//' str w9, [x1]
//' ret
//' .L1:
//' mov w8, 1
//' strb w8, [x0, 4]
//' ldr w8, [x1]
//' ldrb w9, [x1, 4]
//' str w8, [x0]
//' cbz w9, .L2
//' strb wzr, [x1, 4]
//' .L2:
//' ret
//'@if $arm64 & $libcxx & $clang[..18]
//' ldrb w8, [x0, 4]
//' ldrb w9, [x1, 4]
//' cmp w8, 0
//' cset w10, ne
//' cmp w9, 0
//' cset w9, ne
//' eor w9, w10, w9
//' tbz w9, 0, .L0
//' cbz w8, .L1
//' mov w8, 1
//' ldr w9, [x0]
//' strb w8, [x1, 4]
//' ldrb w8, [x0, 4]
//' str w9, [x1]
//' cbz w8, .L2
//' strb wzr, [x0, 4]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr w8, [x1]
//' ldr w9, [x0]
//' str w8, [x0]
//' str w9, [x1]
//' ret
//' .L1:
//' mov w8, 1
//' ldr w9, [x1]
//' strb w8, [x0, 4]
//' ldrb w8, [x1, 4]
//' str w9, [x0]
//' cbz w8, .L2
//' strb wzr, [x1, 4]
//' .L2:
//' ret
//'@if $arm64 & $libstdcxx & $clang[23..]
//' ldrb w8, [x0, 4]
//' ldrb w9, [x1, 4]
//' cbz w8, .L0
//' ldr w8, [x0]
//' cbz w9, .L1
//' ldr w9, [x1]
//' str w9, [x0]
//' str w8, [x1]
//' ret
//' .L0:
//' cbz w9, .L2
//' ldr w8, [x1]
//' mov w9, 1
//' strb w9, [x0, 4]
//' str w8, [x0]
//' strb wzr, [x1, 4]
//' .L2:
//' ret
//' .L1:
//' str w8, [x1]
//' mov w8, 1
//' strb w8, [x1, 4]
//' strb wzr, [x0, 4]
//' ret
//'@if $arm64 & $libstdcxx & $clang[19..23]
//' ldrb w9, [x0, 4]
//' ldrb w8, [x1, 4]
//' cmp w9, 1
//' b.ne .L0
//' ldr w9, [x0]
//' cbz w8, .L1
//' ldr w8, [x1]
//' str w8, [x0]
//' str w9, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr w8, [x1]
//' mov w9, 1
//' strb w9, [x0, 4]
//' str w8, [x0]
//' strb wzr, [x1, 4]
//' .L2:
//' ret
//' .L1:
//' mov w8, 1
//' str w9, [x1]
//' strb w8, [x1, 4]
//' strb wzr, [x0, 4]
//' ret
//'@if $arm64 & $libstdcxx & $clang[18..19]
//' ldrb w8, [x0, 4]
//' ldrb w9, [x1, 4]
//' cbz w8, .L0
//' ldr w8, [x0]
//' cbz w9, .L1
//' ldr w9, [x1]
//' str w9, [x0]
//' str w8, [x1]
//' ret
//' .L0:
//' cbz w9, .L2
//' ldr w8, [x1]
//' mov w9, 1
//' strb w9, [x0, 4]
//' str w8, [x0]
//' strb wzr, [x1, 4]
//' .L2:
//' ret
//' .L1:
//' str w8, [x1]
//' mov w8, 1
//' strb w8, [x1, 4]
//' strb wzr, [x0, 4]
//' ret
//'@if $arm64 & $libstdcxx & $clang[..18]
//' ldrb w8, [x0, 4]
//' ldrb w9, [x1, 4]
//' cbz w8, .L0
//' ldr w8, [x0]
//' cbz w9, .L1
//' ldr w9, [x1]
//' str w9, [x0]
//' str w8, [x1]
//' ret
//' .L0:
//' cbz w9, .L2
//' ldr w8, [x1]
//' mov w9, 1
//' str w8, [x0]
//' strb w9, [x0, 4]
//' strb wzr, [x1, 4]
//' .L2:
//' ret
//' .L1:
//' mov w9, 1
//' str w8, [x1]
//' strb w9, [x1, 4]
//' strb wzr, [x0, 4]
//' ret
void std_optional_int32_swap(std::optional<int32_t>& a, std::optional<int32_t>& b) {
    a.swap(b);
}

//'[option_int64_swap]
//'@if $x64 & $linux & $clang
//' movups xmm0, [rdi]
//' movaps [rsp - 24], xmm0
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' movaps xmm0, [rsp - 24]
//' movups [rsi], xmm0
//' ret
//'@if $x64 & $windows & $clang
//' sub rsp, 24
//' movups xmm0, [rcx]
//' movaps [rsp], xmm0
//' movups xmm0, [rdx]
//' movups [rcx], xmm0
//' movaps xmm0, [rsp]
//' movups [rdx], xmm0
//' add rsp, 24
//' ret
//'@if $x64 & $linux & $gcc
//' movdqu xmm0, [rdi]
//' movdqu xmm1, [rsi]
//' movups [rdi], xmm1
//' movups [rsi], xmm0
//' ret
//'@if $x64 & $windows & $gcc
//' movdqu xmm0, [rcx]
//' movdqu xmm1, [rdx]
//' movups [rcx], xmm1
//' movups [rdx], xmm0
//' ret
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' movups xmm1, [rcx]
//' movups [rcx], xmm0
//' movups [rdx], xmm1
//' ret
//'@if $arm64 & $gcc
//' ldp x4, x5, [x1]
//' ldp x2, x3, [x0]
//' stp x4, x5, [x0]
//' stp x2, x3, [x1]
//' ret
//'@if $arm64 & $clang
//' sub sp, sp, 16
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' add sp, sp, 16
//' ret
void option_int64_swap(opt::option<int64_t>& a, opt::option<int64_t>& b) {
    a.swap(b);
}

//'[std_optional_int64_swap]
//'@if $x64 & $linux & $libcxx & $clang[19..]
//' movzx eax, [rdi + 8]
//' cmp al, [rsi + 8]
//' jne .L0
//' test al, al
//' je .L1
//' mov rax, [rdi]
//' mov rcx, [rsi]
//' mov [rdi], rcx
//' mov [rsi], rax
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov rax, [rdi]
//' mov [rsi], rax
//' mov byte ptr [rsi + 8], 1
//' cmp byte ptr [rdi + 8], 1
//' jne .L1
//' mov byte ptr [rdi + 8], 0
//' ret
//' .L2:
//' mov rax, [rsi]
//' mov [rdi], rax
//' mov byte ptr [rdi + 8], 1
//' cmp byte ptr [rsi + 8], 1
//' jne .L1
//' mov byte ptr [rsi + 8], 0
//' .L1:
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..19]
//' movzx eax, [rdi + 8]
//' test al, al
//' setne cl
//' cmp byte ptr [rsi + 8], 0
//' setne dl
//' xor dl, cl
//' je .L0
//' test al, al
//' je .L1
//' mov rax, [rdi]
//' mov [rsi], rax
//' mov byte ptr [rsi + 8], 1
//' cmp byte ptr [rdi + 8], 0
//' je .L2
//' mov byte ptr [rdi + 8], 0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov rax, [rdi]
//' mov rcx, [rsi]
//' mov [rdi], rcx
//' mov [rsi], rax
//' ret
//' .L1:
//' mov rax, [rsi]
//' mov [rdi], rax
//' mov byte ptr [rdi + 8], 1
//' cmp byte ptr [rsi + 8], 0
//' je .L2
//' mov byte ptr [rsi + 8], 0
//' .L2:
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[19..]
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
//'@if $x64 & $linux & $libstdcxx & $clang[..19]
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
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[15..]
//' movzx eax, [rsi + 8]
//' cmp byte ptr [rdi + 8], 0
//' je .L0
//' mov rdx, [rdi]
//' test al, al
//' je .L1
//' mov rax, [rsi]
//' mov [rdi], rax
//' mov [rsi], rdx
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov rax, [rsi]
//' mov byte ptr [rdi + 8], 1
//' mov [rdi], rax
//' mov byte ptr [rsi + 8], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], rdx
//' mov byte ptr [rsi + 8], 1
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' cmp byte ptr [rdi + 8], 0
//' movzx eax, [rsi + 8]
//' je .L0
//' mov rdx, [rdi]
//' test al, al
//' je .L1
//' mov rax, [rsi]
//' mov [rdi], rax
//' mov [rsi], rdx
//' ret
//' .L0:
//' test al, al
//' je .L2
//' mov rax, [rsi]
//' mov byte ptr [rdi + 8], 1
//' mov [rdi], rax
//' mov byte ptr [rsi + 8], 0
//' .L2:
//' ret
//' .L1:
//' mov [rsi], rdx
//' mov byte ptr [rsi + 8], 1
//' mov byte ptr [rdi + 8], 0
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' movups xmm1, [rcx]
//' movups [rcx], xmm0
//' movups [rdx], xmm1
//' ret
//'@if $arm64 & $gcc[13..]
//' ldrb w3, [x0, 8]
//' ldrb w2, [x1, 8]
//' tbz x3, 0, .L0
//' ldr x3, [x0]
//' tbz x2, 0, .L1
//' ldr x2, [x1]
//' str x2, [x0]
//' str x3, [x1]
//' .L2:
//' ret
//' .L0:
//' tbz x2, 0, .L2
//' ldr x3, [x1]
//' mov w2, 1
//' str x3, [x0]
//' strb w2, [x0, 8]
//' strb wzr, [x1, 8]
//' ret
//' .L1:
//' mov w2, 1
//' str x3, [x1]
//' strb w2, [x1, 8]
//' strb wzr, [x0, 8]
//' ret
//'@if $arm64 & $gcc[..13]
//' ldrb w3, [x0, 8]
//' ldrb w2, [x1, 8]
//' cbz w3, .L0
//' ldr x3, [x0]
//' cbz w2, .L1
//' ldr x2, [x1]
//' str x2, [x0]
//' str x3, [x1]
//' .L2:
//' ret
//' .L0:
//' cbz w2, .L2
//' ldr x3, [x1]
//' mov w2, 1
//' str x3, [x0]
//' strb w2, [x0, 8]
//' strb wzr, [x1, 8]
//' ret
//' .L1:
//' mov w2, 1
//' str x3, [x1]
//' strb w2, [x1, 8]
//' strb wzr, [x0, 8]
//' ret
//'@if $arm64 & $libcxx & $clang[23..]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' cmp w8, w9
//' b.ne .L0
//' cbz w8, .L1
//' ldr x8, [x1]
//' ldr x9, [x0]
//' str x8, [x0]
//' str x9, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' mov w8, 1
//' strb w8, [x1, 8]
//' ldr x8, [x0]
//' ldrb w9, [x0, 8]
//' str x8, [x1]
//' cbz w9, .L1
//' strb wzr, [x0, 8]
//' ret
//' .L2:
//' mov w8, 1
//' strb w8, [x0, 8]
//' ldr x8, [x1]
//' ldrb w9, [x1, 8]
//' str x8, [x0]
//' cbz w9, .L1
//' strb wzr, [x1, 8]
//' .L1:
//' ret
//'@if $arm64 & $libcxx & $clang[19..23]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' cmp w8, w9
//' b.ne .L0
//' cbz w8, .L1
//' ldr x8, [x1]
//' ldr x9, [x0]
//' str x8, [x0]
//' str x9, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' mov w8, 1
//' ldr x9, [x0]
//' strb w8, [x1, 8]
//' ldrb w8, [x0, 8]
//' str x9, [x1]
//' cmp w8, 1
//' b.ne .L1
//' strb wzr, [x0, 8]
//' ret
//' .L2:
//' mov w8, 1
//' ldr x9, [x1]
//' strb w8, [x0, 8]
//' ldrb w8, [x1, 8]
//' str x9, [x0]
//' cmp w8, 1
//' b.ne .L1
//' strb wzr, [x1, 8]
//' .L1:
//' ret
//'@if $arm64 & $libcxx & $clang[18..19]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' cmp w8, 0
//' cset w10, ne
//' cmp w9, 0
//' cset w9, ne
//' eor w9, w10, w9
//' tbz w9, 0, .L0
//' cbz w8, .L1
//' mov w8, 1
//' strb w8, [x1, 8]
//' ldr x8, [x0]
//' ldrb w9, [x0, 8]
//' str x8, [x1]
//' cbz w9, .L2
//' strb wzr, [x0, 8]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr x8, [x1]
//' ldr x9, [x0]
//' str x8, [x0]
//' str x9, [x1]
//' ret
//' .L1:
//' mov w8, 1
//' strb w8, [x0, 8]
//' ldr x8, [x1]
//' ldrb w9, [x1, 8]
//' str x8, [x0]
//' cbz w9, .L2
//' strb wzr, [x1, 8]
//' .L2:
//' ret
//'@if $arm64 & $libcxx & $clang[..18]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' cmp w8, 0
//' cset w10, ne
//' cmp w9, 0
//' cset w9, ne
//' eor w9, w10, w9
//' tbz w9, 0, .L0
//' cbz w8, .L1
//' mov w8, 1
//' ldr x9, [x0]
//' strb w8, [x1, 8]
//' ldrb w8, [x0, 8]
//' str x9, [x1]
//' cbz w8, .L2
//' strb wzr, [x0, 8]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr x8, [x1]
//' ldr x9, [x0]
//' str x8, [x0]
//' str x9, [x1]
//' ret
//' .L1:
//' mov w8, 1
//' ldr x9, [x1]
//' strb w8, [x0, 8]
//' ldrb w8, [x1, 8]
//' str x9, [x0]
//' cbz w8, .L2
//' strb wzr, [x1, 8]
//' .L2:
//' ret
//'@if $arm64 & $libstdcxx & $clang[23..]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' cbz w8, .L0
//' ldr x8, [x0]
//' cbz w9, .L1
//' ldr x9, [x1]
//' str x9, [x0]
//' str x8, [x1]
//' ret
//' .L0:
//' cbz w9, .L2
//' ldr x8, [x1]
//' mov w9, 1
//' strb w9, [x0, 8]
//' str x8, [x0]
//' strb wzr, [x1, 8]
//' .L2:
//' ret
//' .L1:
//' str x8, [x1]
//' mov w8, 1
//' strb w8, [x1, 8]
//' strb wzr, [x0, 8]
//' ret
//'@if $arm64 & $libstdcxx & $clang[19..23]
//' ldrb w9, [x0, 8]
//' ldrb w8, [x1, 8]
//' cmp w9, 1
//' b.ne .L0
//' ldr x9, [x0]
//' cbz w8, .L1
//' ldr x8, [x1]
//' str x8, [x0]
//' str x9, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr x8, [x1]
//' mov w9, 1
//' strb w9, [x0, 8]
//' str x8, [x0]
//' strb wzr, [x1, 8]
//' .L2:
//' ret
//' .L1:
//' mov w8, 1
//' str x9, [x1]
//' strb w8, [x1, 8]
//' strb wzr, [x0, 8]
//' ret
//'@if $arm64 & $libstdcxx & $clang[18..19]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' cbz w8, .L0
//' ldr x8, [x0]
//' cbz w9, .L1
//' ldr x9, [x1]
//' str x9, [x0]
//' str x8, [x1]
//' ret
//' .L0:
//' cbz w9, .L2
//' ldr x8, [x1]
//' mov w9, 1
//' strb w9, [x0, 8]
//' str x8, [x0]
//' strb wzr, [x1, 8]
//' .L2:
//' ret
//' .L1:
//' str x8, [x1]
//' mov w8, 1
//' strb w8, [x1, 8]
//' strb wzr, [x0, 8]
//' ret
//'@if $arm64 & $libstdcxx & $clang[..18]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' cbz w8, .L0
//' ldr x8, [x0]
//' cbz w9, .L1
//' ldr x9, [x1]
//' str x9, [x0]
//' str x8, [x1]
//' ret
//' .L0:
//' cbz w9, .L2
//' ldr x8, [x1]
//' mov w9, 1
//' str x8, [x0]
//' strb w9, [x0, 8]
//' strb wzr, [x1, 8]
//' .L2:
//' ret
//' .L1:
//' mov w9, 1
//' str x8, [x1]
//' strb w9, [x1, 8]
//' strb wzr, [x0, 8]
//' ret
void std_optional_int64_swap(std::optional<int64_t>& a, std::optional<int64_t>& b) {
    a.swap(b);
}

//'[option_int64x2_swap]
//'@if $x64 & $linux & $clang
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
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc
//' sub rsp, 40
//' movdqu xmm1, [rsi]
//' movdqu xmm0, [rdi]
//' mov rax, qword ptr fs:40
//' mov [rsp + 24], rax
//' xor eax, eax
//' mov rax, [rdi + 16]
//' movups [rdi], xmm1
//' mov rdx, [rsi + 16]
//' mov [rdi + 16], rdx
//' mov [rsi + 16], rax
//' movups [rsi], xmm0
//' mov rax, [rsp + 24]
//' sub rax, qword ptr fs:40
//' jne .L0
//' add rsp, 40
//' ret
//' .L0:
//' call __stack_chk_fail@PLT
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' movups xmm3, [rcx]
//' movsd xmm2, [rcx + 16]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' movups [rdx], xmm3
//' movsd [rdx + 16], xmm2
//' ret
//'@if $arm64 & $gcc[15..]
//' sub sp, sp, 48
//' adrp x2, :got:__stack_chk_guard
//' ldr x2, [x2, :got_lo12:__stack_chk_guard]
//' stp x29, x30, [sp, 32]
//' add x29, sp, 32
//' ldr x3, [x2]
//' str x3, [sp, 24]
//' mov x3, 0
//' ldp x6, x7, [x1]
//' ldr x3, [x1, 16]
//' ldp x4, x5, [x0]
//' stp x6, x7, [x0]
//' ldr x2, [x0, 16]
//' str x3, [x0, 16]
//' adrp x0, :got:__stack_chk_guard
//' ldr x0, [x0, :got_lo12:__stack_chk_guard]
//' stp x4, x5, [sp]
//' stp x4, x5, [x1]
//' str x2, [x1, 16]
//' str x2, [sp, 16]
//' ldr x2, [sp, 24]
//' ldr x1, [x0]
//' subs x2, x2, x1
//' mov x1, 0
//' bne .L0
//' ldp x29, x30, [sp, 32]
//' add sp, sp, 48
//' ret
//' .L0:
//' bl __stack_chk_fail
//'@if $arm64 & $gcc[14..15]
//' sub sp, sp, 48
//' adrp x2, :got:__stack_chk_guard;ldr x2, [x2, :got_lo12:__stack_chk_guard]
//' stp x29, x30, [sp, 32]
//' add x29, sp, 32
//' ldr x3, [x2]
//' str x3, [sp, 24]
//' mov x3, 0
//' ldp x6, x7, [x1]
//' ldr x3, [x1, 16]
//' ldp x4, x5, [x0]
//' stp x6, x7, [x0]
//' ldr x2, [x0, 16]
//' str x3, [x0, 16]
//' adrp x0, :got:__stack_chk_guard;ldr x0, [x0, :got_lo12:__stack_chk_guard]
//' stp x4, x5, [sp]
//' stp x4, x5, [x1]
//' str x2, [x1, 16]
//' str x2, [sp, 16]
//' ldr x2, [sp, 24]
//' ldr x1, [x0]
//' subs x2, x2, x1
//' mov x1, 0
//' bne .L0
//' ldp x29, x30, [sp, 32]
//' add sp, sp, 48
//' ret
//' .L0:
//' bl __stack_chk_fail
//'@if $arm64 & $gcc[..14]
//' sub sp, sp, 48
//' adrp x2, :got:__stack_chk_guard
//' ldr x2, [x2, :got_lo12:__stack_chk_guard]
//' stp x29, x30, [sp, 32]
//' add x29, sp, 32
//' ldp x6, x7, [x1]
//' ldr x3, [x2]
//' str x3, [sp, 24]
//' mov x3, 0
//' ldp x4, x5, [x0]
//' stp x6, x7, [x0]
//' ldr x3, [x1, 16]
//' stp x4, x5, [sp]
//' ldr x2, [x0, 16]
//' str x3, [x0, 16]
//' adrp x0, :got:__stack_chk_guard
//' ldr x0, [x0, :got_lo12:__stack_chk_guard]
//' stp x4, x5, [x1]
//' str x2, [x1, 16]
//' str x2, [sp, 16]
//' ldr x2, [sp, 24]
//' ldr x1, [x0]
//' subs x2, x2, x1
//' mov x1, 0
//' bne .L0
//' ldp x29, x30, [sp, 32]
//' add sp, sp, 48
//' ret
//' .L0:
//' bl __stack_chk_fail
//'@if $arm64 & $clang
//' sub sp, sp, 32
//' ldr q0, [x1]
//' ldr x8, [x1, 16]
//' ldr q1, [x0]
//' ldr x9, [x0, 16]
//' str q0, [x0]
//' str x8, [x0, 16]
//' str q1, [x1]
//' str x9, [x1, 16]
//' add sp, sp, 32
//' ret
void option_int64x2_swap(opt::option<int64x2>& a, opt::option<int64x2>& b) {
    a.swap(b);
}

//'[std_optional_int64x2_swap]
//'@if $x64 & $linux & $libcxx & $clang[23..]
//' movzx eax, [rdi + 16]
//' cmp al, [rsi + 16]
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
//' movups xmm0, [rdi]
//' movups [rsi], xmm0
//' mov byte ptr [rsi + 16], 1
//' cmp byte ptr [rdi + 16], 1
//' jne .L1
//' mov byte ptr [rdi + 16], 0
//' ret
//' .L2:
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' mov byte ptr [rdi + 16], 1
//' cmp byte ptr [rsi + 16], 1
//' jne .L1
//' mov byte ptr [rsi + 16], 0
//' .L1:
//' ret
//'@if $x64 & $linux & $libcxx & $clang[19..23]
//' movzx eax, [rdi + 16]
//' cmp al, [rsi + 16]
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
//' movups xmm0, [rdi]
//' movups [rsi], xmm0
//' mov byte ptr [rsi + 16], 1
//' cmp byte ptr [rdi + 16], 1
//' jne .L1
//' mov byte ptr [rdi + 16], 0
//' ret
//' .L2:
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' mov byte ptr [rdi + 16], 1
//' cmp byte ptr [rsi + 16], 1
//' jne .L1
//' mov byte ptr [rsi + 16], 0
//' .L1:
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..19]
//' movzx eax, [rdi + 16]
//' test al, al
//' setne cl
//' cmp byte ptr [rsi + 16], 0
//' setne dl
//' xor dl, cl
//' je .L0
//' test al, al
//' je .L1
//' movups xmm0, [rdi]
//' movups [rsi], xmm0
//' mov byte ptr [rsi + 16], 1
//' cmp byte ptr [rdi + 16], 0
//' je .L2
//' mov byte ptr [rdi + 16], 0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movups xmm0, [rdi]
//' movaps [rsp - 24], xmm0
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' movaps xmm0, [rsp - 24]
//' movups [rsi], xmm0
//' ret
//' .L1:
//' movups xmm0, [rsi]
//' movups [rdi], xmm0
//' mov byte ptr [rdi + 16], 1
//' cmp byte ptr [rsi + 16], 0
//' je .L2
//' mov byte ptr [rsi + 16], 0
//' .L2:
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[23..]
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
//'@if $x64 & $linux & $libstdcxx & $clang[19..23]
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
//'@if $x64 & $linux & $libstdcxx & $clang[..19]
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
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[15..]
//' movzx eax, [rsi + 16]
//' cmp byte ptr [rdi + 16], 0
//' je .L0
//' test al, al
//' je .L1
//' movdqu xmm0, [rdi]
//' movdqu xmm1, [rsi]
//' movups [rdi], xmm1
//' movups [rsi], xmm0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movdqu xmm0, [rsi]
//' mov byte ptr [rdi + 16], 1
//' movups [rdi], xmm0
//' mov byte ptr [rsi + 16], 0
//' .L2:
//' ret
//' .L1:
//' movdqu xmm0, [rdi]
//' mov byte ptr [rsi + 16], 1
//' movups [rsi], xmm0
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $x64 & $linux & $gcc[14..15]
//' cmp byte ptr [rdi + 16], 0
//' movzx eax, [rsi + 16]
//' je .L0
//' test al, al
//' je .L1
//' movdqu xmm0, [rdi]
//' movdqu xmm1, [rsi]
//' movups [rdi], xmm1
//' movups [rsi], xmm0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movdqu xmm0, [rsi]
//' mov byte ptr [rdi + 16], 1
//' movups [rdi], xmm0
//' mov byte ptr [rsi + 16], 0
//' .L2:
//' ret
//' .L1:
//' movdqu xmm0, [rdi]
//' mov byte ptr [rsi + 16], 1
//' movups [rsi], xmm0
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' cmp byte ptr [rdi + 16], 0
//' movzx eax, [rsi + 16]
//' je .L0
//' test al, al
//' je .L1
//' movdqu xmm0, [rdi]
//' movdqu xmm1, [rsi]
//' movups [rdi], xmm1
//' movups [rsi], xmm0
//' ret
//' .L0:
//' test al, al
//' je .L2
//' movdqu xmm3, [rsi]
//' mov byte ptr [rdi + 16], 1
//' movups [rdi], xmm3
//' mov byte ptr [rsi + 16], 0
//' .L2:
//' ret
//' .L1:
//' movdqu xmm2, [rdi]
//' mov byte ptr [rsi + 16], 1
//' movups [rsi], xmm2
//' mov byte ptr [rdi + 16], 0
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
//' movups xmm0, [rdx]
//' movups xmm3, [rcx]
//' movsd xmm2, [rcx + 16]
//' movups [rcx], xmm0
//' movsd xmm1, [rdx + 16]
//' movsd [rcx + 16], xmm1
//' movups [rdx], xmm3
//' movsd [rdx + 16], xmm2
//' ret
//'@if $arm64 & $gcc[14..]
//' ldrb w3, [x0, 16]
//' ldrb w2, [x1, 16]
//' tbz x3, 0, .L0
//' tbz x2, 0, .L1
//' ldr q31, [x0]
//' ldp x2, x3, [x1]
//' stp x2, x3, [x0]
//' str q31, [x1]
//' .L2:
//' ret
//' .L0:
//' tbz x2, 0, .L2
//' ldp x4, x5, [x1]
//' mov w2, 1
//' stp x4, x5, [x0]
//' strb w2, [x0, 16]
//' strb wzr, [x1, 16]
//' ret
//' .L1:
//' ldp x4, x5, [x0]
//' mov w2, 1
//' stp x4, x5, [x1]
//' strb w2, [x1, 16]
//' strb wzr, [x0, 16]
//' ret
//'@if $arm64 & $gcc[13..14]
//' ldrb w3, [x0, 16]
//' ldrb w2, [x1, 16]
//' tbz x3, 0, .L0
//' tbz x2, 0, .L1
//' ldr q0, [x0]
//' ldp x2, x3, [x1]
//' stp x2, x3, [x0]
//' str q0, [x1]
//' .L2:
//' ret
//' .L0:
//' tbz x2, 0, .L2
//' ldp x4, x5, [x1]
//' mov w2, 1
//' stp x4, x5, [x0]
//' strb w2, [x0, 16]
//' strb wzr, [x1, 16]
//' ret
//' .L1:
//' ldp x4, x5, [x0]
//' mov w2, 1
//' stp x4, x5, [x1]
//' strb w2, [x1, 16]
//' strb wzr, [x0, 16]
//' ret
//'@if $arm64 & $gcc[..13]
//' ldrb w3, [x0, 16]
//' ldrb w2, [x1, 16]
//' cbz w3, .L0
//' cbz w2, .L1
//' ldr q0, [x0]
//' ldp x2, x3, [x1]
//' stp x2, x3, [x0]
//' str q0, [x1]
//' .L2:
//' ret
//' .L0:
//' cbz w2, .L2
//' ldp x4, x5, [x1]
//' mov w2, 1
//' stp x4, x5, [x0]
//' strb w2, [x0, 16]
//' strb wzr, [x1, 16]
//' ret
//' .L1:
//' ldp x4, x5, [x0]
//' mov w2, 1
//' stp x4, x5, [x1]
//' strb w2, [x1, 16]
//' strb wzr, [x0, 16]
//' ret
//'@if $arm64 & $libcxx & $clang[23..]
//' ldrb w8, [x0, 16]
//' ldrb w9, [x1, 16]
//' cmp w8, w9
//' b.ne .L0
//' cbz w8, .L1
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x0]
//' mov w8, 1
//' strb w8, [x1, 16]
//' str q0, [x1]
//' ldrb w8, [x0, 16]
//' cbz w8, .L1
//' strb wzr, [x0, 16]
//' ret
//' .L2:
//' ldr q0, [x1]
//' mov w8, 1
//' strb w8, [x0, 16]
//' str q0, [x0]
//' ldrb w8, [x1, 16]
//' cbz w8, .L1
//' strb wzr, [x1, 16]
//' .L1:
//' ret
//'@if $arm64 & $libcxx & $clang[19..23]
//' sub sp, sp, 16
//' ldrb w8, [x0, 16]
//' ldrb w9, [x1, 16]
//' cmp w8, w9
//' b.ne .L0
//' cbz w8, .L1
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' add sp, sp, 16
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x0]
//' mov w8, 1
//' strb w8, [x1, 16]
//' str q0, [x1]
//' ldrb w8, [x0, 16]
//' cmp w8, 1
//' b.ne .L1
//' strb wzr, [x0, 16]
//' add sp, sp, 16
//' ret
//' .L2:
//' ldr q0, [x1]
//' mov w8, 1
//' strb w8, [x0, 16]
//' str q0, [x0]
//' ldrb w8, [x1, 16]
//' cmp w8, 1
//' b.ne .L1
//' strb wzr, [x1, 16]
//' .L1:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libcxx & $clang[18..19]
//' sub sp, sp, 16
//' ldrb w8, [x0, 16]
//' ldrb w9, [x1, 16]
//' cmp w8, 0
//' cset w10, ne
//' cmp w9, 0
//' cset w9, ne
//' eor w9, w10, w9
//' tbz w9, 0, .L0
//' cbz w8, .L1
//' ldr q0, [x0]
//' mov w8, 1
//' strb w8, [x1, 16]
//' str q0, [x1]
//' ldrb w8, [x0, 16]
//' cbz w8, .L2
//' strb wzr, [x0, 16]
//' add sp, sp, 16
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' add sp, sp, 16
//' ret
//' .L1:
//' ldr q0, [x1]
//' mov w8, 1
//' strb w8, [x0, 16]
//' str q0, [x0]
//' ldrb w8, [x1, 16]
//' cbz w8, .L2
//' strb wzr, [x1, 16]
//' .L2:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libcxx & $clang[..18]
//' sub sp, sp, 16
//' ldrb w8, [x0, 16]
//' ldrb w9, [x1, 16]
//' cmp w8, 0
//' cset w10, ne
//' cmp w9, 0
//' cset w9, ne
//' eor w9, w10, w9
//' tbz w9, 0, .L0
//' cbz w8, .L1
//' ldr q0, [x0]
//' mov w8, 1
//' str q0, [x1]
//' strb w8, [x1, 16]
//' ldrb w8, [x0, 16]
//' cbz w8, .L2
//' strb wzr, [x0, 16]
//' add sp, sp, 16
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' add sp, sp, 16
//' ret
//' .L1:
//' ldr q0, [x1]
//' mov w8, 1
//' str q0, [x0]
//' strb w8, [x0, 16]
//' ldrb w8, [x1, 16]
//' cbz w8, .L2
//' strb wzr, [x1, 16]
//' .L2:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libstdcxx & $clang[23..]
//' ldrb w9, [x0, 16]
//' ldrb w8, [x1, 16]
//' cbz w9, .L0
//' cbz w8, .L1
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x1]
//' mov w8, 1
//' strb w8, [x0, 16]
//' str q0, [x0]
//' strb wzr, [x1, 16]
//' .L2:
//' ret
//' .L1:
//' ldr q0, [x0]
//' mov w8, 1
//' strb w8, [x1, 16]
//' str q0, [x1]
//' strb wzr, [x0, 16]
//' ret
//'@if $arm64 & $libstdcxx & $clang[19..23]
//' sub sp, sp, 16
//' ldrb w9, [x0, 16]
//' ldrb w8, [x1, 16]
//' cmp w9, 1
//' b.ne .L0
//' cbz w8, .L1
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' add sp, sp, 16
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x1]
//' mov w8, 1
//' strb w8, [x0, 16]
//' str q0, [x0]
//' strb wzr, [x1, 16]
//' .L2:
//' add sp, sp, 16
//' ret
//' .L1:
//' ldr q0, [x0]
//' mov w8, 1
//' strb w8, [x1, 16]
//' str q0, [x1]
//' strb wzr, [x0, 16]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libstdcxx & $clang[18..19]
//' sub sp, sp, 16
//' ldrb w9, [x0, 16]
//' ldrb w8, [x1, 16]
//' cbz w9, .L0
//' cbz w8, .L1
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' add sp, sp, 16
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x1]
//' mov w8, 1
//' strb w8, [x0, 16]
//' str q0, [x0]
//' strb wzr, [x1, 16]
//' .L2:
//' add sp, sp, 16
//' ret
//' .L1:
//' ldr q0, [x0]
//' mov w8, 1
//' strb w8, [x1, 16]
//' str q0, [x1]
//' strb wzr, [x0, 16]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libstdcxx & $clang[..18]
//' sub sp, sp, 16
//' ldrb w9, [x0, 16]
//' ldrb w8, [x1, 16]
//' cbz w9, .L0
//' cbz w8, .L1
//' ldr q0, [x1]
//' ldr q1, [x0]
//' str q0, [x0]
//' str q1, [x1]
//' add sp, sp, 16
//' ret
//' .L0:
//' cbz w8, .L2
//' ldr q0, [x1]
//' mov w8, 1
//' str q0, [x0]
//' strb w8, [x0, 16]
//' strb wzr, [x1, 16]
//' .L2:
//' add sp, sp, 16
//' ret
//' .L1:
//' ldr q0, [x0]
//' mov w8, 1
//' str q0, [x1]
//' strb w8, [x1, 16]
//' strb wzr, [x0, 16]
//' add sp, sp, 16
//' ret
void std_optional_int64x2_swap(std::optional<int64x2>& a, std::optional<int64x2>& b) {
    a.swap(b);
}

//'[option_find_index_of_value]
//'@if $x64 & $linux & $clang[16..]
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
//'@if $x64 & $linux & $clang[..16]
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
//'@if $x64 & $windows & ($clang | $clang-cl)
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
//'@if $x64 & $linux & $gcc[15..]
//' mov ecx, edx
//' test rsi, rsi
//' je .L0
//' xor edx, edx
//' jmp .L1
//' .L2:
//' add rdx, 1
//' cmp rdx, rsi
//' je .L0
//' .L1:
//' cmp [rdi + rdx*4], ecx
//' jne .L2
//' xor r9d, r9d
//' mov eax, 1
//' mov r8, rdx
//' mov r9b, al
//' mov rax, r8
//' mov rdx, r9
//' ret
//' .L0:
//' xor eax, eax
//' mov r9b, al
//' mov rax, r8
//' mov rdx, r9
//' ret
//'@if $x64 & $linux & $gcc[14..15]
//' test rsi, rsi
//' je .L0
//' xor ecx, ecx
//' jmp .L1
//' .L2:
//' add rcx, 1
//' cmp rcx, rsi
//' je .L0
//' .L1:
//' cmp [rdi + rcx*4], edx
//' jne .L2
//' xor r9d, r9d
//' mov eax, 1
//' mov r8, rcx
//' mov r9b, al
//' mov rax, r8
//' mov rdx, r9
//' ret
//' .L0:
//' xor eax, eax
//' mov r9b, al
//' mov rax, r8
//' mov rdx, r9
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' test rsi, rsi
//' je .L0
//' xor eax, eax
//' jmp .L1
//' .L2:
//' add rax, 1
//' cmp rax, rsi
//' je .L0
//' .L1:
//' cmp [rdi + rax*4], edx
//' jne .L2
//' mov rcx, rax
//' mov eax, 1
//' movzx edx, al
//' mov rax, rcx
//' ret
//' .L0:
//' xor eax, eax
//' movzx edx, al
//' mov rax, rcx
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[16..]
//' cbz x1, .L0
//' neg x6, x0, lsr 2
//' mov x7, 5
//' and x5, x6, 3
//' sub x4, x1, 1
//' add x3, x5, 4
//' cmp x3, x7
//' csel x3, x3, x7, cs
//' cmp x4, x3
//' bcc .L1
//' cbz x5, .L2
//' ldr w3, [x0]
//' cmp w2, w3
//' beq .L3
//' tbz x6, 1, .L4
//' ldr w3, [x0, 4]
//' cmp w2, w3
//' beq .L5
//' cmp x5, 3
//' bne .L6
//' ldr w3, [x0, 8]
//' cmp w2, w3
//' beq .L7
//' .L2:
//' mov x7, x5
//' .L8:
//' sub x8, x1, x5
//' dup v30.4s, w2
//' add x5, x0, x5, lsl 2
//' and x6, x8, -4
//' mov x3, 0
//' b .L9
//' .L10:
//' add x3, x3, 4
//' cmp x6, x3
//' beq .L11
//' .L9:
//' lsl x4, x3, 2
//' ldr q31, [x5, x4]
//' cmeq v31.4s, v31.4s, v30.4s
//' umaxp v31.4s, v31.4s, v31.4s
//' fmov x4, d31
//' cbz x4, .L10
//' add x3, x3, x7
//' .L12:
//' ldr w4, [x0, x3, lsl 2]
//' cmp w2, w4
//' beq .L13
//' add x4, x3, 1
//' cmp x1, x4
//' bls .L0
//' add x0, x0, x3, lsl 2
//' ldr w5, [x0, 4]
//' cmp w2, w5
//' beq .L14
//' add x4, x3, 2
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 8]
//' cmp w2, w5
//' beq .L14
//' add x4, x3, 3
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 12]
//' cmp w5, w2
//' beq .L14
//' add x4, x3, 4
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 16]
//' cmp w2, w5
//' beq .L14
//' add x4, x3, 5
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 20]
//' cmp w2, w5
//' beq .L14
//' add x4, x3, 6
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 24]
//' cmp w2, w5
//' beq .L14
//' add x3, x3, 7
//' cmp x1, x3
//' bls .L0
//' ldr w0, [x0, 28]
//' cmp w2, w0
//' beq .L13
//' .L0:
//' mov x0, x9
//' mov w1, 0
//' and x1, x1, 1
//' ret
//' .L14:
//' mov x3, x4
//' .L13:
//' mov x9, x3
//' mov w1, 1
//' mov x0, x9
//' and x1, x1, 1
//' ret
//' .L11:
//' add x3, x6, x7
//' cmp x6, x8
//' bne .L12
//' b .L0
//' .L1:
//' mov x3, 0
//' b .L12
//' .L7:
//' mov x3, 2
//' b .L13
//' .L3:
//' mov x3, 0
//' b .L13
//' .L4:
//' mov x7, 1
//' b .L8
//' .L5:
//' mov x3, 1
//' b .L13
//' .L6:
//' mov x7, 2
//' b .L8
//'@if $arm64 & $gcc[15..16]
//' cbz x1, .L0
//' neg x6, x0, lsr 2
//' mov x7, 5
//' and x5, x6, 3
//' sub x4, x1, 1
//' add x3, x5, 4
//' cmp x3, x7
//' csel x3, x3, x7, cs
//' cmp x4, x3
//' bcc .L1
//' cbz x5, .L2
//' ldr w3, [x0]
//' cmp w2, w3
//' beq .L3
//' tbz x6, 1, .L4
//' ldr w3, [x0, 4]
//' cmp w2, w3
//' beq .L5
//' cmp x5, 3
//' bne .L6
//' ldr w3, [x0, 8]
//' cmp w2, w3
//' beq .L7
//' fmov d27, x5
//' .L8:
//' adrp x3, .L9
//' sub x7, x1, x5
//' dup v30.2d, v27.d[0]
//' adrp x4, .L10
//' ldr q31, [x3, :lo12:.L9]
//' dup v29.4s, w2
//' ldr q28, [x4, :lo12:.L10]
//' add x5, x0, x5, lsl 2
//' add v30.2d, v30.2d, v31.2d
//' lsr x6, x7, 2
//' mov x3, 0
//' b .L11
//' .L12:
//' add v30.2d, v30.2d, v28.2d
//' cmp x6, x3
//' beq .L13
//' .L11:
//' lsl x4, x3, 4
//' add x3, x3, 1
//' ldr q31, [x5, x4]
//' cmeq v31.4s, v31.4s, v29.4s
//' umaxp v31.4s, v31.4s, v31.4s
//' fmov x4, d31
//' cbz x4, .L12
//' fmov x3, d30
//' .L14:
//' ldr w4, [x0, x3, lsl 2]
//' cmp w2, w4
//' beq .L15
//' add x4, x3, 1
//' cmp x4, x1
//' bcs .L0
//' add x0, x0, x3, lsl 2
//' ldr w5, [x0, 4]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 2
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 8]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 3
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 12]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 4
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 16]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 5
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 20]
//' cmp w2, w5
//' beq .L16
//' add x3, x3, 6
//' cmp x1, x3
//' bls .L0
//' ldr w0, [x0, 24]
//' cmp w2, w0
//' beq .L15
//' .L0:
//' mov w3, 0
//' .L17:
//' mov x0, x8
//' and x1, x3, 1
//' ret
//' .L16:
//' mov x3, x4
//' .L15:
//' mov x8, x3
//' mov w3, 1
//' mov x0, x8
//' and x1, x3, 1
//' ret
//' .L13:
//' mov w3, 0
//' tst x7, 3
//' beq .L17
//' fmov x3, d27
//' and x7, x7, -4
//' add x3, x7, x3
//' b .L14
//' .L2:
//' movi v27.2d, 0
//' b .L8
//' .L1:
//' mov x3, 0
//' b .L14
//' .L7:
//' mov x3, 2
//' b .L15
//' .L3:
//' mov x3, 0
//' b .L15
//' .L4:
//' mov x3, 1
//' fmov d27, x3
//' b .L8
//' .L5:
//' mov x3, 1
//' b .L15
//' .L6:
//' mov x3, 2
//' fmov d27, x3
//' b .L8
//'@if $arm64 & $gcc[..15]
//' cbz x1, .L0
//' mov x3, 0
//' b .L1
//' .L2:
//' add x3, x3, 1
//' cmp x3, x1
//' beq .L0
//' .L1:
//' ldr w4, [x0, x3, lsl 2]
//' cmp w4, w2
//' bne .L2
//' mov x5, x3
//' mov w1, 1
//' mov x0, x5
//' and x1, x1, 1
//' ret
//' .L0:
//' mov x0, x5
//' mov w1, 0
//' and x1, x1, 1
//' ret
//'@if $arm64 & $clang
//' cbz x1, .L0
//' mov x8, x1
//' mov x9, x0
//' mov x0, xzr
//' mov w1, 1
//' .L1:
//' ldr w10, [x9, x0, lsl 2]
//' cmp w10, w2
//' b.eq .L2
//' add x0, x0, 1
//' cmp x8, x0
//' b.ne .L1
//' mov x1, xzr
//' mov x0, x8
//' .L2:
//' ret
//' .L0:
//' mov x0, xzr
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
//'@if $x64 & $linux & $libcxx & $clang
//' test rsi, rsi
//' je .L0
//' xor eax, eax
//' .L1:
//' cmp [rdi + rax*4], edx
//' je .L2
//' inc rax
//' cmp rsi, rax
//' jne .L1
//' .L0:
//' xor edx, edx
//' xor eax, eax
//' ret
//' .L2:
//' mov dl, 1
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang
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
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $linux & $gcc[15..]
//' test rsi, rsi
//' je .L0
//' xor eax, eax
//' jmp .L1
//' .L2:
//' add rax, 1
//' cmp rax, rsi
//' je .L0
//' .L1:
//' cmp [rdi + rax*4], edx
//' jne .L2
//' mov [rsp - 24], rax
//' mov rax, [rsp - 24]
//' mov byte ptr [rsp - 16], 1
//' mov rdx, [rsp - 16]
//' ret
//' .L0:
//' pxor xmm0, xmm0
//' movaps [rsp - 24], xmm0
//' mov rax, [rsp - 24]
//' mov rdx, [rsp - 16]
//' ret
//'@if $x64 & $linux & $gcc[..15]
//' test rsi, rsi
//' je .L0
//' xor eax, eax
//' jmp .L1
//' .L2:
//' add rax, 1
//' cmp rax, rsi
//' je .L0
//' .L1:
//' cmp [rdi + rax*4], edx
//' jne .L2
//' mov [rsp - 24], rax
//' mov eax, 1
//' mov [rsp - 16], al
//' mov rax, [rsp - 24]
//' mov rdx, [rsp - 16]
//' ret
//' .L0:
//' xor eax, eax
//' mov [rsp - 16], al
//' mov rax, [rsp - 24]
//' mov rdx, [rsp - 16]
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[16..]
//' sub sp, sp, 16
//' cbz x1, .L0
//' neg x6, x0, lsr 2
//' mov x7, 5
//' and x5, x6, 3
//' sub x4, x1, 1
//' add x3, x5, 4
//' cmp x3, x7
//' csel x3, x3, x7, cs
//' cmp x4, x3
//' bcc .L1
//' cbz x5, .L2
//' ldr w3, [x0]
//' cmp w2, w3
//' beq .L3
//' tbz x6, 1, .L4
//' ldr w3, [x0, 4]
//' cmp w2, w3
//' beq .L5
//' cmp x5, 3
//' bne .L6
//' ldr w3, [x0, 8]
//' cmp w2, w3
//' beq .L7
//' .L2:
//' mov x6, x5
//' .L8:
//' sub x8, x1, x5
//' dup v30.4s, w2
//' add x5, x0, x5, lsl 2
//' and x7, x8, -4
//' mov x3, 0
//' b .L9
//' .L10:
//' add x3, x3, 4
//' cmp x3, x7
//' beq .L11
//' .L9:
//' lsl x4, x3, 2
//' ldr q31, [x5, x4]
//' cmeq v31.4s, v31.4s, v30.4s
//' umaxp v31.4s, v31.4s, v31.4s
//' fmov x4, d31
//' cbz x4, .L10
//' .L12:
//' add x3, x6, x3
//' .L13:
//' ldr w4, [x0, x3, lsl 2]
//' cmp w2, w4
//' beq .L14
//' add x4, x3, 1
//' cmp x1, x4
//' bls .L0
//' add x0, x0, x3, lsl 2
//' ldr w5, [x0, 4]
//' cmp w2, w5
//' beq .L15
//' add x4, x3, 2
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 8]
//' cmp w2, w5
//' beq .L15
//' add x4, x3, 3
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 12]
//' cmp w2, w5
//' beq .L15
//' add x4, x3, 4
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 16]
//' cmp w2, w5
//' beq .L15
//' add x4, x3, 5
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 20]
//' cmp w2, w5
//' beq .L15
//' add x4, x3, 6
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 24]
//' cmp w2, w5
//' beq .L15
//' add x3, x3, 7
//' cmp x1, x3
//' bls .L0
//' ldr w0, [x0, 28]
//' cmp w2, w0
//' beq .L14
//' .L0:
//' stp xzr, xzr, [sp]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//' .L15:
//' mov x3, x4
//' .L14:
//' mov w0, 1
//' str x3, [sp]
//' strb w0, [sp, 8]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//' .L11:
//' cmp x8, x3
//' bne .L12
//' b .L0
//' .L1:
//' mov x3, 0
//' b .L13
//' .L7:
//' mov x3, 2
//' b .L14
//' .L3:
//' mov x3, 0
//' b .L14
//' .L4:
//' mov x6, 1
//' b .L8
//' .L5:
//' mov x3, 1
//' b .L14
//' .L6:
//' mov x6, 2
//' b .L8
//'@if $arm64 & $gcc[15..16]
//' sub sp, sp, 16
//' cbz x1, .L0
//' neg x6, x0, lsr 2
//' mov x7, 5
//' and x5, x6, 3
//' sub x4, x1, 1
//' add x3, x5, 4
//' cmp x3, x7
//' csel x3, x3, x7, cs
//' cmp x4, x3
//' bcc .L1
//' cbz x5, .L2
//' ldr w3, [x0]
//' cmp w2, w3
//' beq .L3
//' tbz x6, 1, .L4
//' ldr w3, [x0, 4]
//' cmp w2, w3
//' beq .L5
//' cmp x5, 3
//' bne .L6
//' ldr w3, [x0, 8]
//' cmp w2, w3
//' beq .L7
//' fmov d27, x5
//' .L8:
//' adrp x3, .L9
//' sub x7, x1, x5
//' dup v30.2d, v27.d[0]
//' adrp x4, .L10
//' ldr q31, [x3, :lo12:.L9]
//' dup v29.4s, w2
//' ldr q28, [x4, :lo12:.L10]
//' add x5, x0, x5, lsl 2
//' add v30.2d, v30.2d, v31.2d
//' lsr x6, x7, 2
//' mov x3, 0
//' b .L11
//' .L12:
//' add v30.2d, v30.2d, v28.2d
//' cmp x6, x3
//' beq .L13
//' .L11:
//' lsl x4, x3, 4
//' add x3, x3, 1
//' ldr q31, [x5, x4]
//' cmeq v31.4s, v31.4s, v29.4s
//' umaxp v31.4s, v31.4s, v31.4s
//' fmov x4, d31
//' cbz x4, .L12
//' fmov x3, d30
//' .L14:
//' ldr w4, [x0, x3, lsl 2]
//' cmp w2, w4
//' beq .L15
//' add x4, x3, 1
//' cmp x4, x1
//' bcs .L0
//' add x0, x0, x3, lsl 2
//' ldr w5, [x0, 4]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 2
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 8]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 3
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 12]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 4
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 16]
//' cmp w2, w5
//' beq .L16
//' add x4, x3, 5
//' cmp x1, x4
//' bls .L0
//' ldr w5, [x0, 20]
//' cmp w2, w5
//' beq .L16
//' add x3, x3, 6
//' cmp x1, x3
//' bls .L0
//' ldr w0, [x0, 24]
//' cmp w2, w0
//' beq .L15
//' .L0:
//' stp xzr, xzr, [sp]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//' .L16:
//' mov x3, x4
//' .L15:
//' mov w0, 1
//' str x3, [sp]
//' strb w0, [sp, 8]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//' .L13:
//' tst x7, 3
//' beq .L0
//' fmov x3, d27
//' and x7, x7, -4
//' add x3, x7, x3
//' b .L14
//' .L2:
//' movi v27.2d, 0
//' b .L8
//' .L1:
//' mov x3, 0
//' b .L14
//' .L7:
//' mov x3, 2
//' b .L15
//' .L3:
//' mov x3, 0
//' b .L15
//' .L4:
//' mov x3, 1
//' fmov d27, x3
//' b .L8
//' .L5:
//' mov x3, 1
//' b .L15
//' .L6:
//' mov x3, 2
//' fmov d27, x3
//' b .L8
//'@if $arm64 & $gcc[..15]
//' sub sp, sp, 16
//' cbz x1, .L0
//' mov x3, 0
//' b .L1
//' .L2:
//' add x3, x3, 1
//' cmp x3, x1
//' beq .L0
//' .L1:
//' ldr w4, [x0, x3, lsl 2]
//' cmp w4, w2
//' bne .L2
//' mov w0, 1
//' str x3, [sp]
//' strb w0, [sp, 8]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//' .L0:
//' mov w0, 0
//' strb w0, [sp, 8]
//' ldp x0, x1, [sp]
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libcxx & $clang
//' cbz x1, .L0
//' mov x8, x1
//' mov x9, x0
//' mov x0, xzr
//' mov w1, 1
//' .L1:
//' ldr w10, [x9, x0, lsl 2]
//' cmp w10, w2
//' b.eq .L2
//' add x0, x0, 1
//' cmp x8, x0
//' b.ne .L1
//' mov x1, xzr
//' .L0:
//' mov x0, xzr
//' .L2:
//' ret
//'@if $arm64 & $libstdcxx & $clang
//' cbz x1, .L0
//' mov x8, x1
//' mov x9, x0
//' mov x0, xzr
//' mov w1, 1
//' .L1:
//' ldr w10, [x9, x0, lsl 2]
//' cmp w10, w2
//' b.eq .L2
//' add x0, x0, 1
//' cmp x8, x0
//' b.ne .L1
//' mov x1, xzr
//' mov x0, x8
//' .L2:
//' ret
//' .L0:
//' mov x0, xzr
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
//'@if $x64 & $linux & $clang[17..]
//' cmp byte ptr [rdi + 4], 0
//' je .L0
//' mov eax, [rdi]
//' ret
//' .L0:
//' push rax
//' call _ZN3opt4impl16throw_bad_optionEv
//'@if $x64 & $linux & $clang[..17]
//' push rax
//' cmp byte ptr [rdi + 4], 0
//' je .L0
//' mov eax, [rdi]
//' pop rcx
//' ret
//' .L0:
//' call _ZN3opt4impl16throw_bad_optionEv
//'@if $x64 & $windows & $clang
//' sub rsp, 40
//' cmp byte ptr [rcx + 4], 0
//' je .L0
//' mov eax, [rcx]
//' add rsp, 40
//' ret
//' .L0:
//' call _ZN3opt4impl16throw_bad_optionEv
//' int3
//'@if $x64 & $linux & $gcc
//' .L0:
//' push rax
//' call _ZN3opt4impl16throw_bad_optionEv
//'@if $x64 & $windows & $gcc
//' sub rsp, 40
//' cmp byte ptr [rcx + 4], 0
//' je .L0
//' mov eax, [rcx]
//' add rsp, 40
//' ret
//' .L0:
//' call _ZN3opt4impl16throw_bad_optionEv
//' nop
//'@if $x64 & $msvc | $clang-cl
//' sub rsp, 40
//' cmp byte ptr [rcx + 4], 0
//' je .L0
//' mov eax, [rcx]
//' add rsp, 40
//' ret
//' .L0:
//' call ?throw_bad_option@impl@opt@@YAXXZ
//' int3
//'@if $arm64 & $gcc[13..]
//' ldrb w1, [x0, 4]
//' tbz x1, 0, .L0
//' ldr w0, [x0]
//' ret
//' .L0:
//' stp x29, x30, [sp, -16]!
//' mov x29, sp
//' bl _ZN3opt4impl16throw_bad_optionEv
//'@if $arm64 & $gcc[..13]
//' ldrb w1, [x0, 4]
//' cbz w1, .L0
//' ldr w0, [x0]
//' ret
//' .L0:
//' stp x29, x30, [sp, -16]!
//' mov x29, sp
//' bl _ZN3opt4impl16throw_bad_optionEv
//'@if $arm64 & $clang[19..]
//' ldrb w8, [x0, 4]
//' tbz w8, 0, .L0
//' ldr w0, [x0]
//' ret
//' .L0:
//' stp x29, x30, [sp, -16]!
//' mov x29, sp
//' bl _ZN3opt4impl16throw_bad_optionEv
//'@if $arm64 & $clang[17..19]
//' ldrb w8, [x0, 4]
//' cbz w8, .L0
//' ldr w0, [x0]
//' ret
//' .L0:
//' stp x29, x30, [sp, -16]!
//' mov x29, sp
//' bl _ZN3opt4impl16throw_bad_optionEv
//'@if $arm64 & $clang[..17]
//' stp x29, x30, [sp, -16]!
//' mov x29, sp
//' ldrb w8, [x0, 4]
//' cbz w8, .L0
//' ldr w0, [x0]
//' ldp x29, x30, [sp], 16
//' ret
//' .L0:
//' bl _ZN3opt4impl16throw_bad_optionEv
int32_t opt_option_int32_value(opt::option<int32_t>& o) {
    return o.value();
}
