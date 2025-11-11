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
        "ext/glfw/include",
        "ext/assimp/include"
    }

    libdirs {
        "ext/glad/lib"
    }

    filter { "_ACTION:gmake" }
            libdirs { "ext/glfw/lib-mingw-w64","ext/assimp/lib/mingw/" }
    filter { "_ACTION:vs*" }
            libdirs { "ext/glfw/lib-vc2022", "ext/assimp/lib/vc2022/" }

    links {
        "glad",
        "glfw3",
        "opengl32",
        "gdi32",
        --"assimp"
    }

    filter "configurations:Debug"
        symbols "On"
        defines { "DEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

    filter "configurations:Release"
        optimize "On"
        defines { "NDEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

project "resource_packer"

    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "tools/out/"
    objdir "build"

    files {
        "tools/src/**.cpp",
        "tools/src/**.c",
        "tools/include/**.h",
        "tools/include/**.hpp"
    }

    includedirs {
        "tools/include",
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
    debugdir "out"
    dependson {"borealis", "resource_packer"}
    files {
        "test/src/**.cpp",
        "test/src/**.c",
        "test/include/**.hpp",
        "test/include/**.h",
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
        "../lib/",
        "ext/glad/lib"
    }

    links {
        "borealis",
        "glad",
        "glfw3",
        "opengl32",
        "gdi32",
        --"assimp"
    }

    filter { "_ACTION:gmake" }
            libdirs { "ext/glfw/lib-mingw-w64","ext/assimp/lib/mingw/" }
            prebuildcommands { "./tools/out/resource_packer.exe test/resources/ out/" }
    filter { "_ACTION:vs*" }
            libdirs { "ext/glfw/lib-vc2022", "ext/assimp/lib/vc2022/" }
            prebuildcommands { ".\\tools\\out\\resource_packer.exe test/resources/ out/" }


    filter "configurations:Debug"
        symbols "On"
        defines { "DEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

    filter "configurations:Release"
        optimize "On"
        defines { "NDEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

include("ext/glad/premake5.lua")