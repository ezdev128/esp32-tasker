#include <Arduino.h>
#include "esp32-tasker.h"

void task1() {
    log_i("task1!!!");
    deleteTask();
}

[[noreturn]] void task2() {
    log_i("task2!!!");
    for (;;) {
        log_i("task2 loop!");
        delay(100);
    }
}

void task3() {
    log_i("periodic task3");
}

[[noreturn]] void task4() {
    log_i("task4");
    for (;;);
}

void task5() {
    log_i("periodic task5!!!");
    deleteTask();
}

void setup() {
    // Run single shot inline task on any core
    createTask([]() {
        log_i("inline single shot task!");
        deleteTask();
    });

    // Run continues inline task on any core
    createTask([]() {
        log_i("inline continues task!");
        for(;;);
    });

    // Run single shot task callback on any core
    createTask(task1);

    // Run continues task callback on core id 0 (core #1)
    createTaskCore0(task2);

    // Run task3 every 100 ms on core id 1 (core #2)
    createTaskCore1(task3, 100);

    // Run continues named task callback with named task on core id 0 (core #1)
    createTaskCore0(task4, "task 4");

    // Run periodic named task every 500 ms on core id 0 (core #1)
    createTaskCore0(task5, "task 5", 500);
}

void loop() {
    // Delete built-in task in order to launch next ones on the same priority
    deleteTask();
}
