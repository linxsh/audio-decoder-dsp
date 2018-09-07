#ifndef __REGISTER_H__
#define __REGISTER_H__

extern void register_decoder(void);
extern void register_encoder(void);
extern void register_decrypt(void);
extern void register_filter (void);
extern void register_all    (void);

extern DecoderClass *find_register_decoder(CodecID codecId);

extern void printf_register_all(void);

#endif
