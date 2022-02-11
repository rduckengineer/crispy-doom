extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "file_stream.hpp"

#include "catch.hpp"
#include <array>

static_assert(sizeof(boolean) == sizeof(int32_t));

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
// ------------------ 8 BIT SCENARIOS ------------------