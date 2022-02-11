extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "file_stream.hpp"

#include "savegame/savegame.hpp"

#include "catch.hpp"

static_assert(sizeof(boolean) == sizeof(int32_t));

// ------------------ 8 BIT SCENARIOS ------------------
// write
SCENARIO("Writing 8 bits in a file", "[write]") {
  GIVEN("An empty file")
  {
    FileStream file_stream{{}, OpenMode::Write};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};

      WHEN("Writing 8 bits in the stream")
      {
        saveg.write8(0x43);

        THEN("There is no error after the write and the written value is correct")
        {
          CHECK_FALSE(saveg.error());
          CHECK(file_stream[0] == 0x43);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      SaveGame saveg{file_stream.file(), true, error_stream.file()};
      WHEN("Writing 8 bits in the stream")
      {
        saveg.write8(0x43);

        THEN("There is still an error after writing and the value was written")
        {
          CHECK(saveg.error());
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
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};
      WHEN("Writing 8 bits in the stream")
      {
        saveg.write8(0x42);

        THEN("There is an error after the write")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str() == "saveg_write8: Error while writing save game\n");
          CHECK(file_stream.has_error());
          CHECK(saveg.error() == true);
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
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};
      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg.read8();

        THEN("There is no error after reading and the result variable has been set with the correct value")
        {
          CHECK_FALSE(saveg.error());
          CHECK(result == 0x13);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      SaveGame saveg{file_stream.file(), true, error_stream.file()};

      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg.read8();

        THEN("There is an error after the reading and the result variable has been set with the correct value")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str().empty());
          CHECK(saveg.error());
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
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      boolean err = false;
      SaveGameContext context{file_stream.file(), &err, error_stream.file()};
      SaveGame saveg{context};
      WHEN("Reading 8 bits in the stream")
      {
        byte result = saveg.read8();
        THEN("There is an error after the reading and the result is -1")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str() == "saveg_read8: Unexpected end of file while reading save game\n");
          CHECK(file_stream.has_error());
          CHECK(err == true);
          CHECK(result == static_cast<byte>(-1));
        }
      }
    }
  }
}