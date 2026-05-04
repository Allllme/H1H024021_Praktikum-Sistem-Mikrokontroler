## Pertanyaan Praktikum 1A
### 1. Pada kondisi apa program masuk ke blok if?
Program masuk ke blok if ketika nilai variabel timeDelay sudah ≤ 100 ms. Ini berarti LED telah mencapai kecepatan kerlip maksimum. Pada kondisi ini, program akan memberikan jeda 3 detik lalu mereset timeDelay kembali ke 1000 ms.

### 2. Pada kondisi apa program masuk ke blok else?
Program masuk ke blok else ketika nilai timeDelay masih > 100 ms. Artinya LED belum mencapai kecepatan maksimum, sehingga timeDelay dikurangi 100 ms untuk mempercepat kerlip pada siklus berikutnya.

### 3. Apa fungsi dari perintah delay(timeDelay)?
Fungsi delay(timeDelay) menghentikan sementara eksekusi program selama durasi timeDelay milidetik. Karena nilai timeDelay berubah setiap siklus (berkurang 100 ms), durasi LED menyala dan mati pun ikut berubah, menciptakan efek percepatan kerlip yang bertahap dan dapat diamati oleh mata manusia.

### 4. Modifikasi: alur cepat → sedang → mati (tanpa langsung reset)
Program dimodifikasi menggunakan variabel phase untuk melacak tahap saat ini: 
<img width="733" height="1200" alt="image" src="https://github.com/user-attachments/assets/99af4328-b766-4756-bbe9-861fe446bca7" />
#### Penjelasan Kode

```cpp
const int ledPin = 6;
```
> Mendeklarasikan variabel konstan `ledPin` dengan nilai 6.  
> Pin digital 6 akan digunakan sebagai output untuk LED.

```cpp
int timeDelay = 1000;
```
> Variabel `timeDelay` menyimpan nilai delay dalam milidetik.  
> Dimulai dari 1000 ms (1 detik) → LED berkedip lambat di awal.

```cpp
pinMode(ledPin, OUTPUT);
```
> Mengonfigurasi pin 6 sebagai OUTPUT di dalam fungsi `setup()`.  
> Hanya dijalankan sekali saat Arduino pertama kali dinyalakan.

```cpp
digitalWrite(ledPin, HIGH);
delay(timeDelay);
```
> Mengirim logika HIGH (5V) ke pin LED → LED **menyala**.  
> Program berhenti selama `timeDelay` ms sebelum melanjutkan.

```cpp
digitalWrite(ledPin, LOW);
delay(timeDelay);
```
> Mengirim logika LOW (0V) ke pin LED → LED **mati**.  
> Program berhenti lagi selama `timeDelay` ms (satu siklus kedip selesai).

```cpp
if (timeDelay <= 100) {
```
> **Kondisi if:** memeriksa apakah `timeDelay` sudah mencapai batas minimum (100 ms).  
> Jika benar → LED sudah berkedip secepat mungkin, siap direset.

```cpp
    delay(3000);
    timeDelay = 1000;
```
> Memberikan jeda 3 detik agar pengguna bisa mengamati kondisi reset.  
> Kemudian `timeDelay` dikembalikan ke 1000 ms untuk memulai siklus baru.

```cpp
} else {
    timeDelay -= 100;
}
```
> **Kondisi else:** jika `timeDelay` masih di atas 100 ms.  
> Nilai `timeDelay` dikurangi 100 ms → LED akan berkedip lebih cepat di iterasi berikutnya.

---


