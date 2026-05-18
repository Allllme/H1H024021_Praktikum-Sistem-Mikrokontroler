## Jawaban Pertanyaan 6A

### 1. Bagaimana tombol dapat mengubah kondisi LED menggunakan interrupt?

Ketika tombol ditekan, sinyal pada Pin 2 berubah dari HIGH ke LOW (karena menggunakan `INPUT_PULLUP`). Perubahan tepi sinyal ini (_FALLING edge_) memicu external interrupt INT0 pada mikrokontroler ATmega328P.

Alur proses:

```
Tombol ditekan
    ↓
Sinyal Pin 2: HIGH → LOW (FALLING edge)
    ↓
Hardware interrupt INT0 aktif
    ↓
CPU menyimpan konteks (register, PC) ke stack
    ↓
CPU menjalankan ISR: tombolInterrupt()
    ↓
ledState = !ledState (toggle nilai)
    ↓
ISR selesai → konteks dipulihkan
    ↓
loop() melanjutkan → digitalWrite(13, ledState) memperbarui LED
```

Proses ini terjadi secara hardware, bukan software polling, sehingga respons sangat cepat dan CPU tidak "sibuk" menunggu input tombol.

---

### 2. Apa fungsi `attachInterrupt()` pada program tersebut?

Fungsi `attachInterrupt()` bertugas mendaftarkan dan mengonfigurasi external interrupt pada Arduino. Fungsi ini memerlukan tiga parameter:

| Parameter | Nilai pada Program | Keterangan |
|-----------|-------------------|------------|
| `interrupt` | `digitalPinToInterrupt(2)` | Konversi Pin 2 → INT0 (nomor interrupt internal ATmega328P) |
| `ISR` | `tombolInterrupt` | Fungsi yang akan dieksekusi saat interrupt terjadi |
| `mode` | `FALLING` | Kondisi pemicu: sinyal turun dari HIGH ke LOW |

Tanpa `attachInterrupt()`, mikrokontroler tidak memiliki mapping antara sumber interrupt dan fungsi ISR yang harus dijalankan, sehingga interrupt tidak akan berfungsi.

---

### 3. Mengapa ISR tidak disarankan menggunakan `delay()` dan `Serial.print()`?

Terdapat dua alasan teknis utama:

**a) `delay()` tidak berfungsi di dalam ISR:**

Fungsi `delay()` bergantung pada Timer0 overflow interrupt untuk menghitung waktu. Namun, saat ISR sedang berjalan, global interrupt dinonaktifkan (bit I pada register SREG di-clear). Akibatnya, Timer0 interrupt tidak dapat memperbarui counter waktu, sehingga `delay()` akan menunggu selamanya dan menyebabkan sistem hang.

**b) `Serial.print()` tidak aman digunakan dalam ISR:**

Fungsi `Serial.print()` menggunakan buffer circular dan mengandalkan UART TX interrupt secara internal. Karena interrupt dinonaktifkan selama ISR, UART interrupt tidak dapat memproses buffer, sehingga fungsi ini bisa hang atau menghasilkan data yang korup.

**Prinsip umum ISR yang baik:**
- Operasi hanya mengubah flag/variabel global (`volatile`)
- Tidak ada fungsi blocking
- Tidak ada alokasi memori dinamis
- Durasi eksekusi sesingkat mungkin (mikrodetik)

---

### 4. Apa fungsi keyword `volatile` pada variabel `ledState`?

Keyword `volatile` memberitahu compiler bahwa nilai variabel `ledState` dapat berubah kapan saja di luar alur normal program (yaitu di dalam ISR), sehingga compiler tidak boleh melakukan optimasi pada variabel tersebut.

**Masalah tanpa `volatile`:**

```cpp
// Tanpa volatile, compiler bisa mengoptimasi loop() menjadi:
// Jika ledState selalu false pada awal, compiler bisa "meng-cache" nilainya
// dan tidak membacanya ulang dari RAM → nilai tidak pernah update

bool ledState = false; // ← SALAH untuk shared variable dengan ISR
```

