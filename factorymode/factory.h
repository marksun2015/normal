#ifndef __DEVICE_H__
#define __DEVICE_H__

#define DEVICE_BUZZER 0
#define DEVICE_SPEAKER 1

class Device
{
	public:
		virtual void show()=0;
};

#endif
