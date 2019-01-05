
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
target_link_libraries(example
  PRIVATE
  jet-live
  pthread
  )
set_target_properties(example
  PROPERTIES
  CXX_STANDARD 11
  )
