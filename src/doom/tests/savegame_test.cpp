extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "file_stream.hpp"

#include "savegame/savegame.hpp"

#include "catch.hpp"

TEMPLATE_TEST_CASE("Writing in a file", "[write]", int8_t, int16_t, int32_t) {
  GIVEN("An empty file")
  {
    FileStream<sizeof(TestType) + 1> file_stream{{}, OpenMode::Write};
    FileStream<128> error_stream{{}, OpenMode::Write};

    TestType expected = GENERATE(as<TestType>{}, 0,
                                std::numeric_limits<TestType>::min(),
                                std::numeric_limits<TestType>::max(),
                                take(10, random(std::numeric_limits<TestType>::min(),
                                                std::numeric_limits<TestType>::max())));

    AND_GIVEN("No prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};

      WHEN("Writing in the stream")
      {
        saveg.write<TestType>(expected);

        THEN("There is no error after the write and the written value is correct")
        {
          CHECK_FALSE(saveg.error());
          CHECK(file_stream.template as<TestType>() == expected);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      SaveGame saveg{file_stream.file(), true, error_stream.file()};
      WHEN("Writing in the stream")
      {
        saveg.template write<TestType>(expected);
        THEN("There is still an error after writing and the value was written")
        {
          CHECK(saveg.error());
          CHECK(file_stream.template as<TestType>() == expected);
        }
      }
    }
  }
}

TEMPLATE_TEST_CASE("Writing in a file fails", "[write]", int8_t, int16_t, int32_t) {
  GIVEN("A file with an error")
  {
    FileStream<sizeof(TestType) + 1> file_stream{{0,0x37}, OpenMode::Read};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};

      WHEN("Writing in the stream")
      {
        saveg.template write<TestType>(0x4364);

        THEN("There is an error after the write")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str() == "saveg_write8: Error while writing save game\n");

          CHECK(file_stream.has_error());
          CHECK(saveg.error());

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
TEMPLATE_TEST_CASE("Reading in a file", "[read]", int8_t, int16_t, int32_t) {
  GIVEN("A file")
  {
    FileStream<5> file_stream{{0x13,0x37, 0x42}, OpenMode::Read};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};
      WHEN("Reading in the stream")
      {
        auto result = saveg.read<TestType>();

        THEN("There is no error after reading and the result variable has been set with the correct value")
        {
          CHECK(saveg.error() == false);
          CHECK(file_stream.template as<TestType>() == result);
        }
      }
    }

    AND_GIVEN("A prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};
      WHEN("Reading in the stream")
      {
        auto  result = saveg.template read<TestType>();

        THEN("There is an error after the reading and the result variable has been set with the correct value")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str().empty());
          CHECK(saveg.error() == false);
          CHECK(file_stream.template as<TestType>() == result);
        }
      }
    }
  }
}

TEMPLATE_TEST_CASE("Reading in a file with an error", "[read]", int8_t, int16_t, int32_t) {
  GIVEN("A file")
  {
    FileStream<3> file_stream{{0x42, 0x52, 0x54}, OpenMode::Write};
    FileStream<128> error_stream{{}, OpenMode::Write};

    AND_GIVEN("No prior error")
    {
      SaveGame saveg{file_stream.file(), false, error_stream.file()};
      WHEN("Reading in the stream")
      {
        auto  result = saveg.template read<TestType>();

        THEN("There is an error after the reading and the result is -1")
        {
          REQUIRE_FALSE(error_stream.has_error());
          CHECK(error_stream.str() == "saveg_read8: Unexpected end of file while reading save game\n");
          CHECK(file_stream.has_error());
          CHECK(saveg.error());
          CHECK(result == static_cast<int16_t>(-1));
        }
      }
    }
  }
}