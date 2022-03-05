#include "savegame.hpp"

namespace {
SaveGame prod_savegame();
}

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"

FILE *save_stream;
bool savegame_error;
}

#include <iostream>

namespace {
SaveGameContext production_context() {
  SaveGameContext context {
      .stream = save_stream,
      .error = savegame_error,
      .err_os = std::cerr,
  };
  return context;
}

SaveGame prod_savegame() { return SaveGame{production_context()}; }

extern "C" {

byte saveg_read8(void) { return prod_savegame().read<byte>(); }
void saveg_write8(byte value) { prod_savegame().write<byte>(value); }

int16_t saveg_read16(void) { return prod_savegame().read<int16_t>(); }
void saveg_write16(int16_t value) { prod_savegame().write<int16_t>(value); }

int32_t saveg_read32(void) { return prod_savegame().read<int32_t>(); }
void saveg_write32(int32_t value) { prod_savegame().write<int32_t>(value); }

void open_savegame_for_write(const char* filename) {
  save_stream = fopen(filename, "wb");
}

void open_savegame_for_read(const char* filename) {
  save_stream = fopen(filename, "rb");
}

void reset_savegame_error() { production_context().error = false; }

void close_savegame()
{
  fclose(save_stream);
}

boolean has_savegame_open_failed() {
  return static_cast<boolean>(!prod_savegame().isOpen());
}


long current_position()
{
  return prod_savegame().currentPosition();
}

void write_in_stream(const char* content) {
  prod_savegame().write(content);
}

char *read_line(char *line_) {
  if(prod_savegame().readInto(line_))
    return line_;
  return nullptr;
}

size_t read_one_byte(byte *curbyte) {
  auto saveg = prod_savegame();
  *curbyte = prod_savegame().read<int8_t>();
  if(saveg.error())
    throw std::runtime_error("Couldn't read one byte!");
  return 1;
}

void seek_from_start(long offset) {
  prod_savegame().seekFromStart(offset);
}

void seek_from_end(long offset) {
  prod_savegame().seekFromEnd(offset);
}
}
}

byte saveg_read8_from_context(SaveGameContext context)
{
  byte result = -1;

  if (fread(&result, 1, 1, context.stream) < 1)
  {
    if (!context.error)
    {
      context.err_os << "saveg_read8: Unexpected end of file while "
                            "reading save game\n";

      context.error = true;
    }
  }

  return result;
}

void saveg_write8_from_context(SaveGameContext context, byte value)
{
  if (fwrite(&value, 1, 1, context.stream) < 1)
  {
    if (!context.error)
    {
      context.err_os << "saveg_write8: Error while writing save game\n";

      context.error = true;
    }
  }
}

int16_t saveg_read16_from_context(SaveGameContext context)
{
  int16_t result;

  result = saveg_read8_from_context(context);
  result = static_cast<int16_t>(result | saveg_read8_from_context(context) << 8);

  return result;
}

void  saveg_write16_from_context(SaveGameContext context, int16_t value)
{
  saveg_write8_from_context(context, value & 0xff);
  saveg_write8_from_context(context, ((value >> 8) & 0xff));
}


int32_t saveg_read32_from_context(SaveGameContext context)
{
  int32_t result;

  result = saveg_read8_from_context(context);
  result |= saveg_read8_from_context(context) << 8;
  result |= saveg_read8_from_context(context) << 16;
  result |= saveg_read8_from_context(context) << 24;

  return result;
}

void  saveg_write32_from_context(SaveGameContext context, int32_t value)
{
  saveg_write8_from_context(context, value & 0xff);
  saveg_write8_from_context(context, (value >> 8) & 0xff);
  saveg_write8_from_context(context, (value >> 16) & 0xff);
  saveg_write8_from_context(context, (value >> 24) & 0xff);
}
