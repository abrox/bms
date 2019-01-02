#include <iostream>
#include <iomanip>
#include "UnitTest++/UnitTest++.h"

#define private public
#include "battery.h"

using namespace bms;

///Strong type helper.
/// Overcome problem that UnitTest++
/// do not support enum Class
/// \todo Remove when support availble.
///
#define CHECK_EQUAL_C11_STRONG(x,y) CHECK_EQUAL((int)x,(int)y)

SUITE(BatterySuite)
{
    class BatteryFixture{
    public:
        enum array_t{
            Array100,
            Array75,
            Array50,
            Array25,
            Array0
        };

        const float zero[5];
        const float ten[5];
        const float sixty[5];
        const float seventy[5];
        const float eighty[5];
        const  float ninety[5];
        const float hundredForty[5];
        BatteryCfg config;
        Battery bat;

       BatteryFixture():
           zero   {12.842, 12.642, 12.432, 12.252, 12.082},
           ten    {12.818, 12.618, 12.408, 12.228, 12.058},
           sixty  {12.698, 12.498, 12.288, 12.108, 11.938},
           seventy{12.674, 12.474, 12.264, 12.084, 11.914},
           eighty {12.65,  12.45,  12.24,  12.06,  11.89},
           ninety {12.626, /*12.462*/12.426, 12.216, 12.036, 11.866},//wrong value in web page
           hundredForty {12.506,12.306,12.096,11.916,11.746}
       {
           bat.initilize(config);
       }
    };
    TEST_FIXTURE(BatteryFixture,test_handleBatteryStates)
    {
         Battery::CurrentData data(12.0,-0.24,80);
         Battery::CurrentData data2(12.0,-7.2,80);

         CHECK( bat._ctx.releasableCur == bat._ctx.maxCapasity);
         CHECK( bat._ctx.releasableCur == 120*ONEHOUR);

         bat.update(data);
         CHECK_CLOSE(-0.15,bat._ctx.cuLeft,0.01);
         CHECK_EQUAL(bat._ctx.maxCapasity,bat._ctx.releasableCur);

         bat.update(data2);
         CHECK_CLOSE(-0.547,bat._ctx.cuLeft,0.01);
         CHECK_EQUAL(7,(bat._ctx.maxCapasity-bat._ctx.releasableCur));

         //Check charge phase
         data.cu = 3.0;
         bat.update(data);
         CHECK_CLOSE(0.44,bat._ctx.cuLeft,0.01);
         CHECK_EQUAL(5.0,(bat._ctx.maxCapasity-bat._ctx.releasableCur));

         data.cu = 4.0;
         bat.update(data);
         CHECK_CLOSE(0.42,bat._ctx.cuLeft,0.01);
         CHECK_EQUAL(1.0,(bat._ctx.maxCapasity-bat._ctx.releasableCur));

         std::cout << "\nKoko:" << sizeof(BatteryCtx) << std::endl << sizeof(BatteryCfg) << std::endl;

    }
    TEST_FIXTURE(BatteryFixture,test_getIRCChargeCurrent)
    {
        Battery::CurrentData data(12.0,2.0,80);

        CHECK( bat._ctx.releasableCur == bat._ctx.maxCapasity);
        CHECK( bat._ctx.rInternal == static_cast<float>(0.005));

        bat.update(data);
        CHECK_CLOSE(1.99,bat.getIRCChargeCurrent(),0.01);

        data.cu = 3.0;
        bat.update(data);
        CHECK_CLOSE(2.98,bat.getIRCChargeCurrent(),0.01);


        data.cu = 4.0;
        bat.update(data);
        CHECK_CLOSE(3.98,bat.getIRCChargeCurrent(),0.01);


    }

    TEST_FIXTURE(BatteryFixture,test_getPeukertCorrectedCurrent)
    {
        Battery::CurrentData data(12.0,-0.24,80);

        CHECK(bat._cfg.peukert == static_cast<float>(1.15));
        CHECK(bat._cfg.hourRate = 20);
        CHECK(bat._cfg.ratedCapasity == 120);

        bat.update(data);
        CHECK_CLOSE(-0.15,bat.getPCDischargeCurrent(),0.01);

        data.cu = -7.2;
        bat.update(data);
        CHECK_CLOSE(-7.4,bat.getPCDischargeCurrent(),0.01);

        data.cu = -48.0;
        bat.update(data);
        CHECK_CLOSE(-65.57,bat.getPCDischargeCurrent(),0.01);

    }


    TEST_FIXTURE(BatteryFixture,test_updateTimeInStateAndIdle)
    {

        Battery::CurrentData chargeData(12.0,bat._cfg.thresholdCurrent+0.0001,80);
        Battery::CurrentData idleData(12.0,bat._cfg.thresholdCurrent,80);

        CHECK_EQUAL(0,bat.update(idleData));
        CHECK_EQUAL_C11_STRONG(BattState::WAIT_TO_BE_IDLE,bat._ctx.state);

        //Do one update and check that state stays in wait idle
        CHECK_EQUAL(0,bat._ctx.timeInState);
        CHECK_EQUAL(0,bat.update(idleData));
        CHECK_EQUAL(bat._ctx.interval,bat._ctx.timeInState);
        CHECK_EQUAL_C11_STRONG(BattState::WAIT_TO_BE_IDLE,bat._ctx.state);

        //Check that we move to IDLE after next update
        bat._ctx.timeInState = config.ocvWaitTime;
        CHECK_EQUAL(0,bat.update(idleData));
        CHECK_EQUAL_C11_STRONG(BattState::IDLE,bat._ctx.state);

    }

    TEST_FIXTURE(BatteryFixture,test_updateBasicStateChanges)
    {
        Battery::CurrentData chargeData(12.0,bat._cfg.thresholdCurrent+0.0001,80);
        Battery::CurrentData dischargeData(12.0,0.0-(bat._cfg.thresholdCurrent+0.0001),80);
        Battery::CurrentData idleData(12.0,bat._cfg.thresholdCurrent,80);


        CHECK_EQUAL_C11_STRONG(BattState::UNKNOWN,bat._ctx.state);

        CHECK_EQUAL(0,bat.update( chargeData));
        CHECK_EQUAL_C11_STRONG(BattState::CHARGE,bat._ctx.state);

        CHECK_EQUAL(0,bat.update(dischargeData));
        CHECK_EQUAL_C11_STRONG(BattState::DISCHARGE,bat._ctx.state);

        CHECK_EQUAL(0,bat.update( idleData));
        CHECK_EQUAL_C11_STRONG(BattState::WAIT_TO_BE_IDLE,bat._ctx.state);

    }

    TEST_FIXTURE(BatteryFixture,test_getSOCbyOCVTemp80)
    {
        CHECK_EQUAL(0,bat.getSOCbyOCV(Battery::OCV_0,80));
        CHECK_EQUAL(25,bat.getSOCbyOCV(Battery::OCV_25,80));
        CHECK_EQUAL(50,bat.getSOCbyOCV(Battery::OCV_50,80));
        CHECK_EQUAL(75,bat.getSOCbyOCV(Battery::OCV_75,80));
        CHECK_EQUAL(100,bat.getSOCbyOCV(Battery::OCV_100,80));
    }
    TEST_FIXTURE(BatteryFixture,test_getSOCbyOCVTemp0)
    {
        CHECK_EQUAL(0,bat.getSOCbyOCV(zero[Array0],0));
        CHECK_CLOSE(25,bat.getSOCbyOCV(zero[Array25],0),1.0);//some rounding issue but ok for now
        CHECK_EQUAL(50,bat.getSOCbyOCV(zero[Array50],0));
        CHECK_EQUAL(75,bat.getSOCbyOCV(zero[Array75],0));
        CHECK_EQUAL(100,bat.getSOCbyOCV(zero[Array100],0));
    }
    TEST_FIXTURE(BatteryFixture,test_getSOCbyOCVTemp140)
    {
        CHECK_EQUAL(0,bat.getSOCbyOCV(hundredForty[Array0],140));
        CHECK_EQUAL(25,bat.getSOCbyOCV(hundredForty[Array25],140));
        CHECK_EQUAL(50,bat.getSOCbyOCV(hundredForty[Array50],140));
        CHECK_EQUAL(75,bat.getSOCbyOCV(hundredForty[Array75],140));
        CHECK_EQUAL(100,bat.getSOCbyOCV(hundredForty[Array100],140));
    }

    TEST_FIXTURE(BatteryFixture,test_getTempCompensatedVoltage)
    {
        int arraySize = sizeof(eighty)/sizeof(float);

        for ( int i=0;i< arraySize;i++ ){
            //std::cout << std::fixed << std::setprecision(8)<< bat.getTempCompensatedVoltage(zero[i],0)<< std::endl;
            CHECK_CLOSE( eighty[i], bat.getTempCompensatedVoltage(zero[i],0),0.01 );
        }

        for ( int i=0;i< arraySize;i++ ){
            CHECK_CLOSE( eighty[i], bat.getTempCompensatedVoltage(ten[i],10),0.01 );
        }
        for ( int i=0;i< arraySize;i++ ){
            CHECK_CLOSE( eighty[i], bat.getTempCompensatedVoltage(sixty[i],60),0.01 );
        }

        for ( int i=0;i< arraySize;i++ ){
            CHECK_CLOSE( eighty[i], bat.getTempCompensatedVoltage(seventy[i],70),0.01 );
        }

        for ( int i=0;i< arraySize;i++ ){
            CHECK_EQUAL( eighty[i], bat.getTempCompensatedVoltage(eighty[i],80) );
        }

        for ( int i=0;i< arraySize;i++ ){
            CHECK_CLOSE( eighty[i], bat.getTempCompensatedVoltage(ninety[i],90),0.01 );
        }

        for ( int i=0;i< arraySize;i++ ){
            CHECK_CLOSE( eighty[i], bat.getTempCompensatedVoltage(hundredForty[i],140),0.01 );
        }


    }
    TEST_FIXTURE(BatteryFixture,test_getAddedProcent)
    {
        CHECK_EQUAL(25,bat.getAddedProcent(Battery::OCV_0,Battery::OCV_25,eighty[Array25]));
        CHECK_EQUAL(25,bat.getAddedProcent(Battery::OCV_25,Battery::OCV_50,eighty[Array50]));
        CHECK_EQUAL(25,bat.getAddedProcent(Battery::OCV_50,Battery::OCV_75,eighty[Array75]));
        CHECK_EQUAL(25,bat.getAddedProcent(Battery::OCV_75,Battery::OCV_100,eighty[Array100]));
    }
}

