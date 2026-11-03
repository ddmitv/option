#include <opt/option.hpp>
#include <cstdint>
#include <optional>

//'[option_int16_equal]
//'@if $x64 & $linux & $clang[22..]
//' mov eax, edi
//' shr edi, 16
//' mov ecx, esi
//' shr esi, 16
//' cmp dil, sil
//' sete dl
//' xor dil, 1
//' cmp ax, cx
//' sete al
//' or al, dil
//' and al, dl
//' ret
//'@if $x64 & $linux & $clang[19..22]
//' mov ecx, edi
//' shr ecx, 16
//' mov eax, esi
//' shr eax, 16
//' cmp cl, al
//' sete dl
//' xor cl, 1
//' cmp di, si
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $x64 & $linux & $clang[16..19]
//' test edi, 16711680
//' sete cl
//' setne al
//' test esi, 16711680
//' sete dl
//' xor dl, al
//' cmp di, si
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $x64 & $linux & $clang[..16]
//' cmp byte ptr [rsp + 10], 0
//' sete cl
//' setne al
//' cmp byte ptr [rsp + 18], 0
//' sete dl
//' xor dl, al
//' movzx eax, [rsp + 8]
//' cmp ax, [rsp + 16]
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[13..]
//' mov edx, edi
//' shr edx, 16
//' cmp di, si
//' mov ecx, edx
//' sete al
//' shr esi, 16
//' xor ecx, 1
//' or eax, ecx
//' cmp sil, dl
//' sete dl
//' and eax, edx
//' ret
//'@if $x64 & $linux & $gcc[..13]
//' mov edx, edi
//' shr edx, 16
//' cmp di, si
//' sete al
//' mov ecx, edx
//' shr esi, 16
//' xor ecx, 1
//' xor esi, edx
//' or eax, ecx
//' xor esi, 1
//' and eax, esi
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[13..]
//' sxth w3, w1
//' ubfx x2, x0, 16, 8
//' cmp w3, w0, sxth
//' ubfx x1, x1, 16, 8
//' cset w0, eq
//' eor w3, w2, 1
//' cmp w1, w2
//' orr w0, w0, w3
//' cset w1, eq
//' and w0, w1, w0
//' ret
//'@if $arm64 & $gcc[..13]
//' sxth w3, w1
//' ubfx x2, x0, 16, 8
//' cmp w3, w0, sxth
//' ubfx x1, x1, 16, 8
//' eor w1, w2, w1
//' cset w0, eq
//' eor w2, w2, 1
//' eor w1, w1, 1
//' orr w0, w0, w2
//' and w0, w0, w1
//' ret
//'@if $arm64 & $clang[20..]
//' ubfx x8, x0, 16, 8
//' lsr w9, w1, 16
//' and w10, w0, 0xffff
//' lsr x11, x0, 16
//' cmp w8, w9, uxtb
//' cset w8, eq
//' cmp w10, w1, uxth
//' eor w9, w11, 0x1
//' cset w10, eq
//' orr w9, w10, w9
//' and w0, w8, w9
//' ret
//'@if $arm64 & $clang[..20]
//' ubfx x8, x0, 16, 8
//' ubfx x9, x1, 16, 16
//' and w10, w0, 0xffff
//' lsr x11, x0, 16
//' cmp w8, w9, uxtb
//' cset w8, eq
//' cmp w10, w1, uxth
//' eor w9, w11, 0x1
//' cset w10, eq
//' orr w9, w10, w9
//' and w0, w8, w9
//' ret
bool option_int16_equal(opt::option<int16_t> a, opt::option<int16_t> b) {
    return a == b;
}

//'[std_optional_int16_equal]
//'@if $x64 & $linux & $clang[23..]
//' mov ecx, edi
//' shr ecx, 16
//' mov edx, esi
//' shr esi, 16
//' xor eax, eax
//' cmp cl, sil
//' sete al
//' xor ecx, ecx
//' cmp di, dx
//' sete cl
//' bt edi, 16
//' cmovae ecx, eax
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[19..23]
//' mov ecx, edi
//' shr ecx, 16
//' mov eax, esi
//' shr eax, 16
//' cmp cl, al
//' sete al
//' and cl, al
//' cmp cl, 1
//' jne .L0
//' cmp di, si
//' sete al
//' .L0:
//' ret
//'@if $x64 & $linux & $clang[..19]
//' test edi, 16711680
//' setne dl
//' mov ecx, esi
//' and ecx, 16711680
//' setne al
//' xor al, dl
//' xor al, 1
//' test edi, 16711680
//' je .L0
//' test ecx, ecx
//' je .L0
//' cmp di, si
//' sete al
//' .L0:
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[16..]
//' mov eax, edi
//' mov edx, esi
//' mov [rsp - 8], edi
//' xor ecx, ecx
//' shr eax, 16
//' shr edx, 16
//' mov [rsp - 16], esi
//' cmp al, dl
//' jne .L0
//' mov ecx, 1
//' test al, al
//' je .L0
//' movzx eax, [rsp - 16]
//' cmp [rsp - 8], ax
//' sete cl
//' .L0:
//' mov eax, ecx
//' ret
//'@if $x64 & $linux & $gcc[..16]
//' mov eax, edi
//' mov edx, esi
//' mov [rsp - 8], edi
//' xor ecx, ecx
//' shr eax, 16
//' shr edx, 16
//' mov [rsp - 16], esi
//' cmp al, dl
//' je .L0
//' .L1:
//' mov eax, ecx
//' ret
//' .L0:
//' mov ecx, 1
//' test al, al
//' je .L1
//' movzx eax, [rsp - 16]
//' cmp [rsp - 8], ax
//' sete cl
//' mov eax, ecx
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[16..]
//' sub sp, sp, 16
//' ubfx x2, x0, 16, 8
//' str w1, [sp]
//' ubfx x1, x1, 16, 8
//' str w0, [sp, 8]
//' mov w0, 0
//' cmp w1, w2
//' bne .L0
//' mov w0, 1
//' tbz x1, 0, .L0
//' ldrsh w0, [sp]
//' ldrsh w1, [sp, 8]
//' cmp w1, w0
//' cset w0, eq
//' .L0:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[13..16]
//' sub sp, sp, 16
//' ubfx x2, x0, 16, 8
//' str w1, [sp]
//' ubfx x1, x1, 16, 8
//' str w0, [sp, 8]
//' mov w0, 0
//' cmp w1, w2
//' beq .L0
//' .L1:
//' add sp, sp, 16
//' ret
//' .L0:
//' mov w0, 1
//' tbz x1, 0, .L1
//' ldrsh w0, [sp]
//' ldrsh w1, [sp, 8]
//' add sp, sp, 16
//' cmp w1, w0
//' cset w0, eq
//' ret
//'@if $arm64 & $gcc[..13]
//' sub sp, sp, 16
//' ubfx x2, x0, 16, 8
//' str w1, [sp]
//' ubfx x1, x1, 16, 8
//' str w0, [sp, 8]
//' mov w0, 0
//' cmp w1, w2
//' beq .L0
//' .L1:
//' add sp, sp, 16
//' ret
//' .L0:
//' mov w0, 1
//' cbz w1, .L1
//' ldrsh w0, [sp]
//' ldrsh w1, [sp, 8]
//' add sp, sp, 16
//' cmp w1, w0
//' cset w0, eq
//' ret
//'@if $arm64 & $clang[23..]
//' ubfx x8, x0, 16, 8
//' lsr w9, w1, 16
//' and w10, w0, 0xffff
//' lsr x11, x0, 16
//' cmp w8, w9, uxtb
//' cset w8, eq
//' cmp w10, w1, uxth
//' cset w9, eq
//' tst w8, w11
//' csel w0, w9, w8, ne
//' ret
//'@if $arm64 & $clang[20..23]
//' ubfx x9, x0, 16, 8
//' lsr w10, w1, 16
//' mov x8, x0
//' cmp w9, w10, uxtb
//' cset w0, eq
//' b.ne .L0
//' lsr x9, x8, 16
//' and w8, w8, 0xffff
//' cmp w8, w1, uxth
//' cset w8, eq
//' tst w9, 0x1
//' csel w0, w0, w8, eq
//' .L0:
//' ret
//'@if $arm64 & $clang[..20]
//' ubfx x9, x0, 16, 8
//' ubfx x10, x1, 16, 16
//' mov x8, x0
//' cmp w9, w10, uxtb
//' cset w0, eq
//' b.ne .L0
//' lsr x9, x8, 16
//' and w8, w8, 0xffff
//' cmp w8, w1, uxth
//' cset w8, eq
//' tst w9, 0x1
//' csel w0, w0, w8, eq
//' .L0:
//' ret
bool std_optional_int16_equal(std::optional<int16_t> a, std::optional<int16_t> b) {
    return a == b;
}

