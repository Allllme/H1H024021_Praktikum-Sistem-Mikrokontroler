## Pertanyaan Praktikum
## 1. Pada kondisi apa program masuk ke blok if?
Program masuk ke blok if ketika nilai variabel timeDelay sudah ≤ 100 ms. Ini berarti LED telah mencapai kecepatan kerlip maksimum. Pada kondisi ini, program akan memberikan jeda 3 detik lalu mereset timeDelay kembali ke 1000 ms.

## 2. Pada kondisi apa program masuk ke blok else?
Program masuk ke blok else ketika nilai timeDelay masih > 100 ms. Artinya LED belum mencapai kecepatan maksimum, sehingga timeDelay dikurangi 100 ms untuk mempercepat kerlip pada siklus berikutnya.

## 3. Apa fungsi dari perintah delay(timeDelay)?
Fungsi delay(timeDelay) menghentikan sementara eksekusi program selama durasi timeDelay milidetik. Karena nilai timeDelay berubah setiap siklus (berkurang 100 ms), durasi LED menyala dan mati pun ikut berubah, menciptakan efek percepatan kerlip yang bertahap dan dapat diamati oleh mata manusia.

## 4. Modifikasi: alur cepat → sedang → mati (tanpa langsung reset)
Program dimodifikasi menggunakan variabel phase untuk melacak tahap saat ini: 
<img width="733" height="1200" alt="image" src="https://github.com/user-attachments/assets/99af4328-b766-4756-bbe9-861fe446bca7" />

## Pertanyaan Analisis
## Schematic
![WhatsApp Image 2026-04-08 at 23 29 50](https://github.com/user-attachments/assets/56a430a0-51dc-4f4a-ae57-c7eda7bdfb3e)

## 2. Bagaimana program membuat efek LED berjalan dari kiri ke kanan?
Program menggunakan `for (int ledPin = 2; ledPin < 8; ledPin++)`. Variabel `ledPin` dimulai dari pin 2 dan bertambah 1 setiap iterasi. Pada setiap iterasi, pin tersebut di-set HIGH (menyala) selama 100ms, lalu di-set LOW (mati). Karena hanya satu pin yang HIGH pada satu waktu dan berpindah secara berurutan, mata manusia menangkap ilusi LED yang "bergerak" dari kiri ke kanan.

## 3. Bagaimana program membuat LED kembali dari kanan ke kiri?
Program menggunakan perulangan kedua: `for (int ledPin = 7; ledPin >= 2; ledPin--)`. Perbedaan dengan perulangan pertama:
- **Inisialisasi:** `ledPin = 7` (dimulai dari pin paling kanan)
- **Kondisi:** `ledPin >= 2` (berhenti ketika sudah < 2)
- **Decrement:** `ledPin--` (berkurang 1, bergerak ke kiri)

Mekanisme menyala dan matinya LED sama persis, hanya arah traversalnya yang terbalik.

### 4. Program 3 LED Kiri dan 3 LED Kanan Bergantian
<img width="946" height="1085" alt="image" src="https://github.com/user-attachments/assets/6ad99c06-a1d3-4565-bb9a-b0d21a5cb604" />
## Penjelasan Kode

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
