import matplotlib.pyplot as plt
import numpy as np

def plot_ascac_evolution():
    labels = ['Hard CAC\n(Legacy)', 'Soft CAC\n(Static)', 'AS-CAC+\n(Adaptive)']
    throughput = [28.12, 32.47, 33.51]
    delay = [1.48, 1.52, 1.58]
    utilization = [78.0, 87.6, 97.4]
    
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(18, 5))
    
    colors = ['#95a5a6', '#2ecc71', '#f39c12']
    
    # Throughput
    bars1 = ax1.bar(labels, throughput, color=colors, alpha=0.9, edgecolor='black', linewidth=1.5)
    ax1.set_title('Aggregate Throughput', fontsize=14, fontweight='bold')
    ax1.set_ylabel('Throughput (Mbps)', fontsize=12)
    ax1.grid(axis='y', linestyle='--', alpha=0.4)
    ax1.set_ylim(0, 40)
    for i, (bar, v) in enumerate(zip(bars1, throughput)):
        height = bar.get_height()
        ax1.text(bar.get_x() + bar.get_width()/2., height + 0.5,
                f'{v:.2f}',
                ha='center', va='bottom', fontweight='bold', fontsize=11)
        if i > 0:
            improvement = ((v - throughput[0]) / throughput[0]) * 100
            ax1.text(bar.get_x() + bar.get_width()/2., height/2,
                    f'+{improvement:.1f}%',
                    ha='center', va='center', fontweight='bold', fontsize=10,
                    color='white', bbox=dict(boxstyle='round', facecolor='black', alpha=0.7))
    
    # Delay
    bars2 = ax2.bar(labels, delay, color=colors, alpha=0.9, edgecolor='black', linewidth=1.5)
    ax2.set_title('Average End-to-End Delay', fontsize=14, fontweight='bold')
    ax2.set_ylabel('Delay (ms)', fontsize=12)
    ax2.grid(axis='y', linestyle='--', alpha=0.4)
    ax2.set_ylim(0, 2.5)
    ax2.axhline(y=2.0, color='r', linestyle='--', linewidth=2, label='VoIP Limit (2ms)', alpha=0.7)
    ax2.legend(loc='upper left', fontsize=9)
    for bar, v in zip(bars2, delay):
        height = bar.get_height()
        ax2.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                f'{v:.2f}ms',
                ha='center', va='bottom', fontweight='bold', fontsize=11)
    
    # Utilization
    bars3 = ax3.bar(labels, utilization, color=colors, alpha=0.9, edgecolor='black', linewidth=1.5)
    ax3.set_title('Channel Utilization', fontsize=14, fontweight='bold')
    ax3.set_ylabel('Utilization (%)', fontsize=12)
    ax3.grid(axis='y', linestyle='--', alpha=0.4)
    ax3.set_ylim(0, 105)
    for i, (bar, v) in enumerate(zip(bars3, utilization)):
        height = bar.get_height()
        ax3.text(bar.get_x() + bar.get_width()/2., height + 1,
                f'{v:.1f}%',
                ha='center', va='bottom', fontweight='bold', fontsize=11)
        if i > 0:
            improvement = ((v - utilization[0]) / utilization[0]) * 100
            ax3.text(bar.get_x() + bar.get_width()/2., height/2,
                    f'+{improvement:.1f}%',
                    ha='center', va='center', fontweight='bold', fontsize=10,
                    color='white', bbox=dict(boxstyle='round', facecolor='black', alpha=0.7))
    
    plt.suptitle('Performance Evolution: Hard CAC → Soft CAC → AS-CAC+ (Adaptive)',
                 fontsize=16, fontweight='bold', y=1.02)
    plt.tight_layout()
    plt.savefig('graphs/ascac_plus_comparison.png', dpi=300, bbox_inches='tight')
    print("✓ Graph saved: graphs/ascac_plus_comparison.png")
    
    # Print summary
    print("\n" + "="*80)
    print("AS-CAC+ PERFORMANCE SUMMARY")
    print("="*80)
    print(f"{'Metric':<25} | {'Hard CAC':<12} | {'Soft CAC':<12} | {'AS-CAC+':<12} | {'Gain':<12}")
    print("-"*80)
    print(f"{'Throughput (Mbps)':<25} | {throughput[0]:<12.2f} | {throughput[1]:<12.2f} | {throughput[2]:<12.2f} | {((throughput[2]-throughput[0])/throughput[0]*100):<12.1f}%")
    print(f"{'Delay (ms)':<25} | {delay[0]:<12.2f} | {delay[1]:<12.2f} | {delay[2]:<12.2f} | {'Within Spec':<12}")
    print(f"{'Utilization (%)':<25} | {utilization[0]:<12.1f} | {utilization[1]:<12.1f} | {utilization[2]:<12.1f} | {((utilization[2]-utilization[0])/utilization[0]*100):<12.1f}%")
    print("="*80)
    print("\n✓ AS-CAC+ achieves 19.2% throughput gain over Hard CAC")
    print("✓ AS-CAC+ pushes utilization to 97.4% while maintaining delay < 2ms")
    print("✓ Adaptive thresholds enabled admission of 1 additional bursty flow")

if __name__ == "__main__":
    plot_ascac_evolution()