//'[option_int32_equal]
//'@if $x64 & $linux & $clang[22..]
//' mov eax, edi
//' shr rdi, 32
//' mov ecx, esi
//' shr rsi, 32
//' cmp dil, sil
//' sete dl
//' xor dil, 1
//' cmp eax, ecx
//' sete al
//' or al, dil
//' and al, dl
//' ret
//'@if $x64 & $linux & $clang[19..22]
//' mov rcx, rdi
//' shr rcx, 32
//' mov rax, rsi
//' shr rax, 32
//' cmp cl, al
//' sete dl
//' xor cl, 1
//' cmp edi, esi
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $x64 & $linux & $clang[16..19]
//' mov rax, 1095216660480
//' test rdi, rax
//' sete cl
//' setne dl
//' test rsi, rax
//' sete r8b
//' xor r8b, dl
//' cmp edi, esi
//' sete al
//' or al, cl
//' and al, r8b
//' ret
//'@if $x64 & $linux & $clang[..16]
//' cmp byte ptr [rsp + 12], 0
//' sete cl
//' setne al
//' cmp byte ptr [rsp + 20], 0
//' sete dl
//' xor dl, al
//' mov eax, [rsp + 8]
//' cmp eax, [rsp + 16]
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[13..]
//' mov rdx, rdi
//' shr rdx, 32
//' cmp edi, esi
//' mov ecx, edx
//' sete al
//' shr rsi, 32
//' xor ecx, 1
//' or eax, ecx
//' cmp sil, dl
//' sete dl
//' and eax, edx
//' ret
//'@if $x64 & $linux & $gcc[..13]
//' mov rdx, rdi
//' shr rdx, 32
//' cmp edi, esi
//' sete al
//' mov ecx, edx
//' shr rsi, 32
//' xor ecx, 1
//' xor esi, edx
//' or eax, ecx
//' xor esi, 1
//' and eax, esi
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[13..]
//' cmp w0, w1
//' ubfx x0, x0, 32, 8
//' ubfx x1, x1, 32, 8
//' cset w2, eq
//' eor w3, w0, 1
//' cmp w1, w0
//' cset w1, eq
//' orr w0, w2, w3
//' and w0, w1, w0
//' ret
//'@if $arm64 & $gcc[..13]
//' cmp w0, w1
//' ubfx x0, x0, 32, 8
//' ubfx x1, x1, 32, 8
//' cset w2, eq
//' eor w1, w0, w1
//' eor w0, w0, 1
//' eor w1, w1, 1
//' orr w0, w2, w0
//' and w0, w0, w1
//' ret
//'@if $arm64 & $clang
//' ubfx x8, x0, 32, 8
//' lsr x9, x1, 32
//' lsr x10, x0, 32
//' cmp w8, w9, uxtb
//' eor w9, w10, 0x1
//' cset w8, eq
//' cmp w0, w1
//' cset w10, eq
//' orr w9, w10, w9
//' and w0, w8, w9
//' ret
bool option_int32_equal(opt::option<int32_t> a, opt::option<int32_t> b) {
    return a == b;
}

//'[std_optional_int32_equal]
//'@if $x64 & $linux & $clang[23..]
//' mov rcx, rdi
//' shr rcx, 32
//' mov edx, esi
//' shr rsi, 32
//' xor eax, eax
//' cmp cl, sil
//' sete al
//' xor ecx, ecx
//' cmp edi, edx
//' sete cl
//' bt rdi, 32
//' cmovae ecx, eax
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[19..23]
//' mov rcx, rdi
//' shr rcx, 32
//' mov rax, rsi
//' shr rax, 32
//' cmp cl, al
//' sete al
//' and cl, al
//' cmp cl, 1
//' jne .L0
//' cmp edi, esi
//' sete al
//' .L0:
//' ret
//'@if $x64 & $linux & $clang[16..19]
//' mov rdx, 1095216660480
//' test rdi, rdx
//' setne r8b
//' mov rcx, rsi
//' and rcx, rdx
//' setne al
//' xor al, r8b
//' xor al, 1
//' test rdi, rdx
//' je .L0
//' test rcx, rcx
//' je .L0
//' cmp edi, esi
//' sete al
//' .L0:
//' ret
//'@if $x64 & $linux & $clang[..16]
//' mov r8, 1095216660480
//' test rdi, r8
//' setne dl
//' mov rcx, rsi
//' and rcx, r8
//' setne al
//' xor al, dl
//' xor al, 1
//' test rdi, r8
//' je .L0
//' test rcx, rcx
//' je .L0
//' cmp edi, esi
//' sete al
//' .L0:
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[16..]
//' mov rax, rdi
//' mov rdx, rsi
//' mov [rsp - 8], rdi
//' xor ecx, ecx
//' shr rax, 32
//' shr rdx, 32
//' mov [rsp - 16], rsi
//' cmp al, dl
//' jne .L0
//' mov ecx, 1
//' test al, al
//' je .L0
//' mov eax, [rsp - 16]
//' cmp [rsp - 8], eax
//' sete cl
//' .L0:
//' mov eax, ecx
//' ret
//'@if $x64 & $linux & $gcc[..16]
//' mov rax, rdi
//' mov rdx, rsi
//' mov [rsp - 8], rdi
//' xor ecx, ecx
//' shr rax, 32
//' shr rdx, 32
//' mov [rsp - 16], rsi
//' cmp al, dl
//' je .L0
//' .L1:
//' mov eax, ecx
//' ret
//' .L0:
//' mov ecx, 1
//' test al, al
//' je .L1
//' mov eax, [rsp - 16]
//' cmp [rsp - 8], eax
//' sete cl
//' mov eax, ecx
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[16..]
//' sub sp, sp, 16
//' ubfx x2, x0, 32, 8
//' stp x1, x0, [sp]
//' ubfx x1, x1, 32, 8
//' mov w0, 0
//' cmp w1, w2
//' bne .L0
//' mov w0, 1
//' tbz x1, 0, .L0
//' ldr w0, [sp]
//' ldr w1, [sp, 8]
//' cmp w1, w0
//' cset w0, eq
//' .L0:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[13..16]
//' sub sp, sp, 16
//' ubfx x2, x0, 32, 8
//' stp x1, x0, [sp]
//' ubfx x1, x1, 32, 8
//' mov w0, 0
//' cmp w1, w2
//' beq .L0
//' .L1:
//' add sp, sp, 16
//' ret
//' .L0:
//' mov w0, 1
//' tbz x1, 0, .L1
//' ldr w0, [sp]
//' ldr w1, [sp, 8]
//' add sp, sp, 16
//' cmp w1, w0
//' cset w0, eq
//' ret
//'@if $arm64 & $gcc[..13]
//' sub sp, sp, 16
//' ubfx x2, x0, 32, 8
//' stp x1, x0, [sp]
//' ubfx x1, x1, 32, 8
//' mov w0, 0
//' cmp w1, w2
//' beq .L0
//' .L1:
//' add sp, sp, 16
//' ret
//' .L0:
//' mov w0, 1
//' cbz w1, .L1
//' ldr w0, [sp]
//' ldr w1, [sp, 8]
//' add sp, sp, 16
//' cmp w1, w0
//' cset w0, eq
//' ret
//'@if $arm64 & $clang[23..]
//' ubfx x8, x0, 32, 8
//' lsr x9, x1, 32
//' lsr x10, x0, 32
//' cmp w8, w9, uxtb
//' cset w8, eq
//' cmp w0, w1
//' cset w9, eq
//' tst w8, w10
//' csel w0, w9, w8, ne
//' ret
//'@if $arm64 & $clang[..23]
//' ubfx x9, x0, 32, 8
//' lsr x10, x1, 32
//' mov x8, x0
//' cmp w9, w10, uxtb
//' cset w0, eq
//' b.ne .L0
//' lsr x9, x8, 32
//' cmp w8, w1
//' cset w8, eq
//' tst w9, 0x1
//' csel w0, w0, w8, eq
//' .L0:
//' ret
bool std_optional_int32_equal(std::optional<int32_t> a, std::optional<int32_t> b) {
    return a == b;
}

