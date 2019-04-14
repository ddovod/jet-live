
solution "jet-live"
    configurations {
        "Debug"
    }
    platforms {
        "x64"
    }
    language "C++"

PROJ_DIR = path.getabsolute("..")
dofile("jet-live.lua")
dofile("example.lua")

