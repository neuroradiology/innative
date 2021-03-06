// Copyright (c)2019 Black Sphere Studios
// For conditions of distribution and use, see copyright notice in innative.h

#include "stream.h"

using namespace innative;
using namespace utility;

uint64_t Stream::DecodeLEB128(IN_ERROR& err, unsigned int maxbits, bool sign)
{
  size_t shift = 0;
  int byte = 0;
  uint64_t result = 0;
  do
  {
    if(shift >= maxbits)
    {
      err = ERR_FATAL_OVERLONG_ENCODING;
      return 0;
    }
    byte = Get();
    if(byte == -1)
    {
      err = ERR_PARSE_UNEXPECTED_EOF;
      return 0;
    }

    result |= ((byte & 0x7F) << shift);
    shift += 7;
  } while((byte & 0x80) != 0);

  int signbit = byte & 0x40;

  if(shift > maxbits)
  {
    signbit = (1 << (maxbits + 6 - shift)) & byte; // If our encoding is potentially overlong, we must correct the sign bit to the final legal bit
    int bits = (~0 << (maxbits + 7 - shift)) & 0x7F; // Gets the illegal bits of this byte

    if(sign && signbit) // If the sign bit is set, we need to check (~byte)&bits instead of byte&bits
      byte = ~byte;

    if(byte & bits)
    {
      err = ERR_FATAL_INVALID_ENCODING;
      return 0;
    }
  }

  //assert(!(((~0ULL) << maxbits) & result));
  if(sign && signbit != 0)
    result |= (~0ULL << shift);

  err = ERR_SUCCESS;
  return result;
}