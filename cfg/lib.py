import llvm

libraries = ["readline"] + llvm.llvm_libs + ["m", "pthread", "dl"]
library_paths = [] + llvm.llvm_lib_paths
