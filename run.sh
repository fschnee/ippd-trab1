#!/usr/bin/env sh
"${1:-g++}" -std=c++20 nqueens.cpp -O2 -o nqueens -fopenmp && ./nqueens