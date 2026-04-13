## Pertanyaan Praktikum ##
**1. Gambarkan rangkaian schematic yang digunakan!**

Schematic menghubungkan 0-9 segmen seven segment (a–f, dp) masing-masing melalui resistor 220Ω ke pin digital Arduino sesuai tabel wiring. Pin common cathode dihubungkan ke GND Arduino. Resistor 220Ω berfungsi membatasi arus agar tidak melebihi batas aman LED (~20mA): R = (5V – 2V) / 0,02A = 150Ω → pilih 220Ω.
<img width="1147" height="703" alt="image" src="https://github.com/user-attachments/assets/491b8d69-1ffd-4120-8ed8-3598733d479d" />


---

**2. Apa yang terjadi jika nilai `num` lebih dari 15?**

Jika `num > 15`, fungsi `displayDigit()` akan mengakses `digitPattern[num]` di luar batas array (array hanya berindeks 0–15). Dalam C/C++ Arduino, akses out-of-bounds tidak menyebabkan error kompilasi, namun membaca nilai memori yang tidak terdefinisi (*garbage value*). Akibatnya:
- Segmen yang menyala akan acak dan tidak dapat diprediksi
- Berpotensi merusak nilai variabel lain di memori (undefined behavior)

**Solusi:** Tambahkan guard clause di awal fungsi:
```cpp
if (num < 0 || num > 15) return;
```

---

**3. Apakah program menggunakan common cathode atau common anode? Jelaskan!**

Program ini menggunakan Common Cathode (CC). Buktinya:
- Segmen dinyalakan dengan nilai `1` (HIGH = 5V)
- Pada Common Cathode, pin common terhubung ke GND
- Arus mengalir dari pin Arduino → resistor → anoda LED → katoda → GND
- Segmen aktif ketika mendapat tegangan tinggi (HIGH)

Jika Common Anode yang digunakan, semua pola harus dibalik (nilai `0` untuk menyalakan segmen) karena common terhubung ke VCC dan segmen aktif pada logika LOW.

---

**4. Modifikasi program agar tampilan berjalan dari F ke 0**

Perubahan utama: ubah iterasi loop dari ascending (`i=0; i<16; i++`) menjadi descending (`i=15; i>=0; i--`)

**Counter F → 0 (Modifikasi)**
```cpp
#include <Arduino.h>

// ── Pin mapping: a  b  c   d   e  f  g  dp ──────────────────────
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

// ── Pola digit 0–F untuk common cathode ─────────────────────────
byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0},  // 0
  {0,1,1,0,0,0,0,0},  // 1
  {1,1,0,1,1,0,1,0},  // 2
  {1,1,1,1,0,0,1,0},  // 3
  {0,1,1,0,0,1,1,0},  // 4
  {1,0,1,1,0,1,1,0},  // 5
  {1,0,1,1,1,1,1,0},  // 6
  {1,1,1,0,0,0,0,0},  // 7
  {1,1,1,1,1,1,1,0},  // 8
  {1,1,1,1,0,1,1,0},  // 9
  {1,1,1,0,1,1,1,0},  // A
  {0,0,1,1,1,1,1,0},  // b
  {1,0,0,1,1,1,0,0},  // C
  {0,1,1,1,1,0,1,0},  // d
  {1,0,0,1,1,1,1,0},  // E
  {1,0,0,0,1,1,1,0}   // F
};

// ── Fungsi menampilkan digit berdasarkan indeks 0–15 ─────────────
void displayDigit(int num) {
  // Validasi batas array agar tidak terjadi undefined behavior
  if (num < 0 || num > 15) return;

  for (int i = 0; i < 8; i++) {
    // Tulis pola HIGH/LOW ke masing-masing pin segmen
    digitalWrite(segmentPins[i], digitPattern[num][i]);
  }
}

// ── Setup: inisialisasi semua pin sebagai OUTPUT ─────────────────
void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);  // Setiap pin segmen = OUTPUT
  }
}

// ── Loop: tampilkan F → E → D → ... → 1 → 0 lalu ulangi ─────────
void loop() {
  // PERUBAHAN UTAMA: iterasi dari 15 (F) turun ke 0
  // i=15  → tampilkan 'F'
  // i=14  → tampilkan 'E'
  // ...
  // i=0   → tampilkan '0'
  for (int i = 15; i >= 0; i--) {
    displayDigit(i);  // Tampilkan karakter ke-i
    delay(1000);      // Jeda 1 detik per digit
  }
  // Setelah '0', for loop selesai → loop() dipanggil ulang → mulai dari 'F' lagi
}
```

