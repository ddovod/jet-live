
project "example"
    kind "ConsoleApp"

    includedirs {
        path.join(PROJ_DIR, "example/src"),
        path.join(PROJ_DIR, "src"),
        path.join(PROJ_DIR, "libs/efsw/include"),
        path.join(PROJ_DIR, "libs/tiny-process-library"),
        path.join(PROJ_DIR, "libs/utils"),
    }

    files {
        path.join(PROJ_DIR, "example/src/main.cpp"),
        path.join(PROJ_DIR, "example/src/ExampleListener.cpp"),
        path.join(PROJ_DIR, "example/src/SimpleCommandInterpreter.cpp"),
    }

    links {
        "jet-live",
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

    linkoptions {
        "-lpthread",
    }
