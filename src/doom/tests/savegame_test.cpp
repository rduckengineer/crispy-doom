#include "catch.hpp"
#include <array>
#include <memory>

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

static_assert(sizeof(boolean) == sizeof(int32_t));

namespace {
using FilePtr = std::unique_ptr<FILE, decltype(&fclose)>;
auto openFileRead(std::array<byte, 2> &buffer) -> FilePtr;
auto openFileWrite(std::array<byte, 2> &buffer) -> FilePtr;

enum class OpenMode {
  Read,
  Write,
};

struct FileStream {
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
    std::array<byte, 2> buf;
    FilePtr test_file;
};
}

// ------------------ 8 BIT SCENARIOS ------------------
// write
SCENARIO("Writing 8 bits in a file", "[write]") {
  GIVEN("An empty file")
  {
    FileStream file_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      boolean err = false;

      WHEN("Writing 8 bits in the stream")
      {
        saveg_write8_in_stream(file_stream.file(), 0x43, &err);

        THEN("There is no error after the write and the written value is correct")
        {
          CHECK(err == false);
          CHECK(file_stream[0] == 0x43);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      boolean err = true;
      WHEN("Writing 8 bits in the stream")
      {
        saveg_write8_in_stream(file_stream.file(), 0x43, &err);

        THEN("There is still an error after writing and the value was written")
        {
          CHECK(err == true);
          CHECK(file_stream[0] == 0x43);
        }
      }
    }
  }
}

SCENARIO("Writing 8 bits in a file fails", "[write]") {
  GIVEN("A file with an error")
  {
    FileStream file_stream{{0,0x37}, OpenMode::Read};

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Writing 8 bits in the stream")
      {
        saveg_write8_in_stream(file_stream.file(), 0x42, &err);

        THEN("There is an error after the write")
        {
          CHECK(file_stream.has_error());
          CHECK(err == true);
          AND_THEN("The file has not been modified")
          {
            CHECK(file_stream[0] == 0);
            CHECK(file_stream[1] == 0x37);
          }
        }
      }
    }
  }
}

// Read
SCENARIO("Reading 8 bits in a file", "[read]") {
  GIVEN("A file")
  {
    FileStream file_stream{{0x13,0x37}, OpenMode::Read};

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg_read8_in_stream(file_stream.file(), &err);

        THEN("There is no error after reading and the result variable has been set with the correct value")
        {
          CHECK(err == false);
          CHECK(result == 0x13);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      boolean err = true;
      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg_read8_in_stream(file_stream.file(), &err);

        THEN("There is an error after the reading and the result variable has been set with the correct value")
        {
          CHECK(err == true);
          CHECK(result == 0x13);
        }
      }
    }
  }
}

SCENARIO("Reading 8 bits in a file with an error", "[read]") {
  GIVEN("A file")
  {
    FileStream file_stream{{0x42, 0x52}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg_read8_in_stream(file_stream.file(), &err);
        THEN("There is an error after the reading and the result is -1")
        {
          CHECK(file_stream.has_error());
          CHECK(err == true);
          CHECK(result == static_cast<byte>(-1));
        }
      }
    }
  }
}

namespace {
auto openFileRead(std::array<byte, 2> &buffer)
    -> std::unique_ptr<FILE, decltype(&fclose)> {
  return {fmemopen(buffer.data(), 2 * sizeof(byte), "r"), fclose};
}

auto openFileWrite(std::array<byte, 2> &buffer)
    -> std::unique_ptr<FILE, decltype(&fclose)> {
  auto test_file = std::unique_ptr<FILE, decltype(&fclose)>{
      fmemopen(buffer.data(), 2 * sizeof(byte), "a"), fclose};
  setbuf(test_file.get(), NULL);
  return test_file;
}
}
// ------------------ 8 BIT SCENARIOS ------------------