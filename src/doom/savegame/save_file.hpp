#ifndef CRISPY_DOOM_SAVE_FILE_HPP
#define CRISPY_DOOM_SAVE_FILE_HPP

#include "savegame/on_exit.hpp"
#include "savegame/open_mode.hpp"
#include "savegame/overload.hpp"

#include <iosfwd>
#include <cstdint>

class SaveFile {
public:
  using FileVariant = std::variant<std::istream*, std::ostream*, std::monostate>;

  SaveFile(FileVariant file)
    : m_file{file}
  {}

  [[nodiscard]] OpenMode openMode() const;
  [[nodiscard]] bool isOpen() const noexcept {
    return !std::holds_alternative<std::monostate>(m_file);
  }
  [[nodiscard]] bool stream_error() const noexcept;
  [[nodiscard]] long currentPosition() const noexcept;

  void seekFromStart(long r);
  void seekFromEnd(long r);

private:
  template <typename R = void, typename ... Visitor>
  R doOnFile(Visitor&& ... v) const;

protected:
  FileVariant m_file;
};

#endif // CRISPY_DOOM_SAVE_FILE_HPP
