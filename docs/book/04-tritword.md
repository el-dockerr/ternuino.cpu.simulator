# 04 – TritWord: Working With Multiple Trits

A `TritWord` is a small array of trits (defaults to 6). You can:
- Build from an integer: `TritWord.from_int(8, size=6)` → `+0−−−−` (for example)
- Convert back: `.to_int()`

This gives you a compact way to show numbers in balanced ternary and is also used in the visualizer under each register as a mini‑view.

Try:
- Change register values and watch their TritWord mini‑view update in the GUI.
