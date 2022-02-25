add_library(doom_cxx_savegame
        savegame.cpp savegame.hpp
        extra_savegame.cpp
        )

target_include_directories(doom_cxx_savegame PRIVATE ".." "../.." "${CMAKE_CURRENT_BINARY_DIR}/../../..")