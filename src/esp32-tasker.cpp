#include "esp32-tasker.h"

struct TaskParam_t {
    taskCb cb;
    uint32_t msec;
    bool bare;
};

inline __attribute__((always_inline)) uint32_t toMsec(uint32_t msec) {
    return (msec > 0 ? msec : 1) / portTICK_PERIOD_MS;
}

std::string createTaskName(const char *taskName = nullptr) {
    return taskName != nullptr
        ? std::string(taskName)
        : "task" + std::to_string((unsigned long) (esp_timer_get_time() / 1000ULL));
}

void taskLoopEnd(TaskParam_t params) {
    params.cb();
    deleteTask();
}

[[noreturn]] void taskLoopEndless(TaskParam_t params) {
    for (;;) {
        params.cb();
        esp_task_wdt_reset();
        vTaskDelay(toMsec(params.msec));
    }
}

void taskLoopCode(void *param) {
    auto params = *((TaskParam_t *) param);
    if (params.bare) {
        taskLoopEnd(params);
    } else {
        taskLoopEndless(params);
    }
}

bool createTaskToCore(
        taskCb cb,
        uint32_t msec,
        const char *name,
        bool bare,
        uint priority,
        uint32_t stackSize,
        const BaseType_t coreID) {
    auto taskParams = new TaskParam_t {.cb = cb, .msec = msec, .bare = bare};
    auto taskName = createTaskName(name);

    /*
        There are two possible return values:
        1. pdPASS
            This indicates that the task has been created successfully.

        2. pdFAIL
            This indicates that the task has not been created because there is
            insufficient heap memory available for FreeRTOS to allocate enough
            RAM to hold the task data structures and stack.
     */

    auto taskResult = xTaskCreatePinnedToCore(
            /*
                Tasks are simply C functions that never exit and, as such, are normally
                implemented as an infinite loop. The pvTaskCode parameter is simply a
                pointer to the function that implements the task (in effect, just the name
                of the function)
             */
            &taskLoopCode,

            /*
                A descriptive name for the task. This is not used by FreeRTOS in any
                way. It is included purely as a debugging aid. Identifying a task by a
                human-readable name is much simpler than attempting to identify it by
                its handle.
                The application-defined constant configMAX_TASK_NAME_LEN
                defines the maximum length a task name can take—including the NULL
                terminator. Supplying a string longer than this maximum will result in
                the string being silently truncated.
             */
            taskName.c_str(),

            /*
                Each task has its own unique stack that is allocated by the kernel to the
                task when the task is created. The usStackDepth value tells the kernel
                how large to make the stack.
                The value specifies the number of words the stack can hold, not the
                number of bytes. For example, if the stack is 32-bits wide and
                usStackDepth is passed in as 100, then 400 bytes of stack space will be
                allocated (100 * 4 bytes). The stack depth multiplied by the stack width
                must not exceed the maximum value that can be contained in a variable
                of type uint16_t.
                The size of the stack used by the Idle task is defined by the application-
                defined constant configMINIMAL_STACK_SIZE1. The value assigned
                to this constant in the FreeRTOS demo application for the processor
                architecture being used is the minimum recommended for any task. If
                your task uses a lot of stack space, then you must assign a larger value.
                There is no easy way to determine the stack space required by a task.
                It is possible to calculate, but most users will simply assign what they
                think is a reasonable value, then use the features provided by
                FreeRTOS to ensure that the space allocated is indeed adequate, and
                that RAM is not being wasted unnecessarily. Section 12.3, Stack
                Overflow, contains information on how to query the maximum stack
                space that has actually been used by a task.
             */
            stackSize,

            /*
                Task functions accept a parameter of type pointer to void ( void* ). The
                value assigned to pvParameters is the value passed into the task.
                Some examples in this book demonstrate how the parameter can be
                used.
             */
            taskParams,

            /*
                Defines the priority at which the task will execute. Priorities can be
                assigned from 0, which is the lowest priority, to
                (configMAX_PRIORITIES – 1), which is the highest priority.
                configMAX_PRIORITIES is a user defined constant that is described in
                section 3.5.
                Passing a uxPriority value above (configMAX_PRIORITIES – 1) will
                result in the priority assigned to the task being capped silently to the
                maximum legitimate value
             */
            (UBaseType_t) priority,

            /*
                pxCreatedTask can be used to pass out a handle to the task being
                created. This handle can then be used to reference the task in API calls
                that, for example, change the task priority or delete the task.
                If your application has no use for the task handle, then pxCreatedTask
                can be set to NULL.
             */
            nullptr,

            /*
                If the value is tskNO_AFFINITY, the created task is not
                pinned to any CPU, and the scheduler can run it on any core available.
                Values 0 or 1 indicate the index number of the CPU which the task should
                be pinned to. Specifying values larger than (portNUM_PROCESSORS - 1) will
                cause the function to fail.
             */
            coreID
    );

    auto msLabel = taskParams->msec == 0 ? "" : " (each " + std::to_string(taskParams->msec) + " ms)";
    if (taskResult != pdPASS) {
        if (coreID == tskNO_AFFINITY) {
            log_d("task '%s'%s was not created on any available core id",
                  taskName.c_str(), msLabel.c_str(), taskResult);
        } else {
            log_d("task '%s'%s was not created at core id %d, err code: %d",
                  taskName.c_str(), msLabel.c_str(), coreID, taskResult);
        }
    } else {
        if (coreID == tskNO_AFFINITY) {
            log_d("Task '%s'%s created on any available core id", taskName.c_str(), msLabel.c_str());
        } else {
            log_d("Task '%s'%s created at core id %d", taskName.c_str(), msLabel.c_str(), coreID);
        }
    }

    return taskResult == pdPASS;
}

