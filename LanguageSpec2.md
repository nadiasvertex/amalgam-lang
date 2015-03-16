

# Amalgam #

## Generic Programming ##

amalgam supports several orthogonal mechanisms to make generic programming easier. This section explains those mechanisms.

One of the key features of generic programming involves the ability to specify an algorithm that works on many kinds of data. In some languages this is referred to as template programming, in others it is called parametric polymorphism. Still others call it simply generics. Regardless of the name, it involves the ability to abstract out the actual type of data being operated on in both a data structure and data processing context.

### Generic Types ###

A generic type is a user defined type in which some or all of the field types may vary. There are two ways to specify a generic type. The easiest is simply to omit a type annotation on the field:

```
struct node:
   element
   depth   uint8
```

This can be instantiated like this:

```
n1 := node(1, 0)      # n1.element is a uint8
n2 := node("test", 0) # n2.element is a string 
```

Another way that a generic type can be defined is by using type parameters.

```
struct node(element_type type):
   element element_type
   depth   uint8
```

To create an instance of this object you must explicitly pass in the type of the element:

```
n1 := node(uint32, 1, 0)      # n1.element is a uint32
n2 := node(string, "test", 0) # n2.element is a string 
```

Since this method is more verbose you might wonder why you would ever want to use this syntax. There are essentially two reasons:

  1. You want to control very precisely what kind of type is chosen for the generation of element.
  1. You want to use constraints to control various factors involved in the creation of the concrete type.

A generic type may refer to itself:

```
struct node(element_type type):
   element    element_type
   prev, next node(element_type)
```

A generic type may provide defaults:

```
struct node(element_type type = string):
   element    element_type
   prev, next node(element_type)
```

In this case it would become acceptable to create a new node like this:

```
v1 := node("test")
```

This shortcut only works for cases where you are actually instantiating the generic type. If you are referring to it in any declarative sense (some variable is this type) then an unspecialized reference to a generic type will mean any specialization of the type. For example:

```
def append(n node):
   # do something
```

This means that the append function takes any kind of node. Whether it is a node of string or a node of uint32. This also causes append to become a generic function, which we'll cover in more detail later.

### Generic Type Parameters ###

A generic type may take parameters which are used to further construct the type. For example, imagine an extensible vector:

```
struct vector(element_type type, num_components parm):
    v element_type * num_components
```

You could then use it like this:

```
v1 := vector(float32, 3, (0.0, 0.0, 0.0))  # 3-vector of floats
v2 := vector(float32, 2, (0.0, 0.0))       # 2-vector of floats
v3 := vector(sint32,  4, (0, 0, 0, 0))     # 4-vector of signed ints
```

Generic type parameters must resolve to compile-time constants. You may specify defaults for them, just like you can for types:

```
struct vector(element_type type, num_components parm = 2):
    v element_type * num_components
```

In this case you can omit the num\_components element and the vector will be a 2-vector.

```
v1 := vector(sint32, (0, 0))
```

### Generic Functions ###

A generic function is much the same as a generic type, except that an entire function is generated. These kinds of functions have two forms, just like types. The first is:

```
def add(a,b) = a + b # expression form
def print_sum(c):
  print c
```

This is just like the brief generic form of structs. A correctly typed version of the function will be generated in the same module as the call site. You use it in a very natural way:

```
v := add(5,6)
print_sum(v)
```

The other form is more verbose:

```
def add(t type; a,b t) = a + b
def print_sum(t type; a t):
   print a + b
```

It must also be used in a more verbose manner:

```
add_int := add(sint32)
print_int := print_sum(sint32)

v := add_int(5,6)
print_int(v)
```

The primary advantage here agains lies in control. However, functions have an additional ability called specialization. If you provide a specialization, that function will be preferred over an unspecialized function.

#### Specialization ####

```
def add(t, t2 type; a t, b t2) = a + b  # unspecialized, generic function
def add(t sint8; t2 type; a,b t) = a + b # specialized, generic function
```

In this example you may have some special code which does a conversion of b, or there may be some reason why having a be an sint8 is efficient. In any case, a function may be fully or partially specialized. The compiler will always pick the more specialized function if there are arguments that exactly match the formal arguments of the function prototype.

### Generic Function Parameters ###

Just like generic types, generic functions may be parameterized:

```
def identity(t type, num_components parm) = vector(t, num_components, (1,1,1))
```

Defaults values may be assigned to the parameterized and generic components just as in structs.

### Generic Static Expressions ###

In addition to the generic typing and parameterization of functions, you may also use certain static expressions to make compile-time decisions. These constructs are very similar to the macro facilities provided by some languages. Unlike some languages, however, these elements are part of the AST.

#### Static If ####

Sometimes you may wish to compile certain code conditionally. It is good to be able to do that in a way that cooperates with the language, instead of having messy preprocessor macros all over.

### Generic Constraints ###

A constraint is a compile-time check which is performed on the generic type before it is expanded into the concrete form. It allows you to restrict or constrain the ways in which the generic type is generated.

For example, you may have code which requires a vector type. For space reasons you want vector fields to be as small as possible, and for other reasons you only support integer types. You might therefore define the vector like this:

```
struct vector(element_type type) 
         where element_type is integral
           and element_type is signed: 
   x, y, z element_type
```

When you go to instantiate the vector type you would have these results:

```
v1 := vector(sint8,   1,1,1)        # legal
v2 := vector(float32, 1.0,1.0,1.0)  # fails, not integral  
v3 := vector(uint8,   1,1,1)        # fails, not signed
```

The constraint check itself is introduced by the **`where`** keyword. The checks revolve around testing properties of the types. You may test if a type **`is`** something. You may also test generic parameters aginst any compile-time constants. In the above example, we test if the element type _is_ integral or signed. These properties are actually interfaces that some type implements.

For the built-in integer types, it is as if they were defined like this:

```
interface integral
interface signed

interface sint8 is integral, signed
```

If you wanted to use the parameterized vector above, you might want to make sure that the number of elements in the vector is greater than 0.

```
struct vector(element_type type, num_components uint8) where num_components > 0
    v element_type * num_components
```

The check can use any compile-time constant expressions.

Similar checks can be institutued for generic functions:

```
def sort(i) where i is iterator:
   # do something
```