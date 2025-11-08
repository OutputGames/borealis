workspace "borealis"
    architecture "x64"
    configurations { "Debug", "Release" }
    startproject "borealis-test"

project "borealis"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "lib"
    objdir "build"
    dependson "glad"

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp"
    }

    includedirs {
        "include",
        "ext/glad/include",
        "ext/glm",
        "ext/stb/include",
        "ext/glfw/include"
    }

    libdirs {
        "ext/glad/lib"
    }

    filter { "_ACTION:gmake" }
            libdirs { "ext/glfw/lib-mingw-w64" }
    filter { "_ACTION:vs*" }
            libdirs { "ext/glfw/lib-vc2022" }

    links {
        "glad",
        "glfw3",
        "opengl32",
        "gdi32"
    }

    filter "configurations:Debug"
        symbols "On"
        defines { "DEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

    filter "configurations:Release"
        optimize "On"
        defines { "NDEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

project "borealis-test"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "out"
    objdir "build"
    dependson "borealis"

    files {
        "test/src/**.cpp",
        "test/src/**.c",
        "include/**.h",
        "include/**.hpp"
    }

    includedirs {
        "include",
        "test/include",
        "ext/glad/include",
        "ext/glm",
        "ext/stb/include",
        "ext/glfw/include"
    }

    libdirs {
        "lib/"
    }

    links {
        "borealis"
    }

    filter "configurations:Debug"
        symbols "On"
        defines { "DEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

    filter "configurations:Release"
        optimize "On"
        defines { "NDEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

include("ext/glad/premake5.lua")