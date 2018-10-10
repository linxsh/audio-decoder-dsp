#include "os_define.h"
#include "os_malloc.h"
#include "log_printf.h"

#define BLOCK_SIZE  (512*1024)
#define BLOCK_SHIFT (18) //256k
#define CNT_OF_128 (32 * (BLOCK_SIZE >> BLOCK_SHIFT))   //4k
#define CNT_OF_256 (16 * (BLOCK_SIZE >> BLOCK_SHIFT))   //4k
#define CNT_OF_512 (8  * (BLOCK_SIZE >> BLOCK_SHIFT))   //4k
#define CNT_OF_1K  (52 * (BLOCK_SIZE >> BLOCK_SHIFT))   //52k
#define CNT_OF_2K  (16 * (BLOCK_SIZE >> BLOCK_SHIFT))   //32k
#define CNT_OF_4K  (8  * (BLOCK_SIZE >> BLOCK_SHIFT))   //32k
#define CNT_OF_8K  (16 * (BLOCK_SIZE >> BLOCK_SHIFT))   //128k

#define SHIFT_128(a) (a + 6)
#define BYTE_128     (128)
#define BYTE_256     (256)
#define BYTE_512     (512)
#define BYTE_1K      (1024)
#define BYTE_2K      (2048)
#define BYTE_4K      (4096)
#define BYTE_8K      (8192)

typedef struct {
	unsigned char total_cnt;
	unsigned char used_cnt;
	unsigned char free_cnt;
	unsigned char res;
	unsigned char *address;
} MALLOC_BLOCK;

typedef struct {
	unsigned int  total_size;
	MALLOC_BLOCK  block_128;
	MALLOC_BLOCK  block_256;
	MALLOC_BLOCK  block_512;
	MALLOC_BLOCK  block_1K;
	MALLOC_BLOCK  block_2K;
	MALLOC_BLOCK  block_4K;
	MALLOC_BLOCK  block_8K;
	unsigned char block_128_flags[CNT_OF_128];
	unsigned char block_256_flags[CNT_OF_256];
	unsigned char block_512_flags[CNT_OF_512];
	unsigned char block_1K_flags [CNT_OF_1K];
	unsigned char block_2K_flags [CNT_OF_2K];
	unsigned char block_4K_flags [CNT_OF_4K];
	unsigned char block_8K_flags [CNT_OF_8K];
	unsigned int  block_8K_piece [CNT_OF_8K];
	unsigned int  block_128_pos;
	unsigned int  block_256_pos;
	unsigned int  block_512_pos;
	unsigned int  block_1K_pos;
	unsigned int  block_2K_pos;
	unsigned int  block_4K_pos;
	unsigned int  block_8K_pos;
} MALLOC_MANAGER;

static MALLOC_MANAGER mm;

static int search_block(unsigned char* flags,
		unsigned int pos,
		unsigned int total_cnt,
		unsigned int req_cnt,
		unsigned int *newPos)
{
	int i = 0, j = 0, find = 0;

	if (req_cnt == 1) {
		//search backward
		for (i = (pos + 1); i < total_cnt; i++) {
			if (!flags[i]) {
				*newPos = i;
				return 1;
			}
		}
		//search foreward
		for (i = 0; (i < pos + 1); i++) {
			if (!flags[i]) {
				*newPos = i;
				return 1;
			}
		}

		return 0;
	} else if (req_cnt > 1) {
		//search backward
		for (i = (pos + 1); i < (total_cnt - req_cnt + 1); i++) {
			if (!flags[i]) {
				for (j = 1; j < req_cnt; j++) {
					if (flags[i + j]) {
						find = 0;
						break;
					} else
						find = 1;
				}
				if (find) {
					*newPos = i;
					return 1;
				}
			}
		}
		//search foreward
		for (i = 0; i < (pos + 1); i++) {
			if (!flags[i]) {
				for (j = 1; j < req_cnt; j++) {
					if (flags[i + j]) {
						find = 0;
						break;
					} else
						find = 1;
				}
				if (find) {
					*newPos = i;
					return 1;
				}
			}
		}

		return 0;
	}

	return 0;
}

static int search_index(unsigned char *sAddr, unsigned char *eAddr,
		unsigned char* addr, unsigned int blockShift, unsigned int *index)
{
	if ((addr - sAddr) << (32 - blockShift))
		return 0;

	*index = ((addr - sAddr) >> blockShift);
	return 1;
}

