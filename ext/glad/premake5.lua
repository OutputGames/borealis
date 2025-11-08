project "glad"
    kind "StaticLib"
    language "C"
    cdialect "C11"
    targetdir "lib"
    objdir "build"

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp"
    }

    includedirs {
        "include/"
    }

    filter "configurations:Debug"
        symbols "On"
        defines { "DEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

    filter "configurations:Release"
        optimize "On"
        defines { "NDEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }