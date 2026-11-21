cmake_minimum_required(VERSION 3.16)
project(street_rod_psp LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
option(NATIVE_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
add_compile_options(-Wall -Wextra -Werror -Wno-misleading-indentation)
if(NATIVE_SANITIZERS)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()

find_package(SDL2 REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(engine_dir "${CMAKE_CURRENT_BINARY_DIR}/engine")
execute_process(
  COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/scripts/prepare_street_rod_engine.py" "${engine_dir}"
  RESULT_VARIABLE prepare_result
)
if(NOT prepare_result EQUAL 0)
  message(FATAL_ERROR "Could not prepare the Street Rod engine")
endif()
set(engine_sources_dir "${engine_dir}/srport/src")
file(GLOB_RECURSE engine_sources "${engine_sources_dir}/*.c")
set_source_files_properties(${engine_sources} PROPERTIES
  COMPILE_OPTIONS "-Wno-error;-Wno-unused-parameter;-Wno-format-truncation;-fno-strict-aliasing"
)

add_library(port_common runtime/common/port_common.c runtime/common/port_keyboard.c)
target_include_directories(port_common PUBLIC runtime/include)
add_library(port_platform runtime/host/port_sdl.c)
target_link_libraries(port_platform PUBLIC port_common SDL2::SDL2)

add_executable(native_street_rod games/street_rod/engine_main.c games/street_rod/engine_host.c ${engine_sources})
target_include_directories(native_street_rod PRIVATE "${engine_sources_dir}" games/street_rod)
target_link_libraries(native_street_rod PRIVATE port_platform m)

enable_testing()
foreach(test IN ITEMS common keyboard palette_cache host)
  add_executable(test_${test} tests/test_${test}.c)
  target_link_libraries(test_${test} PRIVATE port_platform)
  add_test(NAME ${test} COMMAND test_${test})
endforeach()
set_tests_properties(host PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
