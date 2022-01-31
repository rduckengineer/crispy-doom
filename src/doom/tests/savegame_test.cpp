#include "catch.hpp"
#include <array>
#include <memory>

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

static_assert(sizeof(boolean) == sizeof(int32_t));

namespace {
auto openFileRead(std::array<byte, 2> &buffer) -> std::unique_ptr<FILE, decltype(&fclose)>;
auto openFileWrite(std::array<byte, 2> &buffer) -> std::unique_ptr<FILE, decltype(&fclose)>;
}

// ------------------ 8 BIT SCENARIOS ------------------
// write
SCENARIO("Writing 8 bits in a file", "[write]") {
  GIVEN("An empty file")
  {
    std::array<byte, 2> buf{};
    auto test_file = openFileWrite(buf);

    AND_GIVEN("No prior error")
    {
      boolean err = false;

      WHEN("Writing 8 bits in the stream")
      {
        saveg_write8_in_stream(test_file.get(), 0x43, &err);

        THEN("There is no error after the write and the written value is correct")
        {
          CHECK(err == false);
          CHECK(buf[0] == 0x43);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      boolean err = true;
      WHEN("Writing 8 bits in the stream")
      {
        saveg_write8_in_stream(test_file.get(), 0x43, &err);

        THEN("There is still an error after writing and the value was written")
        {
          CHECK(err == true);
          CHECK(buf[0] == 0x43);
        }
      }
    }
  }
}

SCENARIO("Writing 8 bits in a file fails", "[write]") {
  GIVEN("A file with an error")
  {
    std::array<byte, 2> buf{0,0x37};
    auto test_file = openFileRead(buf);

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Writing 8 bits in the stream")
      {
        saveg_write8_in_stream(test_file.get(), 0x42, &err);

        auto error_from_file = ferror(test_file.get());
        THEN("There is an error after the write")
        {
          CHECK(error_from_file != 0);
          CHECK(err == true);
          AND_THEN("The file has not been modified")
          {
            CHECK(buf[0] == 0);
            CHECK(buf[1] == 0x37);
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
    std::array<byte, 2> buf{0x13,0x37};
    auto test_file = openFileRead(buf);

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg_read8_in_stream(test_file.get(), &err);

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
        byte result = saveg_read8_in_stream(test_file.get(), &err);

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
    std::array<byte, 2> buf{0x42, 0x52};
    auto test_file = openFileWrite(buf);
    setbuf(test_file.get(), NULL);

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg_read8_in_stream(test_file.get(), &err);
        THEN("There is an error after the reading and the result is -1")
        {
          CHECK(ferror(test_file.get()) != 0);
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