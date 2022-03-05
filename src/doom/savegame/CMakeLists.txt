add_library(doom_cxx_savegame
        savegame.cpp savegame.hpp
        extra_savegame.cpp
        open_mode.hpp
        )

target_include_directories(doom_cxx_savegame PRIVATE ".." "../.." "${CMAKE_CURRENT_BINARY_DIR}/../../..")