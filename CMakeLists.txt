cmake_minimum_required(VERSION 3.24)
project(Project1)

set(CMAKE_CXX_STANDARD 17)

add_executable(Project1 main.cpp ProjectStructure.h Database.cpp Database.h DiskAllocation.cpp DiskAllocation.h BPlusTree.cpp BPlusTree.h databaseStorage.cpp databaseStorage.h
)
