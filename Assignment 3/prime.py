import sys

def is_prime(n):
    for i in range(2, n):
        if n % i == 0:
            return False
    return True

def prime_to_nth(N):
    prime_nums = [2]
    
    base = 3
    while(len(prime_nums) != N):
        if is_prime(base):
            prime_nums.append(base)
        base += 1

    return prime_nums


if __name__ == "__main__":
    N = int(sys.argv[1])

    prime_list = prime_to_nth(N)

    if sys.argv[2] == "1":
        printable_str = ""
        for number in prime_list:
            printable_str += f"{str(number)} "
        print(printable_str)
    else:
        print(prime_list[len(prime_list) - 1])