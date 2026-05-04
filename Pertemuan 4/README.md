## Pertanyaan Praktikum
### Bagaimana proses konversi sinyal analog menjadi digital pada Arduino? Jelaskan!
Arduino Uno menggunakan ADC successive approximation register (SAR) 10-bit yang terintegrasi dalam chip ATmega328P. Proses konversi dimulai ketika fungsi analogRead() dipanggil; mikrokontroler akan memilih pin analog yang ditentukan menggunakan multiplexer internal, kemudian mengisi kapasitor sample-and-hold dengan tegangan dari pin tersebut. ADC SAR kemudian membandingkan tegangan yang tersimpan dengan tegangan referensi secara bertahap dari bit paling signifikan (MSB) hingga bit paling tidak signifikan (LSB) menggunakan komparator analog internal [3].
Setiap langkah komparasi membutuhkan satu clock ADC, sehingga total konversi 10-bit memerlukan 13 clock ADC. Dengan prescaler default yang menghasilkan clock ADC sekitar 125 kHz pada Arduino, satu konversi memerlukan waktu sekitar 104 μs atau laju sampling sekitar 9600 sampel per detik [2]. Hasil konversi disimpan dalam register ADCH dan ADCL, lalu dikembalikan sebagai nilai integer oleh fungsi analogRead() [4].

---

### Faktor apa saja yang dapat mempengaruhi keakuratan pembacaan ADC?
Beberapa faktor yang dapat memengaruhi akurasi pembacaan ADC antara lain:
•	Noise Elektromagnetik (EMI): Kabel panjang yang terhubung ke pin analog bertindak sebagai antena yang menangkap interferensi elektromagnetik dari sumber seperti motor, charger, dan sinyal radio, menyebabkan fluktuasi nilai ADC [3].
•	Impedansi Sumber Sinyal: ADC Arduino memiliki impedansi masukan yang terbatas. Jika impedansi sumber sinyal terlalu tinggi (>10 kΩ), kapasitor sample-and-hold tidak akan terisi penuh dalam waktu sampling, menghasilkan pembacaan yang lebih rendah dari nilai sebenarnya [3].
•	Ketidakstabilan Tegangan Referensi: ADC Arduino menggunakan VCC (5 V) sebagai referensi secara default. Jika tegangan suplai berfluktuasi akibat beban berubah (misalnya motor aktif), seluruh skala pengukuran ADC ikut bergeser [2].
•	Kuantisasi Error: Karena ADC hanya dapat merepresentasikan tegangan dalam 1024 nilai diskret, terdapat error kuantisasi maksimum sebesar °0,5 LSB atau sekitar ±2,4 mV pada resolusi 5V/1024 ≈14,9 mV [3].
•	Crosstalk antar Pin: Pembacaan ADC yang cepat pada beberapa pin secara bergantian dapat menyebabkan crosstalk karena kapasitor sample-and-hold belum sepenuhnya habis dari pembacaan sebelumnya [4].

---

### Apa kendala yang mungkin terjadi saat mengintegrasikan ADC dan PWM dalam satu sistem?
Integrasi ADC dan PWM dalam satu sistem Arduino dapat menimbulkan beberapa kendala:
•	Interferensi PWM pada ADC: Sinyal PWM yang berfrekuensi tinggi dapat menginduksi noise pada jalur sinyal analog melalui kopling kapasitif atau induktif, terutama jika kabel sinyal analog berdekatan dengan pin PWM. Hal ini menyebabkan nilai ADC berfluktuasi sinkron dengan frekuensi PWM [1].
•	Berbagi Timer Hardware: Arduino Uno menggunakan beberapa timer hardware (Timer0, Timer1, Timer2) untuk menghasilkan sinyal PWM pada pin-pin berbeda. Penggunaan library Servo.h memanfaatkan Timer1, sehingga pin PWM yang bergantung pada Timer1 (pin 9 dan 10) tidak lagi dapat digunakan untuk analogWrite() saat library Servo aktif [2].
•	Timing dan Latensi: Jika delay() digunakan untuk stabilisasi servo atau LED, pembacaan ADC menjadi tidak responsif selama periode delay. Pada sistem yang membutuhkan respons cepat, delay dapat digantikan dengan pendekatan non-blocking menggunakan millis() [5].
•	Konsumsi Daya: Servo motor dapat menarik arus yang cukup besar (hingga 500 mA atau lebih saat berbeban), yang dapat menyebabkan drop tegangan pada VCC Arduino dan memengaruhi akurasi ADC. Disarankan menggunakan catu daya terpisah untuk servo [5].

---


## Pertanyaan Praktikum 4A

### 1. Apa fungsi perintah `analogRead()` pada rangkaian praktikum ini?

Fungsi `analogRead()` digunakan untuk **membaca tegangan analog** yang masuk pada pin A0 dari wiper (pin tengah) potensiometer, lalu secara otomatis mengonversinya menjadi **nilai integer 10-bit** dalam rentang **0 hingga 1023**.

