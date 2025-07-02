# 🔌 EnergyMonitorC3

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

