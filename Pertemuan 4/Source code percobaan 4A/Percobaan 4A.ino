#include <Servo.h> // Library untuk mengontrol motor servo

Servo myservo; // Membuat objek servo bernama myservo

// ===================== PIN SETUP =====================
const int potensioPin = A0;  // Pin analog A0 untuk membaca potensiometer
const int servoPin = 9;      // Pin digital 9 (PWM) untuk servo

// ===================== VARIABEL =====================
int pos = 0; // Menyimpan sudut servo (0-180 derajat), diinisialisasi 0
int val = 0; // Menyimpan nilai ADC dari potensiometer (0-1023), diinisialisasi 0

void setup() {
  myservo.attach(servoPin); // Hubungkan objek servo ke pin 9
  Serial.begin(9600);       // Aktifkan Serial Monitor dengan baud rate 9600
}

void loop() {
  // ===================== PEMBACAAN ADC =====================
  val = analogRead(potensioPin); // Baca tegangan analog di A0 → nilai 0–1023

  // ===================== KONVERSI DATA =====================
  // Petakan nilai ADC (0–1023) ke sudut servo (0–180 derajat) secara linier
  pos = map(val,
             0,    // nilai minimum ADC
             1023, // nilai maksimum ADC
             0,    // sudut minimum servo
             180); // sudut maksimum servo

  // ===================== OUTPUT SERVO =====================
  myservo.write(pos); // Gerakkan servo ke sudut hasil pemetaan

  // ===================== MONITORING DATA =====================
  Serial.print("ADC Potensio: ");
  Serial.print(val);             // Tampilkan nilai ADC

  Serial.print(" | Sudut Servo: ");
  Serial.println(pos);           // Tampilkan sudut servo

  // ===================== STABILISASI =====================
  delay(15); // Beri waktu 15ms agar servo mencapai posisi yang ditentukan
}
