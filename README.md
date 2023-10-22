# Lumina

Lumina is a spartan C inspired language.

"Lumina" implies illumination and clarity, which can be associated with
the purpose of programming languages to make complex tasks more
understandable and manageable.

> NOTE: The language is unstable and will have breaking changes.

## Features

- **Clarity and Readability:** Lumina prioritizes human-readable code, making
  it an excellent choice for both beginners and experienced programmers.
- **Efficiency:** While emphasizing readability, Lumina doesn't compromise
  on performance. It's designed to be fast and efficient.
- **Modern Syntax:** Enjoy a clean and modern syntax that reduces
  boilerplate code and encourages best practices.
- **Cross-Platform:** Lumina supports Linux for now.

## Packages

- luminafmt: Code Formatter for `.lum` source files.
- lumina-vsc: VS Code syntax highlighting support for `.lum` source files.

## Build

### Using build script

```shell
./build_lumina.sh release
```

### Using `cmake`

```shell
$ cmake -S . -B build && cmake --build build    # Compile lumina executable
$ cd build
$ ./build/lumina main.lum                       # Compile .lum source file
$ ./out                                         # Run your program
```

## Dev

### Memory error detection with `valgrind`

#### Using directly without building new binary

```shell
valgrind --track-origins=yes ./lumina test.lum
```

#### Using with build script and `Makefile`

```shell
./build_lumina.sh debug && make valwip && ./out & echo $?
```

### Concepts

#### Back-end

##### Assemble to binary file and Linking to executable

```shell
nasm -felf64 test.asm
ld test.o -o test
```

- Assemble `source.asm` into a binary file `output_file`.
  - If `output_file` is not passed, it is inferred as source.o.
  - `-f` and `-elf64`. (x64 architecture)
- Link a specific object file with no dependencies into an executable.
  - `test` is the executable.


```shell
./test      # Run generated executable
echo $?     # See exit code
```

## Many Thanks to

- [Hydrogen](https://github.com/orosmatthew/hydrogen-cpp) upon which, Lumina was initially built on.
- [Jai](https://github.com/BSVino/JaiPrimer/blob/master/JaiPrimer.md) & [Odin](https://odin-lang.org/) for being a guiding compass for sane modern language development.
- [Compis](https://github.com/rsms/compis), [laye](https://github.com/laye-lang/laye) for inspiration for builing compilers in C.
- C, Go, Rust, Python and many more...
