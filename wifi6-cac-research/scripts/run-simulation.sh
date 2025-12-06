#!/bin/bash
# Compilation and Execution Script for WiFi 6 CAC Simulation

echo "========================================"
echo "WiFi 6 CAC Simulation - Build & Run"
echo "========================================"

# Navigate to NS-3 directory
cd ../../ns-3 || exit 1

# Copy source files to scratch directory
echo "Copying source files to NS-3 scratch directory..."
cp ../wifi6-cac-research/src/wifi6-cac-airtime.h scratch/
cp ../wifi6-cac-research/src/wifi6-cac-airtime.cc scratch/
cp ../wifi6-cac-research/src/wifi6-cac-simulation.cc scratch/

# Build the simulation
echo "Building simulation..."
./ns3 build

if [ $? -ne 0 ]; then
    echo "Build failed! Please check for compilation errors."
    exit 1
fi

echo "Build successful!"
echo ""
echo "========================================"
echo "Running simulations with different client counts..."
echo "========================================"

# Create results directory
mkdir -p ../wifi6-cac-research/results
cd ../wifi6-cac-research/results || exit 1

# Run simulations for different client counts (25, 30, 35, 40, 45, 50)
for nStations in 25 30 35 40 45 50; do
    echo ""
    echo "Running simulation with $nStations clients..."
    
    # Calculate flows based on station count
    nVoip=$((nStations * 40 / 100))    # 40% VoIP
    nVideo=$((nStations * 30 / 100))   # 30% Video
    nBursty=$((nStations * 20 / 100))  # 20% Bursty
    nWeb=$((nStations * 10 / 100))     # 10% Web
    
    ../../ns-3/build/scratch/ns3-dev-wifi6-cac-simulation-default \
        --nStations=$nStations \
        --nVoipFlows=$nVoip \
        --nVideoFlows=$nVideo \
        --nBurstyFlows=$nBursty \
        --nWebFlows=$nWeb \
        --simTime=60 \
        --threshold=0.80 \
        --enableCac=1 \
        --channelWidth=80 \
        --outputPrefix=wifi6-cac-$nStations
    
    if [ $? -eq 0 ]; then
        echo "✓ Simulation with $nStations clients completed successfully"
    else
        echo "✗ Simulation with $nStations clients failed"
    fi
done

echo ""
echo "========================================"
echo "All simulations complete!"
echo "========================================"
echo ""
echo "Analyzing results..."

# Run analysis script
cd ..
python3 scripts/analyze-results.py results/wifi6-cac-25 results/wifi6-cac-30 results/wifi6-cac-35 \
                           results/wifi6-cac-40 results/wifi6-cac-45 results/wifi6-cac-50

echo ""
echo "========================================"
echo "Complete! Check the results directory for:"
echo "  - CSV data files"
echo "  - PNG graphs"
echo "  - summary_statistics.txt"
echo "========================================"
