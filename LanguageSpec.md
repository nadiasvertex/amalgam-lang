

# Introduction #

This document is the official specification for the amalgam language.


# Tokens #

The language defines only a few tokens. The intention is to allow the language to be easily extensible. Other than keywords, the primary tokens are identifiers, operators and literals.

Here there are listed some commonly used patterns in the tokens described below:

| **Pattern** | **Matches**     | **Description**               |
|:------------|:----------------|:------------------------------|
|  `digit`  | '0' .. '9'  | Numeric digits            |
|  `alpha`  | 'a' .. 'z', 'A' .. 'Z', '_'_| Alphabetical characters and the underscore  |
|  `alnum`  | '0' .. '9', 'a' .. 'z', 'A' .. 'Z', '_'_| Alphanumeric characters and the underscore  |
|    `.`    |  anything but a newline    | The _non-greedy_ wildcard character |
|    `\n`   |  newline | Matches the newline character |

The pattern descriptions below use PEG syntax. The following tables explain the operators used:

| **Operator** | **Matches**     |
|:-------------|:----------------|
|     `+`      |  One or more of the preceding patterns  |
|     `?`      |  One or none of the preceding patterns  |
|     `*`      |  Zero or more of the preceding patterns |
|     `/`      |  Ordered separator. Separates alternatives, the first recognized alternative is chosen. |
|  `list<R,S>` |  Specifies a list recognizing 'R', separated by 'S'. For example, a comma separated list of numbers might be: `list<digit+, ','>` |
|     `<-`     |  Pattern naming. For example, `ident <- alpha+ alnum*` might name an identifier |


## Operators ##

In amalgam, an operator is one or more of:

`~!@$%^&*+={}[]:";'<>?,./`

The compiler pre-defines a few basic operators on the built-in types.
Those operators can be overridden. For example:

```
5 + 5
```

will perform addition on the two integers. It is illegal to define:

```
def +(l,r sint8) = l + r
```

since that would result in infinite recursion. Instead, you must use the built-in method for addition:

```
def +(l,r sint8) = `add(l,r)
```

## Literals ##

Amalgam supports a flexible literal specication a'la C++11. The compiler has built-in support for recognizing certain kinds of literals. You can define your own by defining a literal recognizer. Note that these are executed at runtime, not compile time.

### Predefined Literals ###


| **Pattern**   | **Description**           |
|:--------------|:--------------------------|
| `digit+`    | A decimal integer       |
| `digit+ period digit+`  | A floating point number |
| `".*"`      | A string                |
| `'.*'`      | A string                |
| `""".*"""`  | A multiline string      |
| `'''.*'''`  | A multiline string      |

```
literal_int <- digit+
literal_float <- digit+ '.' digit+
literal_string1 <- '"' .* '"'
literal_string2 <- "'" .* "'"
literal_multiline_string1 <- '"""' (. / \n)* '"""'
literal_multiline_string2 <- "'''" (. / \n)* "'''"
literal_string <- literal_string1 / literal_string2 / literal_multiline_string1 / literal_multiline_string2
```

### User Defined Literals ###

A user defined literal is any of the above literals postfixed by a literal specifier. A literal specifier matches the same pattern as an identifer: an alphabetic character followed by any number of alphanumeric characters.

```
literal_spec <- alpha+ alnum*
literal <- ( literal_float / literal_int / literal_string ) literal_spec?
```

For example:

```
ilength := 45in
mlength := 100mm
```

To use these tokens you must have defined a literal translator function. If your translator is a constant expression, the compiler should be able to elide the call.

```
struct Inch:
  Value int64

def literal_in [i Inch] (v uint64) = Inch(v)
```

A few notes on literal translators:

  1. They must be defined in the module where the type is defined.
  1. Literal specifiers are global. For example, you can't specify "mm" in two modules and import them both into a third module. While this is certainly theoretically possible, the author feels it is a bad idea to have two different definitions of "millimeter" or "inch" or anything else which you'd like to use in this way. Choose one.

This will be covered in more detail in later sections.

## Initializer Literals ##

An initializer literal is a way of describing an initialization that can be swallowed by a function and used to create a complex type. This is very closely related to initializer lists in C++. There are essentially two forms: the list and the map. All known data structures can be initialized from lists, but a map which is converted into a list of lists is very convenient.

```
literal_initializer <- "{" literal_initializer_content "}"
literal_initializer_content <- list<expression, ","> / list<literal_map_element, ",">
literal_map_element <- expression ":" expression
```

A good example of this could be:

```
def do_something:
   a := Map({ "key1" : value1, "key2" : value2})
   b := Map({ somekey1 : value2, somekey2 : value2})
   c := List({ value1, value2, value3, value4 })
   d := Set({ value1, value2, value3, value4 })
```

Note in particular that:

```
  a := { "key1" : value1, "key2" : value2}
