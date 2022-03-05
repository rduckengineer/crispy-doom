extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "file_stream.hpp"

#include "savegame/save_file.hpp"
#include "savegame/writable_save_file.hpp"
#include "savegame/readable_save_file.hpp"

#include "catch.hpp"
#include <sstream>
#include <variant>

TEMPLATE_TEST_CASE("Writing in a file", "[write]", int8_t, int16_t, int32_t) {
  GIVEN("An empty file") {
    FileStream<sizeof(TestType) + 1> file_stream{OpenMode::Write};

    TestType expected = GENERATE(as<TestType>{}, 0,
                                std::numeric_limits<TestType>::min(),
                                std::numeric_limits<TestType>::max(),
                                take(10, random(std::numeric_limits<TestType>::min(),
                                                std::numeric_limits<TestType>::max())));

    WritableSaveFile saveg{file_stream};

    WHEN("Writing in the stream")
    {
      saveg.write<TestType>(expected);

      THEN("There is no error after the write and the written value is correct")
      {
        CHECK(saveg.currentPosition() == sizeof(TestType));
        CHECK(file_stream.template as<TestType>() == expected);
      }
    }
  }
}

TEMPLATE_TEST_CASE("Writing in a file fails", "[write]", int8_t, int16_t, int32_t) {
  GIVEN("A readable file") {
    FileStream<sizeof(TestType) + 1> file_stream{{0,0x37}, OpenMode::Read};

    THEN("Can't open it as write") {
      CHECK_THROWS_AS(WritableSaveFile{file_stream},
                      std::bad_variant_access);
    }
  }
}

// Read
TEMPLATE_TEST_CASE("Reading in a file", "[read]", int8_t, int16_t, int32_t) {
  GIVEN("A file") {
    FileStream<5> file_stream{{0x13,0x37, 0x42}, OpenMode::Read};

    ReadableSaveFile saveg{file_stream};

    WHEN("Reading in the stream")
    {
      auto result = saveg.read<TestType>();

      THEN("There is no error after reading and the result variable has been set with the correct value")
      {
        CHECK(file_stream.template as<TestType>() == result);
      }
    }
  }
}

TEMPLATE_TEST_CASE("Reading in a file with an error", "[read]", int8_t, int16_t, int32_t) {
  GIVEN("A Writable file") {
    FileStream<3> file_stream{{0x42, 0x52, 0x54}, OpenMode::Write};

    THEN("Can't open it as read")
    {
      CHECK_THROWS_AS(ReadableSaveFile{file_stream}, std::bad_variant_access);
    }
  }
}