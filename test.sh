#!/bin/bash

should_compile=(101 102 201 202 203 204 301 401 502 503 504 505 601)

shouldnt_compile=(103 104 105 205 206 207 208 209 210 211 212 213 302 402 506 507 602 603 604)

extern=(501)

test() {
    ./test_single.sh $1
}

test_group() {
    group=("$@")  # Przypisanie wszystkich argumentów funkcji do tablicy
    for testnum in "${group[@]}"; do
        test $testnum
    done
}

test_shouldnt_compile() {
    for testnum in ${shouldnt_compile[@]}; do
        echo "Test nr. $testnum"
        if ./compile.sh poly_test $testnum &> /dev/null; then
            echo -e "\033[0;31mCompiling succeed\033[0m"
        else
            echo -e "\033[0;32mCompilation failed\033[0m"
        fi
        echo
    done
}

echo -e "\033[0;34m------SHOULD COMPILE------\033[0m"
test_group "${should_compile[@]}"

echo -e "\033[0;34m-----SHOULDN'T COMPILE-----\033[0m"
test_shouldnt_compile

echo -e "\033[0;34m----------EXTERN----------\033[0m"
test_group "${extern[@]}"