#include <Arduino_FreeRTOS.h>
#include <queue.h>

// Struct untuk menyimpan data sensor (suhu dan kelembaban)
struct readings {
  int temp;
  int h;
};

QueueHandle_t my_queue; // Handle untuk queue komunikasi antar-task

void setup() {
  Serial.begin(9600);

  // Membuat queue dengan kapasitas 1 item bertipe struct readings
  my_queue = xQueueCreate(1, sizeof(struct readings));

  // Task producer: membaca data sensor
  xTaskCreate(read_data, "read sensors", 128, NULL, 0, NULL);

  // Task consumer: menampilkan data ke Serial Monitor
  xTaskCreate(display, "display", 128, NULL, 0, NULL);
}

void loop() {}

// Task Producer: mengirim data sensor ke queue
void read_data(void *pvParameters) {
  struct readings x;
  for (;;) {
    x.temp = 54;   // Nilai statis (ganti dengan sensor DHT untuk modifikasi)
    x.h = 30;

    // Kirim data ke queue; tunggu hingga queue kosong (portMAX_DELAY)
    xQueueSend(my_queue, &x, portMAX_DELAY);

    vTaskDelay(100); // Periode sampling 100ms
  }
}

// Task Consumer: menerima dan menampilkan data dari queue
void display(void *pvParameters) {
  struct readings x;
  for (;;) {
    // Tunggu data dari queue; blokir hingga data tersedia
    if (xQueueReceive(my_queue, &x, portMAX_DELAY) == pdPASS) {
      Serial.print("temp = ");
      Serial.println(x.temp);
      Serial.print("humidity = ");
      Serial.println(x.h);
    }
  }
}
