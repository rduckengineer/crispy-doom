#ifndef CRISPY_DOOM_FILE_STREAM_HPP
#define CRISPY_DOOM_FILE_STREAM_HPP

#include <memory>
#include <stdexcept>

enum class OpenMode {
  Read,
  Write,
};

struct FileStream {
  using FilePtr = std::unique_ptr<FILE, decltype(&fclose)>;

  FileStream(std::array<byte, 2> buffer, OpenMode open_mode)
      : buf(buffer)
        , test_file([open_mode, this]() -> FilePtr
                  {
                    switch (open_mode) {
                    case OpenMode::Write: return openFileWrite(buf);
                    case OpenMode::Read: return openFileRead(buf);
                    default: std::runtime_error("Unknown open mode");
                    }
                  }())
  {}

  FILE* file() { return test_file.get(); }
  FILE* file() const { return test_file.get(); }
  byte operator[](size_t index) { return  buf[index]; }
  bool has_error() const { return ferror(file()) != 0; }

private:
  auto static openFileRead(std::array<byte, 2> &buffer)
      -> std::unique_ptr<FILE, decltype(&fclose)> {
    return {fmemopen(buffer.data(), 2 * sizeof(byte), "r"), fclose};
  }

  auto static openFileWrite(std::array<byte, 2> &buffer)
      -> std::unique_ptr<FILE, decltype(&fclose)> {
    auto test_file = std::unique_ptr<FILE, decltype(&fclose)>{
        fmemopen(buffer.data(), 2 * sizeof(byte), "a"), fclose};
    setbuf(test_file.get(), NULL);
    return test_file;
  }

private:
  std::array<byte, 2> buf;
  FilePtr test_file;
};

#endif // CRISPY_DOOM_FILE_STREAM_HPP
