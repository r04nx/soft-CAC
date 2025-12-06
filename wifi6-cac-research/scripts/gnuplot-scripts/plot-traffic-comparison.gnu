set terminal pngcairo enhanced font 'Arial,12' size 1000,600
set output '../graphs/traffic-type-comparison.png'

set title "Traffic Type Performance Comparison" font 'Arial,14'
set style data histogram
set style histogram cluster gap 1
set style fill solid 0.8 border -1
set boxwidth 0.9
set grid ytics
set yrange [0:*]
set y2range [0:*]
set y2tics
set ylabel "Count / Airtime"
set y2label "Percentage (%)"
set xlabel "Traffic Type"

set key outside right top

set xtics ("VoIP" 0, "Video" 1, "Bursty" 2)

plot '-' using 2:xtic(1) axes x1y1 title "Requested" lc rgb "#95a5a6", \
     '-' using 2:xtic(1) axes x1y1 title "Admitted" lc rgb "#2ecc71", \
     '-' using 2:xtic(1) axes x1y1 title "Blocked" lc rgb "#e74c3c", \
     '-' using ($2*100):xtic(1) axes x1y2 with linespoints lw 2 pt 7 ps 1.2 title "Admission Rate (%)" lc rgb "#3498db"

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
VoIP 1.0
Video 0.333
Bursty 0.0
e
