#include "kernelcall.h"

void *gx_mallocz(unsigned int size)
{
	void *p = gx_malloc(size);

	if (p)
		gx_memset(p, 0, size);

	return p;
}
