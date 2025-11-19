project "glfw3"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    targetdir "lib"
    objdir "build"

    files {
        "src/**.cpp",
        "src/**.c",
        "include/**.h",
        "include/**.hpp"
    }

    removefiles {
        "src/win32_**",
        "src/posix_**",
        "src/x11_**",
        "src/wgl_**",
        "src/cocoa_**",
        "src/nsgl_**",
        "src/glx_**",
        "src/wl_**",
        "src/xkb_**",
        "src/linux_**",
    }


    filter "system:windows"
        systemversion "latest"
        defines { "_CRT_SECURE_NO_WARNINGS" }
        files {
            "src/win32_**",
            "src/wgl_**",
        }
        links {
            "gdi32"
        }
        defines { "_GLFW_WIN32" }

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
