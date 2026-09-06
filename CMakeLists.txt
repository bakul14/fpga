cmake_minimum_required(VERSION 3.17)

if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

file(GLOB TASK_DIR ${CMAKE_SOURCE_DIR}/semester_*/*/${TASK})

if(NOT TASK OR NOT TASK_DIR)
  message(FATAL_ERROR "Задача '${TASK}' не найдена")
endif()

project(iu4_fpga CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(verilator REQUIRED HINTS $ENV{VERILATOR_ROOT})
find_package(GTest REQUIRED)

file(GLOB_RECURSE CPP_SOURCES ${TASK_DIR}/*.cpp)
file(GLOB_RECURSE VERILOG_SOURCES ${TASK_DIR}/*.v)

add_executable(${TASK} ${CPP_SOURCES})

target_include_directories(${TASK} PRIVATE ${CMAKE_SOURCE_DIR}/common)

target_link_libraries(${TASK} PRIVATE GTest::gtest_main)

verilate(${TASK}
    TRACE_VCD
    VERILATOR_ARGS -Wall
    SOURCES ${VERILOG_SOURCES}
)

file(WRITE ${TASK_DIR}/.clangd "\
CompileFlags:
  CompilationDatabase: ${CMAKE_BINARY_DIR}
")