## Pertanyaan Praktikum 1B
### Schematic
![WhatsApp Image 2026-04-08 at 23 29 50](https://github.com/user-attachments/assets/56a430a0-51dc-4f4a-ae57-c7eda7bdfb3e)

### 2. Bagaimana program membuat efek LED berjalan dari kiri ke kanan?
Program menggunakan `for (int ledPin = 2; ledPin < 8; ledPin++)`. Variabel `ledPin` dimulai dari pin 2 dan bertambah 1 setiap iterasi. Pada setiap iterasi, pin tersebut di-set HIGH (menyala) selama 100ms, lalu di-set LOW (mati). Karena hanya satu pin yang HIGH pada satu waktu dan berpindah secara berurutan, mata manusia menangkap ilusi LED yang "bergerak" dari kiri ke kanan.

### 3. Bagaimana program membuat LED kembali dari kanan ke kiri?
Program menggunakan perulangan kedua: `for (int ledPin = 7; ledPin >= 2; ledPin--)`. Perbedaan dengan perulangan pertama:
- **Inisialisasi:** `ledPin = 7` (dimulai dari pin paling kanan)
- **Kondisi:** `ledPin >= 2` (berhenti ketika sudah < 2)
- **Decrement:** `ledPin--` (berkurang 1, bergerak ke kiri)

Mekanisme menyala dan matinya LED sama persis, hanya arah traversalnya yang terbalik.

### 4. Program 3 LED Kiri dan 3 LED Kanan Bergantian
<img width="946" height="1085" alt="image" src="https://github.com/user-attachments/assets/6ad99c06-a1d3-4565-bb9a-b0d21a5cb604" />

#### Penjelasan Kode

```cpp
int timer = 100;
```
> Variabel `timer` menyimpan nilai delay dalam milidetik antar perpindahan LED.  
> Nilai 100 ms berarti setiap LED menyala selama 0,1 detik sebelum berpindah.  
> Semakin kecil nilainya → LED bergerak semakin cepat.

```cpp
for (int ledPin = 2; ledPin < 8; ledPin++) {
    pinMode(ledPin, OUTPUT);
}
```
> Perulangan `for` di `setup()` untuk menginisialisasi pin 2 sampai 7 sebagai OUTPUT.  
> - Inisialisasi: `ledPin = 2` (mulai dari pin 2)  
> - Kondisi: `ledPin < 8` (berjalan selama ledPin kurang dari 8, yaitu s/d pin 7)  
> - Increment: `ledPin++` (bertambah 1 setiap iterasi)  
> Lebih efisien daripada menulis 6 baris `pinMode()` secara manual.

```cpp
for (int ledPin = 2; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH);
    delay(timer);
    digitalWrite(ledPin, LOW);
}
```
> **Perulangan 1 – LED bergerak kiri ke kanan (pin 2 → 7):**  
> - `digitalWrite(ledPin, HIGH)` → LED pada pin saat ini **menyala**  
> - `delay(timer)` → tahan 100ms agar mata bisa melihat LED  
> - `digitalWrite(ledPin, LOW)` → LED pada pin saat ini **mati** sebelum berpindah  
> Hanya satu LED yang menyala pada satu waktu, menciptakan ilusi gerakan.

```cpp
for (int ledPin = 7; ledPin >= 2; ledPin--) {
    digitalWrite(ledPin, HIGH);
    delay(timer);
    digitalWrite(ledPin, LOW);
}
```
> **Perulangan 2 – LED bergerak kanan ke kiri (pin 7 → 2):**  
> - Inisialisasi: `ledPin = 7` (mulai dari pin 7, yaitu LED paling kanan)  
> - Kondisi: `ledPin >= 2` (berjalan selama ledPin masih ≥ 2)  
> - Decrement: `ledPin--` (berkurang 1 setiap iterasi, arah berlawanan)  
> Mekanisme menyala dan mati sama dengan perulangan pertama.

---


## Pertanyaan Analisis
### 1. Uraikan hasil tugas pada praktikum yang telah dilakukan pada setiap percobaan!

#### 🔴 Percobaan 1A — Percabangan (if-else)

Program menggunakan satu LED pada Pin 6 dengan variabel `timeDelay` yang dimulai dari nilai `1000` ms. Setiap satu siklus kedip (nyala–mati), nilai `timeDelay` dikurangi sebesar 100 ms sehingga LED berkedip semakin cepat.

**Alur perilaku LED yang diamati:**
```
Mati → Kedip Lambat (1000ms) → Semakin Cepat → Kedip Cepat (100ms) → Jeda 3 detik → Reset ke Lambat
```

**Kode program utama:**
```cpp
const int ledPin = 6;
int timeDelay = 1000;

void setup() {
  pinMode(ledPin, OUTPUT); // Konfigurasi pin 6 sebagai output
}

void loop() {
  digitalWrite(ledPin, HIGH); // Nyalakan LED
  delay(timeDelay);            // Tunda sesuai timeDelay saat ini
  digitalWrite(ledPin, LOW);  // Matikan LED
  delay(timeDelay);            // Tunda lagi

  // Percabangan: cek apakah delay sudah mencapai batas minimum
  if (timeDelay <= 100) {
    delay(3000);        // Jeda sebelum reset
    timeDelay = 1000;   // Reset ke nilai awal (lambat)
  } else {
    timeDelay -= 100;   // Kurangi delay 100ms → LED makin cepat
  }
}
```

**Analisis:** Program berjalan sesuai spesifikasi. LED berhasil menunjukkan fase mati → lambat → cepat → reset secara berulang tanpa error.

---

#### 🔴 Percobaan 2A — Perulangan (for)

Program menggunakan 6 LED pada Pin 2–7. LED menyala satu per satu dari kiri ke kanan (Pin 2 → Pin 7), lalu berbalik dari kanan ke kiri (Pin 7 → Pin 2) secara terus-menerus.

**Konfigurasi pin:**

| No | Komponen | Pin Arduino |
|----|----------|-------------|
| 1  | LED 1    | Pin 2       |
| 2  | LED 2    | Pin 3       |
| 3  | LED 3    | Pin 4       |
| 4  | LED 4    | Pin 5       |
| 5  | LED 5    | Pin 6       |
| 6  | LED 6    | Pin 7       |
| 7  | GND      | GND         |

**Kode program utama:**
```cpp
int timer = 100; // Delay antar LED (ms)

void setup() {
  // Inisialisasi semua pin LED sebagai OUTPUT
  for (int ledPin = 2; ledPin < 8; ledPin++) {
    pinMode(ledPin, OUTPUT);
  }
}

void loop() {
  // Kiri ke kanan: Pin 2 sampai Pin 7
  for (int ledPin = 2; ledPin < 8; ledPin++) {
    digitalWrite(ledPin, HIGH); // Nyalakan LED saat ini
    delay(timer);               // Tunggu 100ms
    digitalWrite(ledPin, LOW);  // Matikan LED
  }

  // Kanan ke kiri: Pin 7 sampai Pin 2
  for (int ledPin = 7; ledPin >= 2; ledPin--) {
    digitalWrite(ledPin, HIGH); // Nyalakan LED saat ini
    delay(timer);               // Tunggu 100ms
    digitalWrite(ledPin, LOW);  // Matikan LED
  }
}
```

**Analisis:** Program berjalan sesuai spesifikasi. Efek LED running berhasil terlihat dengan pola bolak-balik tanpa error.

---

### 2. Pengaruh Struktur Perulangan (for dan while)

Struktur perulangan sangat berpengaruh terhadap jalannya program pada Arduino dalam beberapa aspek:

**Efisiensi Kode**
Tanpa perulangan, untuk menginisialisasi 6 pin LED, kita perlu menulis 6 baris `pinMode()` secara manual. Dengan `for`, cukup satu blok kode:
```cpp
// Tanpa perulangan (tidak efisien):
pinMode(2, OUTPUT);
pinMode(3, OUTPUT);
// ... dst.

// Dengan perulangan (efisien):
for (int ledPin = 2; ledPin < 8; ledPin++) {
  pinMode(ledPin, OUTPUT);
}
```

**Kontrol Aliran Program**
- `for` cocok digunakan ketika jumlah iterasi sudah diketahui sejak awal (misalnya: 6 LED, pin 2–7). Program berjalan dalam jumlah langkah yang terukur dan dapat diprediksi.
- `while` cocok digunakan ketika iterasi bergantung pada kondisi yang bisa berubah sewaktu-waktu (misalnya: tekan tombol, nilai sensor). Program akan terus berjalan selama kondisi terpenuhi.

**Pengaruh pada Sistem Fisik**
Pada percobaan LED running, perulangan `for` memungkinkan program mengaktifkan setiap pin secara berurutan dengan jeda waktu tertentu, sehingga menciptakan efek visual "berjalan" yang halus. Tanpa perulangan, efek tersebut tidak dapat dibuat secara dinamis.

**Risiko**
Perulangan `while(true)` tanpa kondisi keluar dapat menyebabkan program terjebak selamanya (*infinite loop*), sehingga mikrokontroler tidak responsif terhadap input lain. Perancangan kondisi perulangan harus dilakukan dengan hati-hati.

---

### 3. Cara Kerja Percabangan (if-else)

Percabangan `if-else` bekerja dengan mengevaluasi sebuah ekspresi kondisi bertipe boolean (`true`/`false`). Berdasarkan hasil evaluasi, program akan memilih salah satu dari dua jalur eksekusi.

**Mekanisme Kerja pada Percobaan 1A:**

```
Evaluasi kondisi: (timeDelay <= 100)
        │
        ├─── TRUE  ──→ Jalankan blok IF
        │              → delay(3000)    [jeda panjang]
        │              → timeDelay = 1000 [reset ke lambat]
        │
        └─── FALSE ──→ Jalankan blok ELSE
                       → timeDelay -= 100 [percepat kedip]
```

**Penjelasan alur:**
1. Program masuk blok **`if`** ketika `timeDelay` sudah mencapai nilai 100 atau kurang, artinya LED telah berkedip pada kecepatan maksimum. Di sini program memberikan jeda 3 detik lalu mereset kecepatan ke awal.
2. Program masuk blok **`else`** selama `timeDelay` masih di atas 100, artinya kecepatan masih bisa ditingkatkan. Setiap iterasi, delay dikurangi 100 ms sehingga LED terlihat berkedip lebih cepat.

**Analogi Sederhana:**
```
Jika (lampu merah) {
    berhenti;
} else {
    jalan terus;
}
```
Percabangan `if-else` pada dasarnya memberikan kemampuan pengambilan keputusan kepada program, layaknya manusia yang merespons kondisi berbeda dengan tindakan yang berbeda pula.

---

### 4. Kombinasi Perulangan dan Percabangan

Kombinasi antara perulangan dan percabangan memungkinkan sistem Arduino menjadi responsif terhadap perubahan kondisi lingkungan secara dinamis dan real-time.

**Konsep Dasar:**
- Perulangan → membuat sistem terus berjalan dan memantau kondisi secara berkala.
- Percabangan → membuat sistem bereaksi berbeda sesuai kondisi yang terdeteksi.

#### Contoh Implementasi: LED Responsif terhadap Sensor/Tombol

```cpp
const int buttonPin = 8;  // Pin tombol
const int ledPins[] = {2, 3, 4, 5, 6, 7}; // 6 LED

void setup() {
  pinMode(buttonPin, INPUT_PULLUP); // Tombol sebagai input
  for (int i = 0; i < 6; i++) {
    pinMode(ledPins[i], OUTPUT);    // LED sebagai output
  }
}

void loop() {
  int buttonState = digitalRead(buttonPin); // Baca kondisi tombol

  for (int i = 0; i < 6; i++) {
    // Percabangan di dalam perulangan
    if (buttonState == LOW) {
      // Jika tombol ditekan: semua LED menyala
      digitalWrite(ledPins[i], HIGH);
    } else {
      // Jika tombol tidak ditekan: LED berkedip berurutan
      digitalWrite(ledPins[i], HIGH);
      delay(100);
      digitalWrite(ledPins[i], LOW);
    }
  }
}
```

**Analisis Kombinasi:**

| Mekanisme | Peran dalam Sistem Responsif |
|-----------|------------------------------|
| `for` loop | Memindai semua pin LED/sensor secara berurutan |
| `if-else` di dalam loop | Menentukan aksi berbeda untuk setiap kondisi yang ditemukan |
| `while` loop | Mempertahankan program berjalan terus selama sistem hidup |
| Kondisi nested | Memungkinkan logika bertingkat untuk respons yang lebih kompleks |

**Contoh Skenario Nyata:**
```
WHILE sistem menyala:
    Baca semua sensor (FOR setiap sensor)
        IF nilai sensor melebihi ambang batas:
            Nyalakan LED peringatan
        ELSE IF nilai sensor normal:
            Matikan LED peringatan
        ELSE:
            Kedipkan LED (status tidak diketahui)
```

Dengan pola ini, sistem Arduino dapat merespons perubahan suhu, cahaya, tekanan, atau input pengguna secara otomatis dan real-time tanpa perlu campur tangan manusia terus-menerus.

---
