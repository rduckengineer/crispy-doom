#ifndef CRISPY_DOOM_FILE_STREAM_HPP
#define CRISPY_DOOM_FILE_STREAM_HPP

#include "savegame/open_mode.hpp"

#include <array>
#include <sstream>

template <size_t N = 2>
struct FileStream {
  using Buffer = std::array<byte, N>;

  static std::ios::openmode toStd(OpenMode mode) {
    return mode == OpenMode::Read ? std::ios::in : std::ios::out;
  }

  FileStream(OpenMode openMode, std::stringstream const& source)
      : m_buf{}
      , m_stream{source.str(), toStd(openMode) | std::ios::binary}
  {}

  FileStream(Buffer buffer, OpenMode open_mode)
      : m_buf(buffer)
      , m_stream{ std::string{reinterpret_cast<char*>(buffer.data()), buffer.size()}, toStd(open_mode) | std::ios::binary}
  {}

  [[nodiscard]] constexpr size_t size() const { return m_buf.size(); }
  [[nodiscard]] std::istream& readStream() { return m_stream; }
  [[nodiscard]] std::ostream& writeStream() { return m_stream; }
  [[nodiscard]] std::stringstream& sstream() { return m_stream; }

  template <typename T>
  [[nodiscard]] T as() const {
    return *reinterpret_cast<T const*>(m_stream.str().data());
  }

  byte& operator[](size_t index) { return m_buf[index]; }
  bool has_error() const { return m_stream.fail(); }

  std::string str() const { return m_stream.str().c_str(); }

private:
  Buffer m_buf;
  std::stringstream m_stream;
};

#endif // CRISPY_DOOM_FILE_STREAM_HPP
