# xo lang: C Language Compiler

## Build

```shell
cmake -S . -B build && cmake --build build
./build/xo test.xo
echo $?

./out
echo $?
```

## Concepts

### asm

#### `syscall`

```asm test.asm
global _start
_start:
mov rax, 60 ;; linux syscall code for exit
mov rdi, 0 ;; code to return. 0 Success 1 Error ...
syscall
```
#### `linking`

```shell
nasm -felf64 test.asm
ld test.o -o test
```

```shell
./test      # Execute generated binary
echo $?     # See exit code
```
#### memory error detection with `valgrind`

```shell
valgrind --track-origins=yes ./build/xo test.xo
```

## Credits

[orosmatthew/hydrogen-cpp](https://github.com/orosmatthew/hydrogen-cpp)
