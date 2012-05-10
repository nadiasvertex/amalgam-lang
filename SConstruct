import cfg
import os
 
is_test = ARGUMENTS.get('test', 0)

include_dirs = []

if not is_test:
    source = ["src/main.cpp"]
    target = "amalgam"
else:
    source = ["test/gtest-all.cc", "test/gtest-main.cc"]
    include_dirs.append("test")
    include_dirs.append("src")
    target = "amalgam-test"
    
base_env = Environment(CPPPATH=include_dirs)

# clang scan-build
base_env["CC"] = os.getenv("CC", base_env["CC"])
base_env["CXX"] = os.getenv("CXX", base_env["CXX"])
base_env["ENV"].update(x for x in os.environ.items() if x[0].startswith("CCC_"))

# Setup debugging and C++11
base_env.Append(CCFLAGS="-g -std=c++0x " + cfg.llvm.llvm_cxx_flags + " -fexceptions")
base_env.Append(LIBPATH=cfg.lib.library_paths)

# Setup linker flags
base_env.Append(LDFLAGS=cfg.llvm.llvm_ld_flags)

# Build
base_env.Program(source=source, LIBS=cfg.lib.libraries, target=target)