void os_malloc_init(void *address)
{
	unsigned int i = 0;
	unsigned char *ptr = (unsigned char *)address;

	if (NULL == address) {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (NULL)\n", __FUNCTION__, __LINE__);
		return;
	}

	os_memset(&mm, 0, sizeof(MALLOC_MANAGER));
	mm.total_size = BLOCK_SIZE;

	mm.block_128.total_cnt = CNT_OF_128;
	mm.block_128.free_cnt  = CNT_OF_128;
	mm.block_128.address   = ptr;
	ptr += (CNT_OF_128) << SHIFT_128(1);

	mm.block_256.total_cnt = CNT_OF_256;
	mm.block_256.free_cnt  = CNT_OF_256;
	mm.block_256.address   = ptr;
	ptr += (CNT_OF_256) << SHIFT_128(2);

	mm.block_512.total_cnt = CNT_OF_512;
	mm.block_512.free_cnt  = CNT_OF_512;
	mm.block_512.address   = ptr;
	ptr += (CNT_OF_512) << SHIFT_128(3);

	mm.block_1K.total_cnt = CNT_OF_1K;
	mm.block_1K.free_cnt  = CNT_OF_1K;
	mm.block_1K.address   = ptr;
	ptr += (CNT_OF_1K) << SHIFT_128(4);

	mm.block_2K.total_cnt = CNT_OF_2K;
	mm.block_2K.free_cnt  = CNT_OF_2K;
	mm.block_2K.address   = ptr;
	ptr += (CNT_OF_2K) << SHIFT_128(5);

	mm.block_4K.total_cnt = CNT_OF_4K;
	mm.block_4K.free_cnt  = CNT_OF_4K;
	mm.block_4K.address   = ptr;
	ptr += (CNT_OF_4K) << SHIFT_128(6);

	mm.block_8K.total_cnt = CNT_OF_8K;
	mm.block_8K.free_cnt  = CNT_OF_8K;
	mm.block_8K.address   = ptr;
	ptr += (CNT_OF_8K) << SHIFT_128(7);

	for (i = 0; i < CNT_OF_8K; i++)
		mm.block_8K_piece[i] = 1;

	mm.block_128_pos = CNT_OF_128 - 1;
	mm.block_256_pos = CNT_OF_256 - 1;
	mm.block_512_pos = CNT_OF_512 - 1;
	mm.block_1K_pos  = CNT_OF_1K  - 1;
	mm.block_2K_pos  = CNT_OF_2K  - 1;
	mm.block_4K_pos  = CNT_OF_4K  - 1;
	mm.block_8K_pos  = CNT_OF_8K  - 1;

	return;
}

void os_malloc_printf(void)
{
	log_printf(COMMON_MODULE, LEVEL_DEBU, "total mem size (%d)K\n", mm.total_size);
	log_printf(COMMON_MODULE, LEVEL_DEBU,
			"total mem cnt: %d*(128) %d*(256) %d*(512) %d*(1K) %d*(2K) %d*(4K) %d*(8K)\n",
			mm.block_128.total_cnt, mm.block_256.total_cnt, mm.block_512.total_cnt,
			mm.block_1K.total_cnt,  mm.block_2K.total_cnt,  mm.block_4K.total_cnt,
			mm.block_8K.total_cnt);
	log_printf(COMMON_MODULE, LEVEL_DEBU,
			"free mem cnt: %d*(128) %d*(256) %d*(512) %d*(1K) %d*(2K) %d*(4K) %d*(8K)\n",
			mm.block_128.free_cnt, mm.block_256.free_cnt, mm.block_512.free_cnt,
			mm.block_1K.free_cnt,  mm.block_2K.free_cnt,  mm.block_4K.free_cnt,
			mm.block_8K.free_cnt);
}

