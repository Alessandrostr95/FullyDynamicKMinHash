#!/bin/bash
# g++ main.cpp -O2 -o test 2>/dev/null
g++ experiments.cpp -O3 -fopenmp -mavx -o experiment

./experiment