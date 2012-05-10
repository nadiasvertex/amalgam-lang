import lib
import llvm
import os
 
VariantDir('build', 'src')

source = ["src/main.cpp"]

base_env = Environment()

# clang scan-build
base_env["CC"] = os.getenv("CC", base_env["CC"])
base_env["CXX"] = os.getenv("CXX", base_env["CXX"])
base_env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))

# Setup debugging and C++11
base_env.Append(CCFLAGS="-g -std=c++0x " + llvm.llvm_cxx_flags + " -fexceptions")
base_env.Append(LIBPATH=lib.library_paths)

# Setup linker flags
base_env.Append(LDFLAGS=llvm.llvm_ld_flags)

# Build
base_env.Program(source=source, LIBS=lib.libraries, target="amalgam")
