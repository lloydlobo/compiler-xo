# Lumina: Compiled with C

Lumina is a simple C inspired language.

"Lumina" implies illumination and clarity, which can be associated with
the purpose of programming languages to make complex tasks more
understandable and manageable.
The `.lum` extension is short, sweet and easy to remember.

> Note: The language is unstable and will have breaking changes.

## Build

```shell
cmake -S . -B build && cmake --build build
./build/lumina test.lum
echo $?

./out
echo $?
```

## Features

- **Clarity and Readability:** Lumina prioritizes human-readable code, making
  it an excellent choice for both beginners and experienced programmers.
- **Efficiency:** While emphasizing readability, Lumina doesn't compromise
  on performance. It's designed to be fast and efficient.
- **Modern Syntax:** Enjoy a clean and modern syntax that reduces
  boilerplate code and encourages best practices.
<!-- TODO:
- **Extensive Standard Library:** Lumina comes with a rich
  standard library, offering a wide range of modules and functions for
  common tasks.
-->
<!-- TODO:
- **Cross-Platform:** Lumina supports multiple platforms, including
  Windows, macOS, and Linux.
-->

## Language tour

### Variables

```lum
let x = 0;
let y = 1;
exit(x);
```

### Operators :: TODO

```C
let x = 0 + 1 + 1 + 2 + 3;
assert(x == 7)
x := 0 + 1 + 1 + 2 + 3 + 5 /* FUTURE */
```

> FUTURE: eliminate the use of `;` semicolons and rely let the parser infer end of statement by the each lines linebreak `\n`

> FUTURE: for constants use `const` type for compile-time immutability

> FUTURE: allow for deferred bindings such that `let x; x = 42;` is possible

> FUTURE: allow for walrus operator that `x := 42;` is possible

### Immutable by default :: FUTURE

```C
i int = 0
i++ /* will not compile */
i mut int = 0 /* shadowed previous ident `i` */
i++           /* will compile */
assert_eq(i, 1)

x := 0
x = 1 /* will not compile */
x mut := 0
x = 1 /* will compile */
```

### Silence var :: FUTURE

```C
x := 1
_ = x
assert_eq(x, 1)
```

> FUTURE: runtime assertion checks `assert_eq(x, 12);` and `assert(x != 11)`

### Arrays :: FUTURE

```C
arr_static [42] int     /* array of 42 integers */
arr_dynamic [..] int   /* dynamic array of integers */

arr [..] int = [0, 1, 2, 3]
n := arr.count /* 4 */
```

### Structures :: FUTURE

```C
/* member variables of a class are automatically initialized */
foo :: struct {
    bar int
    baz float
}

baz foo struct
print("% %\n", baz.bar, baz.baz); /* Always prints "0 0" */

/* with default initializations */
vec3 :: struct {
    x float = 0.4
    y float = 0.3
    z float = 0.1
}

v vec3 struct
print("% % %\n", v.x, v.y, v.z); /* Always prints "0.4 0.3 0.1" */

car := vec3 struct {
    x = 1
    y = 1
    z = 1
}
```

### Enumerations :: FUTURE

```C
Position :: enum int {
    FIRST,
    SECOND,
    THIRD = 80,
    FOURTH,
}
position := Position.FIRST
assert(position.FIRST == 0)

match position
    FIRST print("Gold")
    SECOND print("Silver")
    THIRD {
        assert(position.THIRD == 80)
        print("Bronze")
    }
    _ print("Participation")
```

### Scopes :: FUTURE

```C
z int
x, y := 2, 1
{
    a := x + y
    assert(a == 3)
    z = a
}
assert(z == 3 && x == 2)
assert(a == 3) /* won't compile as `a` is not in current scope */
```

### Control Flow :: FUTURE

```C
x, y := 2, 1

if x / 2 == y return 0
else if x % 2 == y return 1
else return -1
```

```C
x, y := 2, 1
z int

if x / 2 == y {
    z += x
    return z
}
else if x % 2 == y {
    z += y
    return z
}
else {
    return -1
}
```

### Loops :: FUTURE

```C
arr [..] int = [0, 1, 2, 3]
n := arr.count /* 4 */

for i : 0..n-1 {
    print("array[%] = %\n", i, a[i])
}
```

### Functions :: FUTURE

```C
sum :: (x, y)
    return x + y

hypot :: int <- (a float, b float)
    c_sq := (a * a) + (b * b)
    c int = round(c_sq ** 0.5)
    return c

hypot :: int <- (a float, b float)
{
    c_sq := (a * a) + (b * b)
    c int = round(c_sq ** 0.5)
    return c
}

print :: () <- (x float, y float) {...}
print :: (x float, y float) {...}
```

#### Anonymous / Inline named functions

```C
foo :: struct {
    dx int
    dy int
}

m foo /* member variables of a class are automatically initialized */
print("% %\n", m.dx, m.dy); // Always prints "0 0"

fn_1 :: (a int, b int)
{
    /*
     * anonymous function: `m` is the captured global object
     * call anon fn with specified params a and b
     */
    () <- (x int, y int) [m] {
        m.dx, m.dy += x, y
    } (a, b)

    a, b = m.dx, m.dy

    named_local_fn :: () <- (x int, y int) [m] {
        m.dx, m.dy += x, y
    }

    l, m := 2 * a, 2 * b
    named_local_fn(l, m)
}

named_global_fn :: (x int, y int) [m]
{
    m.dx, m.dy += x, y
}

fn_2 :: (a int, b int)
{
    c, d := 2 *a, 2* b
    named_global_fn(c, d)
}
```

> NOTE: function inside a global function needs explicit return types
> `(...args) [capture] {return} (arg1, arg2)` # will never compile
> `() <- (...args) [capture] {return} (arg1, arg2)` # will compile
> `println :: () <- (...args) [capture] {return} (arg1, arg2)` # will compile

>> The capture retains m because it is a global object that doesn’t need to be passed as a parameter.
>>
>> ```jai
>>                                  { ... } // Anonymous code block
>>                        [capture] { ... } // Captured code block
>>      (i: int) -> float [capture] { ... } // Anonymous function
>> f :: (i: int) -> float [capture] { ... } // Named local function
>> f :: (i: int) -> float [capture] { ... } // Named global function
>> ```
>>
>> Taken from `jai` language [primer](https://github.com/BSVino/JaiPrimer/blob/master/JaiPrimer.md#code-refactoring)

### Misc :: FUTURE

#### Variable shadowing

```C
hypot :: foo struct <- (a float, b float)
{
    c_sq := (a * a) + (b * b)
    c int = round(c_sq ** 0.5)
    tmp := c
    /* Variable name shadowing to avoid naming things */
    c foo = struct {
        hyp int = c
    }
    assert(c != tmp, "identifier `c` should be shadowed")
    return c
}
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

Lumina is a C compiled language, built in the spirit of [Hydrogen](https://github.com/orosmatthew/hydrogen-cpp), and inspired by [Jai](https://github.com/BSVino/JaiPrimer/blob/master/JaiPrimer.md), [Compis](https://github.com/rsms/compis), C, Go, Rust, Python and many more.
