#!/usr/bin/env bash
set -euo pipefail

case_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
model="$case_dir/model/traffic-lights.scad"
output_file="${1:-$case_dir/assets/parts-layout.png}"

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

mkdir -p "$(dirname "$output_file")"

"$openscad_bin" \
    --imgsize=1600,1000 \
    --projection=ortho \
    --camera=110,42,35,65,0,45,430 \
    -D 'part="layout"' \
    -o "$output_file" "$model"

echo "Parts preview exported to $output_file"
