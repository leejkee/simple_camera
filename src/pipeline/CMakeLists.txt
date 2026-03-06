file (GLOB_RECURSE PIPELINE_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(sc_pipeline STATIC ${PIPELINE_SRC})

target_compile_features(sc_pipeline PRIVATE cxx_std_14)

target_include_directories(sc_pipeline PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(sc_pipeline PUBLIC
        sc_core
        sc_camera
        sc_algorithm
        ${OpenCV_LIBS}
)
