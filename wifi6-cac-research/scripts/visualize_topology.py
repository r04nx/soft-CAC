import matplotlib.pyplot as plt
import numpy as np
import matplotlib.patches as patches

# Simulation parameters
n_stations = 30
n_voip = int(n_stations * 0.4)   # 12
n_video = int(n_stations * 0.3)  # 9
n_bursty = n_stations - n_voip - n_video # 9
radius = 15.0

# Setup plot
fig, ax = plt.subplots(figsize=(10, 10))
ax.set_xlim(-20, 20)
ax.set_ylim(-20, 20)
ax.set_aspect('equal')
ax.grid(True, linestyle='--', alpha=0.3)
ax.set_title("NS-3 Network Topology (WiFi 6 Dense WLAN)", fontsize=16)
ax.set_xlabel("Distance (m)")
ax.set_ylabel("Distance (m)")

# Draw coverage area
circle = plt.Circle((0, 0), radius, color='blue', alpha=0.05, label='Coverage Area')
ax.add_artist(circle)
ax.plot([0], [0], 'b^', markersize=15, label='Access Point (WiFi 6)', zorder=10)

# Generate random positions for stations (Random Disc)
# We use a fixed seed for reproducibility
np.random.seed(42)

def get_random_pos(n):
    theta = np.random.uniform(0, 2*np.pi, n)
    r = np.sqrt(np.random.uniform(0, radius**2, n))
    x = r * np.cos(theta)
    y = r * np.sin(theta)
    return x, y

# VoIP Nodes (Green)
vx, vy = get_random_pos(n_voip)
ax.scatter(vx, vy, c='#2ecc71', s=100, marker='o', label='VoIP Clients (High Prio)', edgecolors='black', zorder=5)
for i in range(n_voip):
    ax.annotate(f"V{i+1}", (vx[i], vy[i]), xytext=(5, 5), textcoords='offset points', fontsize=8)

# Video Nodes (Orange)
vidx, vidy = get_random_pos(n_video)
ax.scatter(vidx, vidy, c='#f39c12', s=100, marker='s', label='Video Clients (Med Prio)', edgecolors='black', zorder=5)
for i in range(n_video):
    ax.annotate(f"S{i+1}", (vidx[i], vidy[i]), xytext=(5, 5), textcoords='offset points', fontsize=8)

# Bursty Nodes (Grey)
bx, by = get_random_pos(n_bursty)
ax.scatter(bx, by, c='#95a5a6', s=100, marker='D', label='Bursty Clients (Low Prio)', edgecolors='black', zorder=5)
for i in range(n_bursty):
    ax.annotate(f"B{i+1}", (bx[i], by[i]), xytext=(5, 5), textcoords='offset points', fontsize=8)

# Add legend
plt.legend(loc='upper right', framealpha=0.9)

# Add annotations for context
plt.text(-19, -19, f"Total Clients: {n_stations}\nArea Radius: {radius}m\nStandard: 802.11ax", 
         bbox=dict(facecolor='white', alpha=0.8, edgecolor='gray'))

plt.tight_layout()
plt.savefig('graphs/network_topology_viz.png', dpi=300)
print("Topology visualization saved to graphs/network_topology_viz.png")
