#!/usr/bin/env python3
"""
Parse NetAnim XML and generate network topology PNG visualization
"""

import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np

def parse_netanim_xml(xml_file):
    """Parse NetAnim XML file and extract node information"""
    tree = ET.parse(xml_file)
    root = tree.getroot()
    
    nodes = {}
    
    # Parse node information - NetAnim uses locX and locY attributes
    for node in root.findall('.//node'):
        node_id = int(node.get('id'))
        x = float(node.get('locX', 0))
        y = float(node.get('locY', 0))
        
        # Identify APs (Nodes 0 and 1)
        is_ap = (node_id == 0 or node_id == 1)
        
        # Determine description based on ID
        if is_ap:
            desc = f"AP{node_id+1}"
            color = (0, 0, 1) # Blue
        elif node_id < 32: # First 30 stations (2-31)
            desc = f"STA1-{node_id-1}"
            color = (0, 1, 0) # Green
        else: # Next 30 stations (32-61)
            desc = f"STA2-{node_id-31}"
            color = (0, 1, 0) # Green

        nodes[node_id] = {
            'x': x,
            'y': y,
            'id': node_id,
            'desc': desc,
            'color': color,
            'is_ap': is_ap
        }
    
    # Parse node descriptions and colors from ncs elements
    for ncs in root.findall('.//ncs'):
        try:
            node_id = int(ncs.get('n'))
        except (ValueError, TypeError):
            # Skip non-numeric node IDs (like 'RemainingEnergy')
            continue
            
        desc = ncs.get('d', f'Node{node_id}')
        
        # Parse color (r,g,b)
        color_str = ncs.get('c', '0,255,0')  # Default green
        r, g, b = map(int, color_str.split(','))
        color = (r/255, g/255, b/255)
        
        if node_id in nodes:
            nodes[node_id]['desc'] = desc
            nodes[node_id]['color'] = color
            nodes[node_id]['is_ap'] = 'AP' in desc
    
    return nodes

def plot_topology(nodes, output_file):
    """Generate topology visualization with professional styling"""
    fig, ax = plt.subplots(figsize=(16, 10))
    
    # Separate APs and STAs
    aps = {k: v for k, v in nodes.items() if v.get('is_ap', False)}
    stas = {k: v for k, v in nodes.items() if not v.get('is_ap', False)}
    
    # Draw coverage zones (background)
    for node_id, node in aps.items():
        x, y = node['x'], node['y']
        # Outer coverage
        circle = plt.Circle((x, y), 30, color='blue', alpha=0.05, zorder=0)
        ax.add_patch(circle)
        # Boundary
        circle_border = plt.Circle((x, y), 30, fill=False, edgecolor='blue', 
                                 linestyle='--', linewidth=1.5, alpha=0.4, zorder=1)
        ax.add_patch(circle_border)

    # Plot stations
    for node_id, node in stas.items():
        x, y = node['x'], node['y']
        desc = node.get('desc', f'STA{node_id}')
        
        # Determine station type and style
        if 'STA1' in desc:
            color = '#2ecc71' # Emerald Green
            edge_color = '#27ae60'
            label_group = 'AP1 Stations'
        else:
            color = '#e74c3c' # Alizarin Red
            edge_color = '#c0392b'
            label_group = 'AP2 Stations'
            
        # Use simple circle for stations (cleaner look)
        ax.scatter(x, y, c=[color], s=120, marker='o', 
                  edgecolors=edge_color, linewidth=1.5, alpha=0.9, zorder=3)

    # Plot APs (foreground) with "Tower" icon
    for node_id, node in aps.items():
        x, y = node['x'], node['y']
        desc = node.get('desc', f'AP{node_id}')
        
        # AP Icon (Triangle Up for Tower)
        ax.scatter(x, y, c='#2980b9', s=600, marker='^', 
                  edgecolors='#2c3e50', linewidth=2, zorder=5)
        
        # AP Label
        ax.text(x, y-4, desc, ha='center', va='top', fontsize=12, 
               fontweight='bold', color='#2c3e50',
               bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.9, edgecolor='#bdc3c7'))

    # Add grid
    ax.grid(True, alpha=0.2, linestyle='--', linewidth=0.5)
    
    # Labels and title
    ax.set_xlabel('X Position (m)', fontsize=12, fontweight='bold', color='#34495e')
    ax.set_ylabel('Y Position (m)', fontsize=12, fontweight='bold', color='#34495e')
    ax.set_title('Multi-AP Network Topology (NS-3 Simulation)\n2 APs with 30 Stations Each (CCI Scenario)', 
                fontsize=16, fontweight='bold', pad=20, color='#2c3e50')
    
    # Custom Legend
    legend_elements = [
        mpatches.Patch(facecolor='#2980b9', edgecolor='#2c3e50', label='Access Point (AP)'),
        plt.Line2D([0], [0], marker='o', color='w', markerfacecolor='#2ecc71', 
                  markersize=10, label='AP1 Stations (Green)', markeredgecolor='#27ae60'),
        plt.Line2D([0], [0], marker='o', color='w', markerfacecolor='#e74c3c', 
                  markersize=10, label='AP2 Stations (Red)', markeredgecolor='#c0392b'),
        plt.Line2D([0], [0], color='blue', linestyle='--', alpha=0.4,
                  linewidth=1.5, label='Coverage Area (~30m)')
    ]
    
    ax.legend(handles=legend_elements, loc='upper right', fontsize=11, 
             framealpha=0.95, edgecolor='#bdc3c7', fancybox=True)
    
    # Set aspect ratio and limits
    ax.set_aspect('equal')
    ax.set_xlim(-25, 65) # Adjust based on 50m separation
    ax.set_ylim(-25, 45)
    
    # Add statistics box
    stats_text = f"Configuration:\n"
    stats_text += f"• AP Separation: 50m\n"
    stats_text += f"• Stations: {len(stas)} ({len(stas)//2}/AP)\n"
    stats_text += f"• Channel: 36 (CCI)\n"
    stats_text += f"• Bandwidth: 80 MHz"
    
    ax.text(0.02, 0.98, stats_text, transform=ax.transAxes, 
           fontsize=10, verticalalignment='top', fontfamily='monospace',
           bbox=dict(boxstyle='round,pad=0.5', facecolor='#f8f9fa', alpha=0.9, edgecolor='#bdc3c7'))
    
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"✓ Topology visualization saved: {output_file}")
    
    return len(aps), len(stas)

if __name__ == "__main__":
    xml_file = "graphs/wifi6-multi-ap-topology.xml"
    output_file = "graphs/ns3_network_topology.png"
    
    print(f"Parsing NetAnim XML: {xml_file}")
    nodes = parse_netanim_xml(xml_file)
    print(f"Found {len(nodes)} nodes")
    
    print(f"Generating topology visualization...")
    num_aps, num_stas = plot_topology(nodes, output_file)
    
    print(f"\n{'='*60}")
    print(f"Network Topology Generated Successfully!")
    print(f"{'='*60}")
    print(f"APs: {num_aps}")
    print(f"Stations: {num_stas}")
    print(f"Output: {output_file}")
    print(f"{'='*60}")
