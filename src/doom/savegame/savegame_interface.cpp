#include "savegame.hpp"

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}
#include <iostream>
#include <fstream>


namespace {
bool savegame_error;
std::fstream save_file{};
std::istream* read_save_file{};
std::ostream* write_save_file{};

SaveGameContext production_context() {
  SaveGameContext context {
      .error = savegame_error,
      .err_os = std::cerr,
      .read_file = read_save_file,
      .write_file = write_save_file,
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
  save_file.open(filename, std::ios::out | std::ios::binary);
  write_save_file = &save_file;
}

void open_savegame_for_read(const char* filename) {
  save_file.open(filename, std::ios::in | std::ios::binary);
  read_save_file = &save_file;
}

void reset_savegame_error() { production_context().error = false; }

void close_savegame()
{
  if(read_save_file) read_save_file = nullptr;
  if(write_save_file) write_save_file = nullptr;
  save_file.close();
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
  *curbyte = prod_savegame().read<int8_t>();
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

