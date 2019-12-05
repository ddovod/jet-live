
set(JET_LIVE_CONFIGURED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -MD                   - needed to generate depfiles to track dependencies between files.
# -falign-functions=16  - needed to keep enough space between functions to make each function "patchable", so
#                         with this flag each function size will be not less than 16 bytes.
message("Generator: ${CMAKE_GENERATOR}")
message("Build dir: ${CMAKE_BINARY_DIR}")
if (NOT CMAKE_GENERATOR STREQUAL "Xcode")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -MD ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MD ")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # Not all recent versions of clang support this flag.
  # But looks like clang already aligns functions on the 16 bytes border by default.
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -falign-functions=16 ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -falign-functions=16 ")
endif()

# -Wl,-export-dynamic   - needed to make dynamic linker happy when the shared lib
#                         with new code will be loaded into the process' address space.
# -Wl,-export_dynamic   - same semantics, but for macos.
# -Wl,-flat_namespace   - disables "Two-Level Namespace" feature
if (UNIX AND NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export-dynamic ")
elseif (UNIX AND APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export_dynamic -Wl,-flat_namespace -Wl,-segprot,__TEXT,rwx,rwx ")
else()
  message(FATAL_ERROR "Platform is not supported")
endif()
