set terminal pngcairo enhanced font 'Arial,12' size 1000,600
set output '../graphs/admission-timeline.png'

set title "Flow Admission Timeline - CAC Decision Process" font 'Arial,14'
set ylabel "Cumulative Airtime Utilization"
set xlabel "Flow Request Number"
set grid
set yrange [0:1]
set xrange [0:31]

# Add threshold line
set arrow from 0,0.80 to 31,0.80 nohead lc rgb "#e74c3c" lw 2 dt 2
set label "CAC Threshold (80%)" at 16,0.83 center font 'Arial,10' tc rgb "#e74c3c"

# Admission zone
set object 1 rect from 0,0 to 31,0.80 fc rgb "#d5f4e6" fs transparent solid 0.3 noborder
set label "Admission Zone" at 2,0.75 font 'Arial,9' tc rgb "#27ae60"

# Rejection zone  
set object 2 rect from 0,0.80 to 31,1.0 fc rgb "#fadbd8" fs transparent solid 0.3 noborder
set label "Rejection Zone" at 2,0.90 font 'Arial,9' tc rgb "#c0392b"

plot '-' using 1:2 with linespoints lw 2 pt 7 ps 0.8 lc rgb "#3498db" title "Airtime Utilization", \
     '-' using 1:2:3 with labels offset 0,1 font 'Arial,8' notitle

1 0.0117
2 0.0234
3 0.0351
4 0.0468
5 0.0585
6 0.0702
7 0.0819
8 0.0936
9 0.1053
10 0.117
11 0.1287
12 0.1404
13 0.343525
14 0.54665
15 0.749775
16 0.749775
17 0.749775
18 0.749775
19 0.749775
20 0.749775
21 0.749775
22 0.749775
23 0.749775
24 0.749775
25 0.749775
26 0.749775
27 0.749775
28 0.749775
29 0.749775
30 0.749775
e
12 0.1404 "VoIP"
15 0.749775 "Video"
16 0.749775 "Blocked"
e
