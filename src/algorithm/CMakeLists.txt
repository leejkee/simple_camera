file (GLOB_RECURSE ALGO_SRC CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(sc_algorithm STATIC ${ALGO_SRC})

target_compile_features(sc_algorithm PRIVATE cxx_std_14)

target_include_directories(sc_algorithm PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(sc_algorithm PUBLIC
        sc_core
        ${OpenCV_LIBS}
)
