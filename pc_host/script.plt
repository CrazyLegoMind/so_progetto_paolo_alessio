#cambia etichette degli assi
set xlabel "times (s)"
set ylabel "values (V)"

#grafica i dati contenuti nel file
pl [:][:] 'samples.txt' t 'Output oscilloscopio' w l lt 1