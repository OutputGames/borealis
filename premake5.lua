local resource_packer = ""


if _ACTION:startswith("gmake") then
    resource_packer = "./tools/out/resource_packer.exe"
end

if _ACTION:startswith("vs") then
    resource_packer = ".\\tools\\out\\resource_packer.exe"
end

resource_packer = _MAIN_SCRIPT_DIR .. "/tools/out/resource_packer.exe"


workspace "borealis"
    architecture "x64"
    configurations { 
        "Debug", 
        "Debug (RenderDoc)", 
        "Release"
     }
    startproject "borealis-test"

    filter "system:windows"
        systemversion "latest"
        defines { "_CRT_SECURE_NO_WARNINGS" }


project "borealis"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"
    targetdir "lib"
    objdir "build"
    dependson {"glad", "tinygltf", "resource_packer"}

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp",

        "default_assets/**.frag",
        "default_assets/**.vert",
        "default_assets/**.shdinc",
        "default_assets/**.png",
        "default_assets/**.glb",
    }

    includedirs {
        "include",
        "ext/glad/include",
        "ext/glm",
        "ext/stb/",
        "ext/glfw/include",
        "ext/assimp/include",
        "ext/tinygltf",
        "ext"
    }

    libdirs {
        "ext/glad/lib",
    }

    prebuildcommands { "ls", resource_packer.." default_assets/ out/ default_assets.res" }
    filter { "_ACTION:gmake" }
            libdirs { "ext/glfw/lib-mingw-w64","ext/assimp/lib/mingw/" }


    filter { "_ACTION:vs*" }
            libdirs { "ext/glfw/lib-vc2022", "ext/assimp/lib/vc2022/" }
    links {
        "glad",
        "glfw3",
        "opengl32",
        "gdi32",
    }

    filter "configurations:Debug*"
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

    filter "configurations:Debug*"
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
    cppdialect "C++23"
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
        "include/**.hpp",

        "test/resources/**.frag",
        "test/resources/**.vert",
        "test/resources/**.shdinc",
        "test/resources/**.png",
        "test/resources/**.glb",
    }

    includedirs {
        "include",
        "test/include",
        "ext/glad/include",
        "ext/glm",
        "ext/stb/",
        "ext/glfw/include",
    }

    libdirs {
        "../lib/",
        "ext/glad/lib",
    }

    links {
        "borealis",
        "glad",
        "glfw3",
        "opengl32",
        "gdi32",
    }


    prebuildcommands { resource_packer.." default_assets/ out/ default_assets.res" }
    prebuildcommands { resource_packer.." test/resources/ out/ assets.res" }

    filter { "_ACTION:gmake" }
            libdirs { "ext/glfw/lib-mingw-w64","ext/assimp/lib/mingw/" }
    filter { "_ACTION:vs*" }
            libdirs { "ext/glfw/lib-vc2022", "ext/assimp/lib/vc2022/" }


    filter "configurations:Debug*"
        symbols "On"
        defines { "DEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }
    filter "configurations:Debug (RenderDoc)"
        debugcommand "C:\\Program Files\\RenderDoc\\renderdoccmd.exe"
        debugargs { "capture", "%{prj.name}.exe" }

    filter "configurations:Release"
        optimize "On"
        defines { "NDEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

include("ext/glad/premake5.lua")
include("ext/glfw/premake5.lua")
include("ext/stb/premake5.lua")
include("ext/glm/premake5.lua")
