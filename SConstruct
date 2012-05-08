import os
import subprocess

llvm_cxx_flags = subprocess.check_output(["llvm-config-3.0", "--cxxflags"])
llvm_ld_flags = subprocess.check_output(["llvm-config-3.0", "--ldflags"])
llvm_lib_flags = subprocess.check_output(["llvm-config-3.0", "--libs", "engine"])
llvm_include_dir = subprocess.check_output(["llvm-config-3.0", "--includedir"])

llvm_libs = [l.replace("-l", "").strip() for l in llvm_lib_flags.split(" ") if l.startswith("-l")]
llvm_lib_paths = [l.replace("-L", "").strip() for l in llvm_lib_flags.split(" ") if l.startswith("-L")]

library_paths = [] + llvm_lib_paths

source = ["src/main.cpp"]
libraries = ["readline"] + llvm_libs + ["m", "pthread", "dl"]

base_env = Environment()

# clang scan-build
base_env["CC"] = os.getenv("CC", base_env["CC"])
base_env["CXX"] = os.getenv("CXX", base_env["CXX"])
base_env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))

# Setup debugging and C++11
base_env.Append(CCFLAGS="-g -std=c++0x " + llvm_cxx_flags + " -fexceptions")
base_env.Append(LIBPATH=library_paths)

# Setup linker flags
base_env.Append(LDFLAGS=llvm_ld_flags)

# Build
base_env.Program(source=source, LIBS=libraries, target="amalgam")
