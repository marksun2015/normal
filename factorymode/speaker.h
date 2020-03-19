#ifndef __SPEAKER_H__
#define __SPEAKER_H__
#include "factory.h"

class dev_speaker: public Device
{
	public:
		int waveopen();
		virtual void show();
};

#endif
