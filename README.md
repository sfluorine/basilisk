# BASILISK

IDC what u said, just build the project and try the language.

## Basic Syntax


### Functions stuff

```python
def add[x, y] -> {
    x + y # every function must return something.
}

def main[] -> { # every module must have an entry point function, or main function.
    print[add[34, 35]] # function calls.
    0 # yes, even the main function must return the exit code. (integer)
}
```

### Let Bindings

Let bindings lets you to mutate variable within the a function scope.

```python
def dummy[x, y] -> {
    let [z] -> { # you can create new variables inside the square brackets.
        x -> x * x, # every assignment is separated by comma.
        y -> y * y,
        z -> x + y
    }

    let [_] -> { # or if you don't want to create new variable, just add '_' inside the square brackets and you will be fine.
        x -> x * y, # every assignment is separated by comma.
        y -> y * x
    }

    z
}
```

### Records

You can create records for compound types

```python
record Tree {
    lhs,
    rhs
}

def make_tree[lhs, rhs] -> {
    record Tree { lhs, rhs } # this is the syntax for record creation.
}

def main[] -> {
    print[make_tree[34, make_tree[35, 36]]]
    0
}
```
