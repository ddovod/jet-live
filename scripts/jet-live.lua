
configuration "not xcode*"
    buildoptions {
        "-MD"
    }

configuration "macosx"
    linkoptions {
        "-export_dynamic",
        "-flat_namespace",
    }

configuration "linux"
    linkoptions {
        "-export-dynamic",
    }

-- TODO: set "-falign-functions=16" only for GCC

project "lib_subhook"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/subhook")
    }
    files {
        path.join(PROJ_DIR, "libs/subhook/subhook.c")
    }

project "lib_whereami"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/whereami/src")
    }
    files {
        path.join(PROJ_DIR, "libs/whereami/src/whereami.c")
    }

project "lib_efsw"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/efsw/src"),
        path.join(PROJ_DIR, "libs/efsw/include"),
    }
    files {
        path.join(PROJ_DIR, "libs/efsw/src/efsw/Debug.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/DirectorySnapshot.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/DirectorySnapshotDiff.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/DirWatcherGeneric.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/FileInfo.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/FileSystem.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/FileWatcher.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/FileWatcherCWrapper.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/FileWatcherGeneric.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/FileWatcherImpl.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/Log.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/Mutex.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/String.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/System.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/Thread.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/Watcher.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/WatcherGeneric.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/platform/posix/FileSystemImpl.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/platform/posix/MutexImpl.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/platform/posix/SystemImpl.cpp"),
        path.join(PROJ_DIR, "libs/efsw/src/efsw/platform/posix/ThreadImpl.cpp"),
    }
    configuration "linux"
        files {
            path.join(PROJ_DIR, "libs/efsw/src/efsw/FileWatcherInotify.cpp"),
            path.join(PROJ_DIR, "libs/efsw/src/efsw/WatcherInotify.cpp"),
        }
    configuration "macosx"
        files {
            path.join(PROJ_DIR, "libs/efsw/src/efsw/FileWatcherFSEvents.cpp"),
            path.join(PROJ_DIR, "libs/efsw/src/efsw/WatcherFSEvents.cpp"),
        }
        links {
            "CoreServices.framework",
            "CoreFoundation.framework",
        }

project "lib_tiny-process-library"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/tiny-process-library")
    }
    files {
        path.join(PROJ_DIR, "libs/tiny-process-library/process.cpp"),
        path.join(PROJ_DIR, "libs/tiny-process-library/process_unix.cpp"),
    }
    buildoptions {
        "-std=c++11"
    }

project "lib_teenypath"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/teenypath/include")
    }
    files {
        path.join(PROJ_DIR, "libs/teenypath/src/teenypath.cpp")
    }
    buildoptions {
        "-std=c++11"
    }

project "lib_json"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/json")
    }

project "lib_argh"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/argh/include")
    }

project "lib_xxhash"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "libs/xxHash")
    }
    files {
        path.join(PROJ_DIR, "libs/xxHash/xxhash.c")
    }

project "jet-live"
    kind "StaticLib"
    includedirs {
        path.join(PROJ_DIR, "src"),
        path.join(PROJ_DIR, "libs/utils"),
        path.join(PROJ_DIR, "libs/subhook"),
        path.join(PROJ_DIR, "libs/whereami/src"),
        path.join(PROJ_DIR, "libs/efsw/include"),
        path.join(PROJ_DIR, "libs/tiny-process-library"),
        path.join(PROJ_DIR, "libs/teenypath/include"),
        path.join(PROJ_DIR, "libs/json"),
        path.join(PROJ_DIR, "libs/argh/include"),
        path.join(PROJ_DIR, "libs/xxHash"),
    }
    files {
        path.join(PROJ_DIR, "src/jet/live/Utility.cpp"),
        path.join(PROJ_DIR, "src/jet/live/DefaultSymbolsFilter.cpp"),
        path.join(PROJ_DIR, "src/jet/live/FileWatcher.cpp"),
        path.join(PROJ_DIR, "src/jet/live/CompileCommandsCompilationUnitsParser.cpp"),
        path.join(PROJ_DIR, "src/jet/live/Compiler.cpp"),
        path.join(PROJ_DIR, "src/jet/live/Live.cpp"),
        path.join(PROJ_DIR, "src/jet/live/DepfileDependenciesHandler.cpp"),
        path.join(PROJ_DIR, "src/jet/live/CodeReloadPipeline.cpp"),
        path.join(PROJ_DIR, "src/jet/live/LinkTimeRelocationsStep.cpp"),
        path.join(PROJ_DIR, "src/jet/live/FunctionsHookingStep.cpp"),
        path.join(PROJ_DIR, "src/jet/live/StaticsCopyStep.cpp"),
        path.join(PROJ_DIR, "src/jet/live/SignalReloader.cpp"),
        path.join(PROJ_DIR, "src/jet/live/AsyncEventQueue.cpp"),
        path.join(PROJ_DIR, "src/jet/live/BuildConfig.cpp"),
    }
    links {
        "lib_subhook",
        "lib_whereami",
        "lib_teenypath",
        "lib_json",
        "lib_argh",
        "lib_xxhash",
        "lib_efsw",
        "lib_tiny-process-library",
        "dl",
    }
    buildoptions {
        "-std=c++11",
    }
    linkoptions {
        "-lpthread"
    }
    defines {
        "JET_LIVE_CMAKE_BUILD_DIR=\"" .. _WORKING_DIR .. "\"",
    }
    configuration "linux"
        files {
            path.join(PROJ_DIR, "src/jet/live/_linux/ElfProgramInfoLoader.cpp"),
            path.join(PROJ_DIR, "src/jet/live/_linux/Utility.cpp"),
        }
        includedirs { path.join(PROJ_DIR, "libs/ELFIO") }
    configuration "macosx"
        files {
            path.join(PROJ_DIR, "src/jet/live/_macos/MachoProgramInfoLoader.cpp"),
            path.join(PROJ_DIR, "src/jet/live/_macos/Utility.cpp"),
        }
    configuration "xcode*"
        defines {
            "JET_LIVE_CMAKE_GENERATOR=Xcode",
        }
    configuration "gmake"
        defines {
            "JET_LIVE_CMAKE_GENERATOR=\"gmake\"",
        }
    configuration {}
