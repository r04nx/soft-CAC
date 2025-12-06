set terminal pngcairo enhanced font 'Arial,12' size 1000,600
set output '../graphs/qos-effectiveness.png'

set title "QoS Prioritization Effectiveness" font 'Arial,14'
set ylabel "Admission Rate (%)"
set xlabel "Traffic Priority Level"
set style fill solid 0.8 border -1
set boxwidth 0.6
set grid ytics
set yrange [0:110]

set xtics ("Highest\n(VoIP)" 1, "Medium\n(Video)" 2, "Lowest\n(Bursty)" 3)

# Color gradient from green to red
plot '-' using 1:2:3 with boxes lc rgb variable title "Admission Rate", \
     '-' using 1:2:3 with labels offset 0,5 font 'Arial,12' notitle

1 100 0x2ecc71
2 33.3 0xf39c12
3 0 0xe74c3c
e
1 100 "100%"
2 33.3 "33%"
3 0 "0%"
e