---

## Pertanyaan Praktikum ##

**1. Gambarkan rangkaian schematic yang digunakan!**

Rangkaian 2B = rangkaian 2A + Push Button. Satu kaki tombol ke Pin 2, kaki lainnya ke GND. Resistor pull-up internal diaktifkan via `INPUT_PULLUP`, tidak perlu resistor eksternal.
<img width="1077" height="699" alt="image" src="https://github.com/user-attachments/assets/28612051-b041-4c6a-8d97-9e58fc58593e" />


---

**2. Mengapa Push Button menggunakan mode INPUT_PULLUP? Keuntungannya?**

Mode `INPUT_PULLUP` mengaktifkan **resistor pull-up internal ~20–50kΩ** pada ATmega328P. Keuntungan dibanding rangkaian biasa:

| Aspek | Tanpa Pull-up | Dengan INPUT_PULLUP |
|-------|--------------|---------------------|
| Komponen | Perlu resistor eksternal | Tidak perlu resistor |
| Floating state | Pin dapat terbaca acak | Pin selalu terdefinisi |
| Logika | Active-HIGH (perlu pull-down) | Active-LOW (lebih noise-immune) |
| Kerumitan rangkaian | Lebih kompleks | Lebih sederhana |

Cara kerja: Saat tombol tidak ditekan → pin terhubung ke VCC melalui pull-up → terbaca HIGH. Saat ditekan → pin terhubung langsung ke GND → terbaca LOW (lebih kuat dari VCC melalui pull-up).

---

**3. Jika salah satu LED segmen tidak menyala, apa penyebabnya?**

**Sisi Hardware:**
1. Resistor putus atau terlepas dari breadboard
2. Kabel jumper tidak terkoneksi sempurna (koneksi longgar di breadboard)
3. LED segmen rusak (putus/terbakar akibat overcurrent)
4. Kabel pin common cathode terlepas dari GND
5. Pin Arduino rusak atau tidak memberikan tegangan yang cukup

**Sisi Software:**
1. Kesalahan indeks pada array `segmentPins[]` → pin yang salah di-drive
2. Nilai bit pola `digitPattern[][]` salah untuk segmen tersebut (seharusnya 1, ditulis 0)
3. Pin segmen tidak dikonfigurasi `OUTPUT` di `setup()` → tetap dalam mode INPUT, tidak dapat drive LOW/HIGH
4. Nomor pin di `segmentPins[]` tidak sesuai dengan sambungan fisik

---

**4. Modifikasi rangkaian dan program dengan dua push button yang berfungsi sebagai 
penambahan (increment) dan pengurangan (decrement) pada sistem counter**

**Counter dengan 2 Push Button (Increment & Decrement)**
**Tambahan Wiring**

| Komponen | Pin Arduino |
|----------|-------------|
| Push Button UP (increment) | Pin 2 |
| Push Button DOWN (decrement) | Pin 3 |


