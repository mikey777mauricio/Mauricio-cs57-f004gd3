#!/bin/bash
# Bash script tests semantic analysis module 
# Mikey Mauricio    week1    April 18, 2023

make 

echo "---------------------------"

echo testing valid c programs 

echo "---------------------------"

echo valid p1
./miniC.out ./tests/p1.c
echo return_code: $? 

echo "---------------------------"

echo valid p2
./miniC.out ./tests/p2.c
echo return_code: $?

echo "---------------------------"

echo valid p3
./miniC.out ./tests/p3.c
echo return_code: $?

echo "---------------------------"

echo valid p4
./miniC.out ./tests/p4.c
echo return_code: $?

echo "---------------------------"

echo valid p5
./miniC.out ./tests/p5.c
echo return_code: $?

echo "---------------------------"

echo valid sample_miniC
./miniC.out ./tests/sample_miniC.c
echo return_code: $?

echo "---------------------------"

echo testing invalid c programs 

echo "---------------------------"

echo invalid p1
./miniC.out ./semantic_tests/p1.c
echo return_code: $?

echo "---------------------------"

echo invalid p2
./miniC.out ./semantic_tests/p2.c
echo return_code: $?

echo "---------------------------"

echo invalid p3
./miniC.out ./semantic_tests/p3.c
echo return_code: $?

echo "---------------------------"

echo invalid p4
./miniC.out ./semantic_tests/p4.c
echo return_code: $?

echo "---------------------------"

echo valgrind on sample_miniC
make valgrind 
echo "---------------------------"

make clean