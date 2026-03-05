file (GLOB_RECURSE CAMERA_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(sc_camera STATIC ${CAMERA_SRC})

target_compile_features(sc_camera PRIVATE cxx_std_14)

target_include_directories(sc_camera PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(sc_camera PUBLIC
        sc_core
)