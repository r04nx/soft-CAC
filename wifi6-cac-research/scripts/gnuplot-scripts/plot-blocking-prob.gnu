set terminal pngcairo enhanced font 'Arial,12' size 800,600
set output 'blocking-probability.png'

set title "Blocking Probability by Traffic Type" font 'Arial,14'
set ylabel "Blocking Probability (%)"
set xlabel "Traffic Type"
set style fill solid 0.8 border -1
set boxwidth 0.6
set grid ytics
set yrange [0:110]

set xtics ("VoIP" 0, "Video" 1, "Bursty" 2, "Overall" 3)

plot '-' using 1:2 with boxes title "Blocking %" linecolor rgb "#e74c3c", \
     '-' using 1:2:3 with labels offset 0,1 notitle

0 0
1 66.67
2 100
3 50
e
0 0 "0%"
1 66.67 "66.7%"
2 100 "100%"
3 50 "50%"
e
