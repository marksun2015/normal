#include <stdio.h>
#include "factory.h"
#include "buzzer.h"

#if 0
class dev_buzzer: public Device
{
	public:
		virtual void show()
		{
			printf("device is buzzer\n");
		}
};
#endif

void dev_buzzer:: show()
{
	printf("device is buzzer\n");
}


