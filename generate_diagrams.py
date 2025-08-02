#!/usr/bin/env python3
"""
Generate architecture diagram for Ternuino CPU
This script creates a visual representation of the ternary CPU architecture
"""

try:
    import matplotlib.pyplot as plt
    import matplotlib.patches as patches
    from matplotlib.patches import FancyBboxPatch
    import numpy as np
    
    def create_ternuino_diagram():
        """Create a visual diagram of the Ternuino CPU architecture"""
        
        fig, ax = plt.subplots(1, 1, figsize=(12, 8))
        ax.set_xlim(0, 10)
        ax.set_ylim(0, 8)
        ax.axis('off')
        
        # Title
        ax.text(5, 7.5, 'TERNUINO CPU ARCHITECTURE', 
                fontsize=20, fontweight='bold', ha='center')
        
        # Registers box
        reg_box = FancyBboxPatch((0.5, 4.5), 2, 2.5, 
                                boxstyle="round,pad=0.1", 
                                facecolor='lightblue', 
                                edgecolor='black', linewidth=2)
        ax.add_patch(reg_box)
        ax.text(1.5, 6.5, 'REGISTERS', fontweight='bold', ha='center')
        ax.text(1.5, 6, 'A: trit', ha='center')
        ax.text(1.5, 5.7, 'B: trit', ha='center')
        ax.text(1.5, 5.4, 'C: trit', ha='center')
        ax.text(1.5, 5.1, 'PC: int', ha='center')
        
        # Memory box
        mem_box = FancyBboxPatch((3.5, 4.5), 2, 2.5, 
                                boxstyle="round,pad=0.1", 
                                facecolor='lightgreen', 
                                edgecolor='black', linewidth=2)
        ax.add_patch(mem_box)
        ax.text(4.5, 6.5, 'MEMORY', fontweight='bold', ha='center')
        ax.text(4.5, 6, '27 locations', ha='center')
        ax.text(4.5, 5.7, '(3³ addressable)', ha='center')
        ax.text(4.5, 5.4, '0: instruction', ha='center')
        ax.text(4.5, 5.1, '1: instruction', ha='center')
        ax.text(4.5, 4.8, '...', ha='center')
        
        # Instruction Set box
        inst_box = FancyBboxPatch((6.5, 4.5), 2.5, 2.5, 
                                 boxstyle="round,pad=0.1", 
                                 facecolor='lightyellow', 
                                 edgecolor='black', linewidth=2)
        ax.add_patch(inst_box)
        ax.text(7.75, 6.5, 'INSTRUCTION SET', fontweight='bold', ha='center')
        ax.text(7.75, 6.1, 'MOV  ADD  TAND', ha='center', fontsize=9)
        ax.text(7.75, 5.9, 'SUB  MUL  TOR', ha='center', fontsize=9)
        ax.text(7.75, 5.7, 'DIV  JMP  TNOT', ha='center', fontsize=9)
        ax.text(7.75, 5.5, 'TJZ  HLT  NOP', ha='center', fontsize=9)
        
        # Ternary Logic box
        logic_box = FancyBboxPatch((1, 2), 7, 1.5, 
                                  boxstyle="round,pad=0.1", 
                                  facecolor='lavender', 
                                  edgecolor='black', linewidth=2)
        ax.add_patch(logic_box)
        ax.text(4.5, 3.2, 'TERNARY LOGIC', fontweight='bold', ha='center')
        ax.text(4.5, 2.8, 'States: -1 (negative)  0 (neutral)  +1 (positive)', ha='center')
        ax.text(4.5, 2.5, 'TAND = min(a,b)    TOR = max(a,b)    TNOT = -a', ha='center')
        
        # Advantages box
        adv_box = FancyBboxPatch((1, 0.2), 7, 1.3, 
                                boxstyle="round,pad=0.1", 
                                facecolor='mistyrose', 
                                edgecolor='black', linewidth=2)
        ax.add_patch(adv_box)
        ax.text(4.5, 1.2, 'KEY ADVANTAGES', fontweight='bold', ha='center')
        ax.text(4.5, 0.9, '• 58% more information dense than binary', ha='center', fontsize=10)
        ax.text(4.5, 0.7, '• Natural signed number representation', ha='center', fontsize=10)
        ax.text(4.5, 0.5, '• Symmetric arithmetic operations', ha='center', fontsize=10)
        
        # Arrows connecting components
        ax.arrow(2.5, 5.7, 0.8, 0, head_width=0.1, head_length=0.1, fc='black', ec='black')
        ax.arrow(5.5, 5.7, 0.8, 0, head_width=0.1, head_length=0.1, fc='black', ec='black')
        
        plt.tight_layout()
        plt.savefig('docs/ternuino-architecture.png', dpi=300, bbox_inches='tight')
        plt.savefig('docs/ternuino-architecture.svg', bbox_inches='tight')
        print("✓ Generated ternuino-architecture.png and .svg")
        plt.show()
    
    def create_comparison_chart():
        """Create a comparison chart between binary and ternary systems"""
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
        
        # Binary representation
        ax1.set_title('Binary System (Base-2)', fontsize=16, fontweight='bold')
        ax1.text(0.5, 0.9, 'States: 0, 1', transform=ax1.transAxes, fontsize=14, ha='center')
        
        # Create binary representation visual
        binary_vals = ['000', '001', '010', '011', '100', '101', '110', '111', '1000']
        decimal_vals = list(range(9))
        
        for i, (dec, bin_val) in enumerate(zip(decimal_vals, binary_vals)):
            y_pos = 0.8 - i * 0.08
            ax1.text(0.2, y_pos, f'{dec}:', transform=ax1.transAxes, fontsize=12)
            ax1.text(0.4, y_pos, bin_val, transform=ax1.transAxes, fontsize=12, 
                    family='monospace', bbox=dict(boxstyle="round,pad=0.3", facecolor='lightcoral'))
        
        # Ternary representation
        ax2.set_title('Ternary System (Base-3)', fontsize=16, fontweight='bold')
        ax2.text(0.5, 0.9, 'States: -1, 0, +1', transform=ax2.transAxes, fontsize=14, ha='center')
        
        # Create ternary representation visual
        ternary_vals = ['00', '0+', '+-', '+0', '++', '+--', '+-0', '+-+', '+0-']
        
        for i, (dec, tern_val) in enumerate(zip(decimal_vals, ternary_vals)):
            y_pos = 0.8 - i * 0.08
            ax2.text(0.2, y_pos, f'{dec}:', transform=ax2.transAxes, fontsize=12)
            ax2.text(0.4, y_pos, tern_val, transform=ax2.transAxes, fontsize=12, 
                    family='monospace', bbox=dict(boxstyle="round,pad=0.3", facecolor='lightgreen'))
        
        # Remove axes
        ax1.axis('off')
        ax2.axis('off')
        
        # Add efficiency note
        fig.suptitle('Number Representation Efficiency Comparison', fontsize=18, fontweight='bold')
        fig.text(0.5, 0.02, 'Ternary requires fewer digits to represent the same range!', 
                ha='center', fontsize=12, style='italic')
        
        plt.tight_layout()
        plt.savefig('docs/binary-vs-ternary.png', dpi=300, bbox_inches='tight')
        plt.savefig('docs/binary-vs-ternary.svg', bbox_inches='tight')
        print("✓ Generated binary-vs-ternary.png and .svg")
        plt.show()
    
    if __name__ == "__main__":
        print("Generating Ternuino CPU diagrams...")
        create_ternuino_diagram()
        create_comparison_chart()
        print("✓ All diagrams generated successfully!")
        print("Images saved in docs/ directory")

except ImportError:
    print("matplotlib not available. To generate images, install it with:")
    print("pip install matplotlib")
    print("\nAlternatively, you can create the images manually and place them in the docs/ folder:")
    print("- ternuino-architecture.png")
    print("- binary-vs-ternary.png")
