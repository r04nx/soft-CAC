set terminal pngcairo enhanced font 'Arial,12' size 800,600
set output 'admission-results.png'

set title "WiFi 6 CAC - Flow Admission by Traffic Type" font 'Arial,14'
set ylabel "Number of Flows"
set xlabel "Traffic Type"
set style data histograms
set style histogram clustered gap 1
set style fill solid 0.8 border -1
set boxwidth 0.9
set grid ytics
set yrange [0:14]

set xtics ("VoIP" 0, "Video" 1, "Bursty" 2)

plot '-' using 2:xtic(1) title "Admitted" linecolor rgb "#2ecc71", \
     '-' using 2:xtic(1) title "Blocked" linecolor rgb "#e74c3c"

VoIP 12
Video 3
Bursty 0
e
VoIP 0
Video 6
Bursty 9
e
