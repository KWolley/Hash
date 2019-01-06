# Hash Tables

This homework is about hash tables. It differs from "real world" hash
table implementations in important ways:

1. The homework's hash table will not automatically resize. Once it is
   full, it's full and subsequent additions won't work, even if you
   remove items because deleted nodes remain present.

2. The homework's hash table only accepts string keys and string
   values. Real hash tables would be designed to accommodate other
   types (e.g. through generics or templates).

3. The homework's hash tables only depend on the hash code, and does
   not require an 'equals' function to disambiguate distinct keys
   whose hash codes collide. 

## Hash Function Provided For You

The hash function included here is good, but probably not adequate for
production use. It is short and straightforward while also producing
pretty good (but not fantastic) results. If you are super curious,
feel free to write your own hash function or find one (e.g. from "Hash
Functions: An Empirical Comparison" on CodeProject.com:
https://goo.gl/XVggft). You can swap in your hash function with the
hash_table's `hash_func` variable.
   
## Dynamic Arrays

The `hash_table` struct has a field: `hash_node** table`, which might
be new to you. Let me build this up for you.

If we had a field `hash_node* somenode`, you'd be forgiven for
believing that this is just a pointer to a node. It is that, but if
you use array syntax when you allocate the dynamic memory, then your
variable can be used as an array. So to make an array of hash_node
pointers, you'd do this:

```cpp
hash_node* many = new hash_node[8];
```

This allocates memory for eight hash_nodes, and returns a pointer to
the first element. We can then use many[0] through many[7] to refer to
these eight values. And the contents of many[i] is an actual
`hash_node` struct, not a pointer to one:

```cpp
hash_node three = many[3]; // bare struct value
```

To get a dynamic array of pointers to structs, we need to use that
double-pointer syntax when declaring the variable. So we have this:

```cpp
hash_node** table = new hash_node*[8];
```

Note the double pointer on the left, and the one asterisk on the
right. Once you do this, then you can access elements thusly:

```cpp
hash_node* ptrToThree = table[3]; // table[3] is a pointer, not a bare value
```

When you're created a dynamic array of pointers, you'll need to
iterate through the array and set the values to NULL.

For more on dynamic arrays: http://www.cplusplus.com/doc/tutorial/dynamic/

In case you're thinking "this syntax is awful and confusing", well,
you'll get no argument here.

## Function Pointers

The hash table's `hash_func` and `bucket_func` fields are both C-style
function pointers. This lets us pass functions around like any other
data. You'll not need to think hard about these, but since they're in
there I thought I'd point it out. It lets us swap in a different hash
function if we want.

For more info on function pointers:
http://www.learncpp.com/cpp-tutorial/78-function-pointers/