```cpp
#include <Arduino.h>

// ── PIN DEFINITIONS ──────────────────────────────────────────────
// Mapping 8 pin segmen seven segment: a  b  c   d   e  f  g  dp
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

// Pin tombol UP (increment) → tambah counter naik
const int btnUp   = 2;

// Pin tombol DOWN (decrement) → kurangi counter turun
const int btnDown = 3;

// ── DATA: Pola segmen 0–F untuk Common Cathode ───────────────────
// Urutan bit: a  b  c  d  e  f  g  dp
// 1 = segmen menyala (HIGH), 0 = segmen mati (LOW)
byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0},  // 0 → a,b,c,d,e,f
  {0,1,1,0,0,0,0,0},  // 1 → b,c
  {1,1,0,1,1,0,1,0},  // 2 → a,b,d,e,g
  {1,1,1,1,0,0,1,0},  // 3 → a,b,c,d,g
  {0,1,1,0,0,1,1,0},  // 4 → b,c,f,g
  {1,0,1,1,0,1,1,0},  // 5 → a,c,d,f,g
  {1,0,1,1,1,1,1,0},  // 6 → a,c,d,e,f,g
  {1,1,1,0,0,0,0,0},  // 7 → a,b,c
  {1,1,1,1,1,1,1,0},  // 8 → semua (a–g)
  {1,1,1,1,0,1,1,0},  // 9 → a,b,c,d,f,g
  {1,1,1,0,1,1,1,0},  // A → a,b,c,e,f,g
  {0,0,1,1,1,1,1,0},  // b → c,d,e,f,g
  {1,0,0,1,1,1,0,0},  // C → a,d,e,f
  {0,1,1,1,1,0,1,0},  // d → b,c,d,e,g
  {1,0,0,1,1,1,1,0},  // E → a,d,e,f,g
  {1,0,0,0,1,1,1,0}   // F → a,e,f,g
};

// ── STATE VARIABLES ──────────────────────────────────────────────
// Nilai digit yang sedang ditampilkan (0–15)
int currentDigit = 0;

// State tombol pada iterasi SEBELUMNYA untuk edge detection
// Nilai awal HIGH karena INPUT_PULLUP (tombol bebas = HIGH)
bool lastUpState   = HIGH;  // state sebelumnya tombol UP
bool lastDownState = HIGH;  // state sebelumnya tombol DOWN

// ── FUNCTION: displayDigit ───────────────────────────────────────
// Mengupdate tampilan seven segment sesuai nilai digit
// Parameter: num = nilai yang ditampilkan (0–15)
void displayDigit(int num) {
  // Guard: pastikan indeks valid sebelum akses array
  if (num < 0 || num > 15) return;

  // Tulis pola HIGH/LOW ke setiap pin segmen
  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i],   // pin segmen ke-i
                 digitPattern[num][i]); // nilai 1/0 dari tabel pola
  }
}

// ── SETUP ────────────────────────────────────────────────────────
// Dijalankan sekali saat Arduino menyala/reset
void setup() {
  // Semua pin segmen dikonfigurasi OUTPUT
  // agar dapat mengirim sinyal HIGH/LOW ke LED
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }

  // Tombol UP (Pin 2) sebagai input dengan pull-up internal
  // Tidak perlu resistor eksternal; tidak ditekan = HIGH, ditekan = LOW
  pinMode(btnUp,   INPUT_PULLUP);

  // Tombol DOWN (Pin 3) sebagai input dengan pull-up internal
  // Prinsip sama dengan btnUp
  pinMode(btnDown, INPUT_PULLUP);

  // Tampilkan angka '0' saat pertama menyala
  displayDigit(currentDigit);
}

// ── LOOP ─────────────────────────────────────────────────────────
// Dijalankan berulang terus-menerus selama Arduino aktif
void loop() {
  // Baca state tombol saat ini
  bool upState   = digitalRead(btnUp);    // HIGH/LOW dari Pin 2
  bool downState = digitalRead(btnDown);  // HIGH/LOW dari Pin 3

  // ══ TOMBOL UP: Edge Detection HIGH → LOW ══
  // Kondisi ini TRUE hanya pada momen PERTAMA tombol UP ditekan
  // (transisi dari tidak ditekan → ditekan)
  if (lastUpState == HIGH && upState == LOW) {
    currentDigit++;  // Naikan nilai counter satu langkah

    // Wrap-around ke atas: jika sudah melewati 'F' (15), kembali ke '0'
    // Contoh: 15 + 1 = 16 → di-reset ke 0
    if (currentDigit > 15) currentDigit = 0;

    displayDigit(currentDigit);  // Update tampilan
  }

  // ══ TOMBOL DOWN: Edge Detection HIGH → LOW ══
  // Kondisi ini TRUE hanya pada momen PERTAMA tombol DOWN ditekan
  if (lastDownState == HIGH && downState == LOW) {
    currentDigit--;  // Turunkan nilai counter satu langkah

    // Wrap-around ke bawah: jika sudah di bawah '0', kembali ke 'F' (15)
    // Contoh: 0 - 1 = -1 → di-reset ke 15
    if (currentDigit < 0) currentDigit = 15;

    displayDigit(currentDigit);  // Update tampilan
  }

  // Simpan state tombol saat ini untuk dibandingkan di iterasi berikutnya
  // Ini adalah inti mekanisme edge detection
  lastUpState   = upState;    // update state UP
  lastDownState = downState;  // update state DOWN
}
```

