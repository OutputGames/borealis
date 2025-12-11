
local resource_packer = _SCRIPT_DIR .. "/tools/out/resource_packer.exe"


project "borealis"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"
    targetdir "lib"
    objdir "build"
    dependson {"glad", "tinygltf", "resource_packer", "glfw"}

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

    prebuildcommands { resource_packer.." ".._SCRIPT_DIR.."/default_assets/ ".._MAIN_SCRIPT_DIR.."/out/default_assets.res" }

    postbuildcommands {"setx BOREALIS_DIR \"".._SCRIPT_DIR .. "\""}

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

    debugargs {_SCRIPT_DIR.."/default_assets/",_MAIN_SCRIPT_DIR.."/out/default_assets.res"}

    filter "configurations:Debug*"
        symbols "On"
        defines { "DEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }

    filter "configurations:Release"
        optimize "On"
        defines { "NDEBUG" }
        buildoptions { "-Wall", "-fcompare-debug-second" }


include("ext/glad/premake5.lua")
include("ext/glfw/premake5.lua")
include("ext/stb/premake5.lua")
include("ext/glm/premake5.lua")
