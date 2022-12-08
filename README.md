# Simple IR Signal decoder which shows the results on a webpage

## Designed for ESP8266 boards. Tested on Wemos D1 mini.
<br>

### Instructions

1. After flashing the MCU go to [http://ir_server.local](http://ir_server.local)
2. Read the data from the IR remote
3. Send data using the webpage

<br>

### PlatformIO is recommended, the libraries used are:

```ini
lib_deps =
    # tzapu/WiFiManager @ ^0.16.0`
    https://github.com/tzapu/WiFiManager.git
    # z3t0/IRremote @ ^4.0.0
    https://github.com/Arduino-IRremote/Arduino-IRremote.git
```

### Comoponents:
- Wemos D1 mini or similar ESP MCU
- IR Reciever CHQ1838
- C637 or 2N2222 transistor
- 1k Ohm resistor
- 100 Ohm resistor

### Electrical sketch
![circuit](https://user-images.githubusercontent.com/38941045/206435608-b81e2f5a-78aa-4654-85b1-e6e0e9cca1bf.png)

### Project images
![image](https://user-images.githubusercontent.com/38941045/206438367-093614af-ede7-4a38-9048-6c2273c0c426.png)
![MCU_1](https://user-images.githubusercontent.com/38941045/206443257-68f6eef1-241e-4cdb-99bd-f7c4b58b7d41.jpg)
![MCU_2](https://user-images.githubusercontent.com/38941045/206443396-c197ff2b-15d7-4e77-b0ec-5cc9abf2e9a8.jpg)
![MCU3](https://user-images.githubusercontent.com/38941045/206443467-a26e7517-4cde-412b-8ddb-b0d2d29f3bce.jpg)
![rc600b](https://user-images.githubusercontent.com/38941045/206443590-48451fbd-141f-42ed-af1f-6aef0852a933.jpg)

### Edifier **S880DB** (Remote RC600B) codes

|ACTION  |Protocol|Address|Command|
|--------|-------|--------|-------|
|Power   |NEC    |0xE78   |0x1C   |
|Vol Up  |NEC    |0xE78   |0xC    |
|Vol Down|NEC    |0xE78   |0xF    |
|Classic |NEC    |0xE78   |0x0    |
|Vocal   |NEC    |0xE78   |0x1    |
|Dynamic |NEC    |0xE78   |0xE    |
|Monitor |NEC    |0xE78   |0x4C   |
|Play    |NEC    |0xE78   |0x2    |
|Forward |NEC    |0xE78   |0x3    |
|Back    |NEC    |0xE78   |0x40   |
|Source  |NEC    |0xE78   |0xD    |