//'[opt_option_int64_equal]
//'@if $x64 & $linux & $clang[19..]
//' movzx ecx, [rdi + 8]
//' cmp cl, [rsi + 8]
//' sete dl
//' xor cl, 1
//' mov rax, [rdi]
//' cmp rax, [rsi]
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $x64 & $linux & $clang[..19]
//' cmp byte ptr [rdi + 8], 0
//' sete cl
//' setne al
//' cmp byte ptr [rsi + 8], 0
//' sete dl
//' xor dl, al
//' mov rax, [rdi]
//' cmp rax, [rsi]
//' sete al
//' or al, cl
//' and al, dl
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
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
//'@if $x64 & $linux & $gcc[13..]
//' movzx edx, [rdi + 8]
//' mov rax, [rsi]
//' cmp [rdi], rax
//' mov ecx, edx
//' sete al
//' xor ecx, 1
//' or eax, ecx
//' cmp [rsi + 8], dl
//' sete dl
//' and eax, edx
//' ret
//'@if $x64 & $linux & $gcc[..13]
//' movzx edx, [rdi + 8]
//' mov rax, [rsi]
//' cmp [rdi], rax
//' mov ecx, edx
//' sete al
//' xor dl, [rsi + 8]
//' xor ecx, 1
//' xor edx, 1
//' or eax, ecx
//' and eax, edx
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[13..]
//' ldr x5, [x0]
//' ldr x4, [x1]
//' ldrb w2, [x0, 8]
//' ldrb w3, [x1, 8]
//' cmp x5, x4
//' eor w1, w2, 1
//' cset w0, eq
//' cmp w3, w2
//' orr w0, w0, w1
//' cset w1, eq
//' and w0, w1, w0
//' ret
//'@if $arm64 & $gcc[..13]
//' ldr x4, [x0]
//' ldr x3, [x1]
//' ldrb w2, [x0, 8]
//' ldrb w1, [x1, 8]
//' cmp x4, x3
//' cset w0, eq
//' eor w1, w2, w1
//' eor w2, w2, 1
//' eor w1, w1, 1
//' orr w0, w0, w2
//' and w0, w0, w1
//' ret
//'@if $arm64 & $clang
//' ldrb w8, [x0, 8]
//' ldr x9, [x0]
//' ldr x10, [x1]
//' ldrb w11, [x1, 8]
//' eor w12, w8, 0x1
//' cmp x9, x10
//' csinc w9, w12, wzr, ne
//' cmp w8, w11
//' csel w0, wzr, w9, ne
//' ret
bool opt_option_int64_equal(const opt::option<int64_t>& a, const opt::option<int64_t>& b) {
    return a == b;
}