```

Will not yield a map into `a`. It will yield an array of tuples. This is because the language itself has no concept of maps. The initializer literals generate these simple language concepts, which constructor functions can accept as parameters in order to yield some higher order collection.

### Dynamics and Initializer Literals ###

If you create an initializer literal with heterogenous values the compiler will be forced to generate an array of boxed values. This allows you to use dynamics transparently, just like you would in Python. However, just like in other facets of the language, you will increase space and time usage. Also, the dynamism will propagate to the function. If the function does not allow dynamic types, the compiler will emit a diagnostic and the program will fail to translate.

## Identifiers ##

An identifier is the name of a function, variable, or user defined type. They all have the same form:

```
ident <- alpha+ alnum+
```

# Types #

The type system is very sophisticated. It provides for fluid movement between static types, generic types, and dynamic types. In addition, it provides the ability to enforce performance restrictions that turn into compile time errors.

The compiler provides whole program type inference. If a type annotation is left off of a variable, the compiler first tries to determine it's type by traversing the path of the variable as it arrived at the translation location. If that can be done, the variable is annotated and the resolved type becomes the static type of that variable. If it cannot be resolved, the compiler determines that this variable must be a dynamic type and treats it as such. This has memory and performance implications. Generally speaking, a dynamically typed variable will require more memory and be slower than it's typed counterpart.

## Type Annotations ##

The language provides various kinds of annotations which are attached to elements of the language. However, we will here consider _type_ annotations. These tell the compiler specific details about storage size and legal operations on the element in question.

A simple example involves variable typing:

```
var a   uint32,
    b,c uint64,
    d   float32 
```

Here you can see that we have declared a number of variables. They have been annotated with type information. Like most any other statically typed language, these variables are now bound by the restrictions defined by the specified types.

This also means that the compiler has more information about these variables, which lets us generate much faster code.

```
def myfunc(a,b float64) -> (bool, string) =
  (true, "it worked!")
```

Here we have declared some vanilla function. Again, we have annotated the parameters of the function with type information. It will be illegal for anyone to call this function with an incompatible type. Nothing new here, but we wanted to demonstrate that amalgam works basically like any other statically typed language when types are involved.

```
a := 5
```

This is another form of static typing. However, instead of telling the compiler what type 'a' is, you have told the compiler how to initialize 'a'. The compiler will thus annotate that variable with the type requiring the least storage that can hold 'a'. By default numeric literals are signed, so the type of 'a' would be `sint8`.

If the right-hand side value is a custom literal, the variable will be annotated with the type resulting from processing the custom literal.

## Dynamic Types ##

If you wish to indicate that a variable should be dynamically typed, simply leave off the type annotation:

```
var a
```

The compiler will record that 'a' is now a dynamic type. It can be bound to a value of any type, and then rebound to another value of any type. Dynamically typed variables practice extreme late binding. This has the usual productivity and performance implications.

There is also a shortcut: If you simply bind a value to a variable name that was not previously introduced with `:=` or a type annotated `var` then the new name will be created and bound to a dynamic value.

```
a = 10 # 'a' is bound to an object of dynamic type with a value of '10'
```

### Dynamic Type Propagation ###

This works quite well until you consider propagation of dynamic types. For example, what happens when you try to call a function that has type annotations with a dynamically typed input parameter?

One possibility could be that the compiler will issue a diagnostic and force you to make sure that the type is right at compile time. This is sometimes what you want, but it is a little annoying to require this behavior in every case.

Another option could be that the compiler defers issuing errors about this situation until it knows for sure that the types are incompatible. Thus, a runtime panic would ensue if the types did not match. This could defeat the purpose of static typing if dynamic types were allowed to "infect" static code in this manner.

The compromise then, is to have a default policy and allow that to be overidden on a selective or general basis. You can switch behavior by using annotations.

```
@module(dynamic=propagate)
```

This provides the default behaviour: dynamic values are allowed to propagate in and out of functions, and are checked at runtime. If a failure is detected, a runtime diagnostic is issued indicating what went wrong, and where.

```
@module(dynamic=check)
```

This provides static, compile-time checking. The compiler will issue an error diagnostic if it detects that you are passing a dynamic variable into a function which expects a static type, or if a static variable is receiving the output of a function which returns a dynamic type.

There are also function-level annotations:

```
def add(l,r sint8) = l + r

@dynamic(propagate)
def func(pred bool):

  if pred:
    a = 10
    b = 20
  else:
    a = "hello"
    b = "world"

  c = add(a,b) # okay, types checked here at runtime.
```

In this case, the body of the function 'func' will allow dynamic type propagation into 'add'.

```
def add(l,r sint8) = l + r

@dynamic(check)
def func(pred bool):

  if pred:
    a = 10
    b = 20
  else:
    a = "hello"
    b = "world"

  c = add(a,b) # fails
```

After specifying that the compiler needs to check dynamic types, this function will fail because we are not sure of the values of 'a' and 'b'.

### Dynamic Type Reflection ###

In order to make the checked example above work, we need to be able to determine what type the variables are at runtime, and then do the right thing. This forces us to be explicit about things that we might otherwise let the compiler handle for us.

Revisiting the example above:


```
def addint(l,r sint8) = l + r
def addstr(l,r string) = l + r


@dynamic(check)
def func(pred : bool):

  if pred:
    a = 10
    b = 20
  else:
    a = "hello"
    b = "world"
  

  switch:
    case typeof(a) == typeof(sint8) && typeof(b) == typeof(sint8):
      c = addint(sint8(a), sint8(b))
 
    case typeof(a) == typeof(string) && typeof(b) == typeof(string):
      c = addstr(string(a), string(b))

    default:
      # some custom handling of this situation

