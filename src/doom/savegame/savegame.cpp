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

byte saveg_read8_from_context(SaveGameContext context)
{
  assert(context.read_file);

  auto result = static_cast<byte>(context.read_file->get());

  if(context.read_file->fail() && !context.error) {
    context.err_os << "saveg_read8: Unexpected end of file while "
                      "reading save game\n";
    context.error = true;
  }
  return result;
}

void saveg_write8_from_context(SaveGameContext context, byte value)
{
  assert(context.write_file);

  context.write_file->put(static_cast<char>(value));

  if(context.write_file->fail() && !context.error)
  {
    context.err_os << "saveg_write8: Error while writing save game\n";
    context.error = true;
  }
}

int16_t saveg_read16_from_context(SaveGameContext context)
{
  int16_t result = saveg_read8_from_context(context);
  return static_cast<int16_t>(result | saveg_read8_from_context(context) << 8);
}

void  saveg_write16_from_context(SaveGameContext context, int16_t value)
{
  saveg_write8_from_context(context, value & 0xff);
  saveg_write8_from_context(context, ((value >> 8) & 0xff));
}


int32_t saveg_read32_from_context(SaveGameContext context)
{
  int32_t result = saveg_read8_from_context(context);
  result |= saveg_read8_from_context(context) << 8;
  result |= saveg_read8_from_context(context) << 16;
  return result | saveg_read8_from_context(context) << 24;
}

void  saveg_write32_from_context(SaveGameContext context, int32_t value)
{
  saveg_write8_from_context(context, value & 0xff);
  saveg_write8_from_context(context, (value >> 8) & 0xff);
  saveg_write8_from_context(context, (value >> 16) & 0xff);
  saveg_write8_from_context(context, (value >> 24) & 0xff);
}