//'[std_optional_int64_equal]
//'@if $x64 & $linux & $clang[23..]
//' movzx ecx, [rdi + 8]
//' xor eax, eax
//' cmp cl, [rsi + 8]
//' sete al
//' mov rdx, [rdi]
//' xor edi, edi
//' cmp rdx, [rsi]
//' sete dil
//' test cl, cl
//' cmove edi, eax
//' and al, dil
//' ret
//'@if $x64 & $linux & $clang[19..23]
//' movzx ecx, [rdi + 8]
//' cmp cl, [rsi + 8]
//' sete al
//' and cl, al
//' cmp cl, 1
//' jne .L0
//' mov rax, [rdi]
//' cmp rax, [rsi]
//' sete al
//' .L0:
//' ret
//'@if $x64 & $linux & $clang[..19]
//' movzx edx, [rdi + 8]
//' test dl, dl
//' setne r8b
//' movzx ecx, [rsi + 8]
//' test cl, cl
//' setne al
//' xor al, r8b
//' xor al, 1
//' test dl, dl
//' je .L0
//' test cl, cl
//' je .L0
//' mov rax, [rdi]
//' cmp rax, [rsi]
//' sete al
//' .L0:
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[16..]
//' movzx edx, [rdi + 8]
//' xor eax, eax
//' cmp dl, [rsi + 8]
//' jne .L0
//' mov eax, 1
//' test dl, dl
//' je .L0
//' mov rax, [rsi]
//' cmp [rdi], rax
//' sete al
//' .L0:
//' ret
//'@if $x64 & $linux & $gcc[..16]
//' movzx edx, [rdi + 8]
//' xor eax, eax
//' cmp dl, [rsi + 8]
//' je .L0
//' .L1:
//' ret
//' .L0:
//' mov eax, 1
//' test dl, dl
//' je .L1
//' mov rax, [rsi]
//' cmp [rdi], rax
//' sete al
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[16..]
//' ldrb w3, [x0, 8]
//' ldrb w2, [x1, 8]
//' cmp w2, w3
//' bne .L0
//' mov w3, 1
//' tbz x2, 0, .L1
//' ldr x2, [x0]
//' ldr x0, [x1]
//' cmp x2, x0
//' cset w3, eq
//' .L1:
//' mov w0, w3
//' ret
//' .L0:
//' mov w3, 0
//' mov w0, w3
//' ret
//'@if $arm64 & $gcc[15..16]
//' ldrb w3, [x0, 8]
//' ldrb w2, [x1, 8]
//' cmp w2, w3
//' beq .L0
//' mov w3, 0
//' .L1:
//' mov w0, w3
//' ret
//' .L0:
//' mov w3, 1
//' tbz x2, 0, .L1
//' ldr x2, [x0]
//' ldr x0, [x1]
//' cmp x2, x0
//' cset w3, eq
//' mov w0, w3
//' ret
//'@if $arm64 & $gcc[13..15]
//' mov x2, x0
//' ldrb w3, [x1, 8]
//' ldrb w0, [x0, 8]
//' cmp w3, w0
//' beq .L0
//' mov w0, 0
//' .L1:
//' ret
//' .L0:
//' mov w0, 1
//' tbz x3, 0, .L1
//' ldr x0, [x1]
//' ldr x2, [x2]
//' cmp x2, x0
//' cset w0, eq
//' ret
//'@if $arm64 & $gcc[..13]
//' mov x2, x0
//' ldrb w3, [x1, 8]
//' ldrb w0, [x0, 8]
//' cmp w3, w0
//' beq .L0
//' mov w0, 0
//' .L1:
//' ret
//' .L0:
//' mov w0, 1
//' cbz w3, .L1
//' ldr x0, [x1]
//' ldr x2, [x2]
//' cmp x2, x0
//' cset w0, eq
//' ret
//'@if $arm64 & $clang[23..]
//' ldrb w8, [x0, 8]
//' ldrb w9, [x1, 8]
//' ldr x10, [x0]
//' ldr x11, [x1]
//' cmp w8, w9
//' cset w9, eq
//' cmp x10, x11
//' cset w10, eq
//' tst w9, w8
//' csel w0, w10, w9, ne
//' ret
//'@if $arm64 & $clang[..23]
//' ldrb w10, [x1, 8]
//' ldrb w9, [x0, 8]
//' mov x8, x0
//' cmp w9, w10
//' cset w0, eq
//' b.ne .L0
//' cbz w9, .L0
//' ldr x8, [x8]
//' ldr x9, [x1]
//' cmp x8, x9
//' cset w0, eq
//' .L0:
//' ret
bool std_optional_int64_equal(const std::optional<int64_t>& a, const std::optional<int64_t>& b) {
    return a == b;
}

//'[option_int32_equal_with_value]
//'@if $x64 & $linux & $clang[22..]
//' mov eax, edi
//' shr rdi, 32
//' and edi, 1
//' cmp esi, eax
//' sete al
//' and al, dil
//' ret
//'@if $x64 & $linux & $clang[21..22]
//' mov rcx, rdi
//' shr rcx, 32
//' and ecx, 1
//' cmp esi, edi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[20..21]
//' mov rcx, rdi
//' shr rcx, 32
//' cmp esi, edi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[19..20]
//' mov rcx, rdi
//' shr rcx, 32
//' and ecx, 1
//' cmp edi, esi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[16..19]
//' mov rax, rdi
//' shr rax, 32
//' test al, al
//' setne cl
//' cmp edi, esi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[..16]
//' cmp byte ptr [rsp + 12], 0
//' setne cl
//' cmp [rsp + 8], edi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $windows & $clang
//' mov eax, ecx
//' shr rcx, 32
//' and ecx, 1
//' cmp edx, eax
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $gcc
//' cmp esi, edi
//' sete al
//' shr rdi, 32
//' and eax, edi
//' ret
//'@if $x64 & $windows & $gcc
//' cmp edx, ecx
//' sete al
//' shr rcx, 32
//' and eax, ecx
//' ret
//'@if $x64 & $clang-cl
//' mov r8, rcx
//' shr r8, 32
//' and r8d, 1
//' cmp ecx, edx
//' sete al
//' and al, r8b
//' ret
//'@if $x64 & $msvc
//' cmp ecx, edx
//' sete al
//' shr rcx, 32
//' and al, cl
//' ret
//'@if $arm64 & $gcc
//' cmp w1, w0
//' ubfx x0, x0, 32, 8
//' cset w1, eq
//' and w0, w1, w0
//' ret
//'@if $arm64 & $clang[21..]
//' ubfx x8, x0, 32, 1
//' cmp w1, w0
//' csel w0, wzr, w8, ne
//' ret
//'@if $arm64 & $clang[20..21]
//' lsr x8, x0, 32
//' cmp w1, w0
//' cset w9, eq
//' and w0, w8, w9
//' ret
//'@if $arm64 & $clang[..20]
//' ubfx x8, x0, 32, 1
//' cmp w0, w1
//' csel w0, wzr, w8, ne
//' ret
bool option_int32_equal_with_value(opt::option<int32_t> a, int32_t b) {
    return a == b;
}

//'[std_optional_int32_equal_with_value]
//'@if $x64 & $linux & $clang[22..]
//' mov eax, edi
//' shr rdi, 32
//' cmp esi, eax
//' sete al
//' and al, dil
//' ret
//'@if $x64 & $linux & $clang[20..22]
//' mov rcx, rdi
//' shr rcx, 32
//' cmp esi, edi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[19..20]
//' mov rcx, rdi
//' shr rcx, 32
//' cmp edi, esi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $clang[..19]
//' mov rax, rdi
//' shr rax, 32
//' test al, al
//' setne cl
//' cmp edi, esi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $windows & $clang
//' mov eax, ecx
//' shr rcx, 32
//' cmp edx, eax
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $gcc
//' mov rax, rdi
//' mov [rsp - 8], rdi
//' shr rax, 32
//' test al, al
//' je .L0
//' cmp [rsp - 8], esi
//' sete al
//' .L0:
//' ret
//'@if $x64 & $windows & $gcc
//' mov rax, rcx
//' mov [rsp + 8], rcx
//' shr rax, 32
//' test al, al
//' je .L0
//' cmp [rsp + 8], edx
//' sete al
//' .L0:
//' ret
//'@if $x64 & $clang-cl
//' mov r8, rcx
//' shr r8, 32
//' cmp ecx, edx
//' sete al
//' and al, r8b
//' ret
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[13..]
//' sub sp, sp, 16
//' ubfx x2, x0, 32, 8
//' str x0, [sp, 8]
//' tbz x0, 32, .L0
//' ldr w0, [sp, 8]
//' cmp w0, w1
//' cset w2, eq
//' .L0:
//' mov w0, w2
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..13]
//' sub sp, sp, 16
//' str x0, [sp, 8]
//' ubfx x0, x0, 32, 8
//' cbz w0, .L0
//' ldr w0, [sp, 8]
//' cmp w0, w1
//' cset w0, eq
//' .L0:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $clang[20..]
//' lsr x8, x0, 32
//' cmp w1, w0
//' cset w9, eq
//' and w0, w8, w9
//' ret
//'@if $arm64 & $clang[..20]
//' lsr x8, x0, 32
//' cmp w0, w1
//' cset w9, eq
//' and w0, w8, w9
//' ret
bool std_optional_int32_equal_with_value(std::optional<int32_t> a, int32_t b) {
    return a == b;
}

