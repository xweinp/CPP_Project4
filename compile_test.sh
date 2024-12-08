#!/bin/bash


clang++ -ferror-limit=1 -Wall -Wextra -std=c++20 -O2 $1.cpp -o poly_test