__attribute__((unused)) bool createTask(
        taskCb callback,
        const char *taskName,
        uint32_t msec,
        uint priority,
        uint32_t stackSize,
        BaseType_t coreID) {
    return createTaskToCore(callback, msec, taskName, msec == 0, priority, stackSize, coreID);
}

__attribute__((unused)) bool createTask(
        taskCb callback,
        uint32_t milliseconds,
        uint priority,
        uint32_t stackSize,
        BaseType_t coreID) {
    return createTaskToCore(callback, milliseconds, nullptr, milliseconds == 0, priority, stackSize, coreID);
}

__attribute__((unused)) bool createTaskCore0(
        taskCb callback,
        const char *taskName,
        uint32_t milliseconds,
        uint priority,
        uint32_t stackSize) {
    return createTaskToCore(callback, milliseconds, taskName, milliseconds == 0, priority, stackSize, 0);
}

__attribute__((unused)) bool createTaskCore0(
        taskCb callback,
        uint32_t milliseconds,
        uint priority,
        uint32_t stackSize) {
    return createTaskToCore(callback, milliseconds, nullptr, milliseconds == 0, priority, stackSize, 0);
}

__attribute__((unused)) bool createTaskCore1(
        taskCb callback,
        const char *taskName,
        uint32_t milliseconds,
        uint priority,
        uint32_t stackSize) {
    return createTaskToCore(callback, milliseconds, taskName, milliseconds == 0, priority, stackSize, 1);
}

__attribute__((unused)) bool createTaskCore1(
        taskCb callback,
        uint32_t milliseconds,
        uint priority,
        uint32_t stackSize) {
    return createTaskToCore(callback, milliseconds, nullptr, milliseconds == 0, priority, stackSize, 1);
}

__attribute__((unused)) void deleteTask(TaskHandle_t taskHandle) {
    auto taskName = pcTaskGetName(taskHandle);
    log_d("Task '%s' deleted from core id %d", taskName, xPortGetCoreID());
    vTaskDelete(taskHandle);
}
