import os
from collections import deque
import tkinter as tk
from tkinter import ttk, messagebox

from library.ternuino import Ternuino
from library.assembler import TernuinoAssembler
from library.tritlogic import TritLogic

TRIT_COLORS = {
    -1: "#e57373",  # red-ish
    0: "#bdbdbd",   # gray
    1: "#81c784",   # green-ish
}


def instr_to_str(instr):
    if instr is None:
        return "—"
    op = instr[0]
    if len(instr) == 1:
        return op
    return f"{op} " + ", ".join(map(str, instr[1:]))


class CPUVisualizer(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Ternuino Hardware Visualizer")
        self.geometry("1100x780")
        self.minsize(1000, 700)

        self.cpu = Ternuino()
        self.assembler = TernuinoAssembler()
        self.program_names = self._discover_programs()

        # State
        self._running = False
        self._run_delay_ms = 250
        self._trace_a = deque(maxlen=60)
        self._breakpoints = set()
        self._show_tritwords = True

        # UI
        self._build_ui()

        # Initial paint
        self.after(50, self.refresh)

    def _discover_programs(self):
        pdir = os.path.join(os.getcwd(), "programs")
        return [f for f in os.listdir(pdir) if f.endswith(".asm")] if os.path.isdir(pdir) else []

    # UI
    def _build_ui(self):
        # Top control bar
        toolbar = ttk.Frame(self)
        toolbar.pack(fill=tk.X, padx=10, pady=8)

        ttk.Label(toolbar, text="Program:").pack(side=tk.LEFT)
        self.program_var = tk.StringVar(value=self.program_names[0] if self.program_names else "")
        self.program_combo = ttk.Combobox(toolbar, textvariable=self.program_var, values=self.program_names, state="readonly")
        self.program_combo.pack(side=tk.LEFT, padx=6)

        ttk.Button(toolbar, text="Load", command=self.load_program).pack(side=tk.LEFT, padx=4)
        ttk.Button(toolbar, text="Step", command=self.step_once).pack(side=tk.LEFT, padx=4)
        self.run_btn = ttk.Button(toolbar, text="Run", command=self.toggle_run)
        self.run_btn.pack(side=tk.LEFT, padx=4)
        ttk.Button(toolbar, text="Reset", command=self.reset_cpu).pack(side=tk.LEFT, padx=4)

        ttk.Label(toolbar, text="Speed:").pack(side=tk.LEFT, padx=(20, 4))
        self.speed = tk.IntVar(value=250)
        speed_scale = ttk.Scale(toolbar, from_=50, to=1000, variable=self.speed, command=self._on_speed)
        speed_scale.pack(side=tk.LEFT, ipadx=50)

        # Main panes
        main = ttk.Frame(self)
        main.pack(fill=tk.BOTH, expand=True, padx=10, pady=8)

        left = ttk.Frame(main)
        left.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)

        right = ttk.Notebook(main)
        right.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

        # Left: Memory + Registers
        self._build_memory_view(left)
        self._build_registers_view(left)

        # Right tabs: Logic, Transistor, Disassembly
        logic_tab = ttk.Frame(right)
        trans_tab = ttk.Frame(right)
        disasm_tab = ttk.Frame(right)
        right.add(logic_tab, text="Ternary Logic")
        right.add(trans_tab, text="Conceptual Transistor")
        right.add(disasm_tab, text="Disassembly")

        self._build_logic_tab(logic_tab)
        self._build_transistor_tab(trans_tab)
        self._build_disasm_tab(disasm_tab)

    def _build_memory_view(self, parent):
        box = ttk.LabelFrame(parent, text="Memory (27 cells)")
        box.pack(fill=tk.BOTH, expand=True, padx=6, pady=6)

        self.mem_list = tk.Listbox(box, height=18, font=("Consolas", 11))
        self.mem_list.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        self.mem_scroll = ttk.Scrollbar(box, orient=tk.VERTICAL, command=self.mem_list.yview)
        self.mem_list.configure(yscrollcommand=self.mem_scroll.set)
        self.mem_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        # Toggle breakpoints with double click
        self.mem_list.bind('<Double-Button-1>', self._toggle_breakpoint)

    def _build_registers_view(self, parent):
        box = ttk.LabelFrame(parent, text="Registers and PC")
        box.pack(fill=tk.X, padx=6, pady=6)

        row = ttk.Frame(box)
        row.pack(fill=tk.X, padx=6, pady=6)

        self.reg_widgets = {}
        for reg in ("A", "B", "C"):
            f = ttk.Frame(row)
            f.pack(side=tk.LEFT, padx=10)
            ttk.Label(f, text=f"{reg}:").pack()
            canvas = tk.Canvas(f, width=60, height=60, highlightthickness=0)
            canvas.pack()
            oval = canvas.create_oval(10, 10, 50, 50, fill="#bdbdbd", outline="#616161", width=2)
            text = canvas.create_text(30, 30, text="0", font=("Consolas", 12, "bold"))
            self.reg_widgets[reg] = (canvas, oval, text)

            # TritWord mini view
            tw_label = ttk.Label(f, text="", font=("Consolas", 10))
            tw_label.pack(pady=(2, 0))
            self.reg_widgets[reg] += (tw_label,)

        pc_frame = ttk.Frame(box)
        pc_frame.pack(side=tk.LEFT, padx=20)
        self.pc_var = tk.StringVar(value="PC: 0")
        ttk.Label(pc_frame, textvariable=self.pc_var, font=("Consolas", 12, "bold")).pack()

        # Last instruction and trace
        self.last_instr_var = tk.StringVar(value="Last: —")
        ttk.Label(box, textvariable=self.last_instr_var, font=("Consolas", 10)).pack(anchor=tk.W, padx=10)

        # Simple timeline chart for A
        self.timeline = tk.Canvas(box, width=400, height=80, bg="#fafafa", highlightthickness=1, highlightbackground="#ccc")
        self.timeline.pack(padx=10, pady=(4, 8))
        ttk.Label(box, text="Timeline: Register A (last 60 steps)").pack(anchor=tk.W, padx=10)

    def _build_logic_tab(self, parent):
        frm = ttk.Frame(parent)
        frm.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        # Inputs
        inputs = ttk.LabelFrame(frm, text="Inputs")
        inputs.pack(fill=tk.X)
        self.in_a = tk.IntVar(value=0)
        self.in_b = tk.IntVar(value=0)

        for label, var in [("a", self.in_a), ("b", self.in_b)]:
            row = ttk.Frame(inputs)
            row.pack(side=tk.LEFT, padx=16, pady=6)
            ttk.Label(row, text=f"{label}:").pack(side=tk.LEFT)
            ttk.Radiobutton(row, text="-1", value=-1, variable=var, command=self._update_logic).pack(side=tk.LEFT)
            ttk.Radiobutton(row, text="0", value=0, variable=var, command=self._update_logic).pack(side=tk.LEFT)
            ttk.Radiobutton(row, text="+1", value=1, variable=var, command=self._update_logic).pack(side=tk.LEFT)

        # Outputs
        out = ttk.LabelFrame(frm, text="Outputs")
        out.pack(fill=tk.X, pady=10)
        self.tand_var = tk.StringVar()
        self.tor_var = tk.StringVar()
        self.tnot_a_var = tk.StringVar()
        self.tnot_b_var = tk.StringVar()

        for label, var in [("TAND(a,b)", self.tand_var), ("TOR(a,b)", self.tor_var), ("TNOT(a)", self.tnot_a_var), ("TNOT(b)", self.tnot_b_var)]:
            row = ttk.Frame(out)
            row.pack(fill=tk.X, padx=6, pady=4)
            ttk.Label(row, text=label, width=12).pack(side=tk.LEFT)
            ttk.Label(row, textvariable=var, width=6, relief=tk.SUNKEN).pack(side=tk.LEFT)

        self._update_logic()

    def _build_transistor_tab(self, parent):
        frm = ttk.Frame(parent)
        frm.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        desc = ("Conceptual ternary transistor indicator:\n"
                "- Shows a single driver selecting one of three rails: -V, 0, +V\n"
                "- Not an electrical simulation; just an intuition aid")
        ttk.Label(frm, text=desc, justify=tk.LEFT).pack(anchor=tk.W, pady=(0, 10))

        self.trans_var = tk.IntVar(value=0)
        row = ttk.Frame(frm)
        row.pack(pady=8)
        ttk.Radiobutton(row, text="-1", value=-1, variable=self.trans_var, command=self._update_transistor).pack(side=tk.LEFT, padx=8)
        ttk.Radiobutton(row, text="0", value=0, variable=self.trans_var, command=self._update_transistor).pack(side=tk.LEFT, padx=8)
        ttk.Radiobutton(row, text="+1", value=1, variable=self.trans_var, command=self._update_transistor).pack(side=tk.LEFT, padx=8)

        cnv = tk.Canvas(frm, width=420, height=180, bg="white")
        cnv.pack(pady=10)
        # Draw three rails
        cnv.create_rectangle(30, 30, 390, 60, fill="#e57373", outline="#c62828")  # -V
        cnv.create_rectangle(30, 80, 390, 110, fill="#bdbdbd", outline="#616161")  # 0V
        cnv.create_rectangle(30, 130, 390, 160, fill="#81c784", outline="#2e7d32")  # +V
        cnv.create_text(15, 45, text="-V", anchor=tk.W)
        cnv.create_text(15, 95, text="0V", anchor=tk.W)
        cnv.create_text(15, 145, text="+V", anchor=tk.W)
        # Output node
        self.trans_node = cnv.create_oval(400, 85, 410, 95, fill="#bdbdbd")
        self.trans_canvas = cnv
        self._update_transistor()

    def _build_disasm_tab(self, parent):
        frm = ttk.Frame(parent)
        frm.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        self.disasm_text = tk.Text(frm, height=20, font=("Consolas", 11))
        self.disasm_text.pack(fill=tk.BOTH, expand=True)
        self.disasm_text.configure(state=tk.DISABLED)

    # Actions
    def _on_speed(self, _evt=None):
        self._run_delay_ms = max(10, int(self.speed.get()))

    def load_program(self):
        name = self.program_var.get()
        path = os.path.join("programs", name)
        if not name or not os.path.exists(path):
            messagebox.showwarning("Load Program", "Select a valid program")
            return
        try:
            program = self.assembler.parse_file(path)
        except Exception as e:
            messagebox.showerror("Assembler Error", str(e))
            return
        self.cpu = Ternuino()
        self.cpu.load_program(program)
        self._update_disasm(program)
        self.refresh()

    def reset_cpu(self):
        self.cpu = Ternuino()
        self._trace_a.clear()
        self.refresh()

    def step_once(self):
        if not self.cpu.running:
            return
        self.cpu.step()
        self.refresh()

    def toggle_run(self):
        if self._running:
            self._running = False
            self.run_btn.configure(text="Run")
            return
        if not self.cpu.running:
            return
        self._running = True
        self.run_btn.configure(text="Pause")
        self._run_loop()

    def _run_loop(self):
        if not self._running:
            return
        if not self.cpu.running:
            self._running = False
            self.run_btn.configure(text="Run")
            return
        # Stop on breakpoint before executing next instruction
        if self.cpu.pc in self._breakpoints:
            self._running = False
            self.run_btn.configure(text="Run")
            self.refresh()
            return
        self.cpu.step()
        self.refresh()
        self.after(self._run_delay_ms, self._run_loop)

    # Rendering
    def refresh(self):
        # Memory with PC and breakpoint markers
        self.mem_list.delete(0, tk.END)
        for i, instr in enumerate(self.cpu.memory):
            mark_bp = '● ' if i in self._breakpoints else '  '
            mark_pc = '→ ' if i == self.cpu.pc else '  '
            s = instr_to_str(instr)
            line = f"{mark_pc}{mark_bp}{i:02}: {s}"
            self.mem_list.insert(tk.END, line)
        # Highlight PC
        try:
            self.mem_list.selection_clear(0, tk.END)
            self.mem_list.selection_set(self.cpu.pc)
            self.mem_list.see(self.cpu.pc)
        except Exception:
            pass

        # Registers
        for reg in ("A", "B", "C"):
            val = self.cpu.registers.get(reg, 0)
            col = TRIT_COLORS.get(val, "#90caf9")
            canvas, oval, text, tw_label = self.reg_widgets[reg]
            canvas.itemconfigure(oval, fill=col)
            canvas.itemconfigure(text, text=str(val))
            # TritWord view
            try:
                from library.tritword import TritWord
                if self._show_tritwords:
                    tw = TritWord.from_int(int(val), size=6)
                    tw_label.config(text=str(tw))
                else:
                    tw_label.config(text="")
            except Exception:
                tw_label.config(text="")

        # PC
        self.pc_var.set(f"PC: {self.cpu.pc}")

        # Last executed instruction
        prev_pc = max(0, self.cpu.pc - 1)
        last = self.cpu.memory[prev_pc]
        self.last_instr_var.set(f"Last: {instr_to_str(last)} @ {prev_pc:02}")

        # Update trace for A and redraw timeline
        self._trace_a.append(self.cpu.registers.get('A', 0))
        self._draw_timeline()

    def _update_logic(self):
        a = int(self.in_a.get())
        b = int(self.in_b.get())
        self.tand_var.set(str(TritLogic.tand(a, b)))
        self.tor_var.set(str(TritLogic.tor(a, b)))
        self.tnot_a_var.set(str(TritLogic.tnot(a)))
        self.tnot_b_var.set(str(TritLogic.tnot(b)))

    def _update_transistor(self):
        v = int(self.trans_var.get())
        col = TRIT_COLORS.get(v, "#bdbdbd")
        self.trans_canvas.itemconfigure(self.trans_node, fill=col)

    def _update_disasm(self, program):
        self.disasm_text.configure(state=tk.NORMAL)
        self.disasm_text.delete("1.0", tk.END)
        for i, instr in enumerate(program):
            self.disasm_text.insert(tk.END, f"{i:02}: {instr_to_str(instr)}\n")
        self.disasm_text.configure(state=tk.DISABLED)

    # Helpers
    def _toggle_breakpoint(self, event):
        # Determine clicked index
        idx = self.mem_list.nearest(event.y)
        # Adjust because of markers; but our Listbox contains one line per memory index
        if idx in self._breakpoints:
            self._breakpoints.remove(idx)
        else:
            self._breakpoints.add(idx)
        self.refresh()

    def _draw_timeline(self):
        c = self.timeline
        c.delete('all')
        w = int(c.winfo_width()) or 400
        h = int(c.winfo_height()) or 80
        # axes
        c.create_line(0, h/2, w, h/2, fill="#e0e0e0")
        c.create_text(4, 10, text="+1", anchor=tk.W, fill="#2e7d32")
        c.create_text(4, h-10, text="-1", anchor=tk.W, fill="#c62828")
        # plot: map -1,0,+1 to y positions
        if not self._trace_a:
            return
        step = max(1, w // 60)
        x = 5
        for v in list(self._trace_a)[-60:]:
            if v > 0:
                y = h*0.25
                col = TRIT_COLORS[1]
            elif v < 0:
                y = h*0.75
                col = TRIT_COLORS[-1]
            else:
                y = h*0.5
                col = TRIT_COLORS[0]
            c.create_oval(x-2, y-2, x+2, y+2, fill=col, outline=col)
            x += step


if __name__ == "__main__":
    app = CPUVisualizer()
    app.mainloop()
