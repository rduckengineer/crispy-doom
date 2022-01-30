add_executable(doom_tests
        main.cpp catch.hpp
        savegame_test.cpp
)

add_test(NAME savegame_tests COMMAND doom_tests)

target_include_directories(doom_tests PRIVATE ..)
target_include_directories(doom_tests PRIVATE ../..)
target_include_directories(doom_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/../../..")
target_link_libraries(doom_tests PUBLIC doom common)
target_compile_options(doom_tests PUBLIC "-fsanitize=address")
target_link_options(doom_tests PUBLIC "-fsanitize=address")