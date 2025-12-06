import numpy as np
import matplotlib.pyplot as plt
import math

def erlang_b(A, N):
    """
    Calculate blocking probability using Erlang-B formula.
    A: Offered Traffic (Erlangs) = Arrival Rate * Average Holding Time
    N: Number of Servers (Channels/Airtime Units)
    """
    numerator = (A ** N) / math.factorial(N)
    denominator = sum([(A ** i) / math.factorial(i) for i in range(N + 1)])
    return numerator / denominator

def calculate_airtime_blocking(offered_load_flows, capacity_threshold=0.80, avg_flow_airtime=0.05):
    """
    Map Airtime CAC to Erlang Loss Model.
    
    In Airtime CAC:
    - "Servers" (N) can be modeled as the number of flows that fit into the Threshold.
    - N = Capacity_Threshold / Avg_Flow_Airtime
    - Offered Load (A) = Number of Flows attempting to enter
    """
    
    # Effective number of "channels" available in the airtime
    # e.g., if Threshold is 0.80 and each flow takes 0.05 (5%), then N = 16 flows can fit.
    N = int(capacity_threshold / avg_flow_airtime)
    
    blocking_probs = []
    for flows in offered_load_flows:
        # In this simplified mapping, Offered Load A is proportional to active flows
        # assuming they are all trying to be active at once (saturation).
        # For a more dynamic model, A = lambda / mu. 
        # Here we treat 'flows' as the offered load in Erlangs for worst-case.
        A = flows 
        
        pb = erlang_b(A, N)
        blocking_probs.append(pb * 100) # Convert to percentage
        
    return blocking_probs

# Simulation Parameters matching our NS-3 setup
offered_flows = np.arange(1, 51, 1) # 1 to 50 flows
threshold = 0.80

# Scenario 1: VoIP-like flows (Low airtime, ~1-2%)
# VoIP (64kbps) takes very little airtime, maybe 0.012 (1.2%)
pb_voip = calculate_airtime_blocking(offered_flows, threshold, 0.012)

# Scenario 2: Video-like flows (High airtime, ~20%)
# Video (3Mbps) takes significant airtime, maybe 0.20 (20%)
pb_video = calculate_airtime_blocking(offered_flows, threshold, 0.20)

# Scenario 3: Mixed Average (e.g., ~5% avg)
pb_mixed = calculate_airtime_blocking(offered_flows, threshold, 0.05)

# Plotting
plt.figure(figsize=(10, 6))
plt.plot(offered_flows, pb_voip, label='VoIP Only (Small Airtime)', linestyle='--', alpha=0.7)
plt.plot(offered_flows, pb_video, label='Video Only (Large Airtime)', linestyle='--', alpha=0.7)
plt.plot(offered_flows, pb_mixed, label='Mixed Traffic (Avg)', linewidth=3, color='#e74c3c')

plt.title('Analytical Model: Blocking Probability vs Offered Load (Erlang-B)')
plt.xlabel('Offered Load (Number of Flows)')
plt.ylabel('Blocking Probability (%)')
plt.grid(True, linestyle='--', alpha=0.7)
plt.legend()
plt.axhline(y=0, color='k', linestyle='-', alpha=0.3)
plt.ylim(-5, 105)

plt.savefig('graphs/analytical_model_blocking.png', dpi=300)
print("Analytical model graph saved to graphs/analytical_model_blocking.png")
