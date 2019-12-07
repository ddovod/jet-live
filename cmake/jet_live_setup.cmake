
set(JET_LIVE_CONFIGURED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)   # enables compile_commands.json generation
if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(FATAL_ERROR "jet-live works correctly only on Debug configuration")
endif()

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
# -Wl,-rename_section,__TEXT,__text,__JET_TEXT,__text
#                       - moves __TEXT.__text section to the new section __JET_TEXT,__text.
#                         With this flag __JET_TEXT.__text will contain executable code.
#                         This is a workaround to overcome macOS 10.15 strictness to
#                         the __TEXT segment access rights.
# -Wl,-segprot,__JET_TEXT,rwx,rwx
#                       - sets minimum and maximum access rights for the __JET_TEXT segment.
if (UNIX AND NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export-dynamic ")
elseif (UNIX AND APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-export_dynamic -Wl,-flat_namespace -Wl,-rename_section,__TEXT,__text,__JET_TEXT,__text -Wl,-segprot,__JET_TEXT,rwx,rwx ")
else()
  message(FATAL_ERROR "Platform is not supported")
endif()
