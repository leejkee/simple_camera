add_executable(sc_app main.cpp)

target_link_libraries(sc_app PRIVATE
        sc_camera
        sc_core
        sc_algorithm
        sc_pipeline
        sc_renderer
)