```

This simple (and contrived) function shows how we could specifically check the types of 'a' and 'b', and then call the correct function for the types at runtime.

Note that this is basically exactly what the compiler does when you leave the dynamic type propagation setting at default.

### Disabling Dynamic Types ###

There is a third option. If you are sure that you want the utmost performance from your code, you can set a module or function annotation that will tell the compiler not to allow definitions of dynamic types:

```
def add(l,r sint8) = l + r

@dynamic(disable)
def func(pred bool):

  if pred:
    a = 10 # fails, compiler emits diagnostic here
    b = 20
  else:
    a = "hello"
    b = "world"

  c = add(a,b)
```

The above code would fail to compile. The compiler would tell you that you used a dynamic construct, but that the enclosing scope does not allow their use.

## Generic Types ##

Generic types provide a mechanism which is complementary to dynamic types. Whereas a dynamic type allows you to represent any value at runtime, a generic type allows you to represent any value at _compile_ time. Once the generic type has been inferred and assigned for some instance of the code, it is not allowed to vary.

Note that generic types are really only useful for function arguments and struct members. The initializer syntax effectively provides generic-like functionality for function variables.

```
def add(l,r) = l + r

def func(pred bool):
  switch pred:
     case true:
       c = add(10,20)
     case false:
       c = add(100.2, 200.3) 
```

In this case 'add' is a completely statically typed function. We simply don't know what the types are until it is used. For this example, the compiler would generate two versions of the 'add' function. One for `sint8` parameters, and one for `float32` parameters.

If we had code like this:

```
def add(l,r) = l + r

def func(pred bool):

  switch pred:
     case true:
       a, b = (10,20)
     case false:
       a, b = (100.2, 200.3) 

  c = add(a,b)
```

what happens? The compiler's job becomes more difficult. The compiler assumes that, because we used a generic function we want it to try to generate the fastest code possible.

During evaluation of the code in the function up to this point, it may know the range of types for 'a' and 'b'. If it has that information it will generate specializations of 'add' with the combination of all variations. At the call site it will check the type of the parameters and then call the correct version of the function.

A final specialization is _always_ generated for a generic function (unless @dynamic(disable) is set for the module): one where each parameter has a dynamic type. If the types at runtime do not match any of the specialized signatures, the code will fall back to the dynamic specialization.

## Choosing Between Dynamic and Static Types ##

The choice is generally easy to make:

Generics, on the one hand, provide a method to make fast, compact, reusable functions and structs. However, you will see a corresponding increase in code size.

Dynamics, on the other, provide a way to make flexible functions and structs. However, you will see a corresponding increase in execution time and in memory usage.

We recommend that you start out using dynamic types inside functions and define your function parameters generically. This will cause your functions to be specialized only once.

As you grow to understand your problem domain and become more certain that your code solves the problem the way you want, you may begin to annotate some or all of the variables with type information. This is the general philosophy behind multi-resolution programming: you are not required to specify more information about the problem than you have at the time.

## Primitive Types ##

The language defines a few primitive types. These are generally primitive patterns:

| Pattern | Example       | Description         |
|:--------|:--------------|:--------------------|
| bool    | bool          | A true/false value. |
| uint\d+ | uint8, uint32 | An unsigned integer with the given number of bits. Although the language specifies that you may use request and arbitrary number of bits, the implementation may support only certain bit sizes. |
| sint\d+ | sint8, sint32 | A signed integer with the given number of bits. |
| float\d+ | float32, float64 | A real number with the given number of bits of precision. |
| ptrint   | ptrint | An unsigned integer with enough space to hold an address to any byte in the machine. |

## Composite Types ##

| Pattern | Example       | Description         |
|:--------|:--------------|:--------------------|
| string   | string | A series of contiguous characters treated as a single quantity. The size of each character is implementation defined. |
| `"(" list<expression, ","> ")"` | `(5, 10+3, a-x)` | A series of (possibly heterogenous) values allocated contiguously in memory. Generally known as tuples. |
| `"[]" type_annotation` | `[]uint8`, `[] sint16` | Defines an array of some type. An array always defines a series of homogeneous types allocated contiguously in memory. Arrays have an associated size with them, which is kept at run time. Array accesses are always bounds checked. |

# Structs #

A struct is a basic field aggregation mechanism. It supports generics and dynamics.

# Interfaces #

An interface binds together the concept of operations. The operations performed are associated with some state. The state is identified by a struct, and the operations are performed by functions which take a selector.

The interface can be used in the same way that inheritance often is: as a way to pass around objects that share some common functionality. This easily encapsulates the idea of aspect oriented programming in a way that is more flexible and extendable.

```
interface <- "interface" ident ":" operation_list
operation_list <- list<operation, "\n">
operation <- ident ("(" parm_list ")")? return_type?
```

For example:

```
interface Reader:
    read_byte -> uint8
    read(num_bytes) -> []uint8

interface Writer:
    write_byte(byte uint8) -> bool
    write(num_bytes, offset; data []uint8) -> bool

interface ReaderWriter:
    Reader
    Writer
