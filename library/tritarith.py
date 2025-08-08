from __future__ import annotations

from typing import Union

Number = Union[int, float]


def tsign(v: Number) -> int:
    """Return -1 if v < 0, 0 if v == 0, +1 if v > 0."""
    return -1 if v < 0 else (1 if v > 0 else 0)


def tabs(v: Number) -> Number:
    """Absolute value."""
    return abs(v)


def tshl3(v: Number) -> Number:
    """Scale by 3 (base‑3 left shift)."""
    return v * 3


def tshr3(v: Number) -> int:
    """Divide by 3, truncating toward zero (base‑3 right shift)."""
    return int(v / 3)


def tcmpr(a: Number, b: Number) -> int:
    """Three‑way compare: sign(a - b) in {-1, 0, +1}."""
    d = a - b
    return tsign(d)
