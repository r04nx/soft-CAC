import matplotlib.pyplot as plt
import numpy as np

def plot_combined_comparison():
    # Data
    scenarios = ['No CAC', 'Legacy (Hard) CAC', 'Soft CAC (Proposed)']
    
    # Metrics
    throughput = [36.5, 28.12, 32.47]  # Mbps
    delay = [45.2, 1.48, 1.52]        # ms (Log scale needed?)
    utilization = [98.5, 78.0, 87.6]  # %
    packet_loss = [12.4, 0.05, 0.08]  # % (Estimated/Synthetic for completeness if not in logs)
    
    # Setup Plot
    fig, ax1 = plt.subplots(figsize=(10, 6))
    
    # Bar Widths and Positions
    x = np.arange(len(scenarios))
    width = 0.25
    
    # Plot Throughput (Bar 1) - Left Axis
    rects1 = ax1.bar(x - width, throughput, width, label='Throughput (Mbps)', color='#2ecc71', alpha=0.8, edgecolor='black')
    
    # Plot Utilization (Bar 2) - Left Axis
    rects2 = ax1.bar(x, utilization, width, label='Channel Utilization (%)', color='#3498db', alpha=0.8, edgecolor='black')
    
    # Setup Right Axis for Delay (Different Scale)
    ax2 = ax1.twinx()
    # Plot Delay (Bar 3) - Right Axis
    rects3 = ax2.bar(x + width, delay, width, label='Avg Delay (ms)', color='#e74c3c', alpha=0.8, edgecolor='black')
    
    # Axis Labels
    ax1.set_xlabel('Admission Control Strategy', fontweight='bold', fontsize=12)
    ax1.set_ylabel('Throughput (Mbps) / Utilization (%)', fontweight='bold', fontsize=12)
    ax2.set_ylabel('Average Delay (ms)', fontweight='bold', fontsize=12, color='#c0392b')
    
    # Ticks
    ax1.set_xticks(x)
    ax1.set_xticklabels(scenarios, fontsize=11)
    ax1.set_ylim(0, 110) # Utilization goes up to 100
    
    # Log Scale for Delay? 
    # No CAC delay is 45ms, others are ~1.5ms. Linear scale might hide the small ones.
    # Let's use a broken axis or just linear but annotated. 
    # For a research paper, linear with values on top is often clearer if the gap is huge.
    ax2.set_ylim(0, 50)
    
    # Add Values on Top of Bars
    def autolabel(rects, ax, unit=""):
        for rect in rects:
            height = rect.get_height()
            ax.annotate(f'{height:.1f}{unit}',
                        xy=(rect.get_x() + rect.get_width() / 2, height),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom', fontsize=9, fontweight='bold')

    autolabel(rects1, ax1)
    autolabel(rects2, ax1, "%")
    autolabel(rects3, ax2, "ms")
    
    # Legend
    # Combine legends from both axes
    lines1, labels1 = ax1.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    ax1.legend(lines1 + lines2, labels1 + labels2, loc='upper center', bbox_to_anchor=(0.5, 1.15), ncol=3, fontsize=10)
    
    # Title and Grid
    plt.title('Performance Comparison: No CAC vs. Legacy CAC vs. Soft CAC', y=1.15, fontsize=14, fontweight='bold')
    ax1.grid(axis='y', linestyle='--', alpha=0.3)
    
    # Save
    plt.tight_layout()
    plt.savefig('graphs/comprehensive_comparison.png', dpi=300)
    print("Graph saved to graphs/comprehensive_comparison.png")

if __name__ == "__main__":
    plot_combined_comparison()
