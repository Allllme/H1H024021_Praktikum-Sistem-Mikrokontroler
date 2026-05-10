## Jawaban Pertanyaan Praktikum 5A

### Apakah ketiga task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

Ketiga task tidak berjalan secara truly parallel, melainkan berjalan secara concurrent (bergantian sangat cepat) melalui mekanisme preemptive scheduling FreeRTOS.

**Mekanisme:**
1. Scheduler FreeRTOS berjalan berdasarkan tick timer (default 1000 Hz pada Arduino).
2. Setiap task yang memanggil `vTaskDelay()` akan memasuki state Blocked, melepaskan CPU.
3. Scheduler segera memilih task lain yang berada pada state Ready untuk dieksekusi.
4. Karena `context switching` berlangsung dalam orde mikrodetik, ketiga task seolah-olah berjalan bersamaan.

**State Machine Task FreeRTOS:**
```
Ready → Running → Blocked (vTaskDelay) → Ready → Running ...
```

---

### Bagaimana cara menambahkan task keempat? Jelaskan langkahnya!

**Langkah-langkah:**

1. **Deklarasikan prototipe** di bagian atas sketch:
   ```cpp
   void TaskBlink3(void *pvParameters);
   ```

2. **Daftarkan task** di dalam `setup()`:
   ```cpp
   xTaskCreate(TaskBlink3, "task4", 128, NULL, 1, NULL);
   ```

3. **Implementasikan fungsi task** di luar `loop()`:
   ```cpp
   void TaskBlink3(void *pvParameters) {
     pinMode(6, OUTPUT);
     while (1) {
       digitalWrite(6, HIGH);
       vTaskDelay(400 / portTICK_PERIOD_MS);
       digitalWrite(6, LOW);
       vTaskDelay(400 / portTICK_PERIOD_MS);
     }
   }
   ```

> **Catatan:** Arduino Uno hanya memiliki 2KB SRAM. Setiap task mengonsumsi ±128 word (256 byte) stack. Tambahkan task secara hati-hati untuk menghindari stack overflow.

---

### Modifikasi program dengan potensiometer untuk mengontrol kecepatan LED

Tambahkan task `TaskReadPot` yang membaca nilai ADC dari potensiometer (pin A0) dan menyimpannya ke variabel global `volatile`. Task TaskBlink1 & TaskBlink2 menggunakan nilai tersebut sebagai parameter delay.

```cpp
volatile int potValue = 500; // Nilai default delay (ms)

void TaskReadPot(void *pvParameters) {
  while (1) {
    // Map nilai ADC (0–1023) ke range delay (50–1000ms)
    potValue = map(analogRead(A0), 0, 1023, 50, 1000);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void TaskBlink1(void *pvParameters) {
  pinMode(8, OUTPUT);
  while (1) {
    digitalWrite(8, HIGH);
    vTaskDelay(potValue / portTICK_PERIOD_MS); // Delay dinamis dari pot
    digitalWrite(8, LOW);
    vTaskDelay(potValue / portTICK_PERIOD_MS);
  }
}
```

**Hasil:** Memutar potensiometer ke kanan mempercepat kedip LED, memutar ke kiri memperlambat. Ini menunjukkan kemampuan RTOS untuk merespons perubahan input sensor secara real-time.

---


## Jawaban Pertanyaan Praktikum 5B

### Apakah kedua task berjalan secara bersamaan atau bergantian? Jelaskan mekanismenya!

Kedua task berjalan secara bergantian menggunakan mekanisme **blocking queue**:

```
read_data: kirim data → vTaskDelay(100ms) → [BLOCKED]
                                                    ↓
display:                              [BLOCKED tunggu data] → terima data → tampilkan → [BLOCKED lagi]
```

- `read_data` memanggil `xQueueSend()` lalu `vTaskDelay()` → **Blocked 100ms**
- `display` memanggil `xQueueReceive(portMAX_DELAY)` → **Blocked** hingga data tiba
- Begitu `read_data` mengirim data, `display` di-**unblock** oleh scheduler untuk membaca data
- Siklus ini berulang terus-menerus [4]

