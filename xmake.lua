-- xmake.lua (根目录)
set_project("GungnirEngine")

set_arch("x64")
add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_requires("volk", "vulkan-memory-allocator-hpp", "assimp", "glm", "robin-map")

target("core")
    set_kind("static")
    add_includedirs("$(projectdir)")
    add_headerfiles("core/*.h", "core/**/*.h")
    add_files("core/*.cpp", "core/**/*.cpp")
    add_packages("glm", "robin_map")

target("driver")
    set_kind("static")
    add_includedirs("$(projectdir)")
    add_headerfiles("driver/**/*.h")
    add_files("driver/**/*.cpp")
    add_packages("volk", "vulkan-memory-allocator-hpp")
    add_deps("core")