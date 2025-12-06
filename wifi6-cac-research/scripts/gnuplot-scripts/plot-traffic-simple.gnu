set terminal pngcairo enhanced font 'Arial,12' size 1000,600
set output '../graphs/traffic-type-comparison.png'

set title "Traffic Type Performance - Admission vs Blocking" font 'Arial,14'
set ylabel "Number of Flows"
set xlabel "Traffic Type"
set style data histograms
set style histogram clustered gap 1
set style fill solid 0.8 border -1
set boxwidth 0.9
set grid ytics
set yrange [0:14]
set key outside right top

set xtics ("VoIP" 0, "Video" 1, "Bursty" 2)

plot '-' using 2:xtic(1) title "Requested" lc rgb "#95a5a6", \
     '-' using 2:xtic(1) title "Admitted" lc rgb "#2ecc71", \
     '-' using 2:xtic(1) title "Blocked" lc rgb "#e74c3c"

VoIP 12
Video 9
Bursty 9
e
VoIP 12
Video 3
Bursty 0
e
VoIP 0
Video 6
Bursty 9
e
