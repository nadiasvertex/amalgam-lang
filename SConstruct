import os

source = ["src/main.cpp"]
libraries = ["readline"]

base_env = Environment()

# clang scan-build
base_env["CC"] = os.getenv("CC", base_env["CC"])
base_env["CXX"] = os.getenv("CXX", base_env["CXX"])
base_env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))

# Setup debugging and C++11
base_env.Append(CCFLAGS="-g -std=c++0x")

# Build
base_env.Program(source=source, LIBS=libraries, target="amalgam")