//'[option_int64_equal_with_value]
//'@if $x64 & $linux & $clang[19..]
//' cmp [rdi], rsi
//' sete al
//' and al, [rdi + 8]
//' ret
//'@if $x64 & $linux & $clang[..19]
//' cmp byte ptr [rdi + 8], 0
//' setne cl
//' cmp [rdi], rsi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $gcc
//' cmp [rdi], rsi
//' sete al
//' and al, [rdi + 8]
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl | $gcc | $msvc)
//' cmp [rcx], rdx
//' sete al
//' and al, [rcx + 8]
//' ret
//'@if $arm64 & $gcc
//' ldr x2, [x0]
//' ldrb w0, [x0, 8]
//' cmp x2, x1
//' cset w1, eq
//' and w0, w1, w0
//' ret
//'@if $arm64 & $clang
//' ldr x8, [x0]
//' ldrb w9, [x0, 8]
//' cmp x8, x1
//' cset w8, eq
//' and w0, w8, w9
//' ret
bool option_int64_equal_with_value(const opt::option<int64_t>& a, int64_t b) {
    return a == b;
}

//'[std_optional_int64_equal_with_value]
//'@if $x64 & $linux & $clang[19..]
//' cmp [rdi], rsi
//' sete al
//' and al, [rdi + 8]
//' ret
//'@if $x64 & $linux & $clang[..19]
//' cmp byte ptr [rdi + 8], 0
//' setne cl
//' cmp [rdi], rsi
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $windows & ($clang | $clang-cl)
//' cmp [rcx], rdx
//' sete al
//' and al, [rcx + 8]
//' ret
//'@if $x64 & $linux & $gcc
//' movzx eax, [rdi + 8]
//' test al, al
//' je .L0
//' cmp [rdi], rsi
//' sete al
//' .L0:
//' ret
//'@if $x64 & $windows & $gcc
//' movzx eax, [rcx + 8]
//' test al, al
//' je .L0
//' cmp [rcx], rdx
//' sete al
//' .L0:
//' ret
//'@if $x64 & $msvc
//' cmp byte ptr [rcx + 8], 0
//' je .L0
//' cmp [rcx], rdx
//' sete al
//' ret
//' .L0:
//' xor al, al
//' ret
//'@if $arm64 & $gcc[15..]
//' ldrb w2, [x0, 8]
//' tbz x2, 0, .L0
//' ldr x0, [x0]
//' cmp x0, x1
//' cset w2, eq
//' .L0:
//' mov w0, w2
//' ret
//'@if $arm64 & $gcc[13..15]
//' mov x2, x0
//' ldrb w0, [x0, 8]
//' tbz x0, 0, .L0
//' ldr x0, [x2]
//' cmp x0, x1
//' cset w0, eq
//' .L0:
//' ret
//'@if $arm64 & $gcc[..13]
//' mov x2, x0
//' ldrb w0, [x0, 8]
//' cbz w0, .L0
//' ldr x0, [x2]
//' cmp x0, x1
//' cset w0, eq
//' .L0:
//' ret
//'@if $arm64 & $clang
//' ldr x8, [x0]
//' ldrb w9, [x0, 8]
//' cmp x8, x1
//' csel w0, wzr, w9, ne
//' ret
bool std_optional_int64_equal_with_value(const std::optional<int64_t>& a, int64_t b) {
    return a == b;
}

//'[option_int16_less]
//'@if $x64 & $linux & $clang[23..]
//' mov eax, esi
//' shr esi, 16
//' test edi, 65536
//' sete cl
//' cmp di, ax
//' setl al
//' or al, cl
//' and al, sil
//' ret
//'@if $x64 & $linux & $clang[19..23]
//' test esi, 65536
//' jne .L0
//' xor eax, eax
//' ret
//' .L0:
//' test edi, 65536
//' sete cl
//' cmp di, si
//' setl al
//' or al, cl
//' ret
//'@if $x64 & $linux & $clang[16..19]
//' test esi, 16711680
//' je .L0
//' test edi, 16711680
//' sete cl
//' cmp di, si
//' setl al
//' or al, cl
//' ret
//' .L0:
//' xor eax, eax
//' ret
//'@if $x64 & $linux & $clang[..16]
//' cmp byte ptr [rsp + 18], 0
//' je .L0
//' lea rax, [rsp + 8]
//' cmp byte ptr [rax + 2], 0
//' je .L1
//' lea rcx, [rsp + 16]
//' movzx eax, [rax]
//' cmp ax, [rcx]
//' setl al
//' ret
//' .L0:
//' xor eax, eax
//' ret
//' .L1:
//' mov al, 1
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[13..]
//' mov eax, esi
//' mov [rsp - 4], edi
//' shr eax, 16
//' mov [rsp - 8], esi
//' test al, al
//' je .L0
//' cmp byte ptr [rsp - 2], 0
//' je .L0
//' movzx eax, [rsp - 8]
//' cmp [rsp - 4], ax
//' setl al
//' .L0:
//' ret
//'@if $x64 & $linux & $gcc[..13]
//' mov eax, esi
//' shr eax, 16
//' mov edx, eax
//' test al, al
//' je .L0
//' test edi, 16711680
//' je .L0
//' cmp di, si
//' setl dl
//' .L0:
//' mov eax, edx
//' ret
//'@if $x64 & $windows & $gcc
//' mov eax, edx
//' mov [rsp + 8], ecx
//' shr eax, 16
//' mov [rsp + 16], edx
//' test al, al
//' je .L0
//' cmp byte ptr [rsp + 10], 0
//' je .L0
//' movzx eax, [rsp + 16]
//' cmp [rsp + 8], ax
//' setl al
//' .L0:
//' ret
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[15..]
//' sub sp, sp, 16
//' stp w1, w0, [sp, 8]
//' ubfx x0, x1, 16, 8
//' tbz x1, 16, .L0
//' ldrb w1, [sp, 14]
//' tbz x1, 0, .L0
//' ldrsh w0, [sp, 8]
//' ldrsh w1, [sp, 12]
//' cmp w1, w0
//' cset w0, lt
//' .L0:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[13..15]
//' sub sp, sp, 16
//' str w1, [sp]
//' str w0, [sp, 8]
//' ubfx x0, x1, 16, 8
//' tbz x1, 16, .L0
//' ldrb w1, [sp, 10]
//' tbz x1, 0, .L0
//' ldrsh w0, [sp]
//' ldrsh w1, [sp, 8]
//' cmp w1, w0
//' cset w0, lt
//' .L0:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..13]
//' mov w2, w0
//' ubfx x0, x1, 16, 8
//' cbz w0, .L0
//' tst x2, 16711680
//' beq .L0
//' sxth w1, w1
//' cmp w1, w2, sxth
//' cset w0, gt
//' .L0:
//' ret
//'@if $arm64 & $clang[23..]
//' sxth w8, w0
//' sxth w9, w1
//' tst x0, 0x10000
//' ccmp w8, w9, 8, ne
//' cset w8, lt
//' and w0, w8, w1, lsr 16
//' ret
//'@if $arm64 & $clang[..23]
//' sxth w8, w1
//' sxth w9, w0
//' tst x0, 0x10000
//' ccmp w9, w8, 8, ne
//' cset w8, lt
//' tst w1, 0x10000
//' csel w0, wzr, w8, eq
//' ret
bool option_int16_less(opt::option<int16_t> a, opt::option<int16_t> b) {
    return a < b;
}

