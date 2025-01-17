#!/bin/bash

if (( $# != 1 && $# != 2 )); then
    echo "Usage: ./compile_test.sh <name> <testnum>(optional)"
    exit 1
fi

if (( $# == 1 )); then
    name=$1
    clang++ -ferror-limit=1 -Wall -Wextra -std=c++20 -O2 $name.cpp -o poly_test
else
    name=$1
    testnum=$2
    clang++ -ferror-limit=1 -Wall -Wextra -std=c++20 -O2 -DTEST_NUM=$testnum $name.cpp -o poly_test
fi
