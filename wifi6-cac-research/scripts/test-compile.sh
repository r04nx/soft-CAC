#!/bin/bash
# Quick compilation test for WiFi 6 CAC Simulation

echo "Testing NS-3 compilation with WiFi 6 CAC simulation..."

cd ../ns-3 || exit 1

# Copy only the main simulation file
cp ../wifi6-cac-research/wifi6-cac-simulation-standalone.cc scratch/ 2>/dev/null || \
cp ../wifi6-cac-research/wifi6-cac-simulation.cc scratch/wifi6-cac-sim.cc

echo "Configuring NS-3..."
./ns3 configure --enable-examples 2>&1 | tail -20

if [ $? -eq 0 ]; then
    echo "✓ Configuration successful"
    echo "Building simulation..."
    ./ns3 build 2>&1 | tail -30
    
    if [ $? -eq 0 ]; then
        echo "✓ Build successful!"
        echo ""
        echo "You can now run the simulation with:"
        echo "./ns3 run wifi6-cac-sim"
    else
        echo "✗ Build failed. Check errors above."
    fi
else
    echo "✗ Configuration failed. Check errors above."
fi
