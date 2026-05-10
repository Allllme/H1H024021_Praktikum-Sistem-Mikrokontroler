#include <Arduino_FreeRTOS.h>

void TaskLED(void *pvParameters);
void TaskPotensio(void *pvParameters);

int potValue = 0;
int delayLed = 200;

void setup() {

  Serial.begin(9600);

  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(A0, INPUT);

  xTaskCreate(
    TaskLED,
    "LED",
    128,
    NULL,
    1,
    NULL
  );

  xTaskCreate(
    TaskPotensio,
    "Potensio",
    128,
    NULL,
    1,
    NULL
  );

  vTaskStartScheduler();
}

void loop() {
}

void TaskLED(void *pvParameters) {

  while (1) {

    // LED pin 8 nyala
    digitalWrite(8, HIGH);
    digitalWrite(7, LOW);
    vTaskDelay(delayLed / portTICK_PERIOD_MS);

    // LED pin 7 nyala
    digitalWrite(8, LOW);
    digitalWrite(7, HIGH);
    vTaskDelay(delayLed / portTICK_PERIOD_MS);
  }
}

void TaskPotensio(void *pvParameters) {

  while (1) {

    potValue = analogRead(A0);

    // Mengatur kecepatan kedip LED
    delayLed = map(potValue, 0, 1023, 100, 1000);

    Serial.print("Potensio = ");
    Serial.print(potValue);

    Serial.print(" | Delay = ");
    Serial.println(delayLed);

    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}
