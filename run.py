import subprocess
import os

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

def run_tests(executable, folder, prefix, start, end):
    for i in range(start, end + 1):
        test_file = os.path.join(folder, f"{prefix}{i}.txt")
        try:
            with open(test_file, "r") as f:
                result = subprocess.run(
                    [f"./{executable}"],
                    input=f.read(),
                    text=True,
                    capture_output=True
                )
                output = result.stdout.strip().splitlines()
                # Look for the line that ends with "tour distance: <value>"
                for line in reversed(output):
                    if "tour distance:" in line:
                        try:
                            distance = float(line.split(":")[-1].strip())
                            print(f"{prefix}{i}.txt: {distance}")
                        except ValueError:
                            print(f"{prefix}{i}.txt: Could not parse distance")
                        break
                else:
                    print(f"{prefix}{i}.txt: No distance found")
        except FileNotFoundError:
            print(f"{prefix}{i}.txt: File not found")

if __name__ == "__main__":
    compile_cpp(CPP_SOURCE, EXECUTABLE)
    run_tests(EXECUTABLE, TEST_FOLDER, TEST_PREFIX, START, END)
