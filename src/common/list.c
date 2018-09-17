#include "config.h"
#include "os_define.h"
#include "list.h"
#include "encoder.h"
#include "decrypt.h"
#include "filter.h"
#include "log.h"

extern DecoderClass opusDec;

DecoderClass *decClass[] = {
#ifdef CONFIG_OPUS_DECODER
	&opusDec,
#endif
};

DecoderClass *find_register_decoder(CodecID codecId)
{
	unsigned char i  = 0;
	DecoderClass *class = NULL;

	for (i = 0; i < sizeof(decClass)/sizeof(DecoderClass); i++) {
		if (decClass[i]->codec_id == codecId) {
			class = decClass[i];
			break;
		}
	}

	return class;
}

void printf_register_all(void)
{
	unsigned char i;

	i = 0;
	log_printf(COMMON_MODULE, LEVEL_INFO, "=========================\n");
	log_printf(COMMON_MODULE, LEVEL_INFO, "decoder:\n");
	for (i = 0; i < sizeof(decClass)/sizeof(DecoderClass); i++) {
		log_printf(COMMON_MODULE, LEVEL_INFO, "----------------");
		log_printf(COMMON_MODULE, LEVEL_INFO, "%s\n", decClass[i]->name);
		log_printf(COMMON_MODULE, LEVEL_INFO, "%s\n", decClass[i]->long_name);
	}

	i = 0;
	log_printf(COMMON_MODULE, LEVEL_INFO, "=========================\n");
	log_printf(COMMON_MODULE, LEVEL_INFO, "encoder:\n");

	i = 0;
	log_printf(COMMON_MODULE, LEVEL_INFO, "=========================\n");
	log_printf(COMMON_MODULE, LEVEL_INFO, "decrypt:\n");

	i = 0;
	log_printf(COMMON_MODULE, LEVEL_INFO, "=========================\n");
	log_printf(COMMON_MODULE, LEVEL_INFO, "encrypt:\n");

	i = 0;
	log_printf(COMMON_MODULE, LEVEL_INFO, "=========================\n");
	log_printf(COMMON_MODULE, LEVEL_INFO, "filter:\n");

	return;
}
