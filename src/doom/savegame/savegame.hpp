#ifndef CRISPY_DOOM_SAVEGAME_HPP
#define CRISPY_DOOM_SAVEGAME_HPP


extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "savegame/open_mode.hpp"

#include <iostream>
#include <cassert>

struct SaveGameContext
{
  bool& error;
  std::ostream& err_os;
  std::istream* read_file;
  std::ostream* write_file;
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
  explicit SaveGame(std::istream &is, bool initial_error, std::ostream &err_os)
      : m_has_error(initial_error)
      , m_context{m_has_error, err_os, &is, nullptr}
  {}

  explicit SaveGame(std::ostream &os, bool initial_error, std::ostream &err_os)
      : m_has_error(initial_error)
        , m_context{m_has_error, err_os, nullptr, &os}
  {}

  explicit SaveGame(SaveGameContext context)
    : m_context(context)
  {}

  [[nodiscard]] OpenMode openMode() const;
  [[nodiscard]] bool isOpen() const noexcept {
    return m_context.read_file || m_context.write_file;
  }
  [[nodiscard]] bool error() const noexcept { return m_context.error; }
  [[nodiscard]] bool stream_error() const noexcept;
  [[nodiscard]] long currentPosition() const noexcept;

  void seekFromStart(long offset);
  void seekFromEnd(long offset);

  bool readInto(char* buffer);
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
      assert(m_context.write_file);
      *m_context.write_file << value;
    } else {
      throw std::runtime_error("not implemented!");
    }
  }

private:
  bool m_has_error = false;
  SaveGameContext m_context;
};

#endif // CRISPY_DOOM_SAVEGAME_HPP
