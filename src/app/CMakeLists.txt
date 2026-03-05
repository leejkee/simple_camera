add_executable(sc_app main.cpp)

target_link_libraries(sc_app PRIVATE
        sc_camera
        sc_core
)