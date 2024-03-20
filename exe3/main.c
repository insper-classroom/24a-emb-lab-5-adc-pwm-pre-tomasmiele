#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int lista[5] = {-1, -1, -1, -1, -1};;
    int indice = 4;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!

            if (lista[4] == -1) {
                lista[4] = data;
            } else if (lista[3] == -1) { 
                lista[3] = data;
            } else if (lista[2] == -1) { 
                lista[2] = data;
            } else if (lista[1] == -1) { 
                lista[1] = data;
            } else if (lista[0] == -1) { 
                lista[0] = data;
            } else {
                if (indice == 0) {
                    indice = 4;
                }
                lista[indice] = data;
                indice -= 1;
            }
            
            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
        if (lista[0] != -1) {
            int soma = 0;
            for (int i = 0; i < 5; i++) {
                soma += lista[i];
            }
            data += soma/5;
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
