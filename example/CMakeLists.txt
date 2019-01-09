
cmake_minimum_required(VERSION 3.6)
project(example)

add_executable(example "")
target_sources(example
  PRIVATE
  src/main.cpp
  src/ExampleDelegate.cpp
  src/SimpleCommandInterpreter.cpp
  )
target_include_directories(example
  PRIVATE
  src
  )
find_package(Threads REQUIRED)
target_link_libraries(example
  PRIVATE
  jet-live
  Threads::Threads
  )
set_target_properties(example
  PROPERTIES
  CXX_STANDARD 11
  )
target_compile_options(example
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
  $<$<COMPILE_LANGUAGE:C>:-Wall>
  $<$<COMPILE_LANGUAGE:CXX>:-Wall>
  $<$<COMPILE_LANGUAGE:C>:-Wextra>
  $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
  $<$<COMPILE_LANGUAGE:C>:-Wpedantic>
  $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
  $<$<COMPILE_LANGUAGE:C>:-Wshadow>
  $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
  $<$<COMPILE_LANGUAGE:C>:-Wconversion>
  $<$<COMPILE_LANGUAGE:CXX>:-Wconversion>
  )
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(example
    PRIVATE
    $<$<COMPILE_LANGUAGE:C>:-Wvexing-parse>
    $<$<COMPILE_LANGUAGE:CXX>:-Wvexing-parse>
    )
endif()