//'[std_optional_int16_less]
//'@if $x64 & $linux & $libcxx & $clang[23..]
//' mov eax, esi
//' shr esi, 16
//' test edi, 65536
//' sete cl
//' cmp di, ax
//' setl al
//' or al, cl
//' and al, sil
//' ret
//'@if $x64 & $linux & $libcxx & $clang[21..23]
//' test esi, 65536
//' jne .L0
//' xor eax, eax
//' ret
//' .L0:
//' test edi, 65536
//' sete cl
//' cmp di, si
//' setl al
//' or al, cl
//' ret
//'@if $x64 & $linux & $libcxx & $clang[19..21]
//' mov eax, esi
//' and eax, edi
//' shr eax, 16
//' and eax, 1
//' cmp eax, 1
//' jne .L0
//' cmp di, si
//' setl al
//' ret
//' .L0:
//' shr esi, 16
//' and esi, 1
//' test edi, 65536
//' sete al
//' and al, sil
//' ret
//'@if $x64 & $linux & $libcxx & $clang[17..19]
//' test edi, 16711680
//' je .L0
//' mov eax, esi
//' and eax, 16711680
//' je .L0
//' cmp di, si
//' setl al
//' ret
//' .L0:
//' test esi, 16711680
//' setne cl
//' test edi, 16711680
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..17]
//' test esi, 16711680
//' je .L0
//' test edi, 16711680
//' sete cl
//' cmp di, si
//' setl al
//' or al, cl
//' ret
//' .L0:
//' xor eax, eax
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[21..]
//' mov eax, esi
//' and eax, edi
//' shr eax, 16
//' and eax, 1
//' cmp eax, 1
//' jne .L0
//' cmp di, si
//' setl al
//' ret
//' .L0:
//' shr esi, 16
//' test edi, 65536
//' sete al
//' and al, sil
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[19..21]
//' mov eax, esi
//' and eax, edi
//' shr eax, 16
//' and eax, 1
//' cmp eax, 1
//' jne .L0
//' cmp di, si
//' setl al
//' ret
//' .L0:
//' shr esi, 16
//' and esi, 1
//' test edi, 65536
//' sete al
//' and al, sil
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[16..19]
//' test edi, 16711680
//' je .L0
//' mov eax, esi
//' and eax, 16711680
//' je .L0
//' cmp di, si
//' setl al
//' ret
//' .L0:
//' test esi, 16711680
//' setne cl
//' test edi, 16711680
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[..16]
//' test edi, 16711680
//' je .L0
//' mov eax, esi
//' and eax, 16711680
//' je .L0
//' cmp di, si
//' setge al
//' add al, al
//' dec al
//' xor ecx, ecx
//' cmp di, si
//' movzx eax, al
//' cmove eax, ecx
//' shr al, 7
//' ret
//' .L0:
//' and edi, 16711680
//' setne cl
//' xor r8d, r8d
//' cmp edi, 1
//' mov eax, 0
//' sbb eax, eax
//' or al, 1
//' test esi, 16711680
//' movzx esi, al
//' mov eax, 1
//' cmovne eax, esi
//' setne dl
//' xor dl, cl
//' cmove eax, r8d
//' shr al, 7
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[15..]
//' mov eax, esi
//' mov [rsp - 8], edi
//' shr eax, 16
//' and edi, 16711680
//' mov [rsp - 16], esi
//' je .L0
//' test al, al
//' je .L0
//' movzx eax, [rsp - 8]
//' cmp [rsp - 16], ax
//' setg al
//' .L0:
//' ret
//'@if $x64 & $linux & $gcc[14..15]
//' mov edx, edi
//' mov eax, esi
//' mov [rsp - 8], edi
//' shr eax, 16
//' and edx, 16711680
//' mov [rsp - 16], esi
//' je .L0
//' test al, al
//' je .L0
//' movzx eax, [rsp - 8]
//' cmp [rsp - 16], ax
//' setg al
//' .L0:
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' mov edx, edi
//' mov eax, esi
//' mov [rsp - 8], edi
//' shr eax, 16
//' and edx, 16711680
//' mov [rsp - 16], esi
//' je .L0
//' test al, al
//' je .L0
//' movsx edx, [rsp - 16]
//' movsx ecx, [rsp - 8]
//' cmp ecx, edx
//' mov edx, 0
//' setl al
//' cmove eax, edx
//' .L0:
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[14..]
//' sub sp, sp, 16
//' str w1, [sp]
//' ubfx x1, x1, 16, 8
//' str w0, [sp, 8]
//' tbz x0, 16, .L0
//' tbz x1, 0, .L0
//' ldrsh w1, [sp, 8]
//' ldrsh w0, [sp]
//' cmp w1, w0
//' cset w1, lt
//' .L0:
//' mov w0, w1
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[13..14]
//' sub sp, sp, 16
//' str w1, [sp]
//' ubfx x1, x1, 16, 8
//' str w0, [sp, 8]
//' tbz x0, 16, .L0
//' tbz x1, 0, .L0
//' ldrsh w1, [sp, 8]
//' ldrsh w0, [sp]
//' cmp w1, w0
//' cset w1, lt
//' csel w1, w1, wzr, ne
//' .L0:
//' mov w0, w1
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..13]
//' sub sp, sp, 16
//' str w1, [sp]
//' ubfx x1, x1, 16, 8
//' str w0, [sp, 8]
//' tst x0, 16711680
//' beq .L0
//' cbz w1, .L0
//' ldrsh w1, [sp, 8]
//' ldrsh w0, [sp]
//' cmp w1, w0
//' cset w1, lt
//' csel w1, w1, wzr, ne
//' .L0:
//' mov w0, w1
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libcxx & $clang[23..]
//' sxth w8, w0
//' sxth w9, w1
//' tst x0, 0x10000
//' ccmp w8, w9, 8, ne
//' cset w8, lt
//' and w0, w8, w1, lsr 16
//' ret
//'@if $arm64 & $libcxx & $clang[21..23]
//' sxth w8, w1
//' sxth w9, w0
//' tst x0, 0x10000
//' ccmp w9, w8, 8, ne
//' cset w8, lt
//' tst w1, 0x10000
//' csel w0, wzr, w8, eq
//' ret
//'@if $arm64 & $libcxx & $clang[20..21]
//' tbz w0, 16, .L0
//' tbz w1, 16, .L0
//' sxth w8, w0
//' cmp w8, w1, sxth
//' cset w0, lt
//' ret
//' .L0:
//' ubfx x8, x1, 16, 1
//' tst x0, 0x10000
//' csel w0, wzr, w8, ne
//' ret
//'@if $arm64 & $libcxx & $clang[..20]
//' and x8, x1, 0x10000
//' tbz w0, 16, .L0
//' cbz x8, .L0
//' sxth w8, w0
//' and w9, w0, 0xffff
//' cmp w8, w1, sxth
//' mov w8, 255
//' csinc x8, x8, xzr, lt
//' cmp w9, w1, uxth
//' csel x8, xzr, x8, eq
//' lsr x0, x8, 7
//' ret
//' .L0:
//' ands x9, x0, 0x10000
//' ubfx x10, x1, 16, 1
//' cset w11, eq
//' orr x8, x8, x9
//' mov w9, 255
//' tst w11, w10
//' csinc x9, x9, xzr, ne
//' cmp x8, 0
//' csel x8, x9, xzr, ne
//' lsr x0, x8, 7
//' ret
//'@if $arm64 & $libstdcxx & $clang[21..]
//' tbz w0, 16, .L0
//' tbz w1, 16, .L0
//' sxth w8, w0
//' cmp w8, w1, sxth
//' cset w0, lt
//' ret
//' .L0:
//' tst x0, 0x10000
//' cset w8, eq
//' and w0, w8, w1, lsr 16
//' ret
//'@if $arm64 & $libstdcxx & $clang[20..21]
//' tbz w0, 16, .L0
//' tbz w1, 16, .L0
//' sxth w8, w0
//' cmp w8, w1, sxth
//' cset w0, lt
//' ret
//' .L0:
//' ubfx x8, x1, 16, 1
//' tst x0, 0x10000
//' csel w0, wzr, w8, ne
//' ret
//'@if $arm64 & $libstdcxx & $clang[..20]
//' and x8, x1, 0x10000
//' tbz w0, 16, .L0
//' cbz x8, .L0
//' sxth w8, w0
//' and w9, w0, 0xffff
//' cmp w8, w1, sxth
//' mov w8, 255
//' csinc x8, x8, xzr, lt
//' cmp w9, w1, uxth
//' csel x8, xzr, x8, eq
//' lsr x0, x8, 7
//' ret
//' .L0:
//' ands x9, x0, 0x10000
//' ubfx x10, x1, 16, 1
//' cset w11, eq
//' orr x8, x8, x9
//' mov w9, 255
//' tst w11, w10
//' csinc x9, x9, xzr, ne
//' cmp x8, 0
//' csel x8, x9, xzr, ne
//' lsr x0, x8, 7
//' ret
bool std_optional_int16_less(std::optional<int16_t> a, std::optional<int16_t> b) {
    return a < b;
}

