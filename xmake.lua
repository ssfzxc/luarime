add_rules("mode.debug", "mode.release")

add_requires("rime")
add_requires("lua")


target("luarime")
    set_kind("shared")
    add_files("src/*.c")
    add_packages("rime", "lua")

