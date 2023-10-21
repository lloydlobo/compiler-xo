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
answer: int = 42   // variable: type = expr
exit(answer)
```

Declaring and initializing variables with type:

```C
answer: int = 42 // variable: type = expr
exit(answer)

low, high: int = 1, 5
mid: float     = (low + high) / 2
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
x := 0 + 1 + 1 + 2 + 3
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
assert(z == 3 and x == 2)
assert(a == 3) /* won't compile as `a` is not in current scope */
```

> DONE: eliminate the use of `;` semicolons and rely on let the parser infer
> end of statement by the each lines linebreak `\n`
> FUTURE: for constants use `const` type for compile-time immutability
> FUTURE: allow for deferred bindings such that `let x; x = 42;` is possible
> FUTURE: allow for walrus operator that `x := 42;` is possible

### Strong typing

```C
Int_Alias :: int  // alias of int (similar to Odin)
```

### Immutable variables :: FUTURE

```C
foo :: 0 // type evaluated at compile time
sum: int = foo + 2 // foo: int
assert(sum == 2)
foo += 1         // err won't compile an immutable variable

fsum: float = foo + 2.0 // foo: float
fsum += 1.0
assert(fsum == 3.0) // ok

parse_var :: (n_mut int!, n_imut int, n_wo_mut int) -> ()
{
    ^n_mut += 1 // ok
    print("% % %\n", ^n_mut, n_imut, n_wo_mut)
    ^n_wo_mut += 1 // err wont compile
}
n_mut, n_wo_mut := 1, 9
n_imut :: 7 -> int
parse_var(&n_mut!, n_imut, n_wo_mut) // prints "2 7 9"

val :: 2 // immutable ident val of type int and value of 2
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
PI :: 3.14159265 // type_of untyped float
MAX_VALUE :: 100 // type_of untyped int

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
z: int
x, y := 2, 1

if x == y and x % 2 not 0 {
    z += 1
}
else if x % 2 == y or y % 2 == x {
    z -= y
}
else {
    z |= 1
}
```

Without braces use `do` directive:

```C
x, y := 2, 1

if x * 2 == y do return 0
else if x % 2 == y do return 1
else do return -1
```

### Loops :: FUTURE

```C
arr [..] int = [0, 1, 2, 3]
n := arr.count /* 4 */

for i : 0..n-1 {
    print("array[%] = %\n", i, a[i])
}

for i : 0..n-1 do print("%\n", i)
```

### Functions :: FUTURE

`::` evaluated at compile time

```C
sum :: (x: int, y: int)
    return x + y
```

Omit the type from previous parameters when two or more consecutive
parameters sharing same type. Like in Odin,`x: int, y: int` can
be shortened to `x, y: int`:

```C
sum :: (x, y: int)
    return x + y
```

Parameters passed by values rather than by reference.
By default, these parameters are immutable values.

```C
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

#### Named arguments

```C
sum :: (a, b int) -> int
{
    return a + b
}
x, y := 1, 2

sum(a=x, b=y) // 3
```

#### Default values

```C
sum :: (a, b uint, c := 1) -> uint
{
    return a + b + c
}
x, y := 1, 2
z := 1

sum(x, y) // 4
sum(x, y, z) // 4
sum(x, y, 2) // 5

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
```jai
>>                                  { ... } // Anonymous code block
>>                        [capture] { ... } // Captured code block
>>      (i: int) -> float [capture] { ... } // Anonymous function
>> f :: (i: int) -> float [capture] { ... } // Named local function
>> f :: (i: int) -> float [capture] { ... } // Named global function
>> ```
>>
>> Taken from `jai` language [primer](https://github.com/BSVino/JaiPrimer/blob/master/JaiPrimer.md#code-refactoring)

#### Closures :: FUTURE

```

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

#### Compile time struct args


```C
TakeawayPizza :: struct (count_of_pizzas_slices: int) {
    name: string,
    weight: float,
    flavor: Flavor,

    slices: [count_of_pizza_slices] Cheese_Type,
}

make_pizza :: (weight: float) {
    t_pizza: TakeawayPizza(7)

    // Do some initialization of cheese slices here

    t_pizza.weight = weight

    eat_pizza(t_pizza)
}

main :: ()  {
    make_pizza(2.0)

    t1 := TakeawayPizza(3)
}
```

#### Types

```C
x : f64
x : float // float is an alias for f64
```

### Luminsms :: FUTURE

This is a curated list of Lumina functions for solving common problems.

#### Find the nth fibonacci number

```C
n1, n2 := 0, 1
next: int = 0

print_fibonacci :: (num_terms -> int) -> ()
{
    i :: int // always initializes to 0 untyped int
    i = 1
    print("%, \n", n1)
    print("%, \n", n2)

    for i = 2 : i < num_terms : i += 1 {
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
