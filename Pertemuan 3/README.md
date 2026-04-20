## Jawaban Pertanyaan Praktikum 3A

### Pertanyaan 1: Jelaskan proses dari input keyboard hingga LED menyala/mati

Proses pengendalian LED melalui keyboard berlangsung sebagai berikut:

1. **Input pengguna**: Pengguna mengetikkan karakter ('1' atau '0') di Serial Monitor Arduino IDE dan menekan Enter.
2. **Konversi USB-to-Serial**: Karakter tersebut dikirimkan melalui koneksi USB dari komputer ke chip USB-to-Serial (ATmega16U2) pada Arduino Uno.
3. **Transmisi UART**: Chip ATmega16U2 mengkonversi data USB menjadi sinyal UART dengan baud rate 9600 bps dan meneruskannya ke pin RX (pin 0) chip utama ATmega328P. Data dikirim dalam format: 1 start bit + 8 data bit + 1 stop bit = 10 bit per karakter.
4. **Buffer UART**: ATmega328P menerima sinyal UART dan menyimpan byte karakter ke buffer penerimaan UART.
5. **Deteksi data**: Fungsi `Serial.available()` mengembalikan nilai > 0 karena buffer tidak kosong.
6. **Pembacaan karakter**: `Serial.read()` mengambil 1 byte dari buffer.
7. **Logika program**: Program memeriksa nilai karakter:
   - Jika `'1'` → `digitalWrite(PIN_LED, HIGH)` mengaktifkan pin 8 → arus mengalir dari pin 8 melalui resistor → LED → GND → **LED menyala**
   - Jika `'0'` → `digitalWrite(PIN_LED, LOW)` → pin 8 = 0V → tidak ada beda potensial → **LED mati**
8. **Konfirmasi output**: Arduino mengirimkan konfirmasi ("LED ON"/"LED OFF") kembali ke komputer via UART sehingga tampil di Serial Monitor.

Seluruh proses ini terjadi dalam hitungan milidetik.

---

### Pertanyaan 2: Mengapa digunakan `Serial.available()` sebelum membaca data?

`Serial.available()` mengembalikan jumlah byte yang tersedia di buffer penerimaan UART. Fungsi ini digunakan sebelum `Serial.read()` karena:

| Kondisi | Tanpa `Serial.available()` | Dengan `Serial.available()` |
|---------|---------------------------|------------------------------|
| Tidak ada data masuk | `Serial.read()` mengembalikan `-1` (0xFF saat di-cast ke `char`), program mengeksekusi blok `else` dan mencetak "Perintah tidak dikenal" setiap iterasi loop | Program melewati blok baca, tidak ada aksi berlebihan |
| Ada data masuk | Bekerja, tetapi boros CPU untuk polling terus-menerus | Hanya membaca ketika ada data valid |

**Akibat jika baris `Serial.available()` dihilangkan:**
- Serial Monitor akan dibanjiri pesan "Perintah tidak dikenal" karena `Serial.read()` terus mengembalikan -1 saat tidak ada data.
- Sistem menjadi tidak efisien dan output sulit dibaca.

---

### Pertanyaan 3: Modifikasi program untuk LED blink saat input '2'

