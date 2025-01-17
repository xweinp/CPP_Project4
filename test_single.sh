#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: ./test_single.sh <test_number>"
    exit 1
fi

testnum=$1
echo "Test nr. $testnum"
echo "Compiling..."
if ./compile.sh poly_test $testnum; then
    echo -e "\033[0;32mCompiling succeed\033[0m"
    echo "Running..."
    ./poly_test
    echo "Running finished"
else
    echo -e "\033[0;31mCompilation failed\033[0m"
fi
echo