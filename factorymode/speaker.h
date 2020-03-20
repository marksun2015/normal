#ifndef __SPEAKER_H__
#define __SPEAKER_H__
#include "sound.h"

class DEV_Speaker: public Device
{
	public:
		int waveopen();
		virtual void show();
};

#endif