```cpp
#include <Arduino.h>

const int PIN_LED = 8;         // Mendefinisikan konstanta pin LED di pin 8

// Variabel untuk mode blink (Pertanyaan 3)
bool blinkMode = false;                // Flag apakah mode blink aktif
unsigned long lastBlinkTime = 0;       // Menyimpan waktu terakhir LED berubah state
bool ledState = false;                 // State LED saat ini (true=ON, false=OFF)
const long blinkInterval = 500;        // Interval blink dalam milidetik (500ms = 1x/detik)

void setup() {
  Serial.begin(9600);                  // Inisialisasi komunikasi serial dengan baud rate 9600 bps
                                       // Nilai ini harus sama dengan pengaturan Serial Monitor
  Serial.println("Ketik '1' untuk menyalakan LED, '0' untuk mematikan, '2' untuk blink");
  pinMode(PIN_LED, OUTPUT);            // Mengatur pin LED sebagai output digital
}

void loop() {
  // --- Bagian pembacaan input UART ---
  if (Serial.available() > 0) {       // Memeriksa apakah ada data yang masuk di buffer serial
                                       // Mengembalikan jumlah byte yang tersedia; > 0 berarti ada data
    char data = Serial.read();         // Membaca 1 karakter dari buffer serial UART

    if (data == '1') {                 // Jika karakter yang diterima adalah '1'
      blinkMode = false;               // Matikan mode blink terlebih dahulu
      digitalWrite(PIN_LED, HIGH);     // Aktifkan pin 8 → arus mengalir → LED menyala
      Serial.println("LED ON");        // Kirim konfirmasi ke Serial Monitor via UART
    }
    else if (data == '0') {            // Jika karakter yang diterima adalah '0'
      blinkMode = false;               // Matikan mode blink terlebih dahulu
      digitalWrite(PIN_LED, LOW);      // Nonaktifkan pin 8 → tidak ada arus → LED mati
      Serial.println("LED OFF");       // Kirim konfirmasi ke Serial Monitor via UART
    }
    else if (data == '2') {            // Jika karakter yang diterima adalah '2'
      blinkMode = true;                // Aktifkan mode blink
      Serial.println("LED BLINK MODE aktif. Kirim '0' atau '1' untuk keluar.");
    }
    else if (data != '\n' && data != '\r') {  // Filter karakter newline dan carriage return
                                              // Kedua karakter ini dikirim otomatis saat tekan Enter
      Serial.println("Perintah tidak dikenal. Gunakan '0', '1', atau '2'.");
    }
  }

  // --- Bagian non-blocking blink menggunakan millis() (Pertanyaan 3 & 4) ---
  if (blinkMode) {                                      // Hanya jalankan jika mode blink aktif
    unsigned long currentTime = millis();               // Ambil waktu saat ini (ms sejak Arduino nyala)
    if (currentTime - lastBlinkTime >= blinkInterval) { // Cek apakah sudah waktunya toggle LED
      lastBlinkTime = currentTime;                      // Perbarui waktu referensi terakhir
      ledState = !ledState;                             // Toggle state LED (true→false atau false→true)
      digitalWrite(PIN_LED, ledState ? HIGH : LOW);     // Terapkan state baru ke pin LED
    }
    // CATATAN: Tidak ada delay() di sini! Program terus berjalan ke atas (loop)
    // sehingga Serial.available() tetap dapat diperiksa setiap saat → RESPONSIF
  }
}
```

**Penjelasan logika blink non-blocking:**

```
Iterasi 1 (t=0ms)    : Serial.available() → '2' diterima → blinkMode = true
Iterasi 2 (t=1ms)    : blinkMode=true, millis()-lastBlinkTime = 1ms < 500ms → tidak toggle
...
Iterasi N (t=500ms)  : millis()-lastBlinkTime = 500ms ≥ 500ms → toggle LED, lastBlinkTime = 500
...
Iterasi M (t=501ms)  : Serial.available() → '0' diterima → blinkMode = false → LED mati
```

