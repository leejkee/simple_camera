file (GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(sc_core STATIC ${CORE_SRC})

target_compile_features(sc_core PRIVATE cxx_std_14)

target_include_directories(sc_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)