#include "savegame.hpp"

#include <limits>
#include <cstring>

OpenMode SaveGame::openMode() const {
  assert(m_context.read_file || m_context.write_file);
  return m_context.read_file ? OpenMode::Read : OpenMode::Write;
}

bool SaveGame::stream_error() const noexcept {
  if(m_context.read_file)
    return m_context.read_file->fail();
  else if(m_context.write_file)
    return m_context.write_file->fail();
  return true;
}

bool SaveGame::readInto(char *buffer) {
  static constexpr size_t MAX_LINE_LENGTH = 260;
  auto* file = m_context.read_file;
  assert(file);
  file->getline(buffer, MAX_LINE_LENGTH);

  if(file->fail())
    m_context.read_file->clear();

  auto count = m_context.read_file->gcount();
  return count != 0 && count != std::numeric_limits<std::streamsize>::max();
}

long SaveGame::currentPosition() const noexcept {
  assert(m_context.read_file || m_context.write_file);
  if(m_context.read_file)
    return m_context.read_file->tellg();
  else
    return m_context.write_file->tellp();
}

void SaveGame::seekFromStart(long offset) {
  assert(m_context.read_file || m_context.write_file);
  if(m_context.read_file)
    m_context.read_file->seekg(offset, std::ios::beg);
  else if(m_context.write_file)
    m_context.write_file->seekp(offset, std::ios::beg);
}

void SaveGame::seekFromEnd(long offset) {
  assert(m_context.read_file || m_context.write_file);
  if (m_context.read_file)
    m_context.read_file->seekg(offset, std::ios::end);
  else if (m_context.write_file)
    m_context.write_file->seekp(offset, std::ios::end);
}

byte SaveGame::read8()
{
  assert(m_context.read_file);
  return static_cast<byte>(m_context.read_file->get());
}

void SaveGame::write8(byte value) {
  assert(m_context.write_file);
  m_context.write_file->put(static_cast<char>(value));
}

int16_t SaveGame::read16()
{
  int16_t result = read8();
  return static_cast<int16_t>(result | read8() << 8);
}

void SaveGame::write16(int16_t value)
{
  write8(value & 0xff);
  write8((value >> 8) & 0xff);
}


int32_t SaveGame::read32()
{
  int32_t result = read8();
  result |= read8() << 8;
  result |= read8() << 16;
  return result | read8() << 24;
}

void SaveGame::write32(int32_t value)
{
  write8(value & 0xff);
  write8((value >> 8) & 0xff);
  write8((value >> 16) & 0xff);
  write8((value >> 24) & 0xff);
}