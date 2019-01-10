
cmake_minimum_required(VERSION 3.6)
project(tests)

file(GLOB TEST_SOURCES ${PROJECT_SOURCE_DIR}/src/**/*.cpp)
add_executable(tests "")
target_sources(tests
  PRIVATE
  ${TEST_SOURCES}
  )
target_include_directories(tests
  PRIVATE
  src
  ../libs/Catch2
  )
target_link_libraries(tests
  PRIVATE
  jet-live
  )
set_target_properties(tests
  PROPERTIES
  CXX_STANDARD 11
  )
target_compile_options(tests
  PRIVATE
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  )

