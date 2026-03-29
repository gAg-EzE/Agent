#!/bin/bash

SRC="exo1.c"
EXE="pi"
DATA="speedup.dat"
PLOT="plot.gnu"
IMG="speedup.png"

echo "Compilation..."
mpicc -O3 -o $EXE $SRC -lm

if [ $? -ne 0 ]; then
    echo "Erreur compilation"
    exit 1
fi

echo "Execution reference (np=1)"
T1=$(mpirun -np 1 ./$EXE 1000000000 | grep "temps" | awk '{print $3}')

echo "#proc speedup efficacite" > $DATA

for p in 1 2 4
do
    echo "Execution avec $p processus"
    Tp=$(mpirun -np $p ./$EXE 1000000000 | grep "temps" | awk '{print $3}')
    
    speedup=$(echo "$T1 / $Tp" | bc -l)
    efficacite=$(echo "$speedup / $p" | bc -l)
    
    echo "$p $speedup $efficacite" >> $DATA
done

cat <<EOF > $PLOT
set terminal png size 800,600
set output "$IMG"

set title "Speedup MPI - Methode des trapezes"
set xlabel "Nombre de processus"
set ylabel "Speedup"
set grid

plot "$DATA" using 1:2 with linespoints title "Speedup"
EOF

gnuplot $PLOT

echo "Graphique genere: $IMG"
