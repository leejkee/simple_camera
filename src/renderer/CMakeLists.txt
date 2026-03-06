file (GLOB_RECURSE RENDERER_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(sc_renderer STATIC ${RENDERER_SRC})

target_compile_features(sc_renderer PRIVATE cxx_std_14)

target_include_directories(sc_renderer PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(sc_renderer PUBLIC
        sc_core
        sc_camera
        ${OpenCV_LIBS}
)
