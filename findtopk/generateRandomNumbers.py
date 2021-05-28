import random

def gen(n):
    for i in range(n):
        print(random.randint(1, 100000))

gen(1000)
