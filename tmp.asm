; NASM(x86-64)
;
; ```shell
; nasm -f elf64 tmp.asm -o tmp.o
; ld tmp.o -o tmp
; ./tmp
; echo $?
; ```

section .data
    num1 db 10
    num2 db 20
    result db 0
    out_format db "Result: %d", 10

section .text
    global _start

_start:
    ; Load values into registers
    mov al, [num1]
    mov bl, [num2]

    add al, bl
    mov [result], al

    ;;mov rdi, out_format
    ;;mov rsi, rax
    ;;call printf

    ; Exit the program
    mov rax, 60             ; syscall number for exit
    ;;xor rdi, rdi          ; exit status (0)
    ;;mov rdi, 4            ; exit status (4)
    mov rdi, [result]
    syscall


;;section .data
;;    out_format db "Result: %d" , 10, 0
;;
;;section .bss
;;    result resq 4

; declare external fn for linker to resolve
;;section .text
;;    global printf
;;    extern printf
