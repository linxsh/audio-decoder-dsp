#ifndef __LIST_H__
#define __LIST_H__

#include "decoder.h"

extern DecoderClass *find_register_decoder(CodecID codecId);

extern void printf_register_all(void);

#endif