**Keuntungan pendekatan ini**: Program tetap dapat menerima input serial kapanpun, bahkan saat LED sedang berkedip, karena tidak ada `delay()` yang memblokir eksekusi [[3]](#referensi).

---

### Pertanyaan 4: `delay()` vs `millis()` – pengaruh terhadap sistem

| Aspek | `delay()` | `millis()` |
|-------|-----------|------------|
| Sifat | **Blocking** – CPU berhenti total selama durasi delay | **Non-blocking** – CPU terus menjalankan kode lain |
| Responsivitas | Input serial **tidak terbaca** selama delay aktif | Input serial **selalu terbaca** setiap iterasi loop |
| Implementasi | Mudah, satu baris kode | Lebih kompleks, butuh variabel timer |
| Cocok untuk | Program sederhana tanpa input real-time | Sistem multitasking / real-time |
| Analogi | Tidur 8 jam tanpa bisa dibangunkan | Melihat jam setiap saat, beraksi saat waktunya |

**Kesimpulan**: Untuk program blink yang tetap responsif terhadap input serial, **`millis()` wajib digunakan**. Jika `delay(500)` dipakai, perintah '0' atau '1' yang dikirim saat LED dalam kondisi delay akan tertunda minimal 500ms sebelum diproses, bahkan bisa terlewat jika buffer overflow [[3]](#referensi).

---

## Jawaban Pertanyaan Praktikum 3B

### Pertanyaan 1: Cara kerja komunikasi I2C antara Arduino dan LCD

Komunikasi I2C antara Arduino (master) dan LCD I2C (slave, alamat 0x27) berlangsung melalui dua jalur:
- **SDA** (Serial Data Line) – pin A4 Arduino
- **SCL** (Serial Clock Line) – pin A5 Arduino

**Alur komunikasi per transaksi:**

```
1. START CONDITION    : Arduino menurunkan SDA saat SCL HIGH
                        → Semua slave tahu transaksi dimulai

2. ADDRESS FRAME      : Arduino mengirim 7-bit alamat (0x27 = 0100111)
                        + 1-bit R/W (0 = write)
                        → Total 8 bit dikirim bit demi bit, disinkronkan oleh SCL

3. ACK dari slave     : LCD (PCF8574) mencocokkan alamat → sama → tarik SDA LOW
                        → Arduino tahu slave siap menerima data

4. DATA FRAME         : Arduino mengirim byte data (perintah LCD atau karakter ASCII)
                        PCF8574 mengkonversi byte serial → sinyal paralel 4-bit
                        → diteruskan ke kontroler LCD HD44780

5. ACK per byte       : Setiap byte diikuti ACK dari slave

6. STOP CONDITION     : Arduino menaikkan SDA saat SCL HIGH
                        → Transaksi selesai, bus bebas
```

Keunggulan I2C: seluruh proses di atas hanya memerlukan 2 kabel untuk menghubungkan Arduino dengan LCD, berbeda dengan LCD paralel yang membutuhkan 6–10 pin [[2]](#referensi) [[4]](#referensi).

---

### Pertanyaan 2: Apa yang terjadi jika pin kiri dan kanan potensiometer tertukar?

Potensiometer berfungsi sebagai voltage divider (pembagi tegangan). Tegangan pada wiper (kaki tengah) bergantung pada posisi putar.

**Konfigurasi normal:**
```
5V ──── [resistor] ──── WIPER (A0) ──── [resistor] ──── GND
         ↑ meningkat saat putar kanan         ↑ menurun saat putar kanan
```
- Putar kanan → ADC mendekati 1023 (5V) → bar di LCD memanjang ✓

**Konfigurasi tertukar (kiri↔kanan):**
```
GND ──── [resistor] ──── WIPER (A0) ──── [resistor] ──── 5V
```
- Putar kanan → ADC mendekati 0 (0V) → bar di LCD memendek ✗

**Kesimpulan**: Pin tertukar tidak merusak komponen, tetapi membalik arah respons. Bar akan memanjang saat diputar ke kiri dan memendek saat diputar ke kanan – berlawanan dengan ekspektasi [[1]](#referensi).

---

### Pertanyaan 3: Modifikasi – gabungan UART dan I2C

```cpp
#include <Wire.h>              // Library untuk komunikasi I2C (Two-Wire Interface)
                               // Mengaktifkan modul TWI pada ATmega328P
#include <LiquidCrystal_I2C.h> // Library driver LCD berbasis I2C menggunakan chip PCF8574
#include <Arduino.h>

// Inisialisasi objek LCD dengan:
// - Alamat I2C: 0x27 (ganti ke 0x3F jika LCD tidak terdeteksi)
// - Kolom: 16 karakter
// - Baris: 2 baris
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int pinPot = A0;  // Pin analog A0 untuk membaca tegangan dari wiper potensiometer

void setup() {
  Serial.begin(9600);   // Inisialisasi UART dengan baud rate 9600 bps
                        // Digunakan untuk output data ke Serial Monitor (Pertanyaan 3B-3)

  lcd.init();           // Inisialisasi kontroler LCD HD44780 melalui I2C
                        // Arduino mengirimkan Start Condition + alamat 0x27 + perintah init
  lcd.backlight();      // Mengaktifkan lampu latar LCD via pin EN chip PCF8574
}

void loop() {
  // --- Pembacaan ADC ---
  int nilai = analogRead(pinPot);           // Membaca tegangan di pin A0 menggunakan ADC 10-bit
                                            // Menghasilkan nilai integer 0 (0V) hingga 1023 (5V)

  // --- Konversi untuk output UART (Pertanyaan 3B-3) ---
  float volt = nilai * (5.0 / 1023.0);     // Konversi nilai ADC ke tegangan (Volt)
                                            // Rumus: V = ADC × (Vref / resolusi ADC)
                                            // Vref = 5V, resolusi = 2^10 = 1024 step (0–1023)
  int persen = map(nilai, 0, 1023, 0, 100); // Konversi nilai ADC ke persentase (0–100%)
                                            // Menggunakan fungsi map() untuk linear scaling

  // --- Output ke Serial Monitor via UART ---
  Serial.print("ADC: ");   Serial.print(nilai);       // Tampilkan nilai ADC mentah
  Serial.print("  Volt: "); Serial.print(volt, 2);    // Tampilkan tegangan dengan 2 desimal
  Serial.print(" V");
  Serial.print("  Persen: "); Serial.print(persen);   // Tampilkan persentase
  Serial.println("%");                                 // Akhiri baris (tambah newline)

  // --- Output ke LCD via I2C ---
  // Baris pertama: tampilkan label "ADC:" dan nilai numerik
  lcd.setCursor(0, 0);           // Pindahkan kursor ke kolom 0, baris 0 (baris pertama)
                                 // Arduino mengirimkan perintah SET_DDRAM_ADDRESS via I2C
  lcd.print("ADC: ");            // Tulis teks "ADC: " ke LCD
  lcd.print(nilai);              // Tulis nilai ADC (integer) ke LCD
  lcd.print("      ");           // Spasi untuk menghapus sisa karakter sebelumnya (clear parsial)

  // Baris kedua: tampilkan bar visual proporsional dengan nilai ADC
  // Mapping nilai ADC (0–1023) ke panjang bar (0–16 karakter LCD)
  int panjangBar = map(nilai, 0, 1023, 0, 16);  // Setiap "blok" mewakili 1/16 dari nilai penuh

  lcd.setCursor(0, 1);           // Pindahkan kursor ke kolom 0, baris 1 (baris kedua)
  for (int i = 0; i < 16; i++) {        // Iterasi melalui 16 kolom LCD
    if (i < panjangBar) {               // Jika kolom ini masuk dalam rentang bar
      lcd.print((char)255);             // Cetak karakter block penuh (ASCII 255 = █)
                                        // Ini adalah karakter custom di ROM LCD HD44780
    } else {                            // Jika kolom ini di luar rentang bar
      lcd.print(" ");                   // Cetak spasi (mengosongkan kolom)
    }
  }

  delay(200);  // Tunda 200ms sebelum pembacaan berikutnya
               // Mencegah LCD berkedip/flicker akibat refresh terlalu cepat
               // Catatan: delay() di sini aman karena tidak ada input serial yang perlu dibaca
               //          secara real-time di program ini
}
```

**Format output Serial Monitor:**
```
ADC: 0    Volt: 0.00 V  Persen: 0%     ← potensiometer paling kiri
ADC: 512  Volt: 2.50 V  Persen: 50%    ← potensiometer tengah
ADC: 1023 Volt: 5.00 V  Persen: 100%   ← potensiometer paling kanan
```

**Format output LCD:**
```
Baris 1: ADC: 512        (setCursor(0,0))
Baris 2: ████████        (setCursor(0,1)) — 8 blok dari 16 untuk nilai 50%
```

---

### Pertanyaan 4: Tabel Pengamatan ADC dari Serial Monitor

> Catatan: Nilai-nilai di bawah ini dihitung secara teoritis menggunakan rumus  
> `Volt = ADC × (5.0 / 1023)` dan `Persen = ADC × (100 / 1023)`

| ADC  | Volt (V) | Persen (%) |
|------|----------|------------|
| 1    | 0.005    | 0.10%      |
| 21   | 0.103    | 2.05%      |
| 49   | 0.239    | 4.79%      |
| 74   | 0.361    | 7.23%      |
| 96   | 0.469    | 9.38%      |

---

## Pertanyaan Umum Modul 3

### Pertanyaan 1: Keuntungan dan kerugian UART vs I2C

**UART**
| ✅ Keuntungan | ❌ Kerugian |
|---------------|------------|
| Implementasi sangat sederhana, tidak butuh library khusus | Hanya point-to-point (1 master, 1 slave) |
| Dapat digunakan untuk jarak komunikasi lebih jauh | Baud rate harus sama di kedua sisi |
| Kecepatan dapat disesuaikan (300 bps – 2 Mbps) | Membutuhkan 2 jalur data (TX + RX) |
| Asinkron – tidak butuh sinyal clock | Tidak ada mekanisme deteksi kesalahan yang built-in |

**I2C**
| ✅ Keuntungan | ❌ Kerugian |
|---------------|------------|
| Hanya 2 kabel untuk banyak perangkat | Kecepatan lebih rendah dari SPI (maks 400kHz standar) |
| Setiap slave memiliki alamat unik (7-bit = 128 alamat) | Butuh resistor pull-up eksternal pada SDA dan SCL |
| Mendukung multi-master dan multi-slave | Konflik alamat jika dua perangkat punya alamat default sama |
| Built-in ACK/NACK untuk deteksi error | Protokol lebih kompleks dibanding UART |

---

### Pertanyaan 2: Peran alamat I2C (0x27 vs 0x20)

Alamat I2C berfungsi sebagai identitas unik setiap perangkat slave di bus I2C. Analoginya seperti nomor rumah di satu jalan: postman (Arduino) hanya akan mengantar paket ke rumah dengan nomor yang tepat, sementara rumah lain mengabaikan.

**Mekanisme:**
1. Arduino mengirimkan 7-bit alamat target di setiap transaksi
2. Semua slave mendengarkan, tetapi hanya slave dengan alamat cocok yang merespons dengan ACK
3. Slave lain tidak bereaksi

**LCD dengan chip PCF8574:**
- **NXP PCF8574**: alamat default 0x27 (bila A0=A1=A2=HIGH via pull-up)
- **TI PCF8574**: alamat default 0x3F atau 0x20

**Mengubah alamat**: Pin A0, A1, A2 pada chip PCF8574 dapat di-ground atau di-VCC untuk mengubah 3 bit terakhir alamat. Ini memungkinkan hingga 8 LCD dengan chip yang sama dihubungkan ke bus I2C yang sama secara bersamaan [[4]](#referensi).

```
Alamat = 0 1 0 0 A2 A1 A0
0x27   = 0 1 0 0  1  1  1  (A0=A1=A2=HIGH)
0x20   = 0 1 0 0  0  0  0  (A0=A1=A2=LOW)
```

---

### Pertanyaan 3: Alur kerja sistem gabungan UART + I2C

```
┌─────────────────────────────────────────────────────────────┐
│                    ARDUINO UNO (ATmega328P)                 │
│                                                             │
│  ┌─────────────┐      loop()      ┌──────────────────────┐  │
│  │ USART Module│ ◄─── Input ────  │   Program Logic      │  │
│  │ (UART HW)   │                  │                      │  │
│  │ Buffer RX   │ ──→ char data ─→ │ analogRead() → nilai │  │
│  └──────┬──────┘                  │ map() → panjangBar   │  │
│         │ Pin 0 (RX)              │ volt, persen         │  │
│         │                         └──┬───────────────────┘  │
│  USB ←──┘                           │                       │
│  (Serial Monitor)                   ├──→ Serial.print()     │
│                                     │    [USART TX → USB]   │
│                                     │                       │
│                                     └──→ lcd.print()        │
│                                          [TWI Module → I2C] │
│                                          Pin A4 (SDA)       │
│                                          Pin A5 (SCL)       │
└─────────────────────────────────────────────────────────────┘
         │                                      │
         ▼                                      ▼
  Serial Monitor                          LCD I2C (0x27)
  (Output UART)                           (Output I2C)
```

**Mengapa tidak konflik?**
- UART menggunakan modul USART (hardware terpisah) dengan interupsi dan buffer sendiri
- I2C menggunakan modul TWI (Two-Wire Interface, hardware terpisah) dengan register kontrol sendiri
- Keduanya beroperasi secara independen pada hardware yang berbeda di dalam chip ATmega328P
- CPU hanya mengatur kapan data dikirim ke masing-masing modul
