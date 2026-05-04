## Pertanyaan Praktikum
### 1. Bagaimana proses konversi sinyal analog menjadi digital pada Arduino? Jelaskan!

Arduino Uno menggunakan ADC 10-bit yang ada di dalam chip ATmega328P. Cara kerjanya secara garis besar dimulai dari membaca tegangan yang masuk di pin analog, lalu mengubahnya jadi angka antara 0 sampai 1023. Kalau tegangannya 0V hasilnya 0, kalau 5V hasilnya 1023, dan seterusnya secara proporsional.

Prosesnya sendiri ada tiga tahap. Pertama sampling, yaitu nilai tegangan diambil sesaat dan ditahan. Kedua quantization, nilai tegangan itu dipetakan ke salah satu dari 1024 level yang tersedia, di mana tiap levelnya mewakili sekitar 4,88 mV. Ketiga encoding, hasilnya diubah jadi bilangan biner 10-bit yang bisa dibaca lewat fungsi `analogRead()`.

Pada percobaan ini hal tersebut terlihat ketika potensiometer diputar, nilai yang muncul di Serial Monitor berubah dari 0 ke 1023 dan servo bergerak mengikuti secara proporsional.

---

### 2. Faktor apa saja yang dapat mempengaruhi keakuratan pembacaan ADC?

Ada beberapa hal yang bisa mempengaruhi hasil pembacaan ADC. Yang pertama adalah noise atau gangguan dari komponen lain. Waktu servo bergerak misalnya, ada fluktuasi arus yang bisa sedikit mengganggu tegangan referensi ADC sehingga hasilnya jadi tidak stabil. Ini yang menyebabkan nilai di Serial Monitor kadang bergetar meski potensiometer tidak disentuh.

Selain itu, resolusi 10-bit juga punya batasan tersendiri karena perubahan tegangan di bawah 4,88 mV tidak akan terdeteksi sama sekali. Impedansi sumber sinyal juga berpengaruh, kalau resistansinya terlalu tinggi maka proses sampling tidak berjalan sempurna dan hasilnya bisa lebih kecil dari seharusnya. Terakhir adalah kestabilan tegangan referensi, karena ADC menghitung berdasarkan tegangan 5V, kalau tegangan itu sedikit naik turun maka hasil konversinya ikut terpengaruh.

---

### 3. Apa kendala yang mungkin terjadi saat mengintegrasikan ADC dan PWM dalam satu sistem?

Kendala yang paling terasa adalah interferensi sinyal PWM terhadap pembacaan ADC. Sinyal PWM yang berswitching dengan cepat bisa menginduksi gangguan ke jalur sinyal analog, terutama kalau kabelnya berdekatan. Akibatnya nilai `analogRead()` jadi sedikit tidak stabil meski potensiometer tidak digerakkan.

Masalah lain yang bisa muncul adalah gangguan pada catu daya. Komponen seperti servo atau LED yang menarik arus cukup besar melalui PWM bisa menyebabkan tegangan 5V Arduino sedikit turun sesaat, dan karena itu dipakai sebagai referensi ADC, pembacaannya ikut terganggu.

Ada juga soal perbedaan resolusi antara ADC (0–1023) dan PWM (0–255). Waktu nilainya dipetakan pakai `map()`, tiap perubahan 4 angka di ADC baru menghasilkan perubahan 1 angka di PWM, jadi outputnya tidak selalu berubah mulus untuk perubahan input yang kecil. Selain itu penggunaan timer internal juga perlu diperhatikan karena PWM, `delay()`, dan library Servo semuanya berbagi timer yang sama dan bisa saling konflik jika tidak dikelola dengan baik.

---


## Pertanyaan Praktikum 4A

### 1. Apa fungsi perintah `analogRead()` pada rangkaian praktikum ini?

Fungsi `analogRead()` digunakan untuk membaca tegangan analog yang masuk pada pin A0 dari wiper (pin tengah) potensiometer, lalu secara otomatis mengonversinya menjadi **nilai integer 10-bit** dalam rentang **0 hingga 1023**.

- Nilai 0 → tegangan 0 V (potensiometer di posisi minimum / GND)
- Nilai 1023 → tegangan 5 V (potensiometer di posisi maksimum / VCC)

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

Tujuan modifikasi: Membatasi pergerakan servo pada rentang 30°–150° meskipun potensiometer tetap diputar penuh (ADC 0–1023).

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


## Pertanyaan Praktikum 4B

### 1. Jelaskan mengapa LED dapat diatur kecerahannya menggunakan fungsi `analogWrite()`!

Fungsi `analogWrite()` bekerja dengan menghasilkan sinyal **PWM (Pulse Width Modulation)** pada pin yang ditentukan. Sinyal PWM secara bergantian menyalakan (**HIGH**) dan mematikan (**LOW**) LED dengan frekuensi sangat tinggi, yaitu sekitar **490 Hz** pada pin 9 Arduino Uno.

Pada frekuensi tersebut, mata manusia tidak dapat mendeteksi nyala-mati yang terjadi. Sebaliknya, mata kita merasakan rata-rata kecerahan yang proporsional terhadap lamanya waktu LED dalam kondisi HIGH, yang disebut **duty cycle**.

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
