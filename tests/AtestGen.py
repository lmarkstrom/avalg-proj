import os
import random

def generate_tsp_tests():
    start=2, 
    end=30, 
    output_folder="tests", 
    output_prefix="test"

    total_tests = end - start + 1
    small_tests = total_tests // 2

    test_indices = list(range(start, end + 1))
    random.shuffle(test_indices)

    for i, test_num in enumerate(test_indices):
        if i < small_tests:
            num_points = random.randint(1, 99)
        else:
            num_points = random.randint(100, 1000)

        filename = os.path.join(output_folder, f"{output_prefix}{test_num}.txt")
        with open(filename, "w") as f:
            f.write(f"{num_points}\n")
            for j in range(num_points):
                x = round(random.uniform(0, 100), 4)
                y = round(random.uniform(0, 100), 4)
                line = f"{x} {y}"
                if j < num_points - 1:
                    f.write(line + "\n")
                else:
                    f.write(line) 

generate_tsp_tests()
