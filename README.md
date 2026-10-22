# Bluetooth traffic lights

A small, battery-powered toy traffic lights controlled by a Bluetooth fob. It blinks yellow while disconnected, changes to red when the fob connects, and advances between red and green on button presses with yellow transitions.

For a shutter remote reference, see [this Bluetooth camera remote on Amazon](https://www.amazon.eg/-/en/Bluetooth-Remote-Control-Mobile-Camera/dp/B0BXQP9815). Follow [Using another remote](firmware/README.md#using-another-remote) to configure your fob's address and check its button reports.

The hardware uses an **ESP32-WROOM-32 development board with an attached single-18650 battery holder and power switch**, like [this AliExpress board](https://www.aliexpress.com/item/1005008881707615.html). For portable use, it needs **one rechargeable 18650 Li-ion cell, 3.6–3.7 V nominal (4.2 V fully charged)**. Check whether the seller includes the cell, and read the [board and battery requirements](electronics/README.md#board-and-battery) before buying.

| Folder | What is inside |
| --- | --- |
| [firmware/](firmware/README.md) | ESP32 source, PlatformIO configuration, upload instructions, and troubleshooting |
| [electronics/](electronics/README.md) | LED wiring, parts, and electrical assembly checks |
| [enclosure/](enclosure/README.md) | Editable OpenSCAD case, STL and preview export scripts, and fit checks |

The printable design has a low electronics base, a hollow pole for the wires, and a separate three-light head. Run `./enclosure/export-stl.sh` to generate the seven STL files from the current OpenSCAD model, or `./enclosure/export-preview.sh` to refresh the parts image. Check the board, battery holder, and LED fit before printing the full set.

## Start here

1. Follow the [electronics guide](electronics/README.md) to connect the three LEDs, each with its own 330 Ω resistor. Test the circuit before enclosing it.
2. Follow the [firmware guide](firmware/README.md) to install PlatformIO, select the USB port, and upload the program. The firmware is configured for the original ESP32-WROOM-32 board and one specific fob address; another fob may need changes.
3. Check the light sequence and disconnection behaviour using the firmware guide's verification table.
4. Follow the [enclosure guide](enclosure/README.md) to export, slice, and test-fit the seven parts. Wire and assemble the case after checking the fit.

Commands in this overview run from the project root. The firmware guide explicitly changes into `firmware/` for its commands.

The source, configuration defaults, documentation, and PNG preview are versioned. Build output, generated STLs, device backups, and diagnostic logs are local files ignored by Git; a fresh checkout does not contain them.

## License

This project is licensed under the [MIT License](LICENSE).