**Dengan `volatile`:**

```cpp
// Compiler selalu membaca nilai terbaru dari RAM (bukan dari register cache)
// Setiap akses ke ledState menghasilkan instruksi load dari memori

volatile bool ledState = false; // ← BENAR
```

Ini penting karena ISR dan `loop()` berbagi variabel yang sama, dan compiler tidak "tahu" bahwa ISR bisa mengubah nilai tersebut.

---

### 5. Modifikasi Mode Interrupt

Berikut penjelasan dan perbandingan seluruh mode interrupt:

| Mode | Trigger | Perilaku LED | Kapan Digunakan |
|------|---------|--------------|-----------------|
| `FALLING` | HIGH → LOW | Toggle saat tombol **ditekan** | Deteksi penekanan tombol (paling umum) |
| `RISING` | LOW → HIGH | Toggle saat tombol **dilepas** | Deteksi pelepasan tombol |
| `CHANGE` | Kedua arah | Toggle dua kali per siklus tekan-lepas | Encoder rotary, deteksi perubahan apapun |
| `LOW` | Sinyal LOW terus | ISR dipanggil ratusan kali/detik → LED berkedip tidak terkontrol | Jarang digunakan untuk toggle LED |

**Source Code Modifikasi – Mode RISING:**

```cpp
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState;
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  
  // RISING: interrupt dipicu saat sinyal naik LOW → HIGH
  // Dengan INPUT_PULLUP: terjadi saat tombol DILEPAS
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, RISING);
}

void loop() {
  digitalWrite(13, ledState);
}
```

**Source Code Modifikasi – Mode CHANGE:**

```cpp
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState;
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  
  // CHANGE: interrupt dipicu pada setiap perubahan sinyal (FALLING + RISING)
  // LED toggle 2x per siklus tekan-lepas → kembali ke kondisi awal
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, CHANGE);
}

void loop() {
  digitalWrite(13, ledState);
}
```

**Analisis Perubahan Perilaku:**

- **FALLING (default):** LED toggle sekali per penekanan. Perilaku paling intuitif untuk toggle on/off.
- **RISING:** LED toggle sekali per pelepasan. Berguna jika aksi harus dilakukan setelah tombol dilepas (misal: pengukuran lama tombol ditekan).
- **CHANGE:** LED toggle dua kali per siklus (tekan + lepas), sehingga LED tampak berkedip cepat dan akhirnya kembali ke state awal. Berguna untuk encoder rotary atau deteksi kedua tepi sinyal.
- **LOW:** ISR dipanggil terus-menerus selama tombol ditekan, mengakibatkan LED berkedip sangat cepat dan perilaku tidak deterministik. Tidak disarankan untuk kasus toggle LED.

---


## Jawaban Pertanyaan 6B

### 1. Bagaimana fungsi `millis()` bekerja?

`millis()` mengembalikan jumlah milidetik yang telah berlalu sejak Arduino pertama kali dinyalakan (atau di-reset).

**Cara kerja internal:**

1. Saat Arduino boot, framework Arduino mengonfigurasi Timer0 dengan prescaler tertentu untuk menghasilkan overflow interrupt setiap ~1 ms.
2. Setiap kali Timer0 overflow, ISR internal Arduino menginkrementasi counter `timer0_millis`.
3. Fungsi `millis()` hanya mengembalikan nilai `timer0_millis` tersebut.

**Cara kerja pada program:**

```
currentMillis = millis()  → misal: 5230 ms

previousMillis = 5000 ms (saat terakhir LED toggle)

5230 - 5000 = 230 ms  →  belum >= 1000, tidak ada aksi

...beberapa loop kemudian...

currentMillis = 6001 ms

6001 - 5000 = 1001 ms  →  >= 1000!  → toggle LED, previousMillis = 6001
```

