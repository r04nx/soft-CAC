set terminal pngcairo enhanced font 'Arial,12' size 800,600
set output 'airtime-utilization.png'

set title "Airtime Utilization by Traffic Type" font 'Arial,14'
set ylabel "Airtime Fraction"
set xlabel "Traffic Type"
set style data histograms
set style histogram rowstacked
set style fill solid 0.8 border -1
set boxwidth 0.6
set grid ytics
set yrange [0:0.85]

# Add threshold line
set arrow from -0.5,0.80 to 2.5,0.80 nohead lc rgb "#e74c3c" lw 2 dt 2

set label "CAC Threshold (80%)" at 1.5,0.82 center font 'Arial,10' tc rgb "#e74c3c"

set xtics ("Total Airtime" 0)

plot '-' using 2:xtic(1) title "VoIP" linecolor rgb "#3498db", \
     '-' using 2:xtic(1) title "Video" linecolor rgb "#9b59b6", \
     '-' using 2:xtic(1) title "Bursty" linecolor rgb "#95a5a6"

"Total" 0.1404
e
"Total" 0.6094
e
"Total" 0.0000
e
