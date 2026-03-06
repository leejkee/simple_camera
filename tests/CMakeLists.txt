Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.8.1
)

FetchContent_MakeAvailable(Catch2)

include(CTest)
enable_testing()
add_executable(sc_tests test_frame_pool.cpp test_frame_queue.cpp)
target_link_libraries(sc_tests PRIVATE Catch2::Catch2WithMain sc_core)

add_test(NAME sc_tests COMMAND sc_tests)