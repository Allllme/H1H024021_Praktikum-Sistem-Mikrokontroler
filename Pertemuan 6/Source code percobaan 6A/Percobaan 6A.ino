kode 6A:

#include <Arduino.h>

// Variabel volatile agar dapat diubah dalam ISR
volatile bool ledState = false;

// ISR: dijalankan saat tombol ditekan
void tombolInterrupt() {
  ledState = !ledState; // Toggle status LED
}

void setup() {

  // Konfigurasi pin 13 sebagai output (LED)
  pinMode(13, OUTPUT);

  // Konfigurasi pin 2 sebagai input pull-up
  pinMode(2, INPUT_PULLUP);

  // Daftarkan interrupt pada pin 2
  attachInterrupt(
    digitalPinToInterrupt(2),
    tombolInterrupt,
    FALLING
  );
}

void loop() {

  // Menulis status LED sesuai variabel ledState
  digitalWrite(13, ledState);

}
