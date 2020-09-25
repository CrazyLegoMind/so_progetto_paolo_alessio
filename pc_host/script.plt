#cambia etichette degli assi
set xlabel "times (s)"
set ylabel "values (V)"

#grafica i dati contenuti nel file
#pl [:][:] 'samples.txt' t 'Output oscilloscopio' w l lt 1
pl [:][:] 'samples_A0.txt' t 'Output pin A0' w l lt 2
pl [:][:] 'samples_A1.txt' t 'Output pin A1' w l lt 2
pl [:][:] 'samples_A2.txt' t 'Output pin A2' w l lt 2
pl [:][:] 'samples_A3.txt' t 'Output pin A3' w l lt 2
pl [:][:] 'samples_A4.txt' t 'Output pin A4' w l lt 2
pl [:][:] 'samples_A5.txt' t 'Output pin A5' w l lt 2
pl [:][:] 'samples_A6.txt' t 'Output pin A6' w l lt 2
pl [:][:] 'samples_A7.txt' t 'Output pin A7' w l lt 2