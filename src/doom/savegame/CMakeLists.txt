add_library(doom_cxx_savegame
        savegame.cpp savegame.hpp
        )

target_include_directories(doom_cxx_savegame PRIVATE ".." "../.." "${CMAKE_CURRENT_BINARY_DIR}/../../..")