```


# Functions #

Amalgam doesn't have the concept of member functions. Instead, it provides a very flexible way of loosely coupling functions to types. The basic mechanism and theory of operation is taken from Go.

We'll start with basic functions as you know them from any procedural language.

## Basic Functions ##

```
function <- "def" ident ("(" parm_list ")")? return_type? function_epilogue
return_type <- type_annotation / "(" list<type_annotation, ","> ")"
```

Nothing special here. A simple example:

```
def test:
    a = 5 + 10
```

Parameter lists are straightforward, but have some important tie ins to generics.

### Parameter Lists ###

```
parm_list <- list<generic_parm_element, ";"> / list<typed_parm_element, ",">
generic_parm_element <- ident list<ident, ",">? type_annotation?
typed_parm_element <- ident list<ident, ",">? type_annotation
```

The parameter list syntax is designed to work well when mixing generics and specialized parameters. For example, imagine you have the following function:

```
def test(a,b):
    # ... snip
```

This works quite well with generics. The compiler is not confused, the developer is not confused, and all is happy with the world.

However, what happens if we want to add a type to b?

```
def test(a,b uint8):
    # ... snip
```

Oops. The compiler will think that the 'a' and 'b' should be typed. This is frequently what we want, but in this case we want the 'a' to remain generic and 'b' to be specialized. However, we also want to keep the abbreviated form that lets you compress type information when contiguous variables are typed the same in the language.

Consequently, you must use an alternate separator character.

```
def test(a; b uint8):
    # ... snip
```

This works well when extended to multiple parameters:

```
def test(a, b; c uint8; d,e,f):
    # ... snip
```

Here the 'a', 'b', 'd', 'e', and 'f' parameters are generic, and the 'c' parameter has been specialized. The same system may be used when parameter types differ:

```
def test(a uint32; b uint8):
    # ... snip
```

However, if all of the parameters are typed and you want to avoid the ';' separator, you may use the form common in other languages:

```
def test(a uint32, b uint8):
    # ... snip
```

Note that if _any_ of the parameters is typed, this form will require all of the parameters to be typed. For example, this:

```
def test(a, b uint8, c, d, e):
    # ... error
```

will not compile. The recognizer will fail because the intent is ambiguous. It's impossible to determine if you meant 'a' to be typed or generic. There is only one case where it is completely unambiguous, but in order to keep everything simple we have kept the rule simple: Entirely generic or entirely typed you may use commas; mixed generics and specializations use semicolons.

## Functional Functions ##

There is a second kind of syntax that can be used for functions that basically solve an expression and return the result:

```
function <- "def" ident ("(" parm_list ")")? "=" expression
```

An example:

```
def add(a,b) = a + b
```

This is especially useful for implementing custom operators.

## Functions with Selectors ##

In order to satisfy an interface, a struct needs to have a function which implements one or more operations for it. How do we connect a function and a struct?

We use selectors.

```
function <- "def" ident ("[" ident ident "]")? ("(" parm_list ")")? return_type? function_epilogue
function_epilogue <- (functional_function / procedural_function)
functional_function <- "=" expression
procedural_function <- ":" suite
```

An example of this:

```
struct Buffer:
   index uint64
   data  []uint8

def read_byte {b Buffer} -> uint8:
   if b.index >= len(b.data):
       panic

   r := b.data[b.index]
   b.index+=1
  
   return r       

def write_byte {b Buffer} (byte uint8) -> bool:
   if b.index >= len(b.data):
       panic

   b.data[b.index] = byte
   b.index+=1
```

The struct is always passed by reference into the function as a selector. Other than identifying the function as possibly satisfying some operation for some interface, the selector is not special. It uses the same mechanism as the other parameters when being passed.

## Anonymous (Lambda) Functions ##

Functions can also be first-class values. They can be assigned to variables and passed to other functions. Explicit currying syntax is supported. As per C++, you may also specify a capture list.

```
lambda_function <- "def" ("{" list<ident, ","> "}")? ("(" parm_list ")")? function_epilogue
```


An example of a simple lambda in expression form:

```

def do_something:
   a := List({1,2,3,4,5})
   a.sort( def (l,r) = -1 if l < r else 1 if l > r else 0 )

```

A more complex lambda in procedural form:

```

def do_something:
   a := List({(1, 1), (2, 1), (3,2), (4,5)})
   a.sort( 
          def (l,r):
             r_mag := math.sqrt(l[0] * r[0])
             l_mag := math.sqrt(r[0] * r[1])
             return -1 if l_mag < r_mag else 1 if l_mag > r_mag else 0
   )

```


### Capturing ###

Most languages allow capturing of variables from outer scopes into the lambda. These are often called closures. However, it is sometimes useful to ensure that only certain values are captured in order to reduce errors. Thus we allow capture lists in the lambda form of a function. Since you cannot have a selector in lambdas, the selector braces were reused to contain the capture list.  An empty capture list forces the compiler to emit a diagnostic if you have accidentally captured a variable in an outer scope. By default all variables from all enclosing scopes will be captured. If you wish to restrict captures to a limited list of variables names, place their names in a comma delimited list inside the capture braces.

For example:

```

