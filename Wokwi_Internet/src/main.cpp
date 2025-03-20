#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 27    
#define DHTTYPE DHT22  

DHT dht(DHTPIN, DHTTYPE);

// Ganti dengan kredensial WiFi Anda
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Ganti dengan URL API yang benar
const char* serverUrl = "http://3024-175-45-190-5.ngrok-free.app/api/posts"; 

unsigned long previousMillis = 0;
const long interval = 5000;  // Interval 5 detik (5000 ms)

void setup() {
  Serial.begin(115200);
  
  // Hubungkan ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi!");

  dht.begin();
  delay(2000); // Tunggu DHT agar siap
}

void loop() {
  unsigned long currentMillis = millis();

  // Lakukan POST setiap interval yang telah ditentukan
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Pastikan masih terhubung ke WiFi sebelum mengirim data
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi terputus, mencoba menghubungkan kembali...");
      WiFi.begin(ssid, password);
      return;
    }

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Cek apakah sensor DHT berhasil membaca data
    if (isnan(h) || isnan(t)) {
      Serial.println("Gagal membaca sensor DHT!");
      return;
    }

    // Bulatkan nilai suhu & kelembaban
    h = round(h);
    t = round(t);

    // Buat payload JSON
    String payload = "{\"nama_sensor\":\"Sensor GD\", \"nilai1\":" + String(h) + ", \"nilai2\":" + String(t) + "}";
    Serial.println("Mengirim data: " + payload);

    // Inisialisasi HTTPClient
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS); // Menangani redirect otomatis

    // Kirim POST request
    int httpResponseCode = http.POST(payload);

    // Tampilkan hasil HTTP response
    Serial.print("Kode respons HTTP: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) { // Cek jika respons valid
      String response = http.getString();
      Serial.println("Respons dari server:");
      Serial.println(response);
    } else {
      Serial.println("Gagal mengirim data, cek URL dan koneksi!");
    }

    // Tutup koneksi HTTP
    http.end();
  }
}
