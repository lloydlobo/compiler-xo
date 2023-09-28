;;; Defining your own _start but using libc
;;; works on Linux for non-PIE executables

default rel                ; Use RIP-relative for [symbol] addressing modes
extern printf
extern exit                ; unlike _exit, exit flushes stdio buffers

section .text
    global _start
_start:
    ;; RSP is already aligned by 16 on entry at _start, unlike in functions

    lea    rdi, [format_d]
    lea    rsi, [num1]
    xor    eax, eax
    call   printf

    lea    rdi, [format]        ; argument #1   or better  mov edi, format
    lea    rsi, [message]       ; argument #2
    xor    eax, eax             ; no FP args to the variadic function
    call   printf               ; for a PIE executable:  call printf wrt ..plt

    xor    edi, edi             ; arg #1 = 0
    call   exit                 ; exit(0)
    ; exit definitely does not return

section .rodata        ;; read-only data can go in .rodata instead of read-write .data

    message:    db "Hello, world!", 0
    format:     db "%s", 0xa, 0
    format_d:   db "%s", 0xa, 0
    num1:       db "3", 0
    num2:       db 1, 0


;;; Assemble normally, link with gcc -no-pie -nostartfiles hello.o.
;;; This omits the CRT startup files that would normally define a _start that does some stuff before calling main. Libc init functions are called from dynamic linker hooks so printf is usable.
;;; (Source](https://stackoverflow.com/a/55315078)
;;;
;;; $ nasm -f elf64 libc.asm -o libc.o
;;; $ gcc -no-pie -nostartfiles libc.o -o libc
;;; $ file libc
;;;   libc: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, BuildID[sha1]=ce3ee0cb1a2640121c1620318c4fddf53cf31600, not stripped