log := get_logger()
a := List({(1, 1), (2, 1), (3,2), (4,5)})
a.sort( 
        def {} (l,r):
           log.info("sorting vectors")
           r_mag := math.sqrt(l[0] * r[0])
           l_mag := math.sqrt(r[0] * r[1])
           return -1 if l_mag < r_mag else 1 if l_mag > r_mag else 0
)

```


The compiler would emit a diagnostic at this point and fail to compile. Since you specified an empty capture list, the "log" call at the beginning of the lambda suite is illegal.


For example:

```

log := get_logger()
some_time := time.now()
a := List({(1, 1), (2, 1), (3,2), (4,5)})
a.sort( 
        def {log} (l,r):
           log.info("sorting vectors at %v", some_time)
           r_mag := math.sqrt(l[0] * r[0])
           l_mag := math.sqrt(r[0] * r[1])
           return -1 if l_mag < r_mag else 1 if l_mag > r_mag else 0
)

```

In this example we specified `log` in the capture list, but not `some_time`. Consequently, the compiler will also fail here.

### Currying ###

Currying is the process of partially applying functions. The value is that you may create a function which is partially applied, and then handed to another function which fully applies it. Or it may again partially apply it until it arrives at some final destination. This allows a separation of concerns, without forcing you to pass around state or cause function parameter explosion.

The following example is synthetic, but illustrates the point:

```

pred := def (l,r) = -1 if l < r else 1 if l > r else 0
cmp  := pred(10, _)

a:= cmp(5)  # will yield 1
b:= cmp(15) # will yield -1
c:= cmp(10) # will yield 0 

```

In essence, this works as if it had been written like this:

```

pred  := def (l,r) = -1 if l < r else 1 if l > r else 0
cmp   := def (r) = pred(10, r)

a:= cmp(5)  # will yield 1
b:= cmp(15) # will yield -1
c:= cmp(10) # will yield 0 

```

In fact, this is exactly how it's implemented. A new anonymous function is created which does nothing but call the curried function with some captured values.

You may call the function with as many placeholders as you like, and they do not have to be in explicit order. The arguments which are not placeholders will be stripped from the signature, and a new lambda signature will be created for the outer function.

For example:

```

log := def {} (logger, log_type, message):
         print("%s [%s] %s", log_type, logger, message)

debug := log(_, DEBUG, _)
info  := log(_, INFO, _)
warn  := log(_, WARN, _)


debug("root", "some debug info")
info("root.child", "some startup info")
warn("root.child", "something bad might have happened!")

```

#### Interaction of Currying with Generics ####

You might think that currying and generics wouldn't work well together. In fact, they work together just fine. If the curried function is generic, the partially applied version retains also the flexibility of the original, minus whatever parameters are applied and known.

The actual function that a call site will execute is never known until the call site is fully evaluated. In the case of partially applied functions this is still true. The only added detail is that the closure package is also passed around. At the final call site, the complete nature of the fully applied call is known. The capture package is unwrapped and expanded in place as parameters to the original function. During the compile, the complete argument list is used to search for a matching, already-generated function. If one is not found, one is generated, and a call is made to that function.

# Blocks #

amalgam has special support for user defined blocks. These are syntactically separated areas of code that have user-defined semantics.

In general, block customization looks like this:

```
block_enter_def <- "block" "enter" ident \( ident \) ":"
block_exit_def <- "block" "exit" ident \( ident, ident, ident \) ":"
block_def <- "block" ident ("chain-enter" list<ident, ",">)? ("chain-exit" list<ident, ",">)? ("chain" list<ident, ",">)?
block_as <- ("as" ident)? 
block <- ident (\( expression \))? block_as? ":" suite
```


For example, you could define an if block like this:

```
block enter if(expression) = expression == true
```

You could then use it like this:

```
# simple block
if 5+6 > 10:
  # some code

if 5+6 > 10 as result:
  # some code which uses result
```

The code in the block is only evaluated if the expression evaluates to true. Blocks can also be chained on entry or exit. So if you wanted to implement the full Pythonic if/elif/else syntax:

```
block enter if(expression) = expression == true
block enter elif(expression) = expression == true

block if chain-enter elif, else
block elif chain-enter elif, else
block else
```

This lets you say that, if you don't enter the if block, check the next elif or else block. For elif the rule is similar: if you don't enter this elif block, check for another elif block. If that is missing check for an else block.

You can implement Python's for/else like this:

```
block enter for(expression) = expression.has_next()
block exit for(expression, fallthrough):
  if expression.has_next():
    return (false, false)

  if fallthrough:
    return (true, true)

  return (true, false)
 
block for chain else
block else
```

In addition to having user defined entry and exit semantics, a block tags its operations with the block name. This allows you to specify that certain operations behave differently inside a block.

For example, consider a software transactional memory block:

```
block enter transaction(expression):
   # Setup a new transaction
   tx := transaction()
   tx.set_parent(parent_tx)get_thread_local_tx())
   tx.set_thread_local_tx()
   return true

block exit transaction(expression, fallthrough):
   if !tx.aborted():
     tx.commit()

   return true

machine load.uint32.transaction:
   # r0 contains the address to read from
   call stm_read(r0)
   result = load.uint32 r0

