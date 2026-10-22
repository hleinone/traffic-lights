# ESP32 traffic-light firmware

See the [project overview](../README.md), [electronics wiring](../electronics/README.md), and [printable case](../enclosure/README.md).

This firmware connects to the BLE HID remote at `FF:FF:50:00:27:DA`
(AB Shutter 3), prints its input reports over USB, and advances the lights on
each button press.
The address and button reports are specific to the tested fob. See
[Using another remote](#using-another-remote) before substituting one.

## Tools

Install [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html), or use the PlatformIO terminal if you already have its VS Code extension. The ESP32 platform version is pinned in [`platformio.ini`](platformio.ini); PlatformIO downloads its toolchain and ESP-IDF dependencies on the first build, which needs internet access.

The project uses the `esp32dev` profile for the original ESP32-WROOM-32 development board with 4 MB flash and an attached 18650 battery holder. See the [board reference and battery requirements](../electronics/README.md#board-and-battery) when choosing hardware. Other ESP32 variants need their own board configuration; ESP8266 / ESP-WROOM-02 options are not compatible with this firmware.

From the project root, enter the firmware folder and check the CLI:

```sh
cd firmware
pio --version
```

All remaining commands in this guide run from `firmware/`.

## Select the USB port

Connect the board with a micro-USB **data** cable, then list the serial ports:

```sh
pio device list
```

The saved `upload_port` and `monitor_port` in [`platformio.ini`](platformio.ini) are both `/dev/cu.usbserial-0001`. Set both to the port shown for your board if it differs. Port names vary between computers and operating systems.

## Run

1. Disconnect and forget the remote on the phone so the phone does not reclaim it.
2. Close any serial monitor using the board's port before uploading.
3. Upload with the command below. This builds the firmware and replaces the board's existing program. To check compilation without uploading, use `pio run` instead.

```sh
pio run --target upload
```

After a successful upload, view the logs:

```sh
pio device monitor
```

The monitor uses the configured 115200 baud rate. Exit with **Ctrl+C**. Look for `REMOTE_TEST` lines containing `OPEN`, `INPUT`, and `LIGHTS ->` while testing the fob.

To override the saved port for one session, supply it separately to both commands (replace the example port):

```sh
pio run --target upload --upload-port /dev/cu.usbserial-0001
pio device monitor --port /dev/cu.usbserial-0001
```

## Verify the light sequence

| Action | Expected result |
| --- | --- |
| Start the ESP32 with the fob off | Yellow alternates on and off every 0.5 seconds |
| Turn on the nearby fob and wait for connection | Red turns on as soon as the connection succeeds |
| Press and release while red | Red and yellow for 1 second, then green |
| Press and release while green | Yellow for 1.5 seconds, then red |
| Press during either transition | The extra press is ignored |
| Turn the fob off | Once the ESP32 detects the disconnect, yellow starts blinking |
| Reconnect the fob | Red turns on again |

The firmware scans repeatedly while disconnected, so finding the fob can take several seconds. A lost radio connection may also take time to be detected.

## Using another remote

1. Find its Bluetooth address, for example in nRF Connect's device list. The name alone does not identify this particular fob.
2. Update the six address bytes in both `remote_address` in [`src/main.c`](src/main.c) and the scan-result address comparison in [`src/esp_hid_gap.c`](src/esp_hid_gap.c).
3. Rebuild and upload, then inspect the `INPUT` reports in the serial monitor. The current decoder expects consumer-control report ID 2 with two bytes: `01 00` for a press and `00 00` for release. Only the first press report after a release starts a transition.

Another remote can use a different report format even if it has the same name. If it connects but presses have no effect, compare its reports with the conditions in `ESP_HIDH_INPUT_EVENT` in `main.c` before changing the decoder.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| `pio` is not found | Use the PlatformIO terminal or follow its installation guide to add the shell command |
| No USB serial port appears | Check the board's power, USB connection, and that the cable supports data |
| Port is busy | Exit other serial monitors, including the one from a previous upload session |
| Upload cannot connect | Verify the selected port and follow [Espressif's bootloader troubleshooting](https://docs.espressif.com/projects/esptool/en/latest/esp32/troubleshooting.html#bootloader-won-t-respond) |
| Yellow keeps blinking | Check the fob is on, the phone has released it, and both source address filters match it |
| Logs show the expected light state but an LED stays dark | Check the pin, resistor, polarity, and common GND against the electronics guide |

## Diagnostics and local files

The LED pinout and assembly checks are in [electronics/](../electronics/README.md).

The original 4 MB board image is stored locally at `backups/original-firmware.bin`. A previous diagnostic capture is at `logs/remote-test.log`; the helper script [scripts/capture_remote.py](scripts/capture_remote.py) writes future captures there. Backups and logs are ignored by Git, so keep a separate copy of the original image if you need it on another computer.

## Source

Based on Espressif's ESP-IDF v5.5.1 `examples/bluetooth/esp_hid_host`:
https://github.com/espressif/esp-idf/tree/v5.5.1/examples/bluetooth/esp_hid_host

Espressif source license: Unlicense OR CC0-1.0 (headers retained).
Changes: BLE-only configuration, exact remote selection, repeated scanning,
connection state tracking, and no-input/no-output bonding for the one-button fob.
