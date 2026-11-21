# P01.2 cross-firmware Heltec V4.3 R2 external TFT/SD audit

Hardware: KMRTM35018-SPI V1.0 3.5 inch 480x320, 14-pin TFT/touch SPI header plus 4 independent microSD pads. User requires identical soldered harness for WadaMesh, Meshtastic, DarkMesh.

## Meshtastic firmware source verified
`variants/esp32s3/heltec_v4/variant.h`: LoRa SPI GPIO9/10/11/8; RESET12 BUSY13 DIO1=14; RF FEM GPIO2,5,7,46; GPS reset42 enable34 standby40 PPS41 TX38 RX39; VEXT36 battery1 ADC37. These are reserved.

`variants/esp32s3/heltec_v4/platformio.ini`: `heltec-v4-tft` is ST7789 240x320 with SCK17 MOSI33 DC16 CS15 BL21 RST18, CHSC6x I2C touch on 47/48 and touch reset44. **Not compatible as-is with ILI9488 and resistive SPI touch.**

## WadaMesh source verified
`platformio.ini`: analogous ST7789 TFT pin allocation; WadaMesh already has SD-based PNG XYZ/PMTiles offline maps. Keep existing engine, add board port.

## DarkMesh source verified
Default branch `2.5.20-darkmesh_ghost`; root `platformio.ini` uses `variants/*/platformio.ini` rather than current Meshtastic nested layout. No Heltec V4 file found at `variants/esp32s3/heltec_v4/variant.h` or nested platformio path. Support for V4.3 R2 is **not established**; locate actual variant and check toolchain before promising build compatibility.

## Common wiring — BLOCKED, not safe to solder yet
A single external bus would ideally connect LCD SDI/SD MOSI/touch DIN; LCD SCK/SD SCK/touch CLK; LCD SDO/SD MISO/touch OUT (tri-state validation required), plus separate LCD_CS, SD_CS, TOUCH_CS, LCD_DC, LCD_RST, backlight and optional IRQ. Check display/touch IC markings, voltage, and memory-reserved pins.

**GPIO40 and GPIO41 are NOT free**: Meshtastic GPS standby/PPS. GPIO19/20 USB; 26 memory risk; 45 boot strap; GPIO33 current TFT signal but may be repurposed on the new board target. Do not use a firmware-only GPIO conflict as proof of a hardware collision; examine actual PCB schematic.

## Plan
1. Read exact V4.3 R2 schematic and confirm bottom header availability and GPIO memory/boot status.
2. Inspect DarkMesh's actual Heltec variant support.
3. Freeze one electrical pin table with all three firmware teams.
4. Add isolated build environments in each fork, retaining original targets.
5. Validate compile and then hardware SPI/TFT/SD/touch/GNSS/LoRa/USB operation.