def stm_read(address ptrint):
   # assume presence of some code to do this...
   tx := get_thread_local_tx()
   tx.add_read_of(address)
```

This lets us provide hooks deep into the compiler's code generator, without needing to understand how the compiler itself is constructed. In fact, all code generation for amalgam is done this way. There are a set of standard machine templates that specify the machine operations for operations on all of the primitive types.

## Block Definitions ##

A simple block definition indicates that the block exists.

```
block else
```

However, you can also indicate that the block chains to one or more blocks on entry or exit or both. This means that if the block does not enter, it should try the next block in it's chain list. On exit a block will chain only if the block exit handler indicates that it should. If you specify a chain element in a block definition, but do not specify a handler, the default behavior is to chain.

```
block if chain-enter elif, else
```

Note in the above example, might not look quite right because it could admit many else blocks:

```
if false:
  # code
else:
  #code
else:
  #code
```


If you write this code both else blocks will execute. However, since it is nonsensical, the solution is to just not write it. It does not cause a dangling else problem:

```
if true:
   if false:
     #code
   else:
     #code
else:
   #code
```

The final else is not in the chaining scope of the nested if. So the first if block will exit and then skip the trailing else block.

## Block Enter Definitions ##

The entry definition is run just before a block is entered. Code run inside the block entry is _not_ tagged with the block definition. That means that it uses either the generic instruction translation if it is not nested inside another block, or the translation generated from the most closely nested parent block.

The code will be pasted inline by the compiler, at the location where the block begins.

The definition should return true at runtime if the block should be entered, false otherwise. If false is chosen the body and exit section (if any) will be skipped.

## Block Exit Definitions ##

An exit definition is much like an entry definition, but is run just after a block exits. The same process is performed on exit blocks as on entry blocks. Note that block exit always runs, even if block entry did not. That is why block exit takes two parameters: one is the expression to evaluate, and the last is a bool indicating if the block exit is called because the block's mainline fell through (true) or if a break was encountered (false).

A block exit returns a tuple of bools: (exit, chain). If exit is false,  the block will _loop_ back to the start of the block. The entry code will _not_ be executed again. If it returns true, then the chain value comes into play. If chain is true, then the block chain definition will be consulted and the next chained block (if any) will be entered. If it is false, execution will resume _after_ any chained blocks. The intervening chained blocks will be skipped entirely.

# Machine Blocks #

```
machine_block <- "machine" ":" machine_instruction_suite
machine_instruction_suite <- m_load / m_store / m_alu_op / m_atomic_op / m_cmpxchg
```

Machine blocks are special instances of blocks. They function as a very low level interface to the compiler itself. Much of the way the compiler generates code can be adjusted by using machine blocks. However, machine blocks are not a compiler artifact, they are an integral part of the language itself.

Very simply, machine blocks provide a way for you to specify the exact sequence of amalgam virtual machine instructions you want executed. This does not directly translate to any specific processor assembly language, but it does translate to the underlying representation that amalgam uses for operations.

Note:

> The code you write in machine blocks is not excepted from optimizer passes. Therefore, the actual sequence of instructions executed is guaranteed to have the same effect as the sequence you wrote, but may not be the exact same instructions.

A simple example:

```

def add(a,b):
   var c typeof(a)
   machine:
      r0 = load a
      r1 = load b
      r2 = r0 + r1
      store r2 -> c

   return c

```

## Machine Block Operations ##

With the exception of the load and store instructions, the source and destination operands must be a register name. A register name must be 'r' followed by a number. Once a register has been assigned to a value you may not reassign it. Assigning a register sets the type of that register. All register source operands are required to be compatible types. You must use explicit conversion operations to change between incompatible types. Operations must appear one per line.

As an example of many of these constraints:

```
r0 = load a   # r0 gets same type as a
r1 = load b   # r1 gets same type as b
r2 = a + b    # r2 gets result type of a + b 
store r2 -> b # legal, b is updated (but r1 is not)
store r2 -> a # legal, a is updated (but r0 is not)

