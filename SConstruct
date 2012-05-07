source = ["src/main.cpp"]
libraries = ["readline"]

base_env = Environment()
base_env.Append(CCFLAGS="-g -std=c++0x")

base_env.Program(source=source, LIBS=libraries, target="amalgam")
