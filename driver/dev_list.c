#include "kernelcall.h"
#include "register.h"
#include "stream_types.h"
#include "dev_core.h"
#include "dev_list.h"

typedef struct {
	unsigned int busy;
	SubDev       *subDev;
	const char   *subName;
} SubDevList;

static SubDevList sSubDevList[] = {
	{0, NULL, "dev-0"},
	{0, NULL, "dev-1"},
	{0, NULL, "dev-2"},
	{0, NULL, "dev-3"}
};

static MajorDev *dev = NULL;

MajorDev *create_dev(void)
{
	MajorDev *dev = gx_malloc(sizeof(MajorDev));

	return dev;
}

void destroy_dev(void)
{
	gx_free(dev);
	dev = NULL;

	return;
}

MajorDev *search_dev(void)
{
	return dev;
}

unsigned int get_sub_dev_num(void)
{
	return (sizeof(sSubDevList) / sizeof(SubDevList));
}

SubDev *create_sub_dev(unsigned int id)
{
	if (id >= (sizeof(sSubDevList) / sizeof(SubDevList)))
		return NULL;

	if (sSubDevList[id].busy)
		return NULL;

	sSubDevList[id].subDev = gx_mallocz(sizeof(SubDev));
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

SubDev *search_sub_dev(unsigned int id)
{
	if (id >= (sizeof(sSubDevList) / sizeof(SubDevList)))
		return NULL;

	if (!sSubDevList[id].busy)
		return NULL;

	return sSubDevList[id].subDev;
}