Metode pengurangan ini juga aman terhadap overflow `millis()` yang terjadi setelah ~49,7 hari.

---

### 2. Perbedaan utama `delay()` dan `millis()`

| Aspek | `delay()` | `millis()` |
|-------|-----------|------------|
| Sifat eksekusi | **Blocking** – menghentikan seluruh program | **Non-blocking** – program terus berjalan |
| CPU saat menunggu | Berhenti total (busy-wait) | Bebas mengerjakan task lain |
| Multitasking | ❌ Tidak mendukung | ✅ Mendukung banyak task |
| Interrupt selama jeda | Tetap berjalan (timer interrupt) | Tetap berjalan |
| Penggunaan sensor/input selama jeda | ❌ Tidak bisa memproses | ✅ Bisa diproses |
| Presisi | Cukup (dipengaruhi overhead) | Sangat presisi |
| Kerumitan kode | Sederhana | Sedikit lebih kompleks |

**Contoh perbedaan nyata:**

```cpp
// DENGAN delay() - tidak bisa mendeteksi tombol saat menunggu:
delay(1000); // CPU diam total 1 detik

// DENGAN millis() - tombol tetap bisa dideteksi saat menunggu:
if (millis() - prev >= 1000) { /* aksi */ }
// Baris lain bisa dieksekusi di sini
```

---

### 3. Mengapa `millis()` disebut non-blocking?

`millis()` disebut non-blocking** karena:

1. **Tidak menghentikan eksekusi program.** Pemanggilan `millis()` hanya membaca nilai counter, bukan menunggu. Fungsi `loop()` terus dieksekusi setiap siklus.

2. **CPU tetap bebas.** Di antara waktu pengecekan, CPU dapat mengeksekusi instruksi lain, membaca sensor, memproses input, atau menjalankan task lain.

3. **Pola polling waktu, bukan busy-wait.** Program hanya "bertanya" apakah sudah waktunya melakukan aksi, tanpa berhenti untuk menunggu.

Analogi: `delay()` seperti seseorang yang berdiri diam menunggu alarm; `millis()` seperti seseorang yang terus bekerja sambil sesekali melihat jam.

---

### 4. Modifikasi Dua LED dengan Interval Berbeda

**LED1 (Pin 13): berkedip setiap 1000 ms**  
**LED2 (Pin 12): berkedip setiap 500 ms**

**File:** `modul6_timer_dua_led.ino`

```cpp
#include <Arduino.h>

// === Konfigurasi LED 1 (Pin 13, interval 1000 ms) ===
unsigned long prevMillis1 = 0;   // Waktu terakhir LED1 berubah
const long interval1 = 1000;     // Interval LED1: 1 detik
bool ledState1 = false;           // Status LED1 saat ini

// === Konfigurasi LED 2 (Pin 12, interval 500 ms) ===
unsigned long prevMillis2 = 0;   // Waktu terakhir LED2 berubah
const long interval2 = 500;      // Interval LED2: 500 milidetik
bool ledState2 = false;           // Status LED2 saat ini

void setup() {
  // Konfigurasi kedua pin sebagai output
  pinMode(13, OUTPUT);  // LED1
  pinMode(12, OUTPUT);  // LED2
}

void loop() {
  // Ambil waktu saat ini sekali per iterasi (efisiensi)
  unsigned long now = millis();

  // === Task 1: LED1 berkedip setiap 1000 ms ===
  // Cek apakah sudah 1000 ms sejak LED1 terakhir berubah
  if (now - prevMillis1 >= interval1) {
    prevMillis1 = now;          // Perbarui waktu referensi LED1
    ledState1 = !ledState1;     // Toggle state LED1
    digitalWrite(13, ledState1); // Terapkan ke pin 13
  }

  // === Task 2: LED2 berkedip setiap 500 ms ===
  // Cek apakah sudah 500 ms sejak LED2 terakhir berubah
  // Task ini independen dari Task 1 karena menggunakan prevMillis2 terpisah
  if (now - prevMillis2 >= interval2) {
    prevMillis2 = now;          // Perbarui waktu referensi LED2
    ledState2 = !ledState2;     // Toggle state LED2
    digitalWrite(12, ledState2); // Terapkan ke pin 12
  }

  // Kedua task berjalan dalam satu iterasi loop()
  // Tidak ada delay() → program non-blocking
  // Dapat menambahkan task ketiga, keempat, dst. dengan pola yang sama
}
```

