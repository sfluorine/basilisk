cmake_minimum_required(VERSION 3.7...3.27)

project(
    basilisk
    VERSION 1.0
    LANGUAGES C)

set(target
    basilisk)

set(sources
    src/ast.h
    src/ast.c
    src/common.h
    src/common.c
    src/interpreter.h
    src/interpreter.c
    src/lexer.h
    src/lexer.c
    src/main.c
    src/parser.h
    src/parser.c
    src/span.h
    src/span.c
    src/token.h
    src/token.c
    )

add_executable(
    ${target}
    ${sources}
    )
