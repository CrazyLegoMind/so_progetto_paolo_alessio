#cambia etichette degli assi
set xlabel "times (s)"
set ylabel "values (V)"

#grafica i dati contenuti nel file
#pl [:][:] 'samples.txt' t 'Output oscilloscopio' w l lt 1
pl [:][:] 'samples_A0.txt','samples_A0.txt','samples_A1.txt','samples_A2.txt','samples_A3.txt','samples_A4.txt','samples_A5.txt','samples_A6.txt','samples_A6.txt' t 'Output oscilloscopio' w l lt 2