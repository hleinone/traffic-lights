"""Capture USB logs for a bounded session; run with PlatformIO's Python."""
import argparse
from pathlib import Path
import time
import serial

parser = argparse.ArgumentParser()
parser.add_argument("--seconds", type=int, default=45)
parser.add_argument("--port", default="/dev/cu.usbserial-0001")
parser.add_argument("--reset", action="store_true", help="restart the ESP32 before capture")
args = parser.parse_args()
log_path = Path(__file__).resolve().parents[1] / "logs" / "remote-test.log"
log_path.parent.mkdir(exist_ok=True)
with serial.Serial(args.port, 115200, timeout=0.5) as port, log_path.open("a") as log:
    port.dtr = False
    port.rts = False
    if args.reset:
        port.rts = True
        time.sleep(0.1)
        port.rts = False
    deadline = time.monotonic() + args.seconds
    print(f"Listening for {args.seconds} seconds; saving to {log_path}", flush=True)
    while time.monotonic() < deadline:
        line = port.readline().decode("utf-8", errors="replace")
        if line:
            log.write(line)
            log.flush()
            print(line, end="", flush=True)
