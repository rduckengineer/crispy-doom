#include "savegame.hpp"

#include <cstring>
#include <iostream>

OpenMode SaveGame::openMode() const {
  return doOnFile<OpenMode>(
    [](std::istream*) { return OpenMode::Read; },
    [](std::ostream*) { return OpenMode::Write; }
  );
}

bool SaveGame::stream_error() const noexcept {
  return doOnFile<bool>([](auto const& stream) {
    return stream->fail();
  });
}

bool SaveGame::readInto(char *buffer) {
  static constexpr size_t MAX_LINE_LENGTH = 260;
  auto& file = m_context.read();

  file.getline(buffer, MAX_LINE_LENGTH);

  bool done = file.fail();
  // We do read until the end of the file, but EOF triggers a failbit which
  // triggers fails on subsequent reads...
  file.clear();

  return !done;
}

long SaveGame::currentPosition() const noexcept {
  using pos_type = std::ostream::pos_type;

  return doOnFile<pos_type>(
    [](std::ostream* write){ return write->tellp(); },
    [](std::istream* read){ return read->tellg(); }
  );
}

void SaveGame::seekFromStart(long offset) {
  doOnFile(
    [offset](std::ostream* w){ w->seekp(offset, std::ios::beg); },
    [offset](std::istream*r){ r->seekg(offset, std::ios::beg); }
  );
}

void SaveGame::seekFromEnd(long offset) {
  doOnFile(
      [offset](std::ostream* w){ w->seekp(offset, std::ios::end); },
      [offset](std::istream* r){ r->seekg(offset, std::ios::end); }
  );
}

uint8_t SaveGame::read8() {
  return static_cast<uint8_t>(m_context.read().get());
}

void SaveGame::write8(uint8_t value) {
  m_context.write().put(static_cast<char>(value));
}

uint16_t SaveGame::read16() {
  auto result = static_cast<uint16_t>(read8());
  return static_cast<uint16_t>(result | read8() << 8);
}

void SaveGame::write16(uint16_t value) {
  write8(value & 0xff);
  write8((value >> 8) & 0xff);
}

uint32_t SaveGame::read32() {
  uint32_t result = read8();
  result |= read8() << 8;
  result |= read8() << 16;
  return result | read8() << 24;
}

void SaveGame::write32(uint32_t value) {
  write8(value & 0xff);
  write8((value >> 8) & 0xff);
  write8((value >> 16) & 0xff);
  write8((value >> 24) & 0xff);
}

void SaveGame::logOnError(std::string_view error_message) {
  if(stream_error())
    m_context.logError(error_message);
}

template <typename R, typename ... Visitor>
R SaveGame::doOnFile(Visitor&& ... v) const
{
  return std::visit(checked_overload<R>(std::forward<Visitor>(v)...), m_context.file());
}

// SaveGameContext logging
void SaveGameContext::logError(std::string_view err_msg) {
  m_error = true;
  err_os << err_msg;
}