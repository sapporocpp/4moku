# SConstruct
import os

VariantDir("build", "source", duplicate=0)
env = Environment(CXX="clang++", CXXFLAGS=["-std=c++14", "-Wall", "-Wextra"])

add_build_path = lambda filename: os.path.join("build",filename)
env.Program(add_build_path("4moku"), 
            map(add_build_path,["4moku.cpp"]))

