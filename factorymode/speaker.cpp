#include <stdio.h>
#include "factory.h"
#include "speaker.h"

#if 0
class dev_speaker:public Device
{
	public:
		int waveopen()
		{
			printf("speaker wave open\n");
		}

		virtual void show()
		{
			printf("device is speaker\n");
			waveopen();
		}
};
#endif
int dev_speaker::waveopen()
{
	printf("speaker wave open\n");
}

void dev_speaker::show()
{
	printf("device is speaker\n");
	waveopen();
}