**Penjelasan Konsep Multitasking:**

Setiap LED memiliki **variabel timer independen** (`prevMillis1` dan `prevMillis2`), sehingga penghitungan waktu keduanya tidak saling mempengaruhi. Dalam satu iterasi `loop()`:

- Kedua kondisi `if` diperiksa setiap siklus
- Jika `interval1` tercapai → LED1 toggle
- Jika `interval2` tercapai → LED2 toggle
- Kedua kondisi bisa terpenuhi dalam iterasi yang sama atau berbeda

Pola ini adalah dasar dari **cooperative multitasking** pada sistem embedded tanpa RTOS.

**Wiring tambahan:**

| No. | Komponen | Pin Arduino |
|-----|----------|-------------|
| 1 | LED1 (+ resistor 220Ω) | Pin 13 |
| 2 | LED2 (+ resistor 220Ω) | Pin 12 |
| 3 | GND | GND |

---


## Jawaban Pertanyaan Umum

### 1. Keuntungan interrupt dibanding polling

| Aspek | Polling | Interrupt |
|-------|---------|-----------|
| Efisiensi CPU | Rendah (CPU terus memeriksa) | Tinggi (CPU bebas saat tidak ada event) |
| Latensi respons | Bergantung panjang loop | Hampir instan (< beberapa µs) |
| Konsumsi daya | Tinggi | Rendah (CPU bisa sleep) |
| Determinisme waktu | Tidak terjamin | Terjamin (fixed latency) |
| Kompleksitas kode | Sederhana | Sedikit lebih kompleks |
| Skalabilitas | Buruk (semakin banyak input = semakin lambat) | Baik (tidak bergantung jumlah task) |

**Keuntungan utama interrupt:**

1. **Efisiensi CPU:** CPU tidak membuang siklus clock untuk memeriksa kondisi yang jarang berubah.
2. **Respons cepat dan deterministik:** Interrupt ditangani segera, bukan menunggu giliran polling.
3. **Penghematan daya:** CPU dapat masuk mode _sleep_ dan hanya aktif saat interrupt terjadi, sangat penting untuk perangkat baterai/IoT.
4. **Skalabilitas:** Menambah sumber interrupt baru tidak memperlambat sistem, tidak seperti polling yang semakin berat seiring banyaknya input yang diperiksa.

---

### 2. Mengapa timer penting dalam sistem embedded dan real-time?

Timer adalah komponen fundamental dalam sistem embedded karena:

1. **Penjadwalan task periodik:** Menjalankan task pada interval waktu yang tepat dan konsisten tanpa mengorbankan responsivitas sistem.

2. **Penghasil sinyal PWM:** Mengontrol kecepatan motor DC, kecerahan LED, dan berbagai aktuator analog lainnya dengan presisi tinggi.

3. **Pengukuran waktu dan frekuensi:** Mengukur durasi pulsa, menghitung frekuensi sinyal, mengukur jarak (ultrasonik), dan sebagainya.

4. **Komunikasi serial:** Sebagai _baud rate generator_ untuk UART, SPI, dan I2C agar sinkronisasi data akurat.

5. **Sistem real-time:** Memastikan task diselesaikan sebelum _deadline_, yang kritis pada sistem kontrol industri, robotik, dan medis.

6. **Watchdog timer:** Mendeteksi jika sistem hang dan melakukan reset otomatis untuk meningkatkan keandalan sistem.

---

