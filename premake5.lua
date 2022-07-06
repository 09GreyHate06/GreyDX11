workspace "GreyDX11"
    architecture "x86_64"
    startproject "GreyDX11"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["stb_image"] = "GreyDX11/vendor/stb_image/"
IncludeDir["spdlog"]    = "GreyDX11/vendor/spdlog/include"

project "GreyDX11"
    location "GreyDX11"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.inl",
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.stb_image}",
        "%{prj.name}/vendor",
        "%{IncludeDir.spdlog}",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "GDX11_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "GDX11_RELEASE"
        runtime "Release"
        optimize "on"