#!/usr/bin/env python3
"""
WiFi 6 CAC Simulation Results Analysis and Visualization
Generates publication-quality graphs for research paper
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import glob
import os
import sys

# Set publication-quality plot parameters
plt.rcParams['figure.figsize'] = (10, 6)
plt.rcParams['font.size'] = 12
plt.rcParams['axes.labelsize'] = 14
plt.rcParams['axes.titlesize'] = 16
plt.rcParams['xtick.labelsize'] = 12
plt.rcParams['ytick.labelsize'] = 12
plt.rcParams['legend.fontsize'] = 11
plt.rcParams['figure.dpi'] = 300
sns.set_style("whitegrid")

# Traffic type mapping
TRAFFIC_TYPES = {
    0: 'VoIP',
    1: 'Video Streaming',
    2: 'Bursty',
    3: 'Web Browsing'
}

TRAFFIC_COLORS = {
    'VoIP': '#e74c3c',
    'Video Streaming': '#3498db',
    'Bursty': '#2ecc71',
    'Web Browsing': '#f39c12'
}

def load_simulation_data(prefix):
    """Load all simulation output files"""
    data = {}
    
    try:
        # Load delay data
        delay_file = f"{prefix}-delay.csv"
        if os.path.exists(delay_file):
            data['delay'] = pd.read_csv(delay_file)
            print(f"Loaded {len(data['delay'])} delay records")
        
        # Load admission data
        admission_file = f"{prefix}-admission.csv"
        if os.path.exists(admission_file):
            data['admission'] = pd.read_csv(admission_file)
            print(f"Loaded {len(data['admission'])} admission records")
        
        # Load FlowMonitor data
        flowmon_file = f"{prefix}-flowmon.csv"
        if os.path.exists(flowmon_file):
            data['flowmon'] = pd.read_csv(flowmon_file)
            print(f"Loaded {len(data['flowmon'])} flow records")
        
        return data
    except Exception as e:
        print(f"Error loading data: {e}")
        return None

def plot_aggregate_throughput_vs_flows(data_list, output_file='throughput_vs_flows.png'):
    """
    Plot 1: Aggregate Throughput vs Number of Offered Flows
    Shows how total network throughput changes with increasing number of flows
    """
    plt.figure(figsize=(12, 7))
    
    for data_info in data_list:
        data = data_info['data']
        label = data_info['label']
        n_stations = data_info['n_stations']
        
        if 'flowmon' in data:
            flowmon = data['flowmon']
            total_throughput = flowmon['Throughput'].sum()
            n_flows = len(flowmon)
            
            plt.scatter(n_flows, total_throughput, s=150, label=f"{label} ({n_stations} clients)",
                       alpha=0.7, edgecolors='black', linewidth=1.5)
    
    plt.xlabel('Number of Offered Flows', fontweight='bold')
    plt.ylabel('Aggregate Throughput (Mbps)', fontweight='bold')
    plt.title('Aggregate Throughput vs Number of Offered Flows\nWiFi 6 Dense WLAN with Airtime-Based CAC',
              fontweight='bold', pad=20)
    plt.legend(loc='best', framealpha=0.9)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Saved: {output_file}")
    plt.close()

def plot_delay_vs_flows(data_list, output_file='delay_vs_flows.png'):
    """
    Plot 2: Average Delay vs Number of Offered Flows
    Separate curves for each traffic type
    """
    fig, ax = plt.subplots(figsize=(12, 7))
    
    # Collect data by traffic type
    traffic_data = {ttype: {'flows': [], 'delays': []} for ttype in TRAFFIC_TYPES.values()}
    
    for data_info in data_list:
        data = data_info['data']
        n_flows = data_info.get('total_flows', 0)
        
        if 'delay' in data:
            delay_df = data['delay']
            
            for traffic_id, traffic_name in TRAFFIC_TYPES.items():
                traffic_delays = delay_df[delay_df['TrafficType'] == traffic_id]['Delay']
                if len(traffic_delays) > 0:
                    avg_delay = traffic_delays.mean()
                    traffic_data[traffic_name]['flows'].append(n_flows)
                    traffic_data[traffic_name]['delays'].append(avg_delay)
    
    # Plot each traffic type
    for traffic_name, values in traffic_data.items():
        if len(values['flows']) > 0:
            # Sort by number of flows
            sorted_indices = np.argsort(values['flows'])
            flows = np.array(values['flows'])[sorted_indices]
            delays = np.array(values['delays'])[sorted_indices]
            
            ax.plot(flows, delays, marker='o', linewidth=2.5, markersize=8,
                   label=traffic_name, color=TRAFFIC_COLORS.get(traffic_name, 'gray'),
                   alpha=0.8)
    
    # Add VoIP delay threshold line
    ax.axhline(y=150, color='red', linestyle='--', linewidth=2, alpha=0.7,
              label='VoIP Delay Threshold (150ms)')
    
    ax.set_xlabel('Number of Offered Flows', fontweight='bold')
    ax.set_ylabel('Average End-to-End Delay (ms)', fontweight='bold')
    ax.set_title('Average Delay vs Number of Offered Flows by Traffic Type\nWiFi 6 with Airtime-Based CAC',
                fontweight='bold', pad=20)
    ax.legend(loc='best', framealpha=0.9)
    ax.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Saved: {output_file}")
    plt.close()

def plot_blocking_probability(data_list, output_file='blocking_probability.png'):
    """
    Plot 3: Blocking Probability vs Network Load
    Shows admission control effectiveness
    """
    plt.figure(figsize=(12, 7))
    
    loads = []
    blocking_probs = []
    labels = []
    
    for data_info in data_list:
        data = data_info['data']
        label = data_info['label']
        
        if 'admission' in data:
            admission = data['admission']
            total_requests = len(admission)
            blocked = len(admission[admission['Admitted'] == 0])
            blocking_prob = blocked / total_requests if total_requests > 0 else 0
            
            # Calculate offered load (sum of required airtimes)
            offered_load = admission['RequiredAirtime'].sum()
            
            loads.append(offered_load)
            blocking_probs.append(blocking_prob)
            labels.append(label)
    
    # Sort by load
    sorted_indices = np.argsort(loads)
    loads = np.array(loads)[sorted_indices]
    blocking_probs = np.array(blocking_probs)[sorted_indices]
    sorted_labels = [labels[i] for i in sorted_indices]
    
    plt.plot(loads, blocking_probs, marker='s', linewidth=3, markersize=10,
            color='#e74c3c', alpha=0.8, label='With CAC')
    
    # Add markers for each point
    for i, (load, prob, lbl) in enumerate(zip(loads, blocking_probs, sorted_labels)):
        plt.annotate(f'{prob:.2%}', (load, prob), textcoords="offset points",
                    xytext=(0,10), ha='center', fontsize=10)
    
    plt.xlabel('Offered Load (Airtime Utilization)', fontweight='bold')
    plt.ylabel('Blocking Probability', fontweight='bold')
    plt.title('Blocking Probability vs Offered Load\nAirtime-Based CAC in Dense WiFi 6 WLAN',
             fontweight='bold', pad=20)
    plt.legend(loc='best', framealpha=0.9)
    plt.grid(True, alpha=0.3)
    plt.ylim(-0.05, 1.05)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Saved: {output_file}")
    plt.close()

def plot_voip_delay_cdf(data_list, output_file='voip_delay_cdf.png'):
    """
    Plot 4: CDF of VoIP End-to-End Delay
    Shows delay distribution for VoIP traffic
    """
    fig, ax = plt.subplots(figsize=(12, 7))
    
    for data_info in data_list:
        data = data_info['data']
        label = data_info['label']
        
        if 'delay' in data:
            delay_df = data['delay']
            voip_delays = delay_df[delay_df['TrafficType'] == 0]['Delay'].values
            
            if len(voip_delays) > 0:
                # Calculate CDF
                sorted_delays = np.sort(voip_delays)
                cdf = np.arange(1, len(sorted_delays) + 1) / len(sorted_delays)
                
                ax.plot(sorted_delays, cdf, linewidth=2.5, label=label, alpha=0.8)
    
    # Add 150ms threshold line
    ax.axvline(x=150, color='red', linestyle='--', linewidth=2, alpha=0.7,
              label='150ms Threshold')
    
    ax.set_xlabel('End-to-End Delay (ms)', fontweight='bold')
    ax.set_ylabel('Cumulative Probability', fontweight='bold')
    ax.set_title('CDF of VoIP Packet Delays\nWiFi 6 with Airtime-Based CAC',
                fontweight='bold', pad=20)
    ax.legend(loc='best', framealpha=0.9)
    ax.grid(True, alpha=0.3)
    ax.set_xlim(0, 300)
    ax.set_ylim(0, 1.05)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Saved: {output_file}")
    plt.close()

def plot_airtime_utilization(data_list, output_file='airtime_utilization.png'):
    """
    Plot 5: Airtime Utilization Breakdown by Traffic Type
    Stacked bar chart showing airtime distribution
    """
    fig, ax = plt.subplots(figsize=(12, 7))
    
    # Collect airtime data
    scenarios = []
    airtime_by_type = {ttype: [] for ttype in TRAFFIC_TYPES.values()}
    
    for data_info in data_list:
        data = data_info['data']
        label = data_info['label']
        scenarios.append(label)
        
        if 'admission' in data:
            admission = data['admission']
            
            for traffic_id, traffic_name in TRAFFIC_TYPES.items():
                # Map traffic type string to ID
                type_map = {'VOIP': 0, 'VIDEO': 1, 'BURSTY': 2, 'WEB': 3}
                traffic_rows = admission[admission['TrafficType'].isin([traffic_name, list(type_map.keys())[traffic_id]])]
                
                if len(traffic_rows) > 0:
                    total_airtime = traffic_rows[traffic_rows['Admitted'] == 1]['RequiredAirtime'].sum()
                else:
                    total_airtime = 0
                
                airtime_by_type[traffic_name].append(total_airtime)
    
    # Create stacked bar chart
    x = np.arange(len(scenarios))
    width = 0.6
    bottom = np.zeros(len(scenarios))
    
    for traffic_name in TRAFFIC_TYPES.values():
        values = airtime_by_type[traffic_name]
        if len(values) == len(scenarios):
            ax.bar(x, values, width, label=traffic_name, bottom=bottom,
                  color=TRAFFIC_COLORS.get(traffic_name, 'gray'), alpha=0.8,
                  edgecolor='black', linewidth=0.5)
            bottom += np.array(values)
    
    # Add threshold line
    ax.axhline(y=0.80, color='red', linestyle='--', linewidth=2, alpha=0.7,
              label='CAC Threshold (80%)')
    
    ax.set_xlabel('Scenario', fontweight='bold')
    ax.set_ylabel('Airtime Utilization', fontweight='bold')
    ax.set_title('Airtime Utilization Breakdown by Traffic Type\nWiFi 6 Dense WLAN',
                fontweight='bold', pad=20)
    ax.set_xticks(x)
    ax.set_xticklabels(scenarios, rotation=15, ha='right')
    ax.legend(loc='upper left', framealpha=0.9)
    ax.grid(True, alpha=0.3, axis='y')
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Saved: {output_file}")
    plt.close()

def generate_summary_statistics(data_list, output_file='summary_statistics.txt'):
    """Generate text summary of key statistics"""
    with open(output_file, 'w') as f:
        f.write("=" * 80 + "\n")
        f.write("WiFi 6 Airtime-Based CAC Simulation - Summary Statistics\n")
        f.write("=" * 80 + "\n\n")
        
        for data_info in data_list:
            data = data_info['data']
            label = data_info['label']
            n_stations = data_info.get('n_stations', 'N/A')
            
            f.write(f"\n{'='*80}\n")
            f.write(f"Scenario: {label}\n")
            f.write(f"Number of Stations: {n_stations}\n")
            f.write(f"{'='*80}\n\n")
            
            # Admission statistics
            if 'admission' in data:
                admission = data['admission']
                total = len(admission)
                admitted = len(admission[admission['Admitted'] == 1])
                blocked = total - admitted
                blocking_prob = blocked / total if total > 0 else 0
                
                f.write(f"Admission Control:\n")
                f.write(f"  Total Flow Requests: {total}\n")
                f.write(f"  Admitted Flows: {admitted}\n")
                f.write(f"  Blocked Flows: {blocked}\n")
                f.write(f"  Blocking Probability: {blocking_prob:.2%}\n")
                f.write(f"  Total Offered Airtime: {admission['RequiredAirtime'].sum():.4f}\n\n")
            
            # Delay statistics
            if 'delay' in data:
                delay_df = data['delay']
                
                f.write(f"Delay Statistics (ms):\n")
                for traffic_id, traffic_name in TRAFFIC_TYPES.items():
                    traffic_delays = delay_df[delay_df['TrafficType'] == traffic_id]['Delay']
                    if len(traffic_delays) > 0:
                        f.write(f"  {traffic_name}:\n")
                        f.write(f"    Mean: {traffic_delays.mean():.2f}\n")
                        f.write(f"    Median: {traffic_delays.median():.2f}\n")
                        f.write(f"    Std Dev: {traffic_delays.std():.2f}\n")
                        f.write(f"    Min: {traffic_delays.min():.2f}\n")
                        f.write(f"    Max: {traffic_delays.max():.2f}\n")
                        f.write(f"    95th Percentile: {traffic_delays.quantile(0.95):.2f}\n")
                        
                        # VoIP QoS check
                        if traffic_name == 'VoIP':
                            below_threshold = (traffic_delays < 150).sum()
                            total_voip = len(traffic_delays)
                            qos_compliance = below_threshold / total_voip * 100
                            f.write(f"    QoS Compliance (<150ms): {qos_compliance:.1f}%\n")
                f.write("\n")
            
            # Throughput statistics
            if 'flowmon' in data:
                flowmon = data['flowmon']
                f.write(f"Throughput Statistics:\n")
                f.write(f"  Aggregate Throughput: {flowmon['Throughput'].sum():.2f} Mbps\n")
                f.write(f"  Average Per-Flow Throughput: {flowmon['Throughput'].mean():.2f} Mbps\n")
                f.write(f"  Total Packets Transmitted: {flowmon['TxPackets'].sum()}\n")
                f.write(f"  Total Packets Received: {flowmon['RxPackets'].sum()}\n")
                f.write(f"  Average Packet Loss: {flowmon['PacketLoss'].mean():.2f}%\n\n")
    
    print(f"Saved: {output_file}")

def main():
    """Main analysis function"""
    if len(sys.argv) < 2:
        print("Usage: python3 analyze-results.py <output_prefix1> [output_prefix2] ...")
        print("Example: python3 analyze-results.py wifi6-cac-25 wifi6-cac-30 wifi6-cac-35")
        sys.exit(1)
    
    # Load data from all specified prefixes
    data_list = []
    for i, prefix in enumerate(sys.argv[1:]):
        print(f"\nLoading data from: {prefix}")
        data = load_simulation_data(prefix)
        
        if data:
            # Extract number of stations from prefix if possible
            try:
                n_stations = int(prefix.split('-')[-1])
            except:
                n_stations = 25 + i * 5  # Default progression
            
            data_list.append({
                'data': data,
                'label': f"{n_stations} clients",
                'n_stations': n_stations,
                'total_flows': len(data.get('admission', []))
            })
    
    if not data_list:
        print("No data loaded. Exiting.")
        sys.exit(1)
    
    print(f"\n{'='*80}")
    print(f"Generating graphs and analysis...")
    print(f"{'='*80}\n")
    
    # Generate all plots
    plot_aggregate_throughput_vs_flows(data_list)
    plot_delay_vs_flows(data_list)
    plot_blocking_probability(data_list)
    plot_voip_delay_cdf(data_list)
    plot_airtime_utilization(data_list)
    
    # Generate summary statistics
    generate_summary_statistics(data_list)
    
    print(f"\n{'='*80}")
    print("Analysis complete! All graphs and statistics have been generated.")
    print(f"{'='*80}\n")

if __name__ == "__main__":
    main()
