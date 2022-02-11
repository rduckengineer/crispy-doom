extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "file_stream.hpp"

#include "catch.hpp"

SCENARIO("Writing 16 bits in a file", "[write]") {
  GIVEN("An empty file")
  {
    FileStream<3> file_stream{{}, OpenMode::Write};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      boolean err = false;

      WHEN("Writing 16 bits in the stream")
      {
        saveg_write16_in_stream(file_stream.file(), 0x4364, &err, error_stream.file());

        THEN("There is no error after the write and the written value is correct")
        {
          CHECK(err == false);
          CHECK(file_stream[0] == 0x64);
          CHECK(file_stream[1] == 0x43);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      boolean err = true;
      WHEN("Writing 16 bits in the stream")
      {
        saveg_write16_in_stream(file_stream.file(), 0x4364, &err, error_stream.file());

        THEN("There is still an error after writing and the value was written")
        {
          CHECK(err == true);
          CHECK(file_stream[0] == 0x64);
          CHECK(file_stream[1] == 0x43);
        }
      }
    }
  }
}

SCENARIO("Writing 16 bits in a file fails", "[write]") {
  GIVEN("A file with an error")
  {
    FileStream<3> file_stream{{0,0x37, 0x97}, OpenMode::Read};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Writing 16 bits in the stream")
      {
        saveg_write16_in_stream(file_stream.file(), 0x42, &err, error_stream.file());

        THEN("There is an error after the write")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str() == "saveg_write8: Error while writing save game\n");
          CHECK(file_stream.has_error());
          CHECK(err == true);
          AND_THEN("The file has not been modified")
          {
            CHECK(file_stream[0] == 0);
            CHECK(file_stream[1] == 0x37);
            CHECK(file_stream[2] == 0x97);
          }
        }
      }
    }
  }
}

// Read
SCENARIO("Reading 16 bits in a file", "[read]") {
  GIVEN("A file")
  {
    FileStream<3> file_stream{{0x13,0x37, 0x42}, OpenMode::Read};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Reading 16 bits in the stream")
      {
        int16_t result =
            saveg_read16_in_stream(file_stream.file(), &err, error_stream.file());

        THEN("There is no error after reading and the result variable has been set with the correct value")
        {
          CHECK(err == false);
          CHECK(result == 0x3713);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      boolean err = true;
      WHEN("Reading 16 bits in the stream")
      {
        int16_t result = saveg_read16_in_stream(file_stream.file(), &err, error_stream.file());

        THEN("There is an error after the reading and the result variable has been set with the correct value")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str().empty());
          CHECK(err == true);
          CHECK(result == 0x3713);
        }
      }
    }
  }
}

SCENARIO("Reading 16 bits in a file with an error", "[read]") {
  GIVEN("A file")
  {
    FileStream<3> file_stream{{0x42, 0x52, 0x54}, OpenMode::Write};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      WHEN("Reading 16 bits in the stream")
      {
        int16_t result = saveg_read16_in_stream(file_stream.file(), &err, error_stream.file());
        THEN("There is an error after the reading and the result is -1")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str() == "saveg_read8: Unexpected end of file while reading save game\n");
          CHECK(file_stream.has_error());
          CHECK(err == true);
          CHECK(result == static_cast<int16_t>(-1));
        }
      }
    }
  }
}