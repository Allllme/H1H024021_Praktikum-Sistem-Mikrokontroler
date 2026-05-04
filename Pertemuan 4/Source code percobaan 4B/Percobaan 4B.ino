#include <Arduino.h> // Library dasar Arduino (tidak wajib diubah)

// ===================== PIN SETUP =====================
const int potPin = A0;  // Pin analog A0 untuk membaca potensiometer
const int ledPin = 9;   // Pin digital 9 (PWM) untuk LED

// ===================== VARIABEL =====================
int nilaiADC = 0;  // Menyimpan hasil pembacaan ADC (0–1023), diinisialisasi 0
int pwm = 0;       // Menyimpan nilai PWM yang akan dikirim ke LED (0–255), diinisialisasi 0

void setup() {
  pinMode(ledPin, OUTPUT); // Atur pin LED sebagai output digital

  Serial.begin(9600); // Aktifkan Serial Monitor dengan baud rate 9600
}

void loop() {
  // ===================== PEMBACAAN SENSOR =====================
  nilaiADC = analogRead(potPin); // Baca tegangan analog di A0 → nilai 0–1023

  // ===================== PEMROSESAN DATA (SCALING) =====================
  // Petakan nilai ADC (0–1023) ke nilai PWM (0–255) secara linier
  pwm = map(nilaiADC,
            0,    // nilai minimum ADC
            1023, // nilai maksimum ADC
            0,    // PWM minimum (LED mati)
            255); // PWM maksimum (LED terang penuh)

  // ===================== OUTPUT PWM =====================
  analogWrite(ledPin, pwm); // Kirim sinyal PWM ke LED → mengatur kecerahan

  // ===================== MONITORING DATA =====================
  Serial.print("ADC: ");
  Serial.print(nilaiADC); // Tampilkan nilai ADC

  Serial.print(" | PWM: ");
  Serial.println(pwm);    // Tampilkan nilai PWM

  // ===================== STABILISASI SISTEM =====================
  delay(50); // Delay 50ms untuk menstabilkan pembacaan dan tampilan Serial Monitor
}
