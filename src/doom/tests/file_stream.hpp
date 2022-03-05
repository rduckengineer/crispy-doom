#ifndef CRISPY_DOOM_FILE_STREAM_HPP
#define CRISPY_DOOM_FILE_STREAM_HPP

#include "savegame/open_mode.hpp"

#include <array>
#include <memory>
#include <string_view>
#include <algorithm>
#include <stdexcept>

template <size_t N = 2>
struct FileStream {
  using FilePtr = std::unique_ptr<FILE, decltype(&fclose)>;
  using Buffer = std::array<byte, N>;

  FileStream(Buffer buffer, OpenMode open_mode)
      : m_buf(buffer)
      , m_test_file([open_mode, this]() -> FilePtr
                  {
                    switch (open_mode) {
                    case OpenMode::Write: return openFileWrite(m_buf);
                    case OpenMode::Read: return openFileRead(m_buf);
                    default: throw std::runtime_error("Unknown open mode");
                    }
                  }())
  {}

  [[nodiscard]] constexpr size_t size() const { return m_buf.size(); }
  [[nodiscard]] size_t lastNonNullIndex() const {
    auto first = std::find_if(m_buf.rbegin(), m_buf.rend(),
                              [](auto val) { return val != 0; });
    return std::distance(first, m_buf.rend());
  }

  [[nodiscard]] auto buf() const { return m_buf; }

  template <typename T>
  [[nodiscard]] T as() const {
    return *reinterpret_cast<T const*>(m_buf.data());
  }

  [[nodiscard]] FILE* file() { return m_test_file.get(); }
  [[nodiscard]] FILE* file() const { return m_test_file.get(); }

  [[nodiscard]] operator FILE* () { return file(); }
  [[nodiscard]] operator FILE* () const { return file(); }

  byte& operator[](size_t index) { return m_buf[index]; }
  bool has_error() const { return ferror(file()) != 0; }

  std::string_view str() const { return reinterpret_cast<char const*>(m_buf.data()); }

private:
  auto static openFileRead(Buffer &buffer)
      -> std::unique_ptr<FILE, decltype(&fclose)> {
    return {fmemopen(buffer.data(), sizeof(Buffer), "r"), fclose};
  }

  auto static openFileWrite(Buffer &buffer)
      -> std::unique_ptr<FILE, decltype(&fclose)> {
    auto test_file = std::unique_ptr<FILE, decltype(&fclose)>{
        fmemopen(buffer.data(), sizeof(Buffer), "a"), fclose};
    setbuf(test_file.get(), NULL);
    return test_file;
  }

private:
  Buffer m_buf;
  FilePtr m_test_file;
};

#endif // CRISPY_DOOM_FILE_STREAM_HPP
