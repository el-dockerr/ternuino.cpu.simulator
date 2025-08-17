def is_prime(n):
    """
    Prove whether a number is prime.
    """
    if n <= 1:
        return False
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            return False
    return True

def to_balanced_ternary(n):
    """
    Convert a decimal number to its balanced ternary representation.
    """
    if n == 0:
        return "0"

    result = ""
    while n != 0:
        remainder = n % 3
        if remainder == 2:
            remainder = -1
            n += 1
        n = (n - remainder) // 3
        
        # Digit representation: 1 for 1, 0 for 0, T for -1
        if remainder == -1:
            result = "T" + result
        else:
            result = str(remainder) + result
            
    return result

def balanced_ternary_crosssum(bt: str) -> int:
    """Return the sum of the digit magnitudes in a balanced ternary string.

    In balanced ternary digits are 1, 0, and T (for -1). For a quick
    visual "cross sum" it's often more interesting to see how many trits
    are non-zero (i.e. the sum of absolute digit values). That equals 1
    for each '1' or 'T' and 0 for '0'.
    """
    return sum(1 for ch in bt if ch != '0')

def balanced_ternary_digit_sum(bt: str) -> int:
    """Return the algebraic sum of digits (T=-1, 0=0, 1=1)."""
    total = 0
    for ch in bt:
        if ch == '1':
            total += 1
        elif ch == 'T':
            total -= 1
    return total

def count_zeros_in_balanced_ternary(bt: str) -> int:
    """Return the number of zeros in a balanced ternary string."""
    return bt.count('0')

def main():
    """
    Print the balanced ternary representation of numbers from 0 to 1000.
    I found so many interesting patterns in the balanced ternary representation!
    It looks like that a prime number never ends with 0 unless it is not 10 (3).
    But actually, there are no more patterns to discover! I will stop here for now.
    Maybe I will find more interesting properties later.
    """
    print("Decimal -> Balanced Ternary")
    print("----------------------------")
    for i in range(1001):
        balanced_ternary = to_balanced_ternary(i)
        signed_sum = balanced_ternary_digit_sum(balanced_ternary)
        abs_sum = balanced_ternary_crosssum(balanced_ternary)
        zero_count = count_zeros_in_balanced_ternary(balanced_ternary)
        # Use ASCII-friendly labels to avoid Windows console encoding issues
        output_string = f"{i:3d} -> {balanced_ternary}  (sum={signed_sum}, abs_sum={abs_sum}, zeros={zero_count})"

        if is_prime(i):
            print(f"\033[92m{output_string} (PRIME)\033[0m")  # Green color
        else:
            print(output_string)

if __name__ == "__main__":
    main()