r0 = load a  # illegal, r0 is already assigned
r3 = r2      # legal, makes a copy of r2
```

```
register <- "r" \d+
```


| **Syntax** | **Name** | **Description** |
|:-----------|:---------|:----------------|
| _dest_ = load [_type_]? _address_ | load | Loads the value at the address specified. The amount of data loaded depends on the type of the variable and the _type_ specified for load. |
| store [_type_]? _src_ -> _address_ | store | Stores the value from the register specified to the address specified. The amount of data stored depends on the type of the target variable or on the number of bits specified. In no case will it be more than the variable can hold (if the variable's type is known.) |
| _dest_ = atomic load [(_order_)] [_type_] _address_ | atomic load | Works like load, except the load is done atomically. The _order_ parameter must be one of 'unordered', 'monotonic', 'acquire', and 'seq\_cst'. The default is 'seq\_cst'. |
| atomic store [(_order_)] [_type_] _src_ -> _address_ | atomic store | Works like 'store', but perfoms the operation atomically. The _order_ parameter must be one of 'unordered', 'monotonic', 'release', and 'seq\_cst'. The default is 'seq\_cst'. |
| _dest_ = addressof _variable_ | addressof | _variable_ may refer to a local variable, or to any legal variable reference. |
| _dest_ = _src1_ + _src2_ | add | Adds the two registers together, puts the result in _dest_ |
| _dest_ = _src1_ - _src2_ | sub | Subtracts _src2_ from _src1_, stores the result in _dest_ |
| _dest_ = _src1_ `*` _src2_ | mul | Multiplies the two registers together, puts the result in _dest_ |
| _dest_ = _src1_ / _src2_ | div | Divides _src1_ _src2_ times, stores the result in _dest_ |
| _dest_ = _src1_ % _src2_ | mod | Computes the modules of _src1_ by _src2_ and stores the result in _dest_ |
| _dest_ = _src1_ & _src2_ | and | Perform bitwise and on the two source registers, puts the result in _dest_ |
| _dest_ = _src1_ | _src2_ | or | Perform bitwise or on the two source registers, puts the result in _dest_ |
| _dest_ = _src1_ ^ _src2_ | xor | Perform bitwise xor on the two source registers, puts the result in _dest_ |
| _dest_ = _src1_ < _src2_ | less than | Determine if _src1_ is less than _src2_. _dest_ is of type bool and contains the result. |
| _dest_ = _src1_ > _src2_ | greater than | Determine if _src1_ is greater than _src2_. _dest_ is of type bool and contains the result. |
| _dest_ = _src1_ <= _src2_ | less than or equal | Determine if _src1_ is less than or equal to _src2_. _dest_ is of type bool and contains the result. |
| _dest_ = _src1_ >= _src2_ | greater than or equal | Determine if _src1_ is greater than or equal to _src2_. _dest_ is of type bool and contains the result. |
| _dest_ = _src1_ == _src2_ | equal to | Determine if _src1_ is equal to _src2_. _dest_ is of type bool and contains the result. |
| _dest_ = _src1_ != _src2_ | not equal to | Determine if _src1_ is not equal to _src2_. _dest_ is of type bool and contains the result. |
| _dest_ = trunc _src_ to _type_ | truncate | Takes _src_ and truncates it to _type_. _type_ must be a native amalgam type. |
| _dest_ = extend _src_ to _type_ | sign or zero extend | Takes _src_ and extends it to _type_. _type_ must be a native amalgam type. If the target type is a 'uint' the value will be zero extended, otherwise it will be sign extended. |
| _dest_ = convert _src_ to _type_ | convert | Takes _src_ and converts it to _type_. _type_ must be a native amalgam type. This is used to convert floating point values to integers and vice versa. |
| _dest_ = [(_order_)] _address_ ? _compare_ : _src2_ | compare and exchange | Compare the value at _address_ with _compare_. If they are the same, write _src2_ into _address_. This operation is always atomic. _order_ must be one of 'unordered', 'monotonic', 'acq\_rel', and 'seq\_cst'. The default is 'seq\_cst'. |
| _dest_ = atomic _op_ [(_order_)] _src_ -> [_type_] _address_ | atomic read/write/modify | The value at _address_ is read, and _op_ is performed on it atomically with _src_ as the second operand. _dest_ contains the  value at _address_ **before** _op_ is performed. _op_ must be one of 'add', 'sub', 'and', 'or', 'xor', 'max', 'min'. _order_ must be one of 'unordered', 'monotonic', 'acq\_rel', and 'seq\_cst'. The default is 'seq\_cst'. |

### memory ordering ###

```
common_ordering <- "unordered" / "monotonic" / "seq_cst" 
load_ordering <- "acquire" / common_ordering
store_ordering <- "release" / common_ordering
rmw_ordering <- "acq_rel" / common_ordering
```

| **Name** | **Description** |
|:---------|:----------------|
| unordered | The set of values that can be read is governed by the happens-before partial order. A value cannot be read unless some operation wrote it. |
| monotonic | In addition to the guarantees of unordered, there is a single total order for modifications by monotonic operations on each address. All modification orders must be compatible with the happens-before order. There is no guarantee that the modification orders can be combined to a global total order for the whole program (and this often will not be possible). The read in an atomic read-modify-write operation (_compare exchange_ and _atomic op_) reads the value in the modification order immediately before the value it writes. If one atomic read happens before another atomic read of the same address, the later read must see the same value or a later value in the address's modification order. This disallows reordering of monotonic (or stronger) operations on the same address. If an address is written monotonically by one thread, and other threads monotonically read that address repeatedly, the other threads must eventually see the write.|
| acquire | In addition to the guarantees of monotonic, a synchronizes-with edge may be formed with a release operation. |
| release | In addition to the guarantees of monotonic, if this operation writes a value which is subsequently read by an acquire operation, it synchronizes-with that operation. |
| acq\_rel (acquire and release) | Acts as both an acquire and release operation on its address. |
| seq\_cst (sequentially consistent) | In addition to the guarantees of acq\_rel (acquire for an operation which only reads, release for an operation which only writes), there is a global total order on all sequentially-consistent operations on all addresses, which is consistent with the happens-before partial order and with the modification orders of all the affected addresses. Each sequentially-consistent read sees the last preceding write to the same address in this global order. |

### (atomic) load ###

```
m_load <- register "=" "atomic"? "load" (\(load_ordering\))? type_annotation? (ident / register) 
```


Loads the value at the address specified. The amount of data loaded depends on the type of the variable and the type specified. The load\_ordering specification is explained in [#memory\_ordering](#memory_ordering.md). If the type is omitted, the compiler will try to determine what the type should be. In cases where it is not clear what the type should be, an error diagnostic will occur and the program will fail to compile.

In no case will more data be loaded from the target than it can handle. For example, if the type annotation is "uint64" for the load operation, but the source variable is a "uint32", a diagnostic error will be issued and the program will fail to compile.

Examples:
```
r0 = load a      # loads the value of a into r0

