# 🔌 EnergyMonitor

**Station C3 – Monitoring the Polish Power Grid**

EnergyMonitorC3 is a project that scrapes live energy data from the Polish transmission system operator (PSE) website and periodically publishes the collected data to an MQTT broker for further analysis and integration with other systems.

---

## 📌 Project Overview

This project automates the retrieval of real-time power grid data from [PSE](https://www.pse.pl) using web scraping techniques. The extracted data is sent periodically to an MQTT broker, allowing other applications to consume and analyze the current state of the power grid.

### Data collected from PSE includes:

- Power demand (MW)  
- Total generation (MW)  
- Generation by source:  
  - Thermal  
  - Hydro  
  - Wind  
  - Photovoltaic  
- Grid frequency (Hz)  
- Cross-border power exchange balance (import/export)  

---

## ⚙️ Technologies Used

- C++17  
- Web scraping libraries/tools  
- MQTT protocol for data publishing  

---







cURL – for fetching data from the PSE API

nlohmann/json – for JSON parsing

Eclipse Paho MQTT – for MQTT communication

Multithreading & synchronization – semaphores and timers used to control task execution


**🛠️ How It Works**

1. A timer triggers the data retrieval task at a fixed interval (every 10 seconds). 
2. Data is fetched from: https://www.pse.pl/transmissionMapService
3. Once received, the data is:
-Parsed and filtered,
-Displayed in the console,
-Sent to an MQTT broker (pscr/c3) in JSON format.

4. A semaphore ensures synchronization between data reading and sending tasks.


**📤 Example MQTT Payload**

{
  "zapotrzebowanie": 22850,
  
  "generacja": 21900,
  
  "cieplne": 13000,
  
  "wodne": 1200,
  
  "wiatrowe": 4000,
  
  "PV": 1700,
  
  "czestotliwosc": 50.01,
  
  "saldo": -950
}


**🚀 Getting Started**

Requirements:

-C++ compiler (e.g., g++)

-Libraries:

*libcurl

*nlohmann/json

*paho.mqtt.cpp

-SSL certificate (e.g., cacert.pem for cURL)
