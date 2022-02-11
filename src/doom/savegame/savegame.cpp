
extern "C" {
#include "doomtype.h"
#include "p_saveg.h"

byte saveg_read8_from_context(SaveGameContext context)
{
  byte result = -1;

  if (fread(&result, 1, 1, context.stream) < 1)
  {
    if (!*context.error)
    {
      fprintf(context.error_stream, "saveg_read8: Unexpected end of file while "
                            "reading save game\n");

      *context.error = true;
    }
  }

  return result;
}

void  saveg_write8_from_context  (SaveGameContext context, byte value)
{
  if (fwrite(&value, 1, 1, context.stream) < 1)
  {
    if (!*context.error)
    {
      fprintf(context.error_stream, "saveg_write8: Error while writing save game\n");

      *context.error = true;
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

void  saveg_write16_from_context  (SaveGameContext context, int16_t value)
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

void  saveg_write32_from_context  (SaveGameContext context, int32_t value)
{
  saveg_write8_from_context(context, value & 0xff);
  saveg_write8_from_context(context, (value >> 8) & 0xff);
  saveg_write8_from_context(context, (value >> 16) & 0xff);
  saveg_write8_from_context(context, (value >> 24) & 0xff);
}
}