### 3. Alur kerja kombinasi interrupt dan timer

Ketika interrupt dan timer digabung dalam satu sistem, alur kerjanya sebagai berikut:

```
┌─────────────────────────────────────────────┐
│              SISTEM BERJALAN                │
└─────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────┐
│  loop() berjalan secara berulang            │
│  ┌─────────────────────────────────────┐   │
│  │  Cek millis() untuk task periodik   │   │
│  │  Jalankan task jika interval tiba   │   │
│  └─────────────────────────────────────┘   │
└─────────────────────────────────────────────┘
                     │
         Event eksternal terjadi (tombol)
                     │
                     ▼
┌─────────────────────────────────────────────┐
│  Hardware interrupt aktif (FALLING edge)    │
│  CPU menyimpan konteks ke stack             │
│  Global interrupt dinonaktifkan sementara  │
└─────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────┐
│  ISR dieksekusi (singkat, hanya flag toggle)│
│  → ledState = !ledState;                   │
└─────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────┐
│  Konteks dipulihkan dari stack              │
│  Global interrupt diaktifkan kembali        │
│  Timer0 interrupt (millis) tetap akurat     │
└─────────────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────┐
│  loop() melanjutkan dari titik interupsi    │
│  Membaca flag dari ISR → aksi dilakukan     │
└─────────────────────────────────────────────┘
```

**Kunci penting:** Timer0 interrupt (yang memperbarui counter `millis()`) juga merupakan interrupt, sehingga ia akan ditunda sementara selama ISR user berjalan. Ini adalah alasan mengapa ISR harus sesingkat mungkin agar akurasi waktu `millis()` tidak terganggu.

---

### 4. Dampak ISR yang terlalu panjang atau kompleks

Jika ISR terlalu panjang atau kompleks, dampak negatifnya meliputi:

1. **Latensi interrupt lain meningkat:**  
   Selama ISR berjalan, interrupt lain (termasuk interrupt dari hardware lain) tidak dapat diproses. Semakin panjang ISR, semakin besar penundaan penanganan interrupt berikutnya.

2. **Ketidakakuratan timer:**  
   Timer0 interrupt yang memperbarui `millis()` tidak dapat berjalan selama ISR aktif. Jika ISR berlangsung lama (misalnya 5 ms), nilai `millis()` akan "melompat" 5 ms sekaligus, menyebabkan timing yang tidak akurat.

3. **Program utama tertunda:**  
   `loop()` tidak dapat berjalan selama ISR aktif. Jika ISR berjalan 50 ms dan dipicu 20 kali per detik, program utama hanya mendapat waktu berjalan ~1 detik per detik.

4. **Risiko stack overflow:**  
   Setiap pemanggilan ISR menyimpan konteks ke stack. ISR yang panjang dengan banyak variabel lokal dan fungsi bersarang dapat menghabiskan stack RAM yang terbatas (2KB pada ATmega328P).

5. **Pelanggaran deadline real-time:**  
   Pada sistem real-time, setiap task memiliki deadline. ISR yang panjang dapat menyebabkan task periodik terlewat deadlinenya, yang dapat berujung pada kegagalan sistem kritis.

**Praktik terbaik penulisan ISR:**

```cpp
// ✅ BAIK: ISR singkat, hanya set flag
volatile bool buttonPressed = false;

void ISR_Baik() {
  buttonPressed = true;  // Hanya 1 instruksi
}

// Proses di loop() berdasarkan flag
void loop() {
  if (buttonPressed) {
    buttonPressed = false;
    // Lakukan proses panjang di sini, bukan di ISR
  }
}

// ❌ BURUK: ISR panjang dan blocking
void ISR_Buruk() {
  delay(100);           // ❌ Tidak berfungsi, menyebabkan hang
  Serial.println("OK"); // ❌ Tidak aman di ISR
  for(int i=0; i<1000; i++) { /* heavy loop */ } // ❌ Terlalu lama
}
```
