#include <Arduino.h>

unsigned long previousMillis = 0; // Waktu terakhir LED berubah
const long interval = 1000;       // Interval kedip 1 detik

bool ledState = false;            // Status LED

void setup() {

  // Pin 13 sebagai output
  pinMode(13, OUTPUT);

}

void loop() {

  // Ambil waktu saat ini
  unsigned long currentMillis = millis();

  // Cek apakah interval sudah tercapai
  if (currentMillis - previousMillis >= interval) {

    // Simpan waktu sekarang
    previousMillis = currentMillis;

    // Toggle LED
    ledState = !ledState;

    // Tulis ke LED
    digitalWrite(13, ledState);
  }

}
