//
// Created by kamin.deng on 2024/8/14.
//
#include "gpio.h"
#include "osal_debug.h"
#include "osal_overload_mgr.h"
#include "osal_system.h"
#include "osal_test_main.h"
#include "osal_thread.h"
#include "test_framework.h"

using namespace osal;

extern "C" void setup(void) {}

OSALThread init_thread;
extern OSALThread osal_test_thread;
extern "C" [[noreturn]] void loop(void) {
    OSAL_LOGI("OSAL test case being\n");
    init_thread.start(
        "init_thread",
        [](void*) {
            osal_test_main();
            while (!osal_test_thread.isRunning());
            osal_test_thread.join();
            MemoryStatsPrint();
            while (1) {
                HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
                OSALSystem::getInstance().sleep_ms(1000);
            }
        },
        nullptr, 0, 2048);
    OSALSystem::getInstance().StartScheduler();
    while (1);
}