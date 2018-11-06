#ifndef __LIST_H__
#define __LIST_H__

#include "decoder.h"
#include "encoder.h"

extern DecoderClass *find_register_decoder(CodecID codecId);
extern EncoderClass *find_register_encoder(CodecID codecId);

extern void printf_register_all(void);

#endif
