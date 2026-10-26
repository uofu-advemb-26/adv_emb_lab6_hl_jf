/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"

#include <semphr.h>



#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define SIDE_TASK_STACK_SIZE configMINIMAL_STACK_SIZE

int count = 0;
bool on = false;

SemaphoreHandle_t semaphore;
int count;

#define MAIN_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define BLINK_TASK_PRIORITY     ( tskIDLE_PRIORITY + 2UL )
#define MAIN_TASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define BLINK_TASK_STACK_SIZE configMINIMAL_STACK_SIZE



void main_task(__unused void *params) {

}

void preemptible_thread() {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    for (int i = 0; i<30 ; i++){
        printf("preemptible_thread running", i);
    }
    xSemaphoreGive(semaphore);
}

void supervisor_thread() {
    xSemaphoreTake(semaphore, portMAX_DELAY);
    for (int i = 0; i<30 ; i++) {
        printf("Supervisor_thread running", i);
    }
    xSemaphoreGive(semaphore);
}

int main( void )
{
    TaskHandle_t main, side;
    semaphore = xSemaphoreCreateBinary();
    xTaskCreate(preemptible_thread, "lowThread1",
                SIDE_TASK_STACK_SIZE, NULL, 1, &side);
    xTaskCreate(preemptible_thread, "lowThread2",
                SIDE_TASK_STACK_SIZE, NULL, 1, &side);
    xTaskCreate(preemptible_thread, "lowThread3",
                SIDE_TASK_STACK_SIZE, NULL, 1, &side);
    xTaskCreate(preemptible_thread, "highThread",
                SIDE_TASK_STACK_SIZE, NULL, 4, &main);
}
