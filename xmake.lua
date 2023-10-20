-- xmake.lua (根目录)
set_project("Gungnir")

set_arch("x64")
add_rules("mode.debug", "mode.release")
set_languages("c++20")

add_requires("volk", "vulkan-memory-allocator-hpp", "assimp", "glm", "robin-map")

includes("core", "driver","scene", "main")