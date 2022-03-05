#include "savegame.hpp"

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}
#include <memory>
#include <fstream>
#include <iostream>


namespace {
bool savegame_error;

std::fstream file_{};
SaveGameContext::FileVariant save_file;

SaveGameContext production_context() {
  return {savegame_error, std::cerr, save_file};
}

SaveGame prod_savegame() { return SaveGame{production_context()}; }

extern "C" {
void reset_savegame_error() { production_context().resetError(); }

// Read / Write for regular savegame
byte saveg_read8(void) { return prod_savegame().read<byte>(); }
void saveg_write8(byte value) { prod_savegame().write<byte>(value); }

int16_t saveg_read16(void) { return prod_savegame().read<int16_t>(); }
void saveg_write16(int16_t value) { prod_savegame().write<int16_t>(value); }

int32_t saveg_read32(void) { return prod_savegame().read<int32_t>(); }
void saveg_write32(int32_t value) { prod_savegame().write<int32_t>(value); }


// Open / Close
void open_savegame_for_write(const char* filename) {
  file_.open(filename, std::ios::out | std::ios::binary);
  save_file = static_cast<std::ostream*>(&file_);
}

void open_savegame_for_read(const char* filename) {
  file_.open(filename, std::ios::in | std::ios::binary);
  save_file = static_cast<std::istream*>(&file_);
}

boolean has_savegame_open_failed() {
  return static_cast<boolean>(!prod_savegame().isOpen());
}

void close_savegame()
{
  file_.close();
  save_file = std::monostate{};
}

// Position
long current_position()
{
  return prod_savegame().currentPosition();
}

void seek_from_start(long offset) {
  prod_savegame().seekFromStart(offset);
}

void seek_from_end(long offset) {
  prod_savegame().seekFromEnd(offset);
}

// Read-Write for crispy
void write_in_stream(const char* content) {
  prod_savegame().write(content);
}

boolean read_line(char *line_) {
  return static_cast<boolean>(prod_savegame().readInto(line_));
}

boolean read_one_byte(byte *curbyte) {
  auto saveg = prod_savegame();
  *curbyte = saveg.read<int8_t>();
  return static_cast<boolean>(saveg.stream_error());
}
}
}

