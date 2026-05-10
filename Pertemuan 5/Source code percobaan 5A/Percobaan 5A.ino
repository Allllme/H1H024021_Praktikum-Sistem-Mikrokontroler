#include <Arduino_FreeRTOS.h>

// Deklarasi prototipe task
void TaskBlink1(void *pvParameters);  // Task LED pin D8, periode 200ms
void TaskBlink2(void *pvParameters);  // Task LED pin D7, periode 300ms
void Taskprint(void *pvParameters);   // Task counter serial, periode 500ms

void setup() {
  Serial.begin(9600);

  // Membuat Task 1: LED Merah pada pin D8
  // Parameter: (fungsi_task, nama, ukuran_stack, param, prioritas, handle)
  xTaskCreate(TaskBlink1, "task1", 128, NULL, 1, NULL);

  // Membuat Task 2: LED Kuning pada pin D7
  xTaskCreate(TaskBlink2, "task2", 128, NULL, 1, NULL);

  // Membuat Task 3: Counter pada Serial Monitor
  xTaskCreate(Taskprint, "task3", 128, NULL, 1, NULL);

  // Memulai FreeRTOS Scheduler
  vTaskStartScheduler();
}

void loop() {
  // Dikosongkan karena scheduler FreeRTOS mengambil alih kontrol
}

// Task 1: Blink LED pada pin D8 setiap 200ms
void TaskBlink1(void *pvParameters) {
  pinMode(8, OUTPUT);
  while (1) {
    Serial.println("Task1");
    digitalWrite(8, HIGH);
    vTaskDelay(200 / portTICK_PERIOD_MS); // Yield selama 200ms
    digitalWrite(8, LOW);
    vTaskDelay(200 / portTICK_PERIOD_MS); // Yield selama 200ms
  }
}

// Task 2: Blink LED pada pin D7 setiap 300ms
void TaskBlink2(void *pvParameters) {
  pinMode(7, OUTPUT);
  while (1) {
    Serial.println("Task2");
    digitalWrite(7, HIGH);
    vTaskDelay(300 / portTICK_PERIOD_MS); // Yield selama 300ms
    digitalWrite(7, LOW);
    vTaskDelay(300 / portTICK_PERIOD_MS); // Yield selama 300ms
  }
}

// Task 3: Increment dan tampilkan counter setiap 500ms
void Taskprint(void *pvParameters) {
  int counter = 0;
  while (1) {
    counter++;
    Serial.println(counter);
    vTaskDelay(500 / portTICK_PERIOD_MS); // Yield selama 500ms
  }
}
