import matplotlib.pyplot as plt
import re
import os
import numpy as np

def parse_log_file(filename):
    metrics = {
        'throughput': 0.0,
        'delay': 0.0,
        'utilization_ap1': 0.0
    }
    
    if not os.path.exists(filename):
        print(f"Warning: {filename} not found.")
        return metrics

    with open(filename, 'r') as f:
        content = f.read()
        
        # Extract Throughput
        t_match = re.search(r"Total Throughput:\s+([\d\.]+)\s+Mbps", content)
        if t_match: metrics['throughput'] = float(t_match.group(1))
        
        # Extract Delay
        d_match = re.search(r"Avg Delay:\s+([\d\.]+)\s+s", content)
        if d_match: metrics['delay'] = float(d_match.group(1)) * 1000.0 # Convert to ms
        
        # Extract Utilization
        u1_match = re.search(r"AP1 Utilization:\s+([\d\.]+)", content)
        if u1_match: metrics['utilization_ap1'] = float(u1_match.group(1))
        
    return metrics

def plot_ascac_comparison(hard_metrics, soft_metrics, ascac_metrics):
    labels = ['Hard CAC', 'Soft CAC', 'AS-CAC+ (Adaptive)']
    throughput = [hard_metrics['throughput'], soft_metrics['throughput'], ascac_metrics['throughput']]
    delay = [hard_metrics['delay'], soft_metrics['delay'], ascac_metrics['delay']]
    utilization = [hard_metrics['utilization_ap1'] * 100, soft_metrics['utilization_ap1'] * 100, ascac_metrics['utilization_ap1'] * 100]
    
    fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(18, 5))
    
    # Colors: Grey (Legacy), Green (Soft), Gold (Adaptive)
    colors = ['#95a5a6', '#2ecc71', '#f1c40f']
    
    # Throughput Plot
    ax1.bar(labels, throughput, color=colors, alpha=0.9, edgecolor='black')
    ax1.set_title('Aggregate Throughput')
    ax1.set_ylabel('Mbps')
    ax1.grid(axis='y', linestyle='--', alpha=0.5)
    for i, v in enumerate(throughput):
        ax1.text(i, v + 0.5, f"{v:.2f}", ha='center', fontweight='bold')
        
    # Delay Plot
    ax2.bar(labels, delay, color=colors, alpha=0.9, edgecolor='black')
    ax2.set_title('Average End-to-End Delay')
    ax2.set_ylabel('Delay (ms)')
    ax2.grid(axis='y', linestyle='--', alpha=0.5)
    for i, v in enumerate(delay):
        ax2.text(i, v + 0.05, f"{v:.2f}", ha='center', fontweight='bold')
        
    # Utilization Plot
    ax3.bar(labels, utilization, color=colors, alpha=0.9, edgecolor='black')
    ax3.set_title('AP Utilization')
    ax3.set_ylabel('Utilization (%)')
    ax3.grid(axis='y', linestyle='--', alpha=0.5)
    ax3.set_ylim(0, 105)
    for i, v in enumerate(utilization):
        ax3.text(i, v + 1, f"{v:.1f}%", ha='center', fontweight='bold')
        
    plt.suptitle('Performance Evolution: Legacy -> Soft -> Adaptive (AS-CAC+)')
    plt.tight_layout()
    plt.savefig('graphs/ascac_plus_comparison.png', dpi=300)
    print("Graph saved to graphs/ascac_plus_comparison.png")

def main():
    # Parse Results
    # Note: We reuse the logs from previous steps for Hard/Soft to ensure fair comparison
    hard_metrics = parse_log_file('wifi6-multi-ap-hard-cac-cci.log')
    soft_metrics = parse_log_file('wifi6-multi-ap-soft-cac-cci.log')
    ascac_metrics = parse_log_file('wifi6-multi-ap-ascac-plus-cci.log')
    
    # Fallback for demo if logs are still generating
    if ascac_metrics['throughput'] == 0:
        print("Waiting for AS-CAC+ simulation...")
        return

    plot_ascac_comparison(hard_metrics, soft_metrics, ascac_metrics)
    
    # Print Summary
    print("\n=== AS-CAC+ Performance Evolution ===")
    print(f"{'Metric':<20} | {'Hard CAC':<10} | {'Soft CAC':<10} | {'AS-CAC+':<10} | {'Gain (vs Hard)':<15}")
    print("-" * 80)
    print(f"{'Throughput (Mbps)':<20} | {hard_metrics['throughput']:<10.2f} | {soft_metrics['throughput']:<10.2f} | {ascac_metrics['throughput']:<10.2f} | {((ascac_metrics['throughput']-hard_metrics['throughput'])/hard_metrics['throughput'])*100:<15.1f}%")
    print(f"{'Delay (ms)':<20} | {hard_metrics['delay']:<10.2f} | {soft_metrics['delay']:<10.2f} | {ascac_metrics['delay']:<10.2f} | -")
    print(f"{'Utilization (%)':<20} | {hard_metrics['utilization_ap1']*100:<10.1f} | {soft_metrics['utilization_ap1']*100:<10.1f} | {ascac_metrics['utilization_ap1']*100:<10.1f} | {((ascac_metrics['utilization_ap1']-hard_metrics['utilization_ap1'])/hard_metrics['utilization_ap1'])*100:<15.1f}%")

if __name__ == "__main__":
    main()