- Nilai **0** → tegangan 0 V (potensiometer di posisi minimum / GND)
- Nilai **1023** → tegangan 5 V (potensiometer di posisi maksimum / VCC)

Tanpa `analogRead()`, mikrokontroler digital tidak bisa membaca tegangan yang sifatnya kontinu. Fungsi ini menjadi jembatan antara dunia analog (potensiometer) dan dunia digital (mikrokontroler).

---

### 2. Mengapa diperlukan fungsi `map()` dalam program tersebut?

Fungsi `map()` diperlukan karena **rentang nilai ADC (0–1023) tidak sesuai** dengan rentang sudut yang diterima motor servo **(0–180 derajat)**.

Jika nilai ADC langsung dikirim ke `myservo.write()` tanpa konversi:
- Nilai ADC di atas 180 (misalnya 500, 1023) akan melebihi batas sudut servo
- Servo bisa bergetar, merusak gigi mekanik, atau berperilaku tidak terdefinisi

Fungsi `map(val, 0, 1023, 0, 180)` melakukan **pemetaan linier** sehingga:
| Nilai ADC | Sudut Servo |
|-----------|-------------|
| 0         | 0°          |
| 256       | ~45°        |
| 512       | ~90°        |
| 768       | ~135°       |
| 1023      | 180°        |

Dengan demikian, servo selalu menerima nilai sudut yang valid dan aman.

---

### 3. Modifikasi: Servo Bergerak Hanya dalam Rentang 30° hingga 150°

**Tujuan modifikasi:** Membatasi pergerakan servo pada rentang 30°–150° meskipun potensiometer tetap diputar penuh (ADC 0–1023).

#### Program Modifikasi

```cpp
#include <Servo.h>

Servo myservo;

const int potensioPin = A0;
const int servoPin = 9;

int pos = 0;
int val = 0;

void setup() {
  myservo.attach(servoPin);
  Serial.begin(9600);
}

void loop() {
  val = analogRead(potensioPin); // Baca ADC potensiometer (0–1023)

  // PERUBAHAN UTAMA: batas sudut diubah dari (0, 180) menjadi (30, 150)
  // Sehingga servo hanya bergerak antara 30° dan 150°
  pos = map(val,
             0,    // ADC minimum (potensiometer penuh ke kiri)
             1023, // ADC maksimum (potensiometer penuh ke kanan)
             30,   // Sudut MINIMUM servo → diubah dari 0 menjadi 30
             150); // Sudut MAKSIMUM servo → diubah dari 180 menjadi 150

  myservo.write(pos); // Gerakkan servo ke sudut hasil pemetaan (30°–150°)

  Serial.print("ADC Potensio: ");
  Serial.print(val);
  Serial.print(" | Sudut Servo: ");
  Serial.println(pos);

  delay(15);
}
```

#### Penjelasan Perubahan

| Parameter | Program Awal | Program Modifikasi |
|-----------|-------------|-------------------|
| Sudut minimum | `0` | `30` |
| Sudut maksimum | `180` | `150` |
| Rentang gerak | 0° – 180° | 30° – 150° |

**Cara kerja:**
- Ketika potensiometer di posisi **minimum** (ADC = 0) → servo di posisi **30°** (bukan 0°)
- Ketika potensiometer di posisi **tengah** (ADC ≈ 512) → servo di posisi **90°**
- Ketika potensiometer di posisi **maksimum** (ADC = 1023) → servo di posisi **150°** (bukan 180°)

**Mengapa berguna?** Pembatasan ini penting untuk melindungi mekanisme fisik yang terhubung ke servo dari benturan struktural, misalnya pada robot lengan yang memiliki batas fisik pergerakan sendi.

---

# Percobaan 2: Pulse Width Modulation (PWM)

## 🎯 Tujuan
- Memahami konsep dasar PWM (Pulse Width Modulation) sebagai metode untuk menghasilkan sinyal analog semu dari keluaran digital Arduino.
- Mengetahui cara kerja fungsi `analogWrite()` pada Arduino Uno dalam menghasilkan variasi duty cycle.

---

## 💡 Program Lengkap Percobaan 2

```cpp
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
```

---


## Pertanyaan Praktikum 4B

### 1. Jelaskan mengapa LED dapat diatur kecerahannya menggunakan fungsi `analogWrite()`!

Fungsi `analogWrite()` bekerja dengan menghasilkan sinyal **PWM (Pulse Width Modulation)** pada pin yang ditentukan. Sinyal PWM secara bergantian menyalakan (**HIGH**) dan mematikan (**LOW**) LED dengan frekuensi sangat tinggi, yaitu sekitar **490 Hz** pada pin 9 Arduino Uno.

Pada frekuensi tersebut, mata manusia **tidak dapat mendeteksi** nyala-mati yang terjadi. Sebaliknya, mata kita merasakan **rata-rata kecerahan** yang proporsional terhadap lamanya waktu LED dalam kondisi HIGH, yang disebut **duty cycle**.

