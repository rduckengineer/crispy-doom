extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}

#include "catch.hpp"
#include "file_stream.hpp"
#include "savegame/save_file.hpp"
#include "savegame/readable_save_file.hpp"
#include "savegame/writable_save_file.hpp"
#include <sstream>

std::ostream& operator<<(std::ostream& os, OpenMode mode)
{
  return os << (mode == OpenMode::Write ? "Write" : "Read");
}

TEMPLATE_TEST_CASE("Current position updates with reads, writes and seeks",
                   "[write][read]", int8_t, int16_t, int32_t) {
  GIVEN("A file") {
    auto mode = GENERATE(OpenMode::Read, OpenMode::Write);
    FileStream<10> file{{1, 1, 1, 1, 0}, mode};
    file.throwOnFail();

    SaveFile save{file};

    THEN("The open mode is correctly detected") {
      CHECK(save.openMode() == mode);
    }

    THEN("Starting position is 0") {
      long startPos = save.currentPosition();
      CHECK(startPos == 0);
    }

    WHEN("Seeking from start") {
      long offset = GENERATE(Catch::Generators::range(0, 5));
      save.seekFromStart(offset);
      THEN("Current position is the offset") {
        CHECK(save.currentPosition() == offset);
      }
    }

    WHEN("Seeking from end") {
      long offset = GENERATE(Catch::Generators::range(0, 4));
      save.seekFromEnd(-offset);

      THEN("Current position is the end minus the offset") {
        CHECK(save.currentPosition() == file.size() - offset);
      }
    }
  }
}

SCENARIO("Writing/Reading a line string") {
  GIVEN("A writable file") {
    FileStream<512> fileW{OpenMode::Write};
    WritableSaveFile saveW{fileW};

    WHEN("Writing out a string") {
      static constexpr std::string_view text{"test\n"};
      saveW.write(text.data());

      THEN("The offset has moved by the string size") {
        CHECK(saveW.currentPosition() == text.size());
        CHECK(fileW.str() == text);
      }

      GIVEN("A readable fileW") {
        FileStream fileR{fileW.sstream(), OpenMode::Read};
        ReadableSaveFile saveR{fileR};

        WHEN("Reading a line") {
          std::array<char, 260> buffer{};

          bool didRead = saveR.readLineInto(buffer.data());
          CHECK(didRead);

          THEN("The current position has moved and the content has been read") {
            long currentPosition = saveR.currentPosition();
            CHECK(currentPosition == text.size());
            CHECK(std::string_view{buffer.data()} == std::string_view{text.data(), text.size()-1});
          }

          WHEN("Trying to read when the buffer is empty") {
            saveR.readLineInto(buffer.data());
            THEN("There is no error") {
              CHECK_FALSE(saveR.stream_error());
            }
          }
        }
      }
    }
  }
}

SCENARIO("Open file checking") {
  GIVEN("No file") {
    SaveFile save{std::monostate{}};
    THEN("File is not opened") { CHECK_FALSE(save.isOpen()); }
  }

  GIVEN("An open file") {
    FileStream<2> fileStream{OpenMode::Read};
    ReadableSaveFile save{fileStream};
    THEN("File is opened") { CHECK(save.isOpen()); }
  }
}