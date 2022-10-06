#ifndef ESP32_TASKER_H
#define ESP32_TASKER_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>
#include <esp_task_wdt.h>
#include <esp32-hal-log.h>
#include <esp_task_wdt.h>

#ifndef MINIMUM_STACK
#define MINIMUM_STACK       configMINIMAL_STACK_SIZE
#endif // MINIMUM_STACK
#ifndef LIGHT_STACK
#define LIGHT_STACK         2048
#endif // LIGHT_STACK
#ifndef MEDIUM_STACK
#define MEDIUM_STACK        (LIGHT_STACK * 2)
#endif // MEDIUM_STACK
#ifndef HEAVY_STACK
#define HEAVY_STACK         (MEDIUM_STACK * 2)
#endif // HEAVY_STACK

#ifndef PRIO_IDLE
#define PRIO_IDLE tskIDLE_PRIORITY
#endif // PRIO_IDLE
#ifndef PRIO_1
#define PRIO_1 ((UBaseType_t) 1U)
#endif // PRIO_1
#ifndef PRIO_2
#define PRIO_2 ((UBaseType_t) 2U)
#endif // PRIO_2
#ifndef PRIO_3
#define PRIO_3 ((UBaseType_t) 3U)
#endif // PRIO_3
#ifndef PRIO_4
#define PRIO_4 ((UBaseType_t) 4U)
#endif // PRIO_4
#ifndef PRIO_5
#define PRIO_5 ((UBaseType_t) 5U)
#endif // PRIO_5

typedef void (*taskCb)();
__attribute__((unused)) bool createTask(
        taskCb callback,
        const char *taskName,
        uint32_t msec = 0,
        uint priority = PRIO_IDLE,
        uint32_t stackSize = LIGHT_STACK,
        BaseType_t coreID = tskNO_AFFINITY);
__attribute__((unused)) bool createTask(
        taskCb callback,
        uint32_t milliseconds = 0,
        uint priority = PRIO_IDLE,
        uint32_t stackSize = LIGHT_STACK,
        BaseType_t coreID = tskNO_AFFINITY);
__attribute__((unused)) bool createTaskCore0(
        taskCb callback,
        const char *taskName,
        uint32_t milliseconds = 0,
        uint priority = PRIO_IDLE,
        uint32_t stackSize = LIGHT_STACK);
__attribute__((unused)) bool createTaskCore0(
        taskCb callback,
        uint32_t milliseconds = 0,
        uint priority = PRIO_IDLE,
        uint32_t stackSize = LIGHT_STACK);
__attribute__((unused)) bool createTaskCore1(
        taskCb callback,
        const char *taskName,
        uint32_t milliseconds = 0,
        uint priority = PRIO_IDLE,
        uint32_t stackSize = LIGHT_STACK);
__attribute__((unused)) bool createTaskCore1(
        taskCb callback,
        uint32_t milliseconds = 0,
        uint priority = PRIO_IDLE,
        uint32_t stackSize = LIGHT_STACK);
__attribute__((unused)) void deleteTask(TaskHandle_t taskHandle = nullptr);
#endif // ESP32_TASKER_H