//'[option_int32_less]
//'@if $x64 & $linux & $clang[23..]
//' mov eax, esi
//' shr rsi, 32
//' bt rdi, 32
//' setae cl
//' cmp edi, eax
//' setl al
//' or al, cl
//' and al, sil
//' ret
//'@if $x64 & $linux & $clang[19..23]
//' bt rsi, 32
//' jb .L0
//' xor eax, eax
//' ret
//' .L0:
//' mov rax, 4294967296
//' test rdi, rax
//' sete cl
//' cmp edi, esi
//' setl al
//' or al, cl
//' ret
//'@if $x64 & $linux & $clang[16..19]
//' mov rax, 1095216660480
//' test rsi, rax
//' je .L0
//' test rdi, rax
//' sete cl
//' cmp edi, esi
//' setl al
//' or al, cl
//' ret
//' .L0:
//' xor eax, eax
//' ret
//'@if $x64 & $linux & $clang[..16]
//' cmp byte ptr [rsp + 20], 0
//' je .L0
//' lea rax, [rsp + 8]
//' cmp byte ptr [rax + 4], 0
//' je .L1
//' lea rcx, [rsp + 16]
//' mov eax, [rax]
//' cmp eax, [rcx]
//' setl al
//' ret
//' .L0:
//' xor eax, eax
//' ret
//' .L1:
//' mov al, 1
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[13..]
//' mov rax, rsi
//' mov [rsp - 8], rdi
//' shr rax, 32
//' mov [rsp - 16], rsi
//' test al, al
//' je .L0
//' cmp byte ptr [rsp - 4], 0
//' je .L0
//' mov eax, [rsp - 16]
//' cmp [rsp - 8], eax
//' setl al
//' .L0:
//' ret
//'@if $x64 & $linux & $gcc[..13]
//' mov rax, rsi
//' shr rax, 32
//' mov edx, eax
//' test al, al
//' je .L0
//' mov rax, rdi
//' shr rax, 32
//' test al, al
//' je .L0
//' cmp edi, esi
//' setl dl
//' .L0:
//' mov eax, edx
//' ret
//'@if $x64 & $windows & $gcc
//' mov rax, rdx
//' mov [rsp + 8], rcx
//' shr rax, 32
//' mov [rsp + 16], rdx
//' test al, al
//' je .L0
//' cmp byte ptr [rsp + 12], 0
//' je .L0
//' mov eax, [rsp + 16]
//' cmp [rsp + 8], eax
//' setl al
//' .L0:
//' ret
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[13..]
//' sub sp, sp, 16
//' stp x1, x0, [sp]
//' ubfx x0, x1, 32, 8
//' tbz x1, 32, .L0
//' ldrb w1, [sp, 12]
//' tbz x1, 0, .L0
//' ldr w0, [sp]
//' ldr w1, [sp, 8]
//' cmp w1, w0
//' cset w0, lt
//' .L0:
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..13]
//' mov x2, x0
//' ubfx x0, x1, 32, 8
//' cbz w0, .L0
//' tst x2, 1095216660480
//' beq .L0
//' cmp w2, w1
//' cset w0, lt
//' .L0:
//' ret
//'@if $arm64 & $clang[23..]
//' tst x0, 0x100000000
//' lsr x8, x1, 32
//' ccmp w0, w1, 8, ne
//' cset w9, lt
//' and w0, w8, w9
//' ret
//'@if $arm64 & $clang[..23]
//' tst x0, 0x100000000
//' ccmp w0, w1, 8, ne
//' cset w8, lt
//' tst x1, 0x100000000
//' csel w0, wzr, w8, eq
//' ret
bool option_int32_less(opt::option<int32_t> a, opt::option<int32_t> b) {
    return a < b;
}

