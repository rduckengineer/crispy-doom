#ifndef CRISPY_DOOM_READABLE_SAVE_FILE_HPP
#define CRISPY_DOOM_READABLE_SAVE_FILE_HPP

#include "save_file.hpp"
#include <iostream>

class ReadableSaveFile : public SaveFile
{
  public:
    ReadableSaveFile(SaveFile::FileVariant file)
        : ReadableSaveFile(std::get<std::istream*>(file))
    {
    }

    ReadableSaveFile(std::istream* file)
        : SaveFile(file)
    {
    }

    template <typename T> [[nodiscard]] T read()
    {
        if constexpr (sizeof(T) == 1) { return asRead().get(); }
        else if constexpr (sizeof(T) == 2)
        {
            auto result = static_cast<uint16_t>(read<uint8_t>());
            return result | read<uint8_t>() << 8;
        }
        else if constexpr (sizeof(T) == 4)
        {
            uint32_t result = read<uint8_t>();
            result |= read<uint8_t>() << 8;
            result |= read<uint8_t>() << 16;
            return result | read<uint8_t>() << 24;
        }
    }

    bool readLineInto(char* buffer)
    {
        static constexpr size_t MAX_LINE_LENGTH = 260;
        auto&                   file = asRead();

        file.getline(buffer, MAX_LINE_LENGTH);

        bool done = file.fail();
        // We do read until the end of the file, but EOF triggers a failbit
        // which triggers fails on subsequent reads...
        file.clear();

        return !done;
    }

  private:
    std::istream& asRead() { return *std::get<std::istream*>(m_file); }
};

#endif // CRISPY_DOOM_READABLE_SAVE_FILE_HPP
