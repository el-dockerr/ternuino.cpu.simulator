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

def main():
    """
    Print the balanced ternary representation of numbers from 0 to 1000.
    I found so many interesting patterns in the balanced ternary representation!
    It looks like that a prime number never ends with 0 unless it is not 10 (3). I guess there will be something more.
    """
    print("Decimal -> Balanced Ternary")
    print("----------------------------")
    for i in range(1001):
        balanced_ternary = to_balanced_ternary(i)
        output_string = f"{i:3d} -> {balanced_ternary}"
        
        if is_prime(i):
            print(f"\033[92m{output_string} (PRIME)\033[0m") # Green color
        else:
            print(output_string)

if __name__ == "__main__":
    main()