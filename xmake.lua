-- xmake.lua (根目录)
set_project("GungnirEngine")

set_arch("x64")
add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_requires("volk", "vulkan-memory-allocator-hpp", "assimp", "eigen", "robin-map", "shaderc", "boost", "glfw")

target("platform")
    set_kind("binary")
    add_includedirs("$(projectdir)")
    add_headerfiles("platform/*.h")
    add_files("platform/*.cpp")
    add_packages("eigen", "boost", "glfw", "vulkan", "vulkan-memory-allocator-hpp")
    add_deps("common", "driver", "engine")

target("engine")
    set_kind("static")
    add_includedirs("$(projectdir)")
    add_headerfiles("engine/*.h", "engine/**/*.h")
    add_files("engine/*.cpp", "engine/**/*.cpp")
    add_packages("eigen", "robin_map")
    add_deps("common", "driver")

target("common")
    set_kind("static")
    add_includedirs("$(projectdir)")
    add_headerfiles("common/*.h", "common/**/*.h")
    add_files("common/*.cpp", "common/**/*.cpp")
    add_packages("eigen", "robin_map")
    add_defines("GUNGNIR_STATIC")

target("driver")
    set_kind("static")
    add_includedirs("$(projectdir)")
    add_headerfiles("driver/**/*.h")
    add_files("driver/**/*.cpp")
    add_packages("eigen", "volk", "vulkan-memory-allocator-hpp", "shaderc")
    add_deps("common")
    add_defines("GUNGNIR_STATIC")
