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

class SaveGame
{
  template <typename Callable>
  struct onExit{
    Callable fn;
    onExit(Callable&& fn_) : fn(std::move(fn_)) {}
    ~onExit() { fn(); }
  };

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
    [[maybe_unused]] onExit ex{[this](){
      if(m_context.read_file->fail()) {
        m_context.err_os << "saveg_read8: Unexpected end of file while "
                          "reading save game\n";
        m_context.error = true;
      }
    }};

    if constexpr (sizeof(T) == 1) {
      return static_cast<T>(read8());
    } else if constexpr (sizeof(T) == 2) {
      return read16();
    } else if constexpr (sizeof(T) == 4) {
      return read32();
    }
  }

  template <typename T>
  void write(T value)
  {
    [[maybe_unused]] onExit ex { [this]() {
      if (m_context.write_file->fail()) {
        m_context.err_os << "saveg_write8: Error while writing save game\n";
        m_context.error = true;
      }
    }};

    if constexpr (sizeof(T) == 1) {
      write8(value);
    } else if constexpr (sizeof(T) == 2) {
      write16(value);
    } else if constexpr (sizeof(T) == 4) {
      write32(value);
    } else if constexpr (std::is_same_v<T, char const*>) {
      assert(m_context.write_file);
      *m_context.write_file << value;
    } else {
      throw std::runtime_error("not implemented!");
    }
  }

private:
  byte read8();
  int16_t read16();
  int32_t read32();

  void write8(byte value);
  void write16(int16_t value);
  void write32(int32_t value);

private:
  bool m_has_error = false;
  SaveGameContext m_context;
};

#endif // CRISPY_DOOM_SAVEGAME_HPP
