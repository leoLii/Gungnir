-- xmake.lua (根目录)
set_project("GungnirEngine")

set_arch("x64")
add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_requires("volk", "vulkan-memory-allocator-hpp", "assimp", "eigen", "robin-map", "shaderc")

-- target("application")
--     set_kind("binary")
--     add_includedirs("$(projectdir)")
--     add_headerfiles("application/*.h")
--     add_files("application/*.cpp")
--     add_deps("core", "driver", "engine")

target("engine")
    set_kind("binary")
    add_includedirs("$(projectdir)")
    add_headerfiles("engine/*.h", "engine/**/*.h")
    add_files("engine/*.cpp", "engine/**/*.cpp")
    add_packages("eigen", "robin_map")
    add_deps("core", "driver")

target("core")
    set_kind("static")
    add_includedirs("$(projectdir)")
    add_headerfiles("core/*.h", "core/**/*.h")
    add_files("core/*.cpp", "core/**/*.cpp")
    add_packages("eigen", "robin_map")

target("driver")
    set_kind("static")
    add_includedirs("$(projectdir)")
    add_headerfiles("driver/**/*.h")
    add_files("driver/**/*.cpp")
    add_packages("eigen", "volk", "vulkan-memory-allocator-hpp", "sahderc")
    add_deps("core")

