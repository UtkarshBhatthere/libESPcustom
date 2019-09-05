# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES "nvs_flash")
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "src/wifiMan.cpp" "src/soundMan.cpp" "src/mongoose.c" "src/dumpMan.cpp")

set(COMPONENT_ADD_INCLUDEDIRS "include")

register_component()