add_library(doom_cxx_savegame
        savegame_context.hpp
        savegame.hpp savegame.cpp
        savegame_interface.cpp
        extra_savegame.cpp
        open_mode.hpp
        overload.hpp
        on_exit.hpp
        )

target_include_directories(doom_cxx_savegame PRIVATE ".." "../.." "${CMAKE_CURRENT_BINARY_DIR}/../../..")