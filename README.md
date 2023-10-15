# Lumina: Compiled with C

Lumina is a simple C inspired language.

"Lumina" implies illumination and clarity, which can be associated with
the purpose of programming languages to make complex tasks more
understandable and manageable.
The `.lum` extension is short, sweet and easy to remember.

> NOTE: The language is unstable and will have breaking changes.

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

Type is inferred if declared and initialized:

```C
answer := 42 // variable := expr
question := answer
exit(answer)
```

---
> WARNING: all information **from this point on** is unstable or planned

Declaring variables before initializing them:

```C
answer : int // variable : type
answer = 42   // <declared> variable = expr
exit(answer)
```

Declaring and initializing variables with type:

```C
answer = 42 -> int // variable = expr -> type
exit(answer)

low, high = 1, 5 -> int
mid = (low + high) / 2 -> float
```

To sum it up:

- type is inferred if declared and initialized
    `variable := expr`
- type when declared first and then initialized
    `variable : type`
    `variable = expr`

Later:

- immutable variable is declared and initialized with type
    `variable :: expr -> type`
- functions syntax is as follows
    `function :: (...args) -> type { expr }`

### Operators :: TODO

```C
x = 0 + 1 + 1 + 2 + 3
assert(x == 7)
```

### Scopes :: TODO

```C
x, y := 2, 1
{
    a := x + y
    assert(a == 3)
    z = a
}
assert(z == 3 && x == 2)
assert(a == 3) /* won't compile as `a` is not in current scope */
```

> DONE: eliminate the use of `;` semicolons and rely on let the parser infer
>     end of statement by the each lines linebreak `\n`
> FUTURE: for constants use `const` type for compile-time immutability
> FUTURE: allow for deferred bindings such that `let x; x = 42;` is possible
> FUTURE: allow for walrus operator that `x := 42;` is possible


### Immutable variables :: FUTURE

```C
foo :: 0 -> int
sum := foo + 2
assert(sum == 2)
assert(++sum == 3) // ok
foo++              // err won't compile an immutable variable

sum_imut :: 40 + foo + bar -> int
assert(sum_imut == 42)
sum_imut = 3    // err won't compile an immutable variable
sum_imut++      // err won't compile an immutable variable

parse_var :: (n_mut int!, n_imut int, n_wo_mut int) -> ()
{
    ^n_mut += 1 // ok
    print("% % %\n", ^n_mut, n_imut, n_wo_mut)
    ^n_wo_mut += 1 // err wont compile
}
n_mut, n_wo_mut := 1, 9
n_imut :: 7 -> int
parse_var(&n_mut!, n_imut, n_wo_mut) // prints "2 7 9"

val :: 2 -> int // immutable ident val of type int and value of 2
//
// similar to call to inline function evaluated at compile time:
//
// val :: (self self.type) -> self.type {
//     return self
// } (2) // called with self as 2
```
### Constants :: FUTURE

Define constants using uppercase naming convention

```C
PI :: 3.14159265 -> float
MAX_VALUE :: 100 -> int

MAX_VALUE :: int
MAX_VALUE = 100 // err this won't compile

MAX_VALUE := 100 // err needs type at compile time
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

### Enumerations :: FUTURE

```C
Position :: enum u32 {
    FIRST,
    SECOND,
    THIRD :: 80,
    FOURTH,
}
position := Position.FIRST
assert(position.FIRST == 0)

match position
    FIRST :: print("Gold")
    SECOND :: print("Silver")
    THIRD :: {
        assert(position.THIRD == 80)
        print("Bronze")
    }
    _ :: print("Participation")
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

/* taken from invaders.jai */

// ...

Flags :: enum u8 { REPEATING, SHUFFLE, }
Category :: enum u8 { MUSIC, PODCAST, }
{
    // ...
    stream :: struct {
        flags               Flags,
        category            Category,
        user_volume_scale   float,
    }
    stream := get_stream(data, false)
    if stream {
        stream.flags |= .REPEATING
        stream.category |= .MUSIC
        stream.user_volume_scale *= 0.2
    }
    // ...
}
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

`::` evaluated at compile time

```C
sum :: (x, y)
    return x + y

hypot :: (a float, b float) -> float
    c_sq := (a * a) + (b * b)
    c = c_sq ** 0.5 -> float
    return c

hypot :: (a float, b float) -> int
{
    c_sq := (a * a) + (b * b)
    c = round(c_sq ** 0.5) -> int
    return c
}

print :: (x float, y float) -> () {...}
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
    (x int, y int) [m] -> () {
        m.dx, m.dy += x, y
    } (a, b)

    a, b = m.dx, m.dy

    named_local_fn :: (x int, y int) [m] -> () {
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

#### Closures :: FUTURE

```C
get_bdate :: (val int) -> int { return val}

// FIXME: how will we do closures?
birthdate :: |get_bdate| -> int
birthdate :: get_bdate -> int
```
<!--
birthdate :: |get_bdate()| -> int
-->
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

### Luminsms :: FUTURE

This is a curated list of Lumina functions for solving common problems.

#### Find the nth fibonacci number

```C
n1, n2 := 0, 1
next = 0 -> int

print_fibonacci :: (num_terms -> int) -> ()
{
    i :: int // always initializes to 0
    i = 1
    print("%, \n", n1)
    print("%, \n", n2)

    for i = 2 : i < num_terms : i++ {
        next = n1 + n2
        print("%, \n", next)
        n1 = n2
        n2 = next
    }
}

num_terms = 10 -> int
print_fibonacci(num_terms)
```

#### Find the factorial of a number

```C
result :: int
result = 1 // Set default to 1 as 0! == 1 && 1! == 1

factorial :: (n -> int) -> int
{
    if n = 0 return 1
    else return n * factorial(n - 1)
}

num = 5 -> int // Calculate the factorial of 5
result = factorial(num)
```

### Immutable variables :: ARCHIVED

> ARCHIVE: The code below is archived

```C
date := 3
// declared and assigned like a function
birthdate :: date :: int // bdate is evaluated then consumed &
// won't compile
birthdate = 4            // fails to updata immutable variable

x :: 1 + 1 :: int
x :: (1) :: int // not an inline function
x = 2 // won't compile

x = 1 + 1 -> int const
x :: 1 -> const _
x = 2 // won't compile

x :: (1) :: const // not an inline function
y :: (x) :: const // still not an inline function

// inline-function always need types
x :: (i int) -> int {
    return i
}
assert(x(1) == 1)
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
