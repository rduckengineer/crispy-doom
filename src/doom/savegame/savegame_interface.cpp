#include "save_file.hpp"
#include "readable_save_file.hpp"
#include "writable_save_file.hpp"

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}
#include <memory>
#include <fstream>
#include <iostream>


namespace {
std::fstream file_{};
SaveFile::FileVariant save_file;

auto prod_common() { return SaveFile{save_file}; }
auto prod_read() { return ReadableSaveFile{save_file}; }
auto prod_write() { return WritableSaveFile{save_file}; }

extern "C" {
void reset_savegame_error() { /*no-op*/ }

// Read / Write for regular savegame
byte saveg_read8(void) { return prod_read().read<byte>(); }
void saveg_write8(byte value) { prod_write().write<byte>(value); }

int16_t saveg_read16(void) { return prod_read().read<int16_t>(); }
void saveg_write16(int16_t value) { prod_write().write<int16_t>(value); }

int32_t saveg_read32(void) { return prod_read().read<int32_t>(); }
void saveg_write32(int32_t value) { prod_write().write<int32_t>(value); }


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
  return static_cast<boolean>(!prod_common().isOpen());
}

void close_savegame()
{
  file_.close();
  save_file = std::monostate{};
}

// Position
long current_position()
{
  return prod_common().currentPosition();
}

void seek_from_start(long offset) { prod_common().seekFromStart(offset);
}

void seek_from_end(long offset) { prod_common().seekFromEnd(offset);
}

// Read-Write for crispy
void write_in_stream(const char* content) {
  prod_write().write(content);
}

boolean read_line(char *line_) {
  return static_cast<boolean>(prod_read().readLineInto(line_));
}

boolean read_one_byte(byte *curbyte) {
  auto saveg = prod_read();
  *curbyte = saveg.read<int8_t>();
  return static_cast<boolean>(saveg.stream_error());
}
}
}

