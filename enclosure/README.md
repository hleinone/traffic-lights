# Printable traffic-light case

The editable design and export selector are both in [`model/traffic-lights.scad`](model/traffic-lights.scad). Open it in OpenSCAD to see all seven parts, or change `part` at the top to preview one part. The [parts preview](assets/parts-layout.png) shows the current design.

## Tools

Install [OpenSCAD](https://openscad.org/downloads.html) to edit the model and run the exports. The export scripts require Bash and are used from a terminal; they have been run on macOS for this project. Use your printer's slicer to turn the exported STLs into print jobs.

## Export the STL files

From the project root, run:

```sh
./enclosure/export-stl.sh
```

The script finds OpenSCAD on your PATH or at the standard macOS app location and writes seven STLs to **`enclosure/build/`**. That folder is ignored by Git, so the generated print files do not need to be stored with the source. Rerun the command whenever you edit the model. You can pass another output directory as the first argument, or set `OPENSCAD_BIN` if OpenSCAD is installed elsewhere.

| `part` setting | Generated file | Approximate size (mm) |
| --- | --- | --- |
| `base_box` | `base-box.stl` | 34 × 96 × 31 |
| `base_cover` | `base-cover.stl` | 34 × 93 × 5.5 |
| `pole` | `pole.stl` | 30 × 30 × 103, including the flared end and joint tabs |
| `head_box` | `head-box.stl` | 20 × 44 × 17 |
| `head_facade` | `head-facade.stl` | 20 × 44 × 9.9, including visors |
| `head_cover` | `head-cover.stl` | 18 × 42 × 3 |
| `power_switch_hat` | `power-switch-hat.stl` | 3 × 3 × 5 |

Import the STLs into your slicer in millimetres at 100% scale. The model places each exported part on the print bed. The switch hat prints with its closed end down and socket facing up. The pole prints vertically, so a brim may help keep it stable. Review the export output for geometry warnings and inspect the sliced layers after model changes; a successful export does not verify the physical fit.

The pole's four joint tabs are identical and spaced at 90° intervals. The base cover has four matching through-openings, with 0.2 mm clearance on each side by default. If your printer makes the joint too tight or loose, adjust `pole_joint_clearance` near the top of the OpenSCAD file and rerun the export script.

## Export the parts preview

From the project root, run:

```sh
./enclosure/export-preview.sh
```

This renders the `layout` view to [`assets/parts-layout.png`](assets/parts-layout.png) at 1600 × 1000 pixels. Pass another PNG path as the first argument to write elsewhere. Like the STL script, it accepts `OPENSCAD_BIN` if OpenSCAD is installed in a different location.

After changing the model, rerun both export scripts: the PNG and STLs are separate outputs. If PNG export reports `Unable to create NSOpenGLContext`, rerun it in a desktop terminal with graphics access; this has occurred when rendering inside a sandbox.

## Check the physical fit

The base is **34 × 96 × 31 mm outside**. Before printing the full set, compare its interior with your approximately 30 × 90 mm board and attached single-18650 holder; see the [board reference and battery requirements](../electronics/README.md#board-and-battery). Measure the complete assembly with your chosen cell fitted, including terminals and header pins; the earlier battery-depth estimate was 20–25 mm. Check the USB and switch openings against the board, and test an LED against the facade's roughly 10.4 mm through-holes. Edit the source model and rerun the export script if needed.

The base's screw posts are square with the corner facing the cavity rounded, and their outer sides join the two adjacent walls. The screw sizes and mating fit have not yet been verified with the physical hardware. Keep the LED wiring insulated as described in the [electronics guide](../electronics/README.md).

Before final assembly:

1. Dry-fit the board, battery holder, and base cover. Check the USB opening and the switch hat's fit and full travel.
2. Test the pole tabs in the base cover, then the pole-to-head joint and the head's facade and rear cover. The pole tabs guide alignment; the model does not include a locking feature for that joint.
3. Follow the electronics guide to pass the wires through the pole and install the LEDs. Leave enough slack to reopen the base and head.
4. Run the complete light sequence before fastening the base cover. Check that closing it does not pinch wiring or obstruct the switch.

The screw diameter and length, retention method for the head and pole joints, and printer settings still need to be recorded after a physical fit test. No tested values are specified here yet.
