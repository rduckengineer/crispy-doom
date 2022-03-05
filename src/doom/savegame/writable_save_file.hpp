#ifndef CRISPY_DOOM_WRITABLE_SAVE_FILE_HPP
#define CRISPY_DOOM_WRITABLE_SAVE_FILE_HPP

#include "save_file.hpp"

class WritableSaveFile : public SaveFile {
public:
  WritableSaveFile(SaveFile::FileVariant file)
      : SaveFile(std::get<std::ostream*>(file))
  {}

  WritableSaveFile(std::ostream* file)
      : SaveFile(file)
  {}

  template <typename T>
  void write(T value)
  {
    if constexpr (sizeof(T) == 1)
    {
      asWrite().put(static_cast<char>(value));
    }
    else if constexpr (sizeof(T) == 2)
    {
      write<uint8_t>(value & 0xff);
      write<uint8_t>((value >> 8) & 0xff);
    }
    else if constexpr (sizeof(T) == 4)
    {
      write<uint8_t>(value & 0xff);
      write<uint8_t>((value >> 8) & 0xff);
      write<uint8_t>((value >> 16) & 0xff);
      write<uint8_t>((value >> 24) & 0xff);
    }
    else if constexpr (std::is_same_v<T, char const*>) {
      asWrite() << value;
    }
  }

private:
  std::ostream& asWrite() {
    return *std::get<std::ostream*>(m_file);
  }
};

#endif // CRISPY_DOOM_WRITABLE_SAVE_FILE_HPP
