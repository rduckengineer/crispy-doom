add_library(doom_cxx_savegame
        readable_save_file.hpp writable_save_file.hpp
        save_file.hpp save_file.cpp
        savegame_interface.cpp
        extra_savegame.cpp
        open_mode.hpp
        overload.hpp
        on_exit.hpp
        )

target_include_directories(doom_cxx_savegame PRIVATE ".." "../.." "${CMAKE_CURRENT_BINARY_DIR}/../../..")