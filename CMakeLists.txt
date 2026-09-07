cmake_minimum_required(VERSION 3.20)

if(POLICY CMP0144)
  cmake_policy(SET CMP0144 NEW)
endif()

file(GLOB TASK_DIR ${CMAKE_SOURCE_DIR}/semester_*/*/${TASK})

if(NOT TASK OR NOT TASK_DIR)
  message(FATAL_ERROR "Задача '${TASK}' не найдена")
endif()

project(iu4_fpga CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(verilator REQUIRED HINTS $ENV{VERILATOR_ROOT})

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
    GIT_SHALLOW TRUE
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/.deps/googletest-src
    SUBBUILD_DIR ${CMAKE_SOURCE_DIR}/.deps/googletest-subbuild
    BINARY_DIR ${CMAKE_BINARY_DIR}/_deps/googletest-build
)

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE CPP_SOURCES ${TASK_DIR}/*.cpp)
file(GLOB_RECURSE VERILOG_SOURCES ${TASK_DIR}/*.v)

add_executable(${TASK} ${CPP_SOURCES})

target_include_directories(${TASK} PRIVATE ${CMAKE_SOURCE_DIR}/common)

target_link_libraries(${TASK} PRIVATE GTest::gtest_main)

set(VERILATOR_COMMON_ARGS
    -Wall
    --x-assign unique
    --x-initial unique
    --timing
    --timescale-override 1ns/1ps
)

add_custom_target(lint_${TASK} ALL
    COMMAND ${VERILATOR_BIN} --lint-only ${VERILATOR_COMMON_ARGS} ${VERILOG_SOURCES}
    COMMENT "Verilator lint: ${TASK}"
    VERBATIM
)

add_dependencies(${TASK} lint_${TASK})

verilate(${TASK}
    TRACE_VCD
    VERILATOR_ARGS ${VERILATOR_COMMON_ARGS}
    SOURCES ${VERILOG_SOURCES}
)

file(WRITE ${TASK_DIR}/.clangd "\
CompileFlags:
  CompilationDatabase: ${CMAKE_BINARY_DIR}
")
