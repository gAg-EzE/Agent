#!/bin/bash

echo "=============================="
echo "Compilation"
echo "=============================="

mpicc -O3 -o pi exo1.c -lm

if [ $? -ne 0 ]; then
    echo "Erreur de compilation"
    exit 1
fi

echo ""
echo "=============================="
echo "1 processus, n = 5000"
echo "=============================="
mpirun -np 1 ./pi_trap 5000

echo ""
echo "=============================="
echo "1 processus, n = 1000000000"
echo "=============================="
mpirun -np 1 ./pi_trap 1000000000

echo ""
echo "=============================="
echo "Scalabilite pour n = 1000000000"
echo "=============================="

echo ""
echo "--- np = 1 ---"
mpirun -np 1 ./pi_trap 1000000000

echo ""
echo "--- np = 2 ---"
mpirun -np 2 ./pi_trap 1000000000

echo ""
echo "--- np = 4 ---"
mpirun -np 4 ./pi_trap 1000000000

echo ""
echo "Tests terminés"
