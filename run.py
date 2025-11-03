import subprocess
import os
import re
import sys

def compile_cpp():
    print("Compiling.")
    r = subprocess.run(["g++", "-O2", "-std=c++17", "main.cpp", "-o", "main"], capture_output=True, text=True)
    if r.returncode != 0:
        print("Compilation failed:\n", r.stderr)
        sys.exit(1)
    print("Compilation successful.\n")

def extract_distances(out):
    p = re.compile(r"(Naive|Random|Group|Optimized|Graph) tour distance:\s*([\d.]+)")
    return {a: float(b) for a, b in p.findall(out)}

def run_tests():
    GREEN = "\033[92m"
    RESET = "\033[0m"
    labels = ["Naive", "Random", "Group", "Optimized", "Graph"]
    print(f"{'Test File':<15} {'Points':>7} " + " ".join(f"{l:>10}" for l in labels))
    print("-" * 75)

    for i in range(1, 31):
        path = f"tests/test{i}.txt"
        if not os.path.exists(path):
            print(f"test{i}.txt", "File not found")
            continue

        with open(path) as f:
            content = f.read()
        n = int(content.splitlines()[0].strip())
        r = subprocess.run(["./main"], input=content, text=True, capture_output=True)
        d = extract_distances(r.stdout)
        if len(d) == 5:
            best = min(d, key=d.get)
            row = [f"test{i}.txt".ljust(15), f"{n:>7}"]
            for l in labels:
                val = f"{d[l]:>10.2f}"
                if l == best:
                    val = f"{GREEN}{val}{RESET}"
                row.append(val)
            print(" ".join(row))
        else:
            print(f"test{i}.txt", "Incomplete")

compile_cpp()
run_tests()
