#include <stdio.h>

typedef void (* Step1) (void);
typedef void (* Step2) (void);
typedef void (* Step3) (void);
typedef void (* Step4) (void);

struct Drive {
    Step1 one;
    Step2 two;
    Step3 three;
    Step4 four;
};

void CarStep1(void){ printf("上車\n"); }
void CarStep2(void){ printf("校正後照鏡\n"); }
void CarStep3(void){ printf("繫安全帶\n"); }
void CarStep4(void){ printf("發動開走\n"); }


void PlaneStep1(void){ printf("架艦橋 & 登機\n"); }
void PlaneStep2(void){ printf("地勤檢查\n"); }
void PlaneStep3(void){ printf("繫安全帶\n"); }
void PlaneStep4(void){ printf("加速衝刺 & 起飛\n"); }

void DriveAutomobile(struct Drive drive)
{
    drive.one(); drive.two(); drive.three(); drive.four();
}

int main()
{
    printf("這在開車!!\n");
    struct Drive car, plane;
    car.one = CarStep1; car.two = CarStep2;
    car.three = CarStep3; car.four = CarStep4;
    DriveAutomobile(car);

    printf("\n\n這是開飛機!!\n");
    plane.one = PlaneStep1; plane.two = PlaneStep2;
    plane.three = PlaneStep3; plane.four = PlaneStep4;
    DriveAutomobile(plane);
    return 0;
}
