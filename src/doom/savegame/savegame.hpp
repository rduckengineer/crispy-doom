#ifndef CRISPY_DOOM_SAVEGAME_HPP
#define CRISPY_DOOM_SAVEGAME_HPP

#include "savegame_context.hpp"

#include "savegame/on_exit.hpp"
#include "savegame/open_mode.hpp"
#include "savegame/overload.hpp"

class SaveGame
{
public:
  SaveGame(SaveGameContext::FileVariant file, bool initialError, std::ostream& err_os)
    : m_has_error(initialError)
    , m_context{m_has_error, err_os, file}
  {
  }

  explicit SaveGame(SaveGameContext context)
    : m_context(context)
  {}

  [[nodiscard]] OpenMode openMode() const;
  [[nodiscard]] bool isOpen() const noexcept {
    return !std::holds_alternative<std::monostate>(m_context.file());
  }
  [[nodiscard]] bool error() const noexcept { return m_context.hasError(); }
  [[nodiscard]] bool stream_error() const noexcept;
  [[nodiscard]] long currentPosition() const noexcept;

  void seekFromStart(long r);
  void seekFromEnd(long r);

  bool readInto(char* buffer);

  template <typename T>
  [[nodiscard]] T read()
  {
    [[maybe_unused]] onExit ex{[this](){logOnError(
        "saveg_read8: Unexpected end of file while reading save game\n");
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
      logOnError("saveg_write8: Error while writing save game\n"); }
    };

    if constexpr (sizeof(T) == 1) {
      write8(value);
    } else if constexpr (sizeof(T) == 2) {
      write16(value);
    } else if constexpr (sizeof(T) == 4) {
      write32(value);
    } else if constexpr (std::is_same_v<T, char const*>) {
      m_context.write() << value;
    }
  }

private:
  uint8_t read8();
  uint16_t read16();
  uint32_t read32();

  void write8(uint8_t value);
  void write16(uint16_t value);
  void write32(uint32_t value);

  void logOnError(std::string_view error_message);

  template <typename R = void, typename ... Visitor>
  R doOnFile(Visitor&& ... v) const;

private:
  bool m_has_error = false;
  SaveGameContext m_context;
};

#endif // CRISPY_DOOM_SAVEGAME_HPP
