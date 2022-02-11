#ifndef CRISPY_DOOM_SAVEGAME_HPP
#define CRISPY_DOOM_SAVEGAME_HPP

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

class SaveGame
{
public:
  explicit SaveGame(FILE* stream, bool initial_error, FILE* error_stream)
    : m_has_error(initial_error)
    , m_context{stream, &m_has_error, error_stream}
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
    }
  }

  template <typename T>
  T read()
  {
    if constexpr (sizeof(T) == 1) {
      return saveg_read8_from_context(m_context);
    } else if constexpr (sizeof(T) == 2) {
      return saveg_read16_from_context(m_context);
    } else if constexpr (sizeof(T) == 4) {
      return saveg_read32_from_context(m_context);
    }
  }

  void write8(byte value) {
    saveg_write8_from_context(m_context, value);
  }

  byte read8() {
    return saveg_read8_from_context(m_context);
  }

  [[nodiscard]] bool error() const noexcept { return static_cast<bool>(*m_context.error); }

private:
  boolean m_has_error = false;
  SaveGameContext m_context;
};

#endif // CRISPY_DOOM_SAVEGAME_HPP
