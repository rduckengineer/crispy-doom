#include "savegame.hpp"

extern "C" {
#include "doomtype.h"
#include "p_saveg.h"
}
#include <memory>
#include <fstream>


namespace {
bool savegame_error;

std::unique_ptr<std::istream> read_save_file{};
std::unique_ptr<std::ostream> write_save_file{};

SaveGameContext production_context() {
  SaveGameContext context {
      .error = savegame_error,
      .err_os = std::cerr,
      .read_file = read_save_file.get(),
      .write_file = write_save_file.get(),
  };
  return context;
}

SaveGame prod_savegame() { return SaveGame{production_context()}; }

extern "C" {
void reset_savegame_error() { production_context().error = false; }

// Read / Write for regular savegame
byte saveg_read8(void) { return prod_savegame().read<byte>(); }
void saveg_write8(byte value) { prod_savegame().write<byte>(value); }

int16_t saveg_read16(void) { return prod_savegame().read<int16_t>(); }
void saveg_write16(int16_t value) { prod_savegame().write<int16_t>(value); }

int32_t saveg_read32(void) { return prod_savegame().read<int32_t>(); }
void saveg_write32(int32_t value) { prod_savegame().write<int32_t>(value); }


// Open / Close
void open_savegame_for_write(const char* filename) {
  write_save_file = std::make_unique<std::ofstream>(filename, std::ios::out | std::ios::binary);
}

void open_savegame_for_read(const char* filename) {
  read_save_file = std::make_unique<std::ifstream>(filename, std::ios::in | std::ios::binary);
}

boolean has_savegame_open_failed() {
  return static_cast<boolean>(!prod_savegame().isOpen());
}

void close_savegame()
{
  read_save_file.reset();
  write_save_file.reset();
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

