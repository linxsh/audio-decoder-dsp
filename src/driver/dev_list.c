#include "kernelcall.h"
#include "register.h"
#include "dev_stream.h"
#include "dev_list.h"

typedef struct {
	unsigned int busy;
	SubDevStream *subDev;
	const char   *subName;
} SubDevList;

static SubDevList sSubDevList[] = {
	{0, NULL, "dev-0", NULL},
	{0, NULL, "dev-1", NULL},
	{0, NULL, "dev-2", NULL},
	{0, NULL, "dev-3", NULL}
};

static DevStream *dev = NULL;

DevStream *create_dev(void)
{
	DevStream *dev = gx_malloc(sizeof(DevStream));

	return dev;
}

void destroy_dev(void)
{
	gx_free(dev);
	dev = NULL;

	return;
}

DevStream *search_dev(void)
{
	return dev;
}

unsigned int get_sub_dev_num(void)
{
	return (sizeof(sSubDevList) / sizeof(SubDevList));
}

SubDevStream *create_sub_dev(unsigned int id)
{
	if (id >= (sizeof(sSubDevList) / sizeof(SubDevList)))
		return NULL;

	if (sSubDevList[id].busy)
		return NULL;

	sSubDevList[id].subDev = gx_mallocz(sizeof(SubDevStream));
	if (NULL == sSubDevList[id].subDev)
		return NULL;

	sSubDevList[id].busy = 1;

	return sSubDevList[id].subDev;
}

void destroy_sub_dev(unsigned int id)
{
	if (id >= (sizeof(sSubDevList) / sizeof(SubDevList)))
		return;

	if (!sSubDevList[id].busy)
		return;

	gx_free(sSubDevList[id].subDev);
	sSubDevList[id].subDev = NULL;
	sSubDevList[id].busy   = 0;

	return;
}

SubDevStream *search_sub_dev(unsigned int id)
{
	if (id >= (sizeof(sSubDevList) / sizeof(SubDevList)))
		return NULL;

	if (!sSubDevList[id].busy)
		return NULL;

	return sSubDevList[id].subDev;
}