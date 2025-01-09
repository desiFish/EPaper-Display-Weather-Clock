<div align="center">

# 🌡️ Minimalistic E-Paper Weather Station

[![Status](https://img.shields.io/badge/status-active-success.svg)]()
[![License](https://img.shields.io/github/license/desiFish/ESP32-ePaper-Display-Weather-Clock)](/LICENSE)
[![GitHub Issues](https://img.shields.io/github/issues/desiFish/ESP32-ePaper-Display-Weather-Clock.svg)](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/issues)
[![Release](https://img.shields.io/github/v/release/desiFish/ESP32-ePaper-Display-Weather-Clock)](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/releases)

<p align="center">
  <i>🌈 A beautiful, eco-friendly weather station powered by E-Paper display! 🌱</i>
</p>

</div>

<div align="center">

### 🌟 Smart • Efficient • Eco-Friendly 🌟

</div>

---

## 📑 Table of Contents

<div align="center">

| 🔧 Setup & Build | 📊 Features & Specs | 🌱 Environmental | 🤝 Community |
|:---------------:|:------------------:|:----------------:|:------------:|
| [Hardware Components](#hardware) | [Key Features](#features) | [Eco-Friendly Features](#eco-friendly) | [Contributing](#contributing) |
| [Initial Setup](#setup) | [Core Features](#core) | [Environmental Impact](#environmental) | [License](#license) |
| [API Configuration](#api) | [Technical Specs](#specs) | [Battery Performance](#battery) | [Credits](#credits) |
| [Debug Mode](#debug) | [Display Modes](#display) | [Gallery](#gallery) | [Star Project](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/stargazers) |

</div>

---

## 🛠️ Hardware Components {#hardware}

<table>
  <tr>
    <td>🎯 <b>Tested On</b></td>
    <td>
      • Seeed Studio XIAO ESP32 C3<br>
      • Seeed Studio XIAO ESP32 C6
    </td>
  </tr>
  <tr>
    <td>📟 <b>Display</b></td>
    <td>Waveshare 4.2inch TriColor 400x300 E-paper (15s refresh)</td>
  </tr>
  <tr>
    <td>🌡️ <b>Sensors</b></td>
    <td>
      • TMP117 (Temperature)<br>
      • BH1750 (Light sensing)<br>
      • DS3231 (Time keeping)<br>
      • BMP680 (Humidity & pressure)
    </td>
  </tr>
  <tr>
    <td>🔋 <b>Power</b></td>
    <td>LFP 2x4000mAh (8000mAh total) Lithium Iron Phosphate Battery (IFR26700)</td>
  </tr>
  <tr>
    <td>🔌 <b>Other</b></td>
    <td>2xBMS, capacitors, resistors, connectors, etc.</td>
  </tr>
</table>

## 📱 Key Features & Updates {#features}

<table>
<tr>
<td width="50%" align="center">
  
### 🆕 Latest Version
[**v0.1.3-alpha**](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/releases/tag/v0.1.3-alpha)
- 🚀 Performance improvements
- 🔧 Bug fixes
- ✨ New features
  
</td>
<td width="50%" align="center">

### 🎯 Coming Soon
- 📱 Web browser integration
- 🌐 Enhanced weather data
- ⚡ Power optimizations
- 🎨 New display layouts
  
</td>
</tr>
</table>

## ⭐ Core Features {#core}

<div align="center">

🌡️ Medical-grade Temperature | 💧 Humidity & Pressure | 🔋 Smart Battery | 🌍 Weather Data
:-------------------------:|:-------------------------:|:-------------------------:|:-------------------------:
±0.1°C Precision | Real-time Monitoring | Intelligent Power Mgmt | Global Forecasting
`TMP117 Sensor` | `BME680 Sensor` | `2000+ Cycles` | `OpenWeatherMap`

</div>

### 🎮 Smart Features
• 🌙 Moon Phase & Day/Night Tracking  
• 🔄 Auto WiFi Configuration  
• ⏰ Power-efficient Sleep Modes  
• 📊 Environmental Monitoring

## 📊 Battery Performance {#battery}

<table>
  <tr>
    <th>Cycle</th>
    <th>Duration</th>
    <th>Details</th>
  </tr>
  <tr>
    <td>First Cycle</td>
    <td>73 Days</td>
    <td>06/05/2024 - 17/07/2024 (2 months 12 days)</td>
  </tr>
  <tr>
    <td>Second Cycle</td>
    <td>49 Days</td>
    <td>~7 weeks</td>
  </tr>
  <tr>
    <td>Third Cycle</td>
    <td>48 Days</td>
    <td>~7 weeks</td>
  </tr>
</table>

<div align="center">
  <h4>📉 Battery Life Trend</h4>
  First: ████████████████████ 73 days<br>
  Second: █████████████ 49 days<br>
  Third: █████████████ 48 days
</div>

## 📸 Gallery {#gallery}

<table style="border-spacing: 10px; border-collapse: separate;">
  <tr>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd2.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd3.jpg" alt="Clock" width="300"/></td>
  </tr>
  <tr>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd4.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd5.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd6.jpg" alt="Clock" width="300"/></td>
  </tr>
</table>

## 🌱 Eco-Friendly Features {#eco-friendly}

- 📱 E-Paper Display (no backlight needed)
- 🔋 LiFePO4 Batteries (Lithium Iron Phosphate)
  - ✅ Cobalt-free chemistry, reducing environmental impact
  - ✅ 2-3x longer lifespan than traditional Li-ion
  - ✅ Non-toxic and more stable chemistry
  - ✅ Better recyclability due to simpler composition
- 📦 Recycled cardboard construction
- 💡 Smart light sensor for power saving

## 💫 Technical Specifications {#specs}

<details>
<summary>🧠 Core System</summary>

### Core Components
- 💻 XIAO ESP32 C6
  - 32-bit RISC-V single-core CPU up to 160MHz
  - 320KB SRAM, 4MB Flash
  - WiFi 6 & Bluetooth 5.0
  - Ultra-low power consumption: 10µA in deep sleep
  - 11 Digital/Analog pins
  - USB-C interface

</details>

<details>
<summary>📡 Sensors & Connectivity</summary>

### Sensors
- 🌡️ TMP117 High-Precision Temperature Sensor
  - ±0.1°C (max) from -20°C to +50°C
  - 16-bit resolution (0.0078°C)
  - Low power: 3.5µA in shutdown mode
  - Temperature range: -55°C to +150°C
  
- 💨 Bosch BME680 Environmental Sensor
  - Humidity: ±3% accuracy
  - Pressure: ±0.6 hPa absolute accuracy
  - Gas sensor for air quality
  - Temperature range: -40°C to +85°C
  
- 💡 BH1750 Light Sensor
  - 1 - 65535 lux range
  - 16-bit resolution
  - Spectral responsivity close to human eye
  - Low power: 120µA active mode
  
- ⏰ DS3231 RTC Module
  - Accuracy: ±2ppm (±0.432 sec/day)
  - Temperature compensated crystal
  - Battery backup support
  - -40°C to +85°C operating range

</details>

<details>
<summary>⚡ Power System</summary>

### Power & Display
- 🔋 IFR26700 LiFePO4 Battery
  - Nominal voltage: 3.2V
  - Capacity: 4000mAh (8000mAh total with 2 cells)
  - Cycle life: >2000 cycles
  - Size: 26mm × 70mm
  - Max discharge current: 8A
  - Operating temperature: -20°C to +60°C
  - Self-discharge rate: <3% monthly

- ⚡ TP5000 Charging Module
  - Input voltage: 4.5-8V
  - Charging current: 1000mA (adjustable)
  - Charging accuracy: ±1.5%
  - LiFePO4 mode: 3.6V cutoff
  - Over-voltage protection
  - Temperature protection
  - Short circuit protection

- 📟 Waveshare 4.2" E-Paper Display
  - Resolution: 400×300 pixels
  - Three colors: Black, White, Red
  - Active area: 84.8mm × 63.6mm
  - Refresh time: 15 seconds
  - Viewing angle: >170°
  - Operating voltage: 3.3V
  - Ultra-low power consumption
  - No backlight needed
  - SPI interface

</details>

## 🔧 Setup & Configuration {#setup}

### Initial Setup
1. 📱 Power on the device
2. 🔄 Connect to `WCLOCK-WIFI-MANAGER` WiFi network
3. 🌐 Navigate to the IP address shown on the display
4. ⚙️ Enter your WiFi credentials
5. 🎉 Device will restart and connect to your network

### API Configuration {#api}
- 🔑 OpenWeatherMap API key required
- 🌍 Custom API support for personal weather station
- 📍 Configurable location (latitude/longitude)

### Debug Mode {#debug}
- 🐛 Enable via DEBUG_PIN (D6)
- 📊 Shows network diagnostics
- 🔍 Displays detailed error messages

### Power Management
- 🔋 Battery voltage monitoring
- ⚡ Configurable sleep intervals (default: 15 mins)
- 🌙 Night mode with reduced updates
- 📉 Low battery failsafe mode

### Display Modes {#display}
1. Normal Mode
   - Full weather data
   - Temperature, humidity, pressure
   - Moon phase and weather icons
   - Sunrise/sunset times
2. Limited Mode (Low Battery)
   - Basic temperature display
   - Battery status
   - Time and date
3. Night Mode
   - Display sleeps when dark
   - 5-minute wake intervals
   - Power saving features

## 🌿 Environmental Impact {#environmental}

<table>
<tr>
<td width="50%">

### 🔋 Power Efficiency
- 📱 No backlight needed
- 💤 Smart sleep modes
- ⚡ Low power sensors
- 🔆 Ambient light detection

</td>
<td width="50%">

### ♻️ Sustainability
- 🌱 Eco-friendly batteries
- 📦 Recycled materials
- 🔄 Long lifecycle
- 🌍 Low carbon footprint

</td>
</tr>
</table>

## 🤝 Contributing {#contributing}

<div align="center">

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![Contributors](https://img.shields.io/github/contributors/desiFish/ESP32-ePaper-Display-Weather-Clock.svg?style=flat-square)](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/graphs/contributors)

</div>

## 📝 License & Credits {#license}

<table>
<tr>
<td>

### 📜 License
GNU General Public License v3.0
- ✅ Freedom to use commercially
- ✅ Freedom to modify
- ✅ Freedom to distribute
- ❗ Must be open source
- ❗ Must preserve copyright
- ❗ Must use same license

</td>
<td>

### 👏 Credits {#credits}
- 🎨 Weather icons by SeBassTian23
- 📚 GxEPD2 Library
- 💻 Community Contributors

</td>
</tr>
</table>

---

<div align="center">
  
### Made with 💖 and ♻️ materials

<br>

[![Star This Project](https://img.shields.io/github/stars/desiFish/ESP32-ePaper-Display-Weather-Clock?style=social)](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/stargazers)

</div>

