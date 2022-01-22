#include "catch.hpp"

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"

extern void saveg_write8_in_stream(FILE* stream, byte value, boolean * error);

}

SCENARIO("Writing 8 bits in a file") {
  GIVEN("A file")
  {
    FILE *test_file = fopen("testfile", "wb");

    AND_GIVEN("No prior error")
    {
      boolean err = false;

      WHEN("Writing 8 bits in the stream")
      {
        saveg_write8_in_stream(test_file, 0xA3, &err);
        THEN("There is no error after the write")
        {
          CHECK(err == true);
        }
      }
    }
  }
}