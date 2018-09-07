#include "config.h"
#include "register.h"
#include "decoder.h"
#include "encoder.h"
#include "dec_key.h"
#include "filter.h"

DecoderClass *decClass[MAX_DECODER_NUM] = {NULL};

#define REGISTER_DECODER(x)                    \
	do {                                       \
		extern DecoderClass ##x##Dec;          \
		decClass[i] = &##x##Dec;               \
		i++;                                   \
		decClass[i] = NULL;                    \
	} while(0)

void register_decoder(void)
{
	unsigned char i = 0;

#ifdef CONFIG_MPEG12A_DECODER
	REGISTER_DECODER(opus);
#endif

	return;
}

void register_encoder(void)
{
	return;
}

void register_decrypt(void)
{
	return;
}

void register_encrypt(void)
{
	return;
}

void register_filter(void)
{
	return;
}

void register_all(void)
{
	register_decoder();
	register_encoder();
	register_decrypt();
	register_encrypt();
	register_filter ();

	return;
}

DecoderClass *find_register_decoder(CodecID codecId)
{
	unsigned char i  = 0;
	DecoderClass *class = NULL;

	while (decClass[i]) {
		if (decClass[i]->codec_id == codecId) {
			class = decClass[i];
			break;
		}
		i++;
	}

	return class;
}

void printf_register_all(void)
{
	unsigned char i;

	i = 0;
	log_printf(COMMON, INFO, "=========================\n");
	log_printf(COMMON, INFO, "decoder:\n");
	while (decClass[i]) {
		log_printf(COMMON, INFO, "----------------")
		log_printf(COMMON, INFO, "%s\n", decClass[i]->name);
		log_printf(COMMON, INFO, "%s\n", decClass[i]->long_name);
		i++;
	}

	i = 0;
	log_printf(COMMON, INFO, "=========================\n");
	log_printf(COMMON, INFO, "encoder:\n");

	i = 0;
	log_printf(COMMON, INFO, "=========================\n");
	log_printf(COMMON, INFO, "decrypt:\n");

	i = 0;
	log_printf(COMMON, INFO, "=========================\n");
	log_printf(COMMON, INFO, "encrypt:\n");

	i = 0;
	log_printf(COMMON, INFO, "=========================\n");
	log_printf(COMMON, INFO, "filter:\n");

	return;
}
