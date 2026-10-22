#!/usr/bin/env bash
set -euo pipefail

case_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
model="$case_dir/model/traffic-lights.scad"
output_dir="${1:-$case_dir/build}"

if [[ -n "${OPENSCAD_BIN:-}" ]]; then
    openscad_bin="$OPENSCAD_BIN"
elif command -v openscad >/dev/null 2>&1; then
    openscad_bin="$(command -v openscad)"
elif [[ -x /Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD ]]; then
    openscad_bin=/Applications/OpenSCAD.app/Contents/MacOS/OpenSCAD
else
    echo "OpenSCAD was not found. Install it or set OPENSCAD_BIN to its executable." >&2
    exit 1
fi

mkdir -p "$output_dir"

parts=(base_box base_cover pole head_box head_facade head_cover power_switch_hat)
files=(base-box.stl base-cover.stl pole.stl head-box.stl head-facade.stl head-cover.stl power-switch-hat.stl)

for i in "${!parts[@]}"; do
    echo "Exporting ${files[$i]}..."
    "$openscad_bin" --hardwarnings --export-format binstl \
        -D "part=\"${parts[$i]}\"" \
        -o "$output_dir/${files[$i]}" "$model"
done

echo "${#parts[@]} STL files exported to $output_dir"
