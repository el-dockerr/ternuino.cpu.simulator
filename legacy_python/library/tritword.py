from library.ternuino import TRIT


class TritWord:
    def __init__(self, trits=None, size=6):
        self.size = size
        if trits is None:
            self.trits = [0] * size
        else:
            assert all(t in TRIT for t in trits)
            self.trits = trits[:size]

    def __repr__(self):
        return ''.join({ -1: '-', 0: '0', 1: '+' }[t] for t in self.trits)

    def to_int(self):
        result = 0
        for i, t in enumerate(reversed(self.trits)):
            result += t * (3 ** i)
        return result

    @classmethod
    def from_int(cls, value, size=6):
        trits = []
        for _ in range(size):
            rem = (value + 1) % 3 - 1  # ergibt -1, 0 oder 1
            trits.insert(0, rem)
            value = (value - rem) // 3
        return cls(trits, size)