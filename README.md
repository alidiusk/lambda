# Lambda
Lambda calculus interpreter written in `C`. Can be used as a REPL or to evaluate lambda expressions found in files.

Uses a recursive descent parser to parse the lambda expression, then performs normal order reduction.

## Usage
```bash
# Evaluate a lambda expression in a file.
$ ./lambda [path]
# Enter the REPL
$ ./lambda
```

## Building
No libraries are required, but the project has only been compiled on Linux.
Simply use `make` to build the project. It will create a binary called `lambda`.

## Resources
Many resources were used in the creation of this interpreter, including:
 - https://craftinginterpreters.com/
 - http://blog.rchain.coop/2018/08/15/computational-calculi-primer/
 - https://personal.utdallas.edu/~gupta/courses/apl/lambda.pdf
 - https://www.cs.tufts.edu/comp/105-2019f/reduction.pdf
 - http://www.itu.dk/people/sestoft/papers/sestoft-lamreduce.pdf

Licensed under the GLPv3.
