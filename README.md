<div align="center">
  <h1>🌡️ Minimalistic E-Paper Weather Station</h1>
  <p>
    <img src="https://img.shields.io/badge/ESP32-C6-blue?style=flat-square&logo=espressif"/>
    <img src="https://img.shields.io/badge/Battery-8000mAh-green?style=flat-square"/>
    <img src="https://img.shields.io/badge/Display-4.2%22-orange?style=flat-square"/>
    <img src="https://img.shields.io/badge/Eco-Friendly-brightgreen?style=flat-square"/>
  </p>
  <p>
    <img src="https://img.shields.io/badge/status-active-success.svg" alt="Status">
    <a href="/LICENSE"><img src="https://img.shields.io/github/license/desiFish/ESP32-ePaper-Display-Weather-Clock" alt="License"></a>
    <a href="https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/issues"><img src="https://img.shields.io/github/issues/desiFish/ESP32-ePaper-Display-Weather-Clock.svg" alt="GitHub Issues"></a>
    <a href="https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/releases"><img src="https://img.shields.io/github/v/release/desiFish/ESP32-ePaper-Display-Weather-Clock" alt="Release"></a>
  </p>
</div>

<p align="center">
  <i>🌈 A beautiful, eco-friendly weather station powered by E-Paper display! 🌱</i>
</p>

</div>

> ⚠️ **IMPORTANT**: Please read the [Notes & Warnings] section carefully before proceeding with this project. It contains crucial safety information about battery configuration, charging, and sensor placement.

<div align="center">

### 🌟 Smart • Efficient • Eco-Friendly 🌟

</div>

---

## 🛠️ Hardware Components

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
      • BME680 (Humidity & pressure)
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

## 📱 Key Features & Updates

<table>
<tr>
<td width="50%" align="center">
  
### 🆕 Latest Version
[**v0.2.2-beta**](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/releases/tag/v0.2.2-beta)
- 🚀 Power Efficiency improvements 
- 📝 Documentation improvements
  
</td>
<td width="50%" align="center">

### 🎯 Coming Soon
- 📱 Web browser integration
- 🌐 Enhanced weather data graph
- 🎨 New display layouts
  
</td>
</tr>
</table>

## ⭐ Core Features

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

## ⚡ Power Performance

<div align="center">
  
