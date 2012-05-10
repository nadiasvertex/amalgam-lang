import os
import subprocess

llvm_cxx_flags = subprocess.check_output(["llvm-config-3.0", "--cxxflags"])
llvm_ld_flags = subprocess.check_output(["llvm-config-3.0", "--ldflags"])
llvm_lib_flags = subprocess.check_output(["llvm-config-3.0", "--libs", "engine"])
llvm_include_dir = subprocess.check_output(["llvm-config-3.0", "--includedir"])

llvm_libs = [l.replace("-l", "").strip() for l in llvm_lib_flags.split(" ") if l.startswith("-l")]
llvm_lib_paths = [l.replace("-L", "").strip() for l in llvm_lib_flags.split(" ") if l.startswith("-L")]