void *os_malloc(unsigned int size)
{
	unsigned int req_size = size;

	while (1) {
		if (req_size > BYTE_4K) {
			int i = 0;
			int req_cnt = (req_size >> SHIFT_128(7)) + (((req_size & 0x1fff) == 0) ? 0 : 1);

			if (search_block(mm.block_8K_flags, mm.block_8K_pos, CNT_OF_8K, req_cnt, &mm.block_8K_pos)) {
				for (i = 0; i < req_cnt; i++)
					mm.block_8K_flags[mm.block_8K_pos + i] = 1;

				mm.block_8K_piece[mm.block_8K_pos] = req_cnt;
				return (void *)(mm.block_8K.address + (mm.block_8K_pos << SHIFT_128(7)));
			} else {
				log_printf(COMMON_MODULE, LEVEL_ERRO,
						"%s %d: size (%d)\n",
						__FUNCTION__, __LINE__, size);
				break;
			}
		} else if (req_size > BYTE_2K) {
			if (search_block(mm.block_4K_flags, mm.block_4K_pos, CNT_OF_4K, 1, &mm.block_4K_pos)) {
				mm.block_4K_flags[mm.block_4K_pos] = 1;
				return (void *)(mm.block_4K.address + (mm.block_4K_pos << SHIFT_128(6)));
			} else {
				req_size = BYTE_8K;
				log_printf(COMMON_MODULE, LEVEL_DEBU,
						"%s %d: size (%d) to (%d)\n",
						__FUNCTION__, __LINE__, size, req_size);
			}
		} else if (req_size > BYTE_1K) {
			if (search_block(mm.block_2K_flags, mm.block_2K_pos, CNT_OF_2K, 1, &mm.block_2K_pos)) {
				mm.block_2K_flags[mm.block_2K_pos] = 1;
				return (void *)(mm.block_2K.address + (mm.block_2K_pos << SHIFT_128(5)));
			} else {
				req_size = BYTE_4K;
				log_printf(COMMON_MODULE, LEVEL_DEBU,
						"%s %d: size (%d) to (%d)\n",
						__FUNCTION__, __LINE__, size, req_size);
			}
		} else if (req_size > BYTE_512) {
			if (search_block(mm.block_1K_flags, mm.block_1K_pos, CNT_OF_1K, 1, &mm.block_1K_pos)) {
				mm.block_1K_flags[mm.block_1K_pos] = 1;
				return (void *)(mm.block_1K.address + (mm.block_1K_pos << SHIFT_128(4)));
			} else {
				req_size = BYTE_2K;
				log_printf(COMMON_MODULE, LEVEL_DEBU,
						"%s %d: size (%d) to (%d)\n",
						__FUNCTION__, __LINE__, size, req_size);
			}
		} else if (req_size > BYTE_256) {
			if (search_block(mm.block_512_flags, mm.block_512_pos, CNT_OF_512, 1, &mm.block_512_pos)) {
				mm.block_512_flags[mm.block_512_pos] = 1;
				return (void *)(mm.block_512.address + (mm.block_512_pos << SHIFT_128(3)));
			} else {
				req_size = BYTE_1K;
				log_printf(COMMON_MODULE, LEVEL_DEBU,
						"%s %d: size (%d) to (%d)\n",
						__FUNCTION__, __LINE__, size, req_size);
			}
		} else if (req_size > BYTE_128) {
			if (search_block(mm.block_256_flags, mm.block_256_pos, CNT_OF_256, 1, &mm.block_256_pos)) {
				mm.block_256_flags[mm.block_256_pos] = 1;
				return (void *)(mm.block_256.address + (mm.block_256_pos << SHIFT_128(2)));
			} else {
				req_size = BYTE_512;
				log_printf(COMMON_MODULE, LEVEL_DEBU,
						"%s %d: size (%d) to (%d)\n",
						__FUNCTION__, __LINE__, size, req_size);
			}
		} else {
			if (search_block(mm.block_128_flags, mm.block_128_pos, CNT_OF_128, 1, &mm.block_128_pos)) {
				mm.block_128_flags[mm.block_128_pos] = 1;
				return (void *)(mm.block_128.address + (mm.block_128_pos << SHIFT_128(1)));
			} else {
				req_size = BYTE_256;
				log_printf(COMMON_MODULE, LEVEL_DEBU,
						"%s %d: size (%d) to (%d)\n",
						__FUNCTION__, __LINE__, size, req_size);
			}
		}
	}

	return NULL;
}

