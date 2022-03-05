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

template <typename T>
void readOrWrite(SaveGame& saveg, OpenMode mode)
{
  if(mode == OpenMode::Write)
    saveg.write<T>(1);
  else
    [[maybe_unused]] auto r = saveg.read<T>();
}

TEMPLATE_TEST_CASE("Current position updates with reads, writes and seeks",
                   "[write][read]", int8_t, int16_t, int32_t) {
  std::stringstream err_stream;
  using initBuffer = std::array<byte, 10>;


  GIVEN("A writable empty file") {
    initBuffer initialContent =
        GENERATE(initBuffer{}, initBuffer{1, 1, 1, 1, 0});
    auto mode = GENERATE(OpenMode::Read, OpenMode::Write);
    FileStream<10> file{initialContent, mode};
    INFO("Mode " << mode << " with content of size "
                 << file.lastNonNullIndex());

    SaveGame save{file, false, err_stream};

    long const expectedStart =
        mode == OpenMode::Write ? file.lastNonNullIndex() : 0;

    /*THEN("The open mode is correctly detected") {
      CHECK(save.openMode() == mode);
    }*/

    THEN("Starting position is 0") {
      long startPos = save.currentPosition();
      CHECK(startPos == expectedStart);
    }

    WHEN("Write N bytes") {
      readOrWrite<TestType>(save, mode);

      THEN("Current position moved forward by N bytes") {
        long position = save.currentPosition();
        long expectedPosition =
            std::min(file.size(), expectedStart + sizeof(TestType));
        CHECK(position == expectedPosition);
      }
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
      save.seekFromEnd(offset);
      THEN("Current position is the offset") {
        CHECK(save.currentPosition() ==
              std::max<long>(file.lastNonNullIndex() - offset, 0));
      }
    }
  }
}

SCENARIO("Writing/Reading a line string") {
  std::stringstream err_os;
  GIVEN("A writable file") {
    FileStream<512> fileW{{}, OpenMode::Write};
    SaveGame saveW{fileW, false, err_os};

    WHEN("Writing out a string") {
      static constexpr std::string_view text{"test\n"};
      saveW.write(text.data());
      THEN("The offset has moved by the string size") {
        long currentPosition = saveW.currentPosition();
        CHECK(currentPosition == text.size());
      }

      GIVEN("A readable fileW") {
        FileStream fileR{fileW.buf(), OpenMode::Read};
        SaveGame saveR{fileR, false, err_os};

        WHEN("Reading a line") {
          std::array<char, 10> buffer{};

          CHECK(saveR.readInto(buffer.data()));

          THEN("The current position has moved and the content has been read") {
            long currentPosition = saveR.currentPosition();
            CHECK(currentPosition == text.size());
            CHECK(std::string_view{buffer.data()} == text);
          }
        }
      }
    }
  }
}

SCENARIO("Open file checking") {
  GIVEN("No file") {
    std::stringstream err_os;
    SaveGame save{nullptr, false, err_os};
    THEN("File is not opened") { CHECK_FALSE(save.isOpen()); }
  }

  GIVEN("An open file") {
    FileStream<2> fileStream{{}, OpenMode::Read};
    std::stringstream err_os;
    SaveGame save{fileStream, false, err_os};
    THEN("File is opened") { CHECK(save.isOpen()); }
  }
}