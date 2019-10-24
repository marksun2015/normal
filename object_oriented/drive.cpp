/*
 * g++ -o drive drive.cpp
 */

#include <iostream>
#include <cstdio>
using namespace std;

class DriveAutomobile{
    public:
        virtual void step1(){ printf("上車\n"); }
        virtual void step2(){ printf("校正後照鏡\n"); }
        virtual void step3(){ printf("繫安全帶\n"); }
        virtual void step4(){ printf("發動開走\n"); }

        void go(){
            step1(); step2(); step3(); step4();
        }
};

class DrivePlane : public DriveAutomobile{
    public:
        void step1(){ printf("架艦橋 & 登機\n"); }
        void step2(){ printf("地勤檢查\n"); }
        void step4(){ printf("加速衝刺 & 起飛\n"); }
};

int main()
{
    printf("這在開車!!\n");
    DriveAutomobile *car = new DriveAutomobile();
    car->go();

    printf("\n\n這是開飛機!!\n");
    DriveAutomobile *plane = new DrivePlane();
    plane->go();
    return 0;
}

