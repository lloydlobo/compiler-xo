# Lumina: Compiled with C

"Lumina" implies illumination and clarity, which can be associated with
the purpose of programming languages to make complex tasks more
understandable and manageable.
The `.lum` extension is short, sweet and easy to remember.

## Build

```shell
cmake -S . -B build && cmake --build build
./build/lumina test.lum
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
valgrind --track-origins=yes ./build/lumina test.lum
```

## Credits

Lumina is a C compiled language, built in the spirit of
[Hydrogen](https://github.com/orosmatthew/hydrogen-cpp) language originally
compiled with C++.
