# 🔌 EnergyMonitor

**Station C3 – Monitoring the Polish Power Grid**

EnergyMonitorC3 is a C++17 application that automatically retrieves energy data from the Polish transmission system operator (PSE) via web scraping and API calls, then periodically publishes it to an MQTT broker for further analysis and integration with other systems.

---

## 📌 Project Overview

This project aims to monitor the real-time state of the Polish power system. Data such as power demand, generation from different sources, grid frequency, and cross-border exchange balance are fetched from the official PSE API and sent to an MQTT broker.

This enables use of the data in analytical systems, visualizations, or custom monitoring applications.

### Collected data:

- Power demand (MW)  
- Total power generation (MW)  
- Generation by source:  
  - Thermal  
  - Hydro  
  - Wind  
  - Photovoltaic (PV)  
- Grid frequency (Hz)  
- Cross-border power exchange balance (import/export)  

---

## ⚙️ Technologies and Libraries Used

- **C++17** – modern C++ standard  
- **libcurl** – for HTTP requests  
- **nlohmann/json** – for easy JSON parsing and generation  
- **Paho MQTT C++ client** – asynchronous MQTT client for broker communication  
- Multithreading (`<thread>`) to fetch data every 10 seconds  

---

## 🛠️ How It Works — Code Breakdown

### 1. Data fetching (`fetch_data_from_api`)

- Uses `libcurl` to perform GET request on PSE endpoint (`https://www.pse.pl/transmissionMapService`)  
- Configures HTTP headers and required cookies  
- Receives JSON response as string and forwards it for processing  

### 2. Data processing (`process_data`)

- Parses JSON using `nlohmann::json`  
- Extracts summary data and sums cross-border exchange balance  
- Creates new JSON object with selected fields to send forward  
- Prints detailed info to console (`display_data` function)  
- Publishes data to MQTT broker (`send_to_mqtt` function)  

### 3. MQTT publishing (`send_to_mqtt`)

- Creates MQTT message with QoS 1 and publishes it on topic `pscr/c3`  
- Handles exceptions and connection errors  

### 4. Main loop (`main`)

- Connects to MQTT broker with provided credentials  
- In infinite loop, fetches and processes data every 10 seconds  
- Handles MQTT and cURL errors  

---

## 🚀 Getting Started

### Requirements:

- C++17 compatible compiler (e.g. g++ 7+ or MSVC)  
- Libraries: `libcurl`, `nlohmann/json`, Paho MQTT C++ client  
- CA certificate (`cacert.pem`) for HTTPS connections  

### Build and run:

```bash
g++ -std=c++17 -o EnergyMonitor main.cpp -lpaho-mqttpp3 -lpaho-mqtt3as -lcurl -pthread
./EnergyMonitor
```
