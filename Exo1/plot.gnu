set terminal png size 800,600
set output "speedup.png"

set title "Speedup MPI - Methode des trapezes"
set xlabel "Nombre de processus"
set ylabel "Speedup"
set grid

plot "speedup.dat" using 1:2 with linespoints title "Speedup"