---

### Apakah program ini berpotensi mengalami race condition? Jelaskan!

**Program ini TIDAK berpotensi race condition** karena menggunakan FreeRTOS Queue sebagai mekanisme komunikasi.

**Alasan:**
- Queue FreeRTOS bersifat **thread-safe** secara internal — menggunakan `critical section` untuk melindungi operasi read/write
- Hanya satu task yang boleh mengakses queue pada satu waktu
- `xQueueSend()` dan `xQueueReceive()` bersifat **atomic** dari perspektif task lain

**Namun, race condition DAPAT terjadi jika:**
```cpp
// BERBAHAYA: akses variabel global tanpa perlindungan mutex
volatile int sharedData = 0;

void task1(void *p) { sharedData = 42; }   // Task 1 menulis
void task2(void *p) { int x = sharedData; } // Task 2 membaca
```

Solusinya adalah menggunakan `xSemaphoreTake()` / `xSemaphoreGive()` (mutex) untuk melindungi shared variable [4][5].

---

### Modifikasi dengan sensor DHT sesungguhnya

Lihat kode `modul6_taskqueue_dht.ino` di atas. **Hasilnya:** Serial Monitor menampilkan nilai suhu dan kelembaban yang dinamis dan berubah sesuai kondisi lingkungan nyata, membuktikan bahwa queue mampu meneruskan data sensor real-time secara aman antar-task.

---

## Pertanyaan Praktikum (5.7)
### Jelaskan perbedaan antara `loop()` Arduino biasa vs sistem RTOS!

| Aspek | Arduino Biasa (`loop()`) | Arduino + RTOS |
|-------|--------------------------|----------------|
| Eksekusi | Sekuensial, satu thread | Multi-task concurrent |
| Blocking | `delay()` memblokir semua program | `vTaskDelay()` hanya blokir satu task |
| Respon | Tidak deterministik jika ada delay panjang | Deterministik, terjamin oleh scheduler |
| Modularitas | Semua kode dalam satu loop | Setiap task independen dan terisolasi |
| Kompleksitas | Sederhana untuk proyek kecil | Lebih kompleks, tapi skalabel |

Pada Arduino biasa, `loop()` adalah satu-satunya konteks eksekusi. Pada RTOS, `loop()` menjadi **Idle Task** (prioritas 0) yang hanya berjalan saat tidak ada task lain aktif [3].

---
### Mengapa fungsi `loop()` dibiarkan kosong?

Setelah `vTaskStartScheduler()` dipanggil (atau otomatis oleh library `Arduino_FreeRTOS`), kendali program sepenuhnya diserahkan ke FreeRTOS. Jika kode dimasukkan ke `loop()`, hal ini dapat:
1. **Menginterferensi scheduler** dan menyebabkan perilaku tidak terduga
2. **Mengonsumsi CPU** untuk Idle Task yang seharusnya memberikan kesempatan ke task lain

Semua logika program harus ditempatkan di dalam task yang didaftarkan via `xTaskCreate()`. Fungsi `loop()` kosong = manifestasi dari paradigma RTOS yang sepenuhnya berbasis task [1].

---

### Apa insight utama dari praktikum ini?

Tiga insight utama:

1. **Paradigma baru pemrograman embedded:** RTOS bukan sekadar library, melainkan cara berpikir berbeda — program dibagi menjadi task-task independen yang masing-masing punya "nyawa" sendiri.

2. **Komunikasi terstruktur mencegah bug:** Penggunaan queue, semaphore, dan mutex bukan hanya formalitas, melainkan fondasi keamanan data pada sistem multitask. Race condition adalah bug yang sulit di-debug pada bare-metal.

3. **Relevansi industri:** Konsep RTOS yang dipelajari di sini adalah fondasi dari sistem-sistem nyata seperti autopilot drone, sistem kendali industri, dan perangkat IoT. Kemampuan memahami FreeRTOS membuka jalan ke embedded development level profesional [5].

---
