extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "catch.hpp"
#include "file_stream.hpp"
#include "savegame/savegame.hpp"
#include <sstream>

std::ostream& operator<<(std::ostream& os, OpenMode mode)
{
  return os << (mode == OpenMode::Write ? "Write" : "Read");
}

TEMPLATE_TEST_CASE("Current position updates with reads, writes and seeks",
                   "[write][read]", int8_t, int16_t, int32_t) {
  std::stringstream err_stream;

  GIVEN("A writable empty file") {
    FileStream<5> file{{}, OpenMode::Write};
    SaveGame save{file, false, err_stream};

    THEN("Starting position is 0") {
      long startPos = current_position_for_file(file);
      CHECK(startPos == 0);
    }

    WHEN("Write N bytes") {
      save.write<TestType>(42);

      THEN("Current position moved forward by N bytes") {
        long position = current_position_for_file(file);
        CHECK(position == sizeof(TestType));
      }
    }

    WHEN("Seeking from start") {
      long offset = GENERATE(Catch::Generators::range(0, 5));
      seek_from_start_in_file(file, offset);
      THEN("Current position is the offset") {
        CHECK(current_position_for_file(file) == offset);
      }
    }

    WHEN("Seeking from end") {
      long offset = GENERATE(Catch::Generators::range(0, 4));
      seek_from_end_in_file(file, offset);
      THEN("Current position is the offset") {
        CHECK(current_position_for_file(file) == 0);
      }
    }
  }

  GIVEN("A writable non-empty file") {
    FileStream<10> file{{1, 1, 1, 1, 0}, OpenMode::Write};
    SaveGame save{file, false, err_stream};

    long expectedStart = 4;

    THEN("Starting position is end of file") {
      long startPos = current_position_for_file(file);
      CHECK(startPos == expectedStart);
    }

    WHEN("Write N bytes") {
      save.write<TestType>(42);

      THEN("Current position moved forward by N bytes") {
        long position = current_position_for_file(file);
        CHECK(position == expectedStart + sizeof(TestType));
      }
    }

    WHEN("Seeking from start") {
      long offset = GENERATE(Catch::Generators::range(0, 5));
      seek_from_start_in_file(file, offset);
      THEN("Current position is the offset") {
        CHECK(current_position_for_file(file) == offset);
      }
    }

    WHEN("Seeking from end") {
      long offset = GENERATE(Catch::Generators::range(0, 4));
      seek_from_end_in_file(file, offset);
      THEN("Current position is the offset") {
        CHECK(current_position_for_file(file) == 4 - offset);
      }
    }
  }

  GIVEN("A readable non-empty file") {
    FileStream<5> file{{1, 1, 1, 1, 0}, OpenMode::Read};
    SaveGame save{file, false, err_stream};

    THEN("Starting position is 0") {
      long startPos = current_position_for_file(file);
      CHECK(startPos == 0);
    }

    WHEN("Reading N bytes") {
        save.read<TestType>();

      THEN("Current position moved forward by N bytes") {
        long position = current_position_for_file(file);
        CHECK(position == sizeof(TestType));
      }
    }

    WHEN("Seeking from start") {
      long offset = GENERATE(Catch::Generators::range(0, 5));
      seek_from_start_in_file(file, offset);
      THEN("Current position is the offset") {
        CHECK(current_position_for_file(file) == offset);
      }
    }

    WHEN("Seeking from end") {
      long offset = GENERATE(Catch::Generators::range(0, 4));
      seek_from_end_in_file(file, offset);
      THEN("Current position is the offset") {
        CHECK(current_position_for_file(file) == 4 - offset);
      }
    }
  }
}

SCENARIO("Open file checking") {
  GIVEN("No file") {
    FILE* file = nullptr;
    THEN("File is not opened") { CHECK_FALSE(is_file_opened(file)); }
  }

  GIVEN("An open file") {
    FileStream<2> fileStream{{}, OpenMode::Read};
    FILE* file = fileStream;
    THEN("File is opened") { CHECK(is_file_opened(file)); }
  }
}