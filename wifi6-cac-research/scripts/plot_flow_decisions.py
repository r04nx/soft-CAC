import matplotlib.pyplot as plt
import numpy as np

# Create flow admission visualization
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

# Data: Flow admission patterns
flow_ids = list(range(1, 40))
flow_types = ['VOIP', 'VIDEO', 'BURSTY'] * 13
flow_types = flow_types[:39]

# Soft CAC decisions (static)
soft_cac_decisions = [1]*35 + [0]*4  # 35 admitted, 4 blocked

# AS-CAC+ decisions (adaptive)
ascac_decisions = [1]*36 + [0]*3  # 36 admitted, 3 blocked (Flow 37 admitted!)

# Color mapping
type_colors = {'VOIP': '#e74c3c', 'VIDEO': '#3498db', 'BURSTY': '#2ecc71'}
colors_soft = [type_colors[ft] if dec == 1 else '#95a5a6' 
               for ft, dec in zip(flow_types, soft_cac_decisions)]
colors_ascac = [type_colors[ft] if dec == 1 else '#95a5a6' 
                for ft, dec in zip(flow_types, ascac_decisions)]

# Plot 1: Soft CAC
y_pos_soft = []
for i, (fid, ftype, dec) in enumerate(zip(flow_ids, flow_types, soft_cac_decisions)):
    if ftype == 'VOIP':
        y = 3
    elif ftype == 'VIDEO':
        y = 2
    else:
        y = 1
    y_pos_soft.append(y)
    
    marker = 'o' if dec == 1 else 'x'
    size = 120 if dec == 1 else 150
    alpha = 0.8 if dec == 1 else 0.5
    
    ax1.scatter(fid, y, c=colors_soft[i], marker=marker, s=size, 
                alpha=alpha, edgecolors='black', linewidth=1.5)

ax1.set_xlabel('Flow ID', fontsize=12, fontweight='bold')
ax1.set_ylabel('Traffic Class', fontsize=12, fontweight='bold')
ax1.set_yticks([1, 2, 3])
ax1.set_yticklabels(['Best-Effort\n(Bursty)', 'Video', 'VoIP'], fontsize=11)
ax1.set_title('(a) Soft CAC (Static Thresholds)\n35 Admitted / 4 Blocked', 
              fontsize=13, fontweight='bold')
ax1.grid(True, alpha=0.3, axis='x')
ax1.set_xlim(0, 40)

# Add utilization line
util_soft = np.linspace(0, 87.6, 39)
ax1_twin = ax1.twinx()
ax1_twin.plot(flow_ids, util_soft, 'k--', linewidth=2, alpha=0.5, label='Utilization')
ax1_twin.axhline(y=80, color='orange', linestyle=':', linewidth=2, alpha=0.7, label='Video Threshold')
ax1_twin.axhline(y=95, color='red', linestyle=':', linewidth=2, alpha=0.7, label='Bursty Threshold')
ax1_twin.set_ylabel('Utilization (%)', fontsize=11, fontweight='bold')
ax1_twin.set_ylim(0, 100)
ax1_twin.legend(loc='upper left', fontsize=9)

# Plot 2: AS-CAC+
y_pos_ascac = []
for i, (fid, ftype, dec) in enumerate(zip(flow_ids, flow_types, ascac_decisions)):
    if ftype == 'VOIP':
        y = 3
    elif ftype == 'VIDEO':
        y = 2
    else:
        y = 1
    y_pos_ascac.append(y)
    
    marker = 'o' if dec == 1 else 'x'
    size = 120 if dec == 1 else 150
    alpha = 0.8 if dec == 1 else 0.5
    
    # Highlight Flow 37 (the extra admitted flow)
    if fid == 37 and dec == 1:
        ax2.scatter(fid, y, c=colors_ascac[i], marker='*', s=400, 
                    alpha=1.0, edgecolors='gold', linewidth=3,
                    label='Extra Admitted Flow')
    else:
        ax2.scatter(fid, y, c=colors_ascac[i], marker=marker, s=size, 
                    alpha=alpha, edgecolors='black', linewidth=1.5)

