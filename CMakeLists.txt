cmake_minimum_required(VERSION 3.16)
project(adaptive_gamma C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- options -------------------------------------------------------------
option(AG_SANITIZE "Build with AddressSanitizer + UBSan (Linux/macOS/clang)" OFF)
option(AG_WERROR   "Treat compiler warnings as errors (use in CI)"           OFF)

enable_testing()

# ---- warnings and sanitizers (must come before any target is created) ----
if(MSVC)
  add_compile_options(/W4)
  if(AG_WERROR)
    add_compile_options(/WX)
  endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wtype-limits)
  if(AG_WERROR)
    add_compile_options(-Werror)
  endif()
  if(AG_SANITIZE)
    if(MINGW)
      message(FATAL_ERROR
        "AG_SANITIZE is not supported with MinGW GCC. "
        "Run the sanitizer build on Linux/WSL, or use clang.")
    endif()
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
  endif()
endif()

# ---- library: every src/*.c -----------------------------------------------
file(GLOB LIB_SOURCES CONFIGURE_DEPENDS src/*.c)
add_library(agamma ${LIB_SOURCES})
target_include_directories(agamma PUBLIC include)
if(UNIX)
  target_link_libraries(agamma PUBLIC m)
endif()

# ---- tests: every tests/test_*.c becomes an executable and a ctest entry ---
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/test_*.c)
foreach(t ${TEST_SOURCES})
  get_filename_component(name ${t} NAME_WE)
  add_executable(${name} ${t})
  target_link_libraries(${name} PRIVATE agamma)
  add_test(NAME ${name} COMMAND ${name})
endforeach()
