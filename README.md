# 🔊 ESP32 Bluetooth A2DP Speaker
> A simple Bluetooth audio sink implementation for the ESP32 using the ESP-IDF framework.

[![Platform](https://img.shields.io/badge/Platform-ESP--IDF-blue.svg)](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
[![Protocol](https://img.shields.io/badge/Protocol-A2DP%20Sink-orange.svg)](#)

## 📋 Overview
This project transforms an ESP32 into a Bluetooth Audio Sink (Speaker). It receives high-quality PCM audio data via Bluetooth A2DP and outputs it through the **I2S (Inter-IC Sound)** interface to an external DAC (Digital-to-Analog Converter).

## 🛠️ Hardware Requirements
* **Microcontroller:** ESP32 (WROVER or DEVKIT)
* **Audio DAC:** External I2S DAC (e.g., PCM5102A, MAX98357A)
* **Output:** Speakers or Headphones

### 🔌 Wiring Diagram (I2S)
Based on the current firmware configuration, connect your DAC as follows:

| ESP32 Pin | DAC Pin | Description |
| :--- | :--- | :--- |
| **GPIO 25** | **WS / LCK** | Word Select (Left/Right Clock) |
| **GPIO 26** | **BCK** | Bit Clock |
| **GPIO 22** | **DIN** | Data Input |
| **GND** | **GND** | Ground |
| **3.3V / 5V** | **VCC** | Power |

---

## 🚀 Software Implementation Details

### 1. Bluetooth Stack
The system uses the **Bluedroid** stack in Classic Bluetooth mode. To optimize performance and memory, BLE (Bluetooth Low Energy) memory is released at startup:
```c
esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
