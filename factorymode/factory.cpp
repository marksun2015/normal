#include <stdio.h>
#include "speaker.h"
#include "buzzer.h"

#define DEVICE_BUZZER 0
#define DEVICE_SPEAKER 1

class Sound
{
	public:
		Device* CreateProduct(int num)
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

	Device* buzzer = sound_device->CreateProduct(DEVICE_BUZZER);  
	buzzer->show();
	
	Device* speaker = sound_device->CreateProduct(DEVICE_SPEAKER);
	speaker->show();
	
}
