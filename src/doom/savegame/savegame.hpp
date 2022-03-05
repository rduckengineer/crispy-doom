#ifndef CRISPY_DOOM_SAVEGAME_HPP
#define CRISPY_DOOM_SAVEGAME_HPP


extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
#include "fcntl.h"
}

#include "savegame/open_mode.hpp"

#include <iostream>

struct SaveGameContext
{
  FILE* stream;
  bool& error;
  std::ostream& err_os;
};

byte saveg_read8_from_context(SaveGameContext context);
void saveg_write8_from_context(SaveGameContext context, byte value);
int16_t saveg_read16_from_context(SaveGameContext context);
void saveg_write16_from_context(SaveGameContext context, int16_t value);
int32_t saveg_read32_from_context(SaveGameContext context);
void saveg_write32_from_context(SaveGameContext context, int32_t value);

class SaveGame
{
public:
  explicit SaveGame(FILE *stream, bool initial_error, std::ostream &err_os)
    : m_has_error(initial_error)
    , m_context{stream, m_has_error, err_os}
  {}

  explicit SaveGame(SaveGameContext context)
    : m_context(context)
  {}

  template <typename T>
  void write(T value)
  {
    if constexpr (sizeof(T) == 1) {
      saveg_write8_from_context(m_context, value);
    } else if constexpr (sizeof(T) == 2) {
      saveg_write16_from_context(m_context, value);
    } else if constexpr (sizeof(T) == 4) {
      saveg_write32_from_context(m_context, value);
    } else if constexpr (std::is_same_v<T, char const*>) {
      fputs(value, m_context.stream);
    } else {
      throw std::runtime_error("not implemented!");
    }
  }

  template <typename T>
  [[nodiscard]] T read()
  {
    if constexpr (sizeof(T) == 1) {
      return saveg_read8_from_context(m_context);
    } else if constexpr (sizeof(T) == 2) {
      return saveg_read16_from_context(m_context);
    } else if constexpr (sizeof(T) == 4) {
      return saveg_read32_from_context(m_context);
    }
  }

  bool readInto(char* buffer) {
    static constexpr size_t MAX_LINE_LENGTH = 260;
    return fgets(buffer, MAX_LINE_LENGTH, m_context.stream) != nullptr;
  }

  [[nodiscard]] bool isOpen() const noexcept { return m_context.stream != nullptr; }
  [[nodiscard]] bool error() const noexcept { return m_context.error; }

  long currentPosition() const { return ftell(m_context.stream); }

  void seekFromStart(long offset) { fseek(m_context.stream, offset, SEEK_SET); }
  void seekFromEnd(long offset) { fseek(m_context.stream, offset, SEEK_END); }

private:
  bool m_has_error = false;
  SaveGameContext m_context;
};

#endif // CRISPY_DOOM_SAVEGAME_HPP
