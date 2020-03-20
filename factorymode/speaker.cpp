#include <stdio.h>
#include "speaker.h"

int DEV_Speaker::waveopen()
{
	printf("speaker wave open\n");
}

void DEV_Speaker::show()
{
	printf("device is speaker\n");
	waveopen();
}

