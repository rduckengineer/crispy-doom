#ifndef CRISPY_DOOM_FILE_STREAM_HPP
#define CRISPY_DOOM_FILE_STREAM_HPP

#include <array>
#include <memory>
#include <string_view>
#include <stdexcept>

enum class OpenMode {
  Read,
  Write,
};

template <size_t N = 2>
struct FileStream {
  using FilePtr = std::unique_ptr<FILE, decltype(&fclose)>;
  using Buffer = std::array<byte, N>;

  FileStream(Buffer buffer, OpenMode open_mode)
      : buf(buffer)
      , test_file([open_mode, this]() -> FilePtr
                  {
                    switch (open_mode) {
                    case OpenMode::Write: return openFileWrite(buf);
                    case OpenMode::Read: return openFileRead(buf);
                    default: throw std::runtime_error("Unknown open mode");
                    }
                  }())
  {}

  template <typename T>
  [[nodiscard]] T as() const {
    return *reinterpret_cast<T const*>(buf.data());
  }

  [[nodiscard]] FILE* file() { return test_file.get(); }
  [[nodiscard]] FILE* file() const { return test_file.get(); }

  [[nodiscard]] operator FILE* () { return file(); }
  [[nodiscard]] operator FILE* () const { return file(); }

  byte& operator[](size_t index) { return  buf[index]; }
  bool has_error() const { return ferror(file()) != 0; }

  std::string_view str() const { return reinterpret_cast<char const*>(buf.data()); }

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
  Buffer buf;
  FilePtr test_file;
};

#endif // CRISPY_DOOM_FILE_STREAM_HPP
