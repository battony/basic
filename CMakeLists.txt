cmake_minimum_required(VERSION 3.10)

project(basic LANGUAGES CXX ASM)

set(basic-include include)
file(GLOB_RECURSE basic-sources include/*.cxx include/*Amd64.s)

set(basic-compile-options-optimize -O3 -DNDEBUG)
set(basic-compile-options-sanitize -fno-sanitize=all)
set(basic-compile-options-warnings
    -Werror -pedantic-errors
    -Wall -Wextra -Wpedantic
    -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy
    -Wduplicated-branches -Wduplicated-cond -Wextra-semi -Wfloat-equal
    -Wlogical-op -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual
    -Wredundant-decls -Wsign-conversion -Wsign-promo)
set(basic-compile-options ${basic-compile-options-optimize}
    ${basic-compile-options-sanitize} ${basic-compile-options-warnings})

add_library(basic STATIC)
target_sources(basic PRIVATE ${basic-sources})
target_include_directories(basic PUBLIC ${basic-include})
target_compile_features(basic PRIVATE cxx_std_20)
target_compile_options(basic PRIVATE ${basic-compile-options})
target_link_options(basic PRIVATE ${basic-compile-options})

add_executable(basic_exe)
target_sources(basic_exe PRIVATE main.cxx)
target_compile_features(basic_exe PRIVATE cxx_std_20)
target_compile_options(basic_exe PRIVATE ${basic-compile-options})
target_link_options(basic_exe PRIVATE ${basic-compile-options})
target_link_libraries(basic_exe PRIVATE basic)
