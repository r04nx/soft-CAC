set terminal pngcairo enhanced font 'Arial,12' size 1000,600
set output 'performance-metrics.png'

set multiplot layout 1,2 title "WiFi 6 CAC Performance Metrics" font 'Arial,16'

# First plot - Throughput
set title "Aggregate Throughput"
set ylabel "Throughput (Mbps)"
set xlabel "Metric"
set style fill solid 0.8 border -1
set boxwidth 0.5
set grid ytics
set yrange [0:10]
set xtics ("Aggregate" 0)

plot '-' using 1:2 with boxes title "Throughput" linecolor rgb "#2ecc71", \
     '-' using 1:2:3 with labels offset 0,0.5 notitle

0 8.316
e
0 8.316 "8.32 Mbps"
e

# Second plot - Delay
set title "Average End-to-End Delay"
set ylabel "Delay (ms)"
set xlabel "Metric"
set yrange [0:3]
set xtics ("Avg Delay" 0)

# Add VoIP threshold line
set arrow from -0.3,150 to 0.3,150 nohead lc rgb "#e74c3c" lw 2 dt 2
set label "VoIP Threshold\n(150ms)" at 0.15,2.5 right font 'Arial,9' tc rgb "#e74c3c"

plot '-' using 1:2 with boxes title "Delay" linecolor rgb "#3498db", \
     '-' using 1:2:3 with labels offset 0,0.15 notitle

0 1.47
e
0 1.47 "1.47 ms"
e

unset multiplot