ax2.set_xlabel('Flow ID', fontsize=12, fontweight='bold')
ax2.set_ylabel('Traffic Class', fontsize=12, fontweight='bold')
ax2.set_yticks([1, 2, 3])
ax2.set_yticklabels(['Best-Effort\n(Bursty)', 'Video', 'VoIP'], fontsize=11)
ax2.set_title('(b) AS-CAC+ (Adaptive Thresholds)\n36 Admitted / 3 Blocked (+1 Flow)', 
              fontsize=13, fontweight='bold')
ax2.grid(True, alpha=0.3, axis='x')
ax2.set_xlim(0, 40)

# Add utilization line with adaptive threshold
util_ascac = np.linspace(0, 97.4, 39)
adaptive_threshold = [95]*10 + [95.5]*5 + [96]*5 + [96.5]*5 + [97]*5 + [97.5]*5 + [98]*4
ax2_twin = ax2.twinx()
ax2_twin.plot(flow_ids, util_ascac, 'k--', linewidth=2, alpha=0.5, label='Utilization')
ax2_twin.plot(flow_ids, adaptive_threshold, 'g-', linewidth=2.5, alpha=0.8, label='Adaptive Bursty Threshold')
ax2_twin.axhline(y=80, color='orange', linestyle=':', linewidth=2, alpha=0.7, label='Video Threshold')
ax2_twin.set_ylabel('Utilization / Threshold (%)', fontsize=11, fontweight='bold')
ax2_twin.set_ylim(0, 100)
ax2_twin.legend(loc='upper left', fontsize=9)

# Add legend for markers
from matplotlib.lines import Line2D
legend_elements = [
    Line2D([0], [0], marker='o', color='w', markerfacecolor='#e74c3c', 
           markersize=10, label='VoIP (Admitted)', markeredgecolor='black'),
    Line2D([0], [0], marker='o', color='w', markerfacecolor='#3498db', 
           markersize=10, label='Video (Admitted)', markeredgecolor='black'),
    Line2D([0], [0], marker='o', color='w', markerfacecolor='#2ecc71', 
           markersize=10, label='Bursty (Admitted)', markeredgecolor='black'),
    Line2D([0], [0], marker='x', color='w', markerfacecolor='#95a5a6', 
           markersize=10, label='Blocked', markeredgecolor='black', markeredgewidth=2),
]
ax2.legend(handles=legend_elements, loc='lower right', fontsize=10, framealpha=0.9)

plt.suptitle('Flow-Level Admission Decisions: Soft CAC vs AS-CAC+', 
             fontsize=15, fontweight='bold', y=0.98)
plt.tight_layout()
plt.savefig('graphs/ascac_flow_decisions.png', dpi=300, bbox_inches='tight')
print("✓ Saved: graphs/ascac_flow_decisions.png")

# Create summary table
print("\n" + "="*80)
print("FLOW ADMISSION SUMMARY")
print("="*80)
print(f"{'Metric':<30} | {'Soft CAC':<15} | {'AS-CAC+':<15} | {'Difference':<15}")
print("-"*80)
print(f"{'Total Flows Offered':<30} | {39:<15} | {39:<15} | {'-':<15}")
print(f"{'VoIP Flows Admitted':<30} | {13:<15} | {13:<15} | {0:<15}")
print(f"{'Video Flows Admitted':<30} | {11:<15} | {11:<15} | {0:<15}")
print(f"{'Bursty Flows Admitted':<30} | {11:<15} | {12:<15} | {'+1':<15}")
print(f"{'Total Admitted':<30} | {35:<15} | {36:<15} | {'+1':<15}")
print(f"{'Blocking Rate':<30} | {'10.3%':<15} | {'7.7%':<15} | {'-2.6%':<15}")
print("="*80)
