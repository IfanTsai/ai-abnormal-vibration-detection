## ai-abnormal-vibration-detection

### Hardware

- MCU: STM32L432KCU3

- Acceleration Sensor: LIS3DH

### Start

```bash
sudo apt install -y gcc-arm-none-eabi stlink-tools
make
st-flash write build/ai-abnormal-vibration-detection.bin 0x08000000
```
