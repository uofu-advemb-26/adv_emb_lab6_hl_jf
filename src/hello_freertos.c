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



void main_task(__unused void *params) { }


void busy_busy(int prio)
{
    for (int i = 0; ; i++) {
        vTaskDelay(500);
        printf("\n currently running: task %d\n", prio);
    }
}


void busy_yield(int prio)
{
    for (int i = 0; ; i++) {
        printf("\n currently running: task %d\n", prio);
        vTaskDelay(500);
        taskYIELD();
    }
}

void preemptible_thread_low() {
    while(1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        busy_yield(1);
        xSemaphoreGive(semaphore);
    }
}

void preemptible_thread_med() {
    vTaskDelay(500);
    while(1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        busy_yield(2);
        xSemaphoreGive(semaphore);
    }
}

void preemptible_thread_high() {
    vTaskDelay(1000);
    while(1) {
        xSemaphoreTake(semaphore, portMAX_DELAY);
        for (int i = 0; i<30 ; i++){
            printf("\n high thread has sem", i);
            vTaskDelay(500);
        }
        xSemaphoreGive(semaphore);
    }
}

    void supervisor_thread() {
        while(1) { 
            xSemaphoreTake(semaphore, portMAX_DELAY);
            for (int i = 0; i<30 ; i++) {
                printf("\n Supervisor_thread running", i);
                vTaskDelay(500);
            }
            xSemaphoreGive(semaphore);
        }
    }

int main( void )
{
    stdio_init_all();

    TaskHandle_t main, side;
    semaphore = xSemaphoreCreateMutex();
    // xSemaphoreGive(semaphore); // Start available so tasks dont get stuck on init
    xTaskCreate(preemptible_thread_low, "low",
                SIDE_TASK_STACK_SIZE, NULL, 1, &side);
    xTaskCreate(preemptible_thread_med  , "med",
                SIDE_TASK_STACK_SIZE, NULL, 2, &side);
    // xTaskCreate(preemptible_thread_high, "high",
    //             SIDE_TASK_STACK_SIZE, NULL, 3, &side);
    // xTaskCreate(supervisor_thread, "supervisor",
    //             SIDE_TASK_STACK_SIZE, NULL, 4, &main);

    vTaskStartScheduler();
}