### 🔋 Power Optimization Results
![Optimization](https://img.shields.io/badge/Power-Optimized-success?style=for-the-badge)
![CPU](https://img.shields.io/badge/CPU-Monitored-blue?style=for-the-badge)
![Efficiency](https://img.shields.io/badge/Efficiency-Enhanced-green?style=for-the-badge)

</div>

> 📊 **Performance Metrics**
> 
> After recent code optimizations, power efficiency has significantly improved:
> 
> | Mode | Duration | Status |
> |------|----------|---------|
> | Normal Active Mode | ~22s | ![Status](https://img.shields.io/badge/22s-Normal-blue) |
> | Sleep Transition | ~20s | ![Status](https://img.shields.io/badge/20s-Expected-green) |
> | Sleep-to-Sleep | <1s | ![Status](https://img.shields.io/badge/<1s-Optimal-success) |
>
> ✨ **Key Improvements**:
> - CPU power thoroughly monitored and conserved
> - Optimized mode transitions
> - Minimal wake time in sleep cycles

## 📊 Battery Performance

> 📝 **Note**: Historical data below is from testing with Li-ion ICR batteries. The shorter duration in cycles 2 and 3 (49 and 48 days vs 73 days) was due to incomplete charging (batteries not reaching full 4.2V). New performance data with 8000mAh LFP batteries (which are more cost-effective) is being collected.

### ICR Battery Data

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

### LFP Battery Data (Ongoing)
<table>
  <tr>
    <th>Cycle</th>
    <th>Full Charge Date</th>
    <th>Battery Dead Date</th>
    <th>Duration</th>
  </tr>
  <tr>
    <td>First Cycle</td>
    <td>January 09, 2025</td>
    <td>Coming Soon</td>
    <td>Ongoing</td>
  </tr>
</table>

<div align="center">
  <h4>📉 Battery Life Trend (ICR Battery Data)</h4>
  First: ████████████████████ 73 days<br>
  Second: █████████████ 49 days<br>
  Third: █████████████ 48 days
</div>

### Current Consumption
> ⚠️ **Note**: Precise current consumption measurements for the entire project are not available due to equipment limitations. Please refer to the Battery Performance section above for real-world usage data and longevity estimates.

## 📸 Gallery

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
  <tr>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd7.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd8.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd9.jpg" alt="Clock" width="300"/></td>
  </tr>
  <tr>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd10.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd11.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd12.jpg" alt="Clock" width="300"/></td>
  </tr>
  <tr>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd13.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd14.jpg" alt="Clock" width="300"/></td>
    <td width="33%" style="padding: 0;"><img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd15.jpg" alt="Clock" width="300"/></td>
  </tr>
</table>

## 🌱 Eco-Friendly Features

- 📱 E-Paper Display (no backlight needed)
- 🔋 LiFePO4 Batteries (Lithium Iron Phosphate)
  - ✅ Cobalt-free chemistry, reducing environmental impact
  - ✅ 2-3x longer lifespan than traditional Li-ion
  - ✅ Non-toxic and more stable chemistry
  - ✅ Better recyclability due to simpler composition
- 📦 Recycled cardboard construction
- 💡 Smart light sensor for power saving

## 💫 Technical Specifications

<details>
<summary>🧠 Core System</summary>

### Core Components
- 💻 **XIAO ESP32 C6** (Recommended)
  - 32-bit RISC-V single-core CPU up to 160MHz
  - 320KB SRAM, 4MB Flash
  - WiFi 6 & Bluetooth 5.0
  - Ultra-low power consumption: ~15µA in deep sleep
    - Note: 14-15µA deep sleep current only achievable when power is supplied through battery pins (BAT+ and BAT-)
  - 11 Digital/Analog pins
  - USB-C interface

### Power Consumption Comparison
- ⚡ **Deep Sleep Current**:
  - XIAO ESP32 C6: ~15µA (Recommended for this project)
  - XIAO ESP32 C3: Higher deep sleep current
  - XIAO ESP32 S3: Potentially lower deep sleep current (untested)
    - Not used due to project's modest performance needs
    - Could be viable alternative for maximum battery life

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

<details>
<summary>📟 Display & Ghosting</summary>

### E-Paper Display Ghosting
- 👻 **Ghosting Issue**: Even after using full scan/update mode, ghosting can occur
  - Red lines may appear over black background
  - Occurs when same elements stay static for extended periods
  
- 🛡️ **GhostProtek Mode**
  - Automatically inverts display colors periodically
  - Activates after predetermined number of boots
  - Prevents static elements from burning in
  - Helps maintain display longevity
  - Experimental feature to reduce ghosting artifacts

</details>

## 🔧 Setup & Configuration

### Initial Setup
1. 📱 Power on the device
2. 🔄 Connect to `WCLOCK-WIFI-MANAGER` WiFi network
3. 🌐 Navigate to the IP address shown on the display
4. ⚙️ Enter your WiFi credentials
5. 🎉 Device will restart and connect to your network

### API Configuration
- 🔑 OpenWeatherMap API key required
- 🌍 Custom API support for personal weather station
- 📍 Configurable location (latitude/longitude)

### Debug Mode
- 🐛 Enable via DEBUG_PIN (D6)
- 📊 Shows network diagnostics
- 🔍 Displays detailed error messages

### Power Management
- 🔋 Battery voltage monitoring
- ⚡ Configurable sleep intervals (default: 15 mins)
- 🌙 Night mode with reduced updates
- 📉 Low battery failsafe mode

### Display Modes
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

## 🌿 Environmental Impact

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

## ⚠️ Important Notes & Warnings

### 🔄 DS3231 Battery Safety ⚡
<div align="center">
<img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd16.jpeg" width="300"><br>
Most DS3231 modules are sold with CR2032 or similar cells which are not rechargeable. If you are using a non-rechargeable cell then **REMOVE** either the resistor or the diode marked in the above image.
</div>

### 🔌 TP5000 Charging Configuration ⚡
<div align="center">
<img src="https://github.com/KamadoTanjiro-beep/epdWeatherClockV1/blob/main/resources/epd17.png" width="300"><br>
**IMPORTANT**: There are some misinformation over internet regarding the charging mode of TP5000. If you are using LiFePO4 cells with TP5000 then **DO NOT** short the above marked area with caption F.
</div>

### Battery Configuration Safety
- 🔋 **Parallel Cell Configuration**: While this project uses two LiFePO4 cells in parallel, this is generally not recommended with TP5000 or similar charging circuits
  - Currently safe due to verified cell balance
  - Long-term balance cannot be guaranteed
  - Only implemented due to ultra-low current draw

### Power Management Considerations
- ⚡ **Current Draw**: This project's extremely low current consumption (well below rated cell capacity) makes the parallel configuration safer
- 🛡️ **Protection**: Always use proper BMS protection for your cells
- 📝 **BMS Setup**: Due to limited 1S LiFePO4 BMS availability:
  - Using 4.2V BMS for 2.5V cutoff protection (Low Discharge Cutoff)
  - TP5000 connected directly to cells for proper 3.6V charging
  - This is a temporary solution until better 1S LFP BMS options become available

### USB Charging Safety
- ⚡ **USB Connection Warning**: Disconnect battery when connecting USB to ESP32C6
  - XIAO modules are designed for LiPo/Li-ion batteries (4.2V charging)
  - Not compatible with LFP battery charging (3.6V required)
  - Dedicated USB-C module recommended
  - Connect USB-C module output to TP5000 input
  - Detailed schematics will be provided later
  - Simple setup despite complex explanation

### Voltage Compatibility
- ✅ **Operating Range**: 
  - All components work perfectly with LFP's lower voltage
  - Tested operational down to 2.8V
  - No impact on battery life or performance
  - LFP cells maintain stable 3.2V for extended periods
  - 6+ months of testing
  - Components designed for wide voltage range operation

### Enclosure Design
- 💨 **Ventilation Requirements**: 
  - Proper air vents are crucial
  - Ensures accurate sensor readings
  - Prevents heat buildup
  - Essential for environmental monitoring
- 🔧 **Sensor Orientation**:
  - TMP117 and BME680 must be mounted horizontally
  - Sensors should face downward
  - Prevents dust and particle accumulation
  - Ensures accurate long-term readings

> ⚠️ **Please consider these points carefully before replicating this setup. Battery safety is crucial!**

## 🤝 Contributing

<div align="center">

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](http://makeapullrequest.com)
[![Contributors](https://img.shields.io/github/contributors/desiFish/ESP32-ePaper-Display-Weather-Clock.svg?style=flat-square)](https://github.com/desiFish/ESP32-ePaper-Display-Weather-Clock/graphs/contributors)

</div>

## 📝 License & Credits

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

### 👏 Credits
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

