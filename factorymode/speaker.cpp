#include <stdio.h>
#include "factory.h"
#include "speaker.h"

int dev_speaker::waveopen()
{
	printf("speaker wave open\n");
}

void dev_speaker::show()
{
	printf("device is speaker\n");
	waveopen();
}