**Ilustrasi duty cycle:**

```
value = 0   (0%)   : |_________|_________|  → LED mati total
value = 64  (25%)  : |██|______|██|______| → LED redup
value = 128 (50%)  : |████|____|████|____| → LED setengah terang
value = 192 (75%)  : |██████|__|██████|__| → LED terang
value = 255 (100%) : |█████████|█████████| → LED menyala penuh
```

Secara fisis, daya rata-rata yang diterima LED = **duty cycle × P_max**, sehingga kecerahan berbanding lurus dengan nilai `value` pada `analogWrite()`.

---

### 2. Apa hubungan antara nilai ADC (0–1023) dan nilai PWM (0–255)?

Nilai ADC memiliki resolusi **10-bit** (menghasilkan 1024 nilai: 0–1023), sedangkan nilai PWM memiliki resolusi **8-bit** (menghasilkan 256 nilai: 0–255). Keduanya dihubungkan melalui fungsi `map()` yang melakukan **pemetaan linier proporsional**.

**Secara matematis:**

```
PWM = (ADC / 1023) × 255  ≈  ADC × 255 / 1023  ≈  ADC / 4
```

| Nilai ADC | Rumus                     | Nilai PWM | Kecerahan LED  |
|-----------|---------------------------|-----------|----------------|
| 0         | 0 × 255 / 1023            | 0         | Mati           |
| 256       | 256 × 255 / 1023 ≈ 63.8  | ~64       | Redup          |
| 512       | 512 × 255 / 1023 ≈ 127.6 | ~128      | Sedang         |
| 768       | 768 × 255 / 1023 ≈ 191.4 | ~191      | Terang         |
| 1023      | 1023 × 255 / 1023         | 255       | Sangat Terang  |

**Artinya:** setiap kenaikan nilai ADC sebesar ±4 akan meningkatkan nilai PWM sebesar ±1. Hubungan ini memastikan perubahan posisi potensiometer terpetakan secara proporsional ke perubahan kecerahan LED.

---

### 3. Modifikasi: LED Hanya Menyala pada Rentang Kecerahan Sedang (PWM 50–200)

**Tujuan modifikasi:** LED tidak pernah mati total dan tidak pernah mencapai kecerahan penuh. Kecerahan dibatasi pada rentang PWM **50 hingga 200**, meskipun potensiometer diputar penuh.

#### Program Modifikasi

```cpp
#include <Arduino.h>

const int potPin = A0;
const int ledPin = 9;

int nilaiADC = 0;
int pwm = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  nilaiADC = analogRead(potPin); // Baca ADC potensiometer (0–1023)

  // PERUBAHAN UTAMA: batas PWM diubah dari (0, 255) menjadi (50, 200)
  // LED hanya beroperasi pada rentang kecerahan sedang
  pwm = map(nilaiADC,
            0,    // ADC minimum
            1023, // ADC maksimum
            50,   // PWM MINIMUM → diubah dari 0 menjadi 50
            200); // PWM MAKSIMUM → diubah dari 255 menjadi 200

  // constrain() sebagai pengaman agar nilai PWM tidak keluar rentang 50–200
  // berguna jika terjadi noise pada pembacaan ADC
  pwm = constrain(pwm, 50, 200);

  analogWrite(ledPin, pwm); // Output PWM ke LED

  Serial.print("ADC: ");
  Serial.print(nilaiADC);
  Serial.print(" | PWM: ");
  Serial.println(pwm);

  delay(50);
}
```

#### Penjelasan Perubahan

| Parameter | Program Awal | Program Modifikasi |
|-----------|-------------|-------------------|
| PWM minimum | `0` (LED mati) | `50` (LED selalu sedikit menyala) |
| PWM maksimum | `255` (LED terang penuh) | `200` (LED tidak pernah terang penuh) |
| Rentang kecerahan | 0% – 100% | ~19.6% – ~78.4% |
| Fungsi pengaman | — | `constrain(pwm, 50, 200)` |

**Cara kerja:**
- Ketika potensiometer di posisi **minimum** (ADC = 0) → PWM = **50** → LED menyala redup (bukan mati)
- Ketika potensiometer di posisi **tengah** (ADC ≈ 512) → PWM = **125** → LED menyala sedang
- Ketika potensiometer di posisi **maksimum** (ADC = 1023) → PWM = **200** → LED menyala terang (tidak penuh)

**Fungsi `constrain(pwm, 50, 200)`:**  
Ditambahkan sebagai lapisan pengaman. Jika karena noise ADC menghasilkan nilai di luar 0–1023, maka hasil `map()` bisa keluar dari rentang 50–200. `constrain()` memastikan nilai PWM selalu terkunci dalam batas yang diinginkan.

**Aplikasi nyata teknik ini:** Sistem pencahayaan yang membutuhkan kecerahan minimum agar tetap berfungsi, seperti backlight layar LCD yang tidak boleh mati total, atau lampu indikator yang harus selalu terlihat.

---
