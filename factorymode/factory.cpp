#include <stdio.h>
#include "speaker.h"
#include "buzzer.h"

class Sound
{
	public:
		Device* CreateSoundDevice(int num)
		{
			switch(num)   //通過傳參例項化物件
			{
				case DEVICE_BUZZER:
					return new dev_buzzer();
					break;
				case DEVICE_SPEAKER:
					return new dev_speaker();
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
