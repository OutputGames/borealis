
workspace "borealis"
    architecture "x64"
    configurations { 
        "Debug", 
        "Release"
     }
    filter "system:windows"
        systemversion "latest"
        defines { "_CRT_SECURE_NO_WARNINGS" }
include("projects.lua")