r1 = addressof a # get address of a into r1
r2 = load sint32 r1   
                 # loads the value of a into r2 (assume it 
                 #  is a signed 32-bit value.)  

r3 = load s.link # gets the value of the link field from
                 # struct s into r3

r4 = load uint64 r3
                 # gets the value pointed to by r3 into r4,
                 #  reads a uint64

r5 = atomic load (acquire) a 
                 # atomically gets the value of a into r5, uses
                 # the 'acquire' ordering, which assumes a 
                 # subsequent atomic store (release).

r6 = atomic load a
                 # atomically gets the value of a into r6, uses
                 # the 'seq_cst' ordering.
```


### (atomic) store ###

```
m_store <- "atomic"? "store" type_annotation? (\(store_ordering\))? register "->" (ident / register) 
```


Stores the value to the address specified. The amount of data stored depends on the type of the target variable and the type specified. The store\_ordering specification is explained in [#memory\_ordering](#memory_ordering.md). If the type is omitted, the compiler will try to determine what the type should be. In cases where it is not clear what the type should be, an error diagnostic will occur and the program will fail to compile.

In no case will more data be stored to the target than it can handle. For example, if the type annotation is "uint64" for the store operation, but the target variable is a "uint32", a diagnostic error will be issued and the program will fail to compile.

Examples:
```
store r0 -> a    # stores the value of r0 into a

store sint32 r2 -> r1   
                 # stores the value in r2 into the address
                 # pointed to by r1, assumes that r1 points at
                 # an sint32.

store r3 -> s.link 
                 #stores r3 into the link field of
                 # struct s.

store uint64 r5 -> r4
                 # stores the value of r5 into the address
                 # pointed at by r4, assumes r4 points to a
                 # uint64

atomic store (release) r6 -> a 
                 # atomically stores the value of r6 into a, uses
                 # the 'release' ordering, which assumes a 
                 # previous atomic load (acquire).

atomic store r7 -> a
                 # atomically stores r7 into a, uses
                 # the 'seq_cst' ordering.
```

### arithmetic operations ###

```
m_alu_op <- register "=" register ("+" / "-" / "*" / "/" / "&" / "|" / "^" / "%") register 
```

Performs one of the supported arithmetic operations. The source is always an existing register, and the destination is always a new register. The two source registers must be of compatible types.

Examples:
```
r1 = 4
r2 = 2

r3 = r1 + r2 # add r1 and r2
r4 = r1 - r2 # subtract r2 from r1
r5 = r1 * r2 # multiple r1 and r2
```

### comparison operations ###

```
m_cmp_op <- register "=" register ("==" / "!=" / "<=" / ">=" / "<" / ">") register 
```

Performs the given comparison. The result of the comparison is placed in the destination register, which always has type bool.

Examples:
```
r0 = 1
r1 = 2

r2 = r1 == r0 # false 
r3 = r1 != r0 # true
r4 = r1 > r2  # true
r5 = r1 < r2  # false
```

### atomic read/modify/write operations ###

```
m_atomic_rmw <- register "=" "atomic" ("add" / "sub" / "and" / "or" / "xor" / "max" / "min") (\(rmw_ordering\))? register "->" type_annotation? (register / ident)
```

Performs one of the supported read / modify / write operations atomically on the given address. The result is a copy of the value at the given address before the operation was performed.

Examples:
```
r0 = 1
r1 = atomic add (acq_rel) r0 -> a  # atomically increments a.
                                   # r1 contains the value of a
                                   # before the increment. uses
                                   # acquire / release semantics.

r2 = atomic sub r0 -> a            # atomically decrements a.
                                   # r2 contains the value of a
                                   # before the decrement. uses
                                   # sequentially consistent
                                   # semantics.

r3 = addressof a
r4 = 4
r5 = r3 + r4

r6 = atomic increment r0 -> uint64 r5    

                                  # atomically increments the value
                                  # pointed to by the address in r5.
                                  # assumes that the address references
                                  # a uint64                                    
```


### atomic compare and exchange ###

```
m_cmpxchg <- register "=" (register / ident) "?" register ":" register
```

Performs an atomic compare and exchange operation. The destination will hold the value at the read/write address before the exchange operation was made (assuming it is made at all.) If the value at the read/write address is the same as the value in the compare register, then the value in the update register will be written to the read/write address. The examples make this much clearer.

Examples:
```
r0 = 1
r1 = 2
r2 = a ? r0 : r1  # if a == r0, then r1 will be written to a. in
                  # all cases, r2 hold the value of a before this
                  # instruction began.
```