#include <stdio.h>
#include "speaker.h"
#include "buzzer.h"

class Sound
{
	public:
		Device* CreateSoundDevice(int num)
		{
			switch(num)  
			{
				case DEVICE_BUZZER:
					return new DEV_Buzzer();
					break;
				case DEVICE_SPEAKER:
					return new DEV_Speaker();
					break;
				default:
					return NULL;
					break;
			}
		}
};

int main()
{
	Sound* sound_device = new Sound();     

	Device* buzzer = sound_device->CreateSoundDevice(DEVICE_BUZZER);  
	buzzer->show();
	
	Device* speaker = sound_device->CreateSoundDevice(DEVICE_SPEAKER);
	speaker->show();
	
}