void *os_realloc(void *address, unsigned int size)
{
	void *newAddress;
	unsigned int oldSize = 0, index = 0;
	unsigned char *oldPtr = (unsigned char*)address;
	unsigned char *blockEndPtr = mm.block_8K.address + ((CNT_OF_8K) << SHIFT_128(7));

	if ((oldPtr >= mm.block_8K.address) && (oldPtr < blockEndPtr)) {
		if (!search_index(mm.block_8K.address, blockEndPtr, oldPtr, SHIFT_128(7), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO,
					"%s %d: address (%p), size (%d)\n",
					__FUNCTION__, __LINE__, address, size);
			return NULL;
		}
		oldSize = mm.block_8K_piece[index] * BYTE_8K;
		if (size > oldSize) {
			newAddress = os_malloc(size);
			if (NULL == newAddress) {
				os_free(address);
				return NULL;
			} else {
				os_memcpy(newAddress, address, oldSize);
				os_free(address);
				return newAddress;
			}
		} else
			return address;
	} else if ((oldPtr >= mm.block_4K.address) && (oldPtr < mm.block_8K.address)) {
		oldSize = BYTE_4K;
		if (size > oldSize) {
			newAddress = os_malloc(size);
			if (NULL == newAddress) {
				os_free(address);
				return NULL;
			} else {
				os_memcpy(newAddress, address, oldSize);
				os_free(address);
				return newAddress;
			}
		} else
			return address;
	} else if ((oldPtr >= mm.block_2K.address) && (oldPtr < mm.block_4K.address)) {
		oldSize = BYTE_2K;
		if (size > oldSize) {
			newAddress = os_malloc(size);
			if (NULL == newAddress) {
				os_free(address);
				return NULL;
			} else {
				os_memcpy(newAddress, address, oldSize);
				os_free(address);
				return newAddress;
			}
		} else
			return address;
	} else if ((oldPtr >= mm.block_1K.address) && (oldPtr < mm.block_2K.address)) {
		oldSize = BYTE_1K;
		if (size > oldSize) {
			newAddress = os_malloc(size);
			if (NULL == newAddress) {
				os_free(address);
				return NULL;
			} else {
				os_memcpy(newAddress, address, oldSize);
				os_free(address);
				return newAddress;
			}
		} else
			return address;
	} else if ((oldPtr >= mm.block_512.address) && (oldPtr < mm.block_1K.address)) {
		oldSize = BYTE_512;
		if (size > oldSize) {
			newAddress = os_malloc(size);
			if (NULL == newAddress) {
				os_free(address);
				return NULL;
			} else {
				os_memcpy(newAddress, address, oldSize);
				os_free(address);
				return newAddress;
			}
		} else
			return address;
	} else if ((oldPtr >= mm.block_256.address) && (oldPtr < mm.block_512.address)) {
		oldSize = BYTE_256;
		if (size > oldSize) {
			newAddress = os_malloc(size);
			if (NULL == newAddress) {
				os_free(address);
				return NULL;
			} else {
				os_memcpy(newAddress, address, oldSize);
				os_free(address);
				return newAddress;
			}
		} else
			return address;
	} else if ((oldPtr >= mm.block_128.address) && (oldPtr < mm.block_256.address)) {
		oldSize = BYTE_128;
		if (size > oldSize) {
			newAddress = os_malloc(size);
			if (NULL == newAddress) {
				os_free(address);
				return NULL;
			} else {
				os_memcpy(newAddress, address, oldSize);
				os_free(address);
				return newAddress;
			}
		} else
			return address;
	} else {
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
		return NULL;
	}

	return NULL;
}

void os_free(void *address)
{
	unsigned int index = 0;
	unsigned char *ptr = (unsigned char*)address;
	unsigned char *blockEndPtr = mm.block_8K.address + ((CNT_OF_8K) << SHIFT_128(7));

	if ((ptr >= mm.block_8K.address) && (ptr < blockEndPtr)) {
		if (!search_index(mm.block_8K.address, blockEndPtr, ptr, SHIFT_128(7), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
			return;
		}
		mm.block_8K_flags[index] = 0;
		mm.block_8K_piece[index] = 1;
	} else if ((ptr >= mm.block_4K.address) && (ptr < mm.block_8K.address)) {
		if (!search_index(mm.block_4K.address, mm.block_8K.address, ptr, SHIFT_128(6), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
			return;
		}
		mm.block_4K_flags[index] = 0;
	} else if ((ptr >= mm.block_2K.address) && (ptr < mm.block_4K.address)) {
		if (!search_index(mm.block_2K.address, mm.block_4K.address, ptr, SHIFT_128(5), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
			return;
		}
		mm.block_2K_flags[index] = 0;
	} else if ((ptr >= mm.block_1K.address) && (ptr < mm.block_2K.address)) {
		if (!search_index(mm.block_1K.address, mm.block_2K.address, ptr, SHIFT_128(4), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
			return;
		}
		mm.block_1K_flags[index] = 0;
	} else if ((ptr >= mm.block_512.address) && (ptr < mm.block_1K.address)) {
		if (!search_index(mm.block_512.address, mm.block_1K.address, ptr, SHIFT_128(3), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
			return;
		}
		mm.block_512_flags[index] = 0;
	} else if ((ptr >= mm.block_256.address) && (ptr < mm.block_512.address)) {
		if (!search_index(mm.block_256.address, mm.block_512.address, ptr, SHIFT_128(2), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
			return;
		}
		mm.block_256_flags[index] = 0;
	} else if ((ptr >= mm.block_128.address) && (ptr < mm.block_256.address)) {
		if (!search_index(mm.block_128.address, mm.block_256.address, ptr, SHIFT_128(1), &index)) {
			log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);
			return;
		}
		mm.block_128_flags[index] = 0;
	} else
		log_printf(COMMON_MODULE, LEVEL_ERRO, "%s %d: address (%p)\n", __FUNCTION__, __LINE__, address);

	return;
}
