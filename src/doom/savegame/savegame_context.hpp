//
// Created by rduck on 05/03/2022.
//

#ifndef CRISPY_DOOM_SAVEGAME_CONTEXT_HPP
#define CRISPY_DOOM_SAVEGAME_CONTEXT_HPP

#include <variant>
#include <iosfwd>
#include <string_view>

struct SaveGameContext
{
  using FileVariant = std::variant<std::istream*, std::ostream*, std::monostate>;

  SaveGameContext(bool& error, std::ostream& err_os, FileVariant file)
      : m_error{error}
      , err_os{err_os}
      , m_file{file}
  {}

  [[nodiscard]] auto file() const -> FileVariant { return m_file; }
  [[nodiscard]] auto read() const -> std::istream& { return *std::get<std::istream*>(file()); }
  [[nodiscard]] auto write() const -> std::ostream& { return *std::get<std::ostream*>(file()); }

  [[nodiscard]] bool hasError() const { return m_error; }
  void resetError() { m_error = false; }
  void logError(std::string_view err_msg);

private:
  bool& m_error;
  std::ostream& err_os;
  FileVariant m_file;
};

#endif // CRISPY_DOOM_SAVEGAME_CONTEXT_HPP
