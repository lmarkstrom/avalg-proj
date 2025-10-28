import subprocess
import os
import re

CPP_SOURCE = "main.cpp"
EXECUTABLE = "main"
TEST_FOLDER = "tests"
TEST_PREFIX = "test"
START = 1
END = 30

def compile_cpp(source_file, output_binary):
    print("Compiling C++ source...")
    result = subprocess.run(["g++", "-O2", "-std=c++17", source_file, "-o", output_binary])
    if result.returncode != 0:
        print("Compilation failed.")
        exit(1)
    print("Compilation successful.")

def extract_all_distances(output_lines):
    distances = {}
    for line in output_lines:
        match = re.match(r"(Naive|Random|Optimized) tour distance:\s*([\d.]+)", line)
        if match:
            label = match.group(1)
            dist = float(match.group(2))
            distances[label] = dist
    return distances

def run_tests(executable, folder, prefix, start, end):
    GREEN = "\033[92m"
    RESET = "\033[0m"

    print(f"{'Test File':<15} {'Points':>10} {'Naive':>10} {'Random':>10} {'Optimized':>10}")
    print("-" * 55)

    for i in range(start, end + 1):
        test_file = os.path.join(folder, f"{prefix}{i}.txt")
        try:
            with open(test_file, "r") as f:
                lines = f.readlines()
                num_points = int(lines[0].strip())
                result = subprocess.run(
                    [f"./{EXECUTABLE}"],
                    input="".join(lines),
                    text=True,
                    capture_output=True
                )
                output = result.stdout.strip().splitlines()
                distances = extract_all_distances(output)

                if len(distances) == 3:
                    best_label = min(distances, key=distances.get)
                    print(f"{prefix}{i}.txt".ljust(15), f"{num_points:>7}", end="")

                    for label in ["Naive", "Random", "Optimized"]:
                        dist = distances[label]
                        if label == best_label:
                            print(f"{GREEN}{dist:>10.2f}{RESET}", end="")
                        else:
                            print(f"{dist:>10.2f}", end="")
                    print()
                else:
                    print(f"{prefix}{i}.txt".ljust(15), f"{num_points:>7}", "Incomplete".rjust(48))
        except FileNotFoundError:
            print(f"{prefix}{i}.txt".ljust(15), "N/A".rjust(7), "File not found".rjust(48))



if __name__ == "__main__":
    compile_cpp(CPP_SOURCE, EXECUTABLE)
    run_tests(EXECUTABLE, TEST_FOLDER, TEST_PREFIX, START, END)