//'[std_optional_int32_less]
//'@if $x64 & $linux & $libcxx & $clang[23..]
//' mov eax, esi
//' shr rsi, 32
//' bt rdi, 32
//' setae cl
//' cmp edi, eax
//' setl al
//' or al, cl
//' and al, sil
//' ret
//'@if $x64 & $linux & $libcxx & $clang[21..23]
//' bt rsi, 32
//' jb .L0
//' xor eax, eax
//' ret
//' .L0:
//' mov rax, 4294967296
//' test rdi, rax
//' sete cl
//' cmp edi, esi
//' setl al
//' or al, cl
//' ret
//'@if $x64 & $linux & $libcxx & $clang[19..21]
//' mov rax, rsi
//' and rax, rdi
//' shr rax, 32
//' and eax, 1
//' cmp eax, 1
//' jne .L0
//' cmp edi, esi
//' setl al
//' ret
//' .L0:
//' mov rax, 4294967296
//' test rsi, rax
//' setne cl
//' test rdi, rax
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $libcxx & $clang[17..19]
//' mov rax, 1095216660480
//' test rdi, rax
//' je .L0
//' mov rcx, rsi
//' and rcx, rax
//' je .L0
//' cmp edi, esi
//' setl al
//' ret
//' .L0:
//' test rsi, rax
//' setne cl
//' test rdi, rax
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $libcxx & $clang[..17]
//' mov rax, 1095216660480
//' test rsi, rax
//' je .L0
//' test rdi, rax
//' sete cl
//' cmp edi, esi
//' setl al
//' or al, cl
//' ret
//' .L0:
//' xor eax, eax
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[19..]
//' mov rax, rsi
//' and rax, rdi
//' shr rax, 32
//' and eax, 1
//' cmp eax, 1
//' jne .L0
//' cmp edi, esi
//' setl al
//' ret
//' .L0:
//' mov rax, 4294967296
//' test rsi, rax
//' setne cl
//' test rdi, rax
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[16..19]
//' mov rax, 1095216660480
//' test rdi, rax
//' je .L0
//' mov rcx, rsi
//' and rcx, rax
//' je .L0
//' cmp edi, esi
//' setl al
//' ret
//' .L0:
//' test rsi, rax
//' setne cl
//' test rdi, rax
//' sete al
//' and al, cl
//' ret
//'@if $x64 & $linux & $libstdcxx & $clang[..16]
//' mov rax, 1095216660480
//' test rdi, rax
//' je .L0
//' mov rcx, rsi
//' and rcx, rax
//' je .L0
//' cmp edi, esi
//' setge al
//' add al, al
//' dec al
//' xor ecx, ecx
//' cmp edi, esi
//' movzx eax, al
//' cmove eax, ecx
//' shr al, 7
//' ret
//' .L0:
//' and rdi, rax
//' setne cl
//' xor r8d, r8d
//' cmp rdi, 1
//' mov edi, 0
//' sbb edi, edi
//' or dil, 1
//' test rsi, rax
//' movzx esi, dil
//' mov eax, 1
//' cmovne eax, esi
//' setne dl
//' xor dl, cl
//' cmove eax, r8d
//' shr al, 7
//' ret
//'@if $x64 & $windows & $clang
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
//'@if $x64 & $linux & $gcc[14..]
//' mov rdx, rdi
//' mov rax, rsi
//' mov [rsp - 8], rdi
//' shr rdx, 32
//' mov [rsp - 16], rsi
//' shr rax, 32
//' test dl, dl
//' je .L0
//' test al, al
//' je .L0
//' mov eax, [rsp - 16]
//' cmp [rsp - 8], eax
//' setl al
//' .L0:
//' ret
//'@if $x64 & $linux & $gcc[..14]
//' mov rdx, rdi
//' mov rax, rsi
//' mov [rsp - 8], rdi
//' shr rdx, 32
//' mov [rsp - 16], rsi
//' shr rax, 32
//' test dl, dl
//' je .L0
//' test al, al
//' je .L0
//' mov ecx, [rsp - 16]
//' xor eax, eax
//' cmp [rsp - 8], ecx
//' je .L0
//' setl al
//' .L0:
//' ret
//'@if $x64 & $windows & $gcc
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
//'@if $x64 & $clang-cl
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
//'@if $x64 & $msvc
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
//'@if $arm64 & $gcc[14..]
//' sub sp, sp, 16
//' stp x1, x0, [sp]
//' ubfx x1, x1, 32, 8
//' tbz x0, 32, .L0
//' tbz x1, 0, .L0
//' ldr w1, [sp, 8]
//' ldr w0, [sp]
//' cmp w1, w0
//' cset w1, lt
//' .L0:
//' mov w0, w1
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[13..14]
//' sub sp, sp, 16
//' stp x1, x0, [sp]
//' ubfx x1, x1, 32, 8
//' tbz x0, 32, .L0
//' tbz x1, 0, .L0
//' ldr w1, [sp, 8]
//' ldr w0, [sp]
//' cmp w1, w0
//' cset w1, lt
//' csel w1, w1, wzr, ne
//' .L0:
//' mov w0, w1
//' add sp, sp, 16
//' ret
//'@if $arm64 & $gcc[..13]
//' sub sp, sp, 16
//' stp x1, x0, [sp]
//' ubfx x1, x1, 32, 8
//' tst x0, 1095216660480
//' beq .L0
//' cbz w1, .L0
//' ldr w1, [sp, 8]
//' ldr w0, [sp]
//' cmp w1, w0
//' cset w1, lt
//' csel w1, w1, wzr, ne
//' .L0:
//' mov w0, w1
//' add sp, sp, 16
//' ret
//'@if $arm64 & $libcxx & $clang[23..]
//' tst x0, 0x100000000
//' lsr x8, x1, 32
//' ccmp w0, w1, 8, ne
//' cset w9, lt
//' and w0, w8, w9
//' ret
//'@if $arm64 & $libcxx & $clang[21..23]
//' tst x0, 0x100000000
//' ccmp w0, w1, 8, ne
//' cset w8, lt
//' tst x1, 0x100000000
//' csel w0, wzr, w8, eq
//' ret
//'@if $arm64 & $libcxx & $clang[20..21]
//' tbz x0, 32, .L0
//' tbz x1, 32, .L0
//' cmp w0, w1
//' cset w0, lt
//' ret
//' .L0:
//' ubfx x8, x1, 32, 1
//' tst x0, 0x100000000
//' csel w0, wzr, w8, ne
//' ret
//'@if $arm64 & $libcxx & $clang[..20]
//' and x8, x1, 0x100000000
//' tbz x0, 32, .L0
//' cbz x8, .L0
//' cmp w0, w1
//' mov w8, 255
//' csinc x8, x8, xzr, lt
//' csel x8, xzr, x8, eq
//' lsr x0, x8, 7
//' ret
//' .L0:
//' ands x9, x0, 0x100000000
//' ubfx x10, x1, 32, 1
//' cset w11, eq
//' orr x8, x8, x9
//' mov w9, 255
//' tst w11, w10
//' csinc x9, x9, xzr, ne
//' cmp x8, 0
//' csel x8, x9, xzr, ne
//' lsr x0, x8, 7
//' ret
//'@if $arm64 & $libstdcxx & $clang[21..]
//' tbz x0, 32, .L0
//' tbz x1, 32, .L0
//' cmp w0, w1
//' cset w0, lt
//' ret
//' .L0:
//' lsr x8, x1, 32
//' tst x0, 0x100000000
//' cset w9, eq
//' and w0, w8, w9
//' ret
//'@if $arm64 & $libstdcxx & $clang[20..21]
//' tbz x0, 32, .L0
//' tbz x1, 32, .L0
//' cmp w0, w1
//' cset w0, lt
//' ret
//' .L0:
//' ubfx x8, x1, 32, 1
//' tst x0, 0x100000000
//' csel w0, wzr, w8, ne
//' ret
//'@if $arm64 & $libstdcxx & $clang[..20]
//' and x8, x1, 0x100000000
//' tbz x0, 32, .L0
//' cbz x8, .L0
//' cmp w0, w1
//' mov w8, 255
//' csinc x8, x8, xzr, lt
//' csel x8, xzr, x8, eq
//' lsr x0, x8, 7
//' ret
//' .L0:
//' ands x9, x0, 0x100000000
//' ubfx x10, x1, 32, 1
//' cset w11, eq
//' orr x8, x8, x9
//' mov w9, 255
//' tst w11, w10
//' csinc x9, x9, xzr, ne
//' cmp x8, 0
//' csel x8, x9, xzr, ne
//' lsr x0, x8, 7
//' ret
bool std_optional_int32_less(std::optional<int32_t> a, std::optional<int32_t> b) {
    return a < b;
}