---

## Pertanyaan Umum (Bagian 2.7) ##

**1. Uraikan hasil tugas pada praktikum setiap percobaan!**

**Percobaan 2A:**  
Seven Segment berhasil menampilkan counter heksadesimal 0 → 1 → 2 → ... → F secara berurutan dengan interval 1 detik per digit, berjalan terus menerus (looping). Semua 16 karakter tampil sesuai pola digitPattern yang didefinisikan. Tidak ada segmen yang salah menyala.

**Percobaan 2B:**  
Push Button berhasil diintegrasikan sebagai kontrol interaktif. Setiap kali tombol ditekan sekali, counter naik satu langkah dan tampilan Seven Segment diperbarui. Teknik edge detection berhasil mencegah penghitungan ganda (bouncing), sehingga satu penekanan tombol = satu kenaikan counter, meskipun tombol ditahan lama.

---

**2. Bagaimana prinsip kerja Seven Segment Display dalam menampilkan angka dan karakter?**

Seven Segment Display terdiri dari **7 segmen LED** (a, b, c, d, e, f, g) yang tersusun membentuk angka 8, ditambah satu titik desimal (dp). Setiap karakter ditampilkan dengan **mengaktifkan kombinasi segmen tertentu**:

```
 _
|_|   → Angka 0: segmen a,b,c,d,e,f menyala; tidak mati
|_|

 _
  |   → Angka 1: segmen b,c menyala
  |

 _
 _|   → Angka 2: segmen a,b,d,e,g menyala
|_
```

Pada **Common Cathode**:
- Pin common = GND
- Segmen menyala saat pin diberi HIGH (5V)
- Arus: Pin Arduino → Resistor 220Ω → Anoda LED → Katoda → GND

Program menterjemahkan setiap digit ke vektor biner 8-bit dalam array `digitPattern[][]` yang menentukan pin mana yang HIGH atau LOW.

---

**3. Jelaskan bagaimana sistem counter bekerja pada program tersebut!**

**Percobaan 2A (Counter Otomatis):**
```
loop() dipanggil → for(i=0; i<16; i++) → displayDigit(i) → delay(1000ms) → next i
```
Counter bekerja secara **time-based**: variabel loop `i` diincrement otomatis setiap 1 detik. Setelah `i=15` (F), `for` loop selesai → `loop()` dipanggil ulang → mulai dari `i=0` lagi.

**Percobaan 2B (Counter Event-Driven):**
```
loop() → baca digitalRead(btnUp) → bandingkan dengan lastUpState
→ jika HIGH→LOW: currentDigit++ → displayDigit() → simpan lastUpState
```
Counter bekerja secara **event-driven**: hanya bereaksi saat ada penekanan tombol (edge). Variabel `currentDigit` persisten antar iterasi loop, sehingga nilai tersimpan sampai ada input baru.

**Mekanisme Wrap-around:**
- UP: `if(currentDigit > 15) currentDigit = 0;` → setelah F kembali ke 0
- DOWN: `if(currentDigit < 0) currentDigit = 15;` → sebelum 0 kembali ke F

---
