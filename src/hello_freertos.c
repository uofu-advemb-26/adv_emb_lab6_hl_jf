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

void preemptible_thread_low() {
    while(1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        for (int i = 0; i<30 ; i++){
            printf("low thread has sem", i);
        }
        xSemaphoreGive(semaphore);
    }
}

void preemptible_thread_med() {
    vTaskDelay(500);
    while(1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        for (int i = 0; i<30; i++){
            printf("med thread has sem", i);
        }
        xSemaphoreGive(semaphore);
    }
}

void preemptible_thread_high() {
    vTaskDelay(1000);
    while(1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        for (int i = 0; i<30 ; i++){
            printf("high thread has sem", i);
        }
        xSemaphoreGive(semaphore);
    }
}

void supervisor_thread() {
    while(1) { 
        xSemaphoreTake(semaphore, portMAX_DELAY);
        for (int i = 0; i<30 ; i++) {
            printf("Supervisor_thread running", i);
        }
        xSemaphoreGive(semaphore);
    }
}

int main( void )
{
    stdio_init_all();

    TaskHandle_t main, side;
    semaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(semaphore); // Start available so tasks dont get stuck on init
    xTaskCreate(preemptible_thread_low, "low",
                SIDE_TASK_STACK_SIZE, NULL, 1, &side);
    xTaskCreate(preemptible_thread_med  , "med",
                SIDE_TASK_STACK_SIZE, NULL, 1, &side);
    xTaskCreate(preemptible_thread_high, "high",
                SIDE_TASK_STACK_SIZE, NULL, 1, &side);
    xTaskCreate(supervisor_thread, "supervisor",
                SIDE_TASK_STACK_SIZE, NULL, 4, &main);

    vTaskStartScheduler();
}