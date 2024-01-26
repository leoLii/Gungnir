-- xmake.lua (根目录)
set_project("GungnirEngine")

set_arch("x64")
add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_requires("vulkan-hpp", "vulkan-memory-allocator-hpp", "assimp", "eigen", "robin-map", "spirv-reflect", "boost", "glfw", "fmt", "zlib")
add_includedirs("$(projectdir)", "$(projectdir)/third-party")
add_linkdirs("$(projectdir)/third-party")

includes("platform/xmake.lua")
includes("engine/xmake.lua")
includes("driver/xmake.lua")
includes("common/xmake.lua")
