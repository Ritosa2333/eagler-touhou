"""Create a windowed TH19 configuration for unattended parity captures."""
import argparse
from pathlib import Path
import struct


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--size", choices=("1280x960", "960x720", "640x480"), default="1280x960")
    args = parser.parse_args()
    if args.source.resolve() == args.output.resolve():
        parser.error("Use a separate output configuration")
    data = bytearray(args.source.read_bytes())
    if len(data) != 256 or data[:4] != bytes.fromhex("06001900"):
        parser.error("Expected a version-6 TH19 configuration (256 bytes)")
    # Original 1.10c: 0x413c55 passes the buffer at 0x62bd14 to
    # 0x4d6df0. 0x4b45c0 reads resolution at 0x62bddb; 0x4b4080
    # checks bit 0x100 at 0x62bde4 before showing the startup dialog.
    # 0x594550 maps configuration indices 5, 4, 3 to dialog controls
    # 214, 215, 216 (the three windowed resolutions).
    data[0xc7] = {"1280x960": 5, "960x720": 4, "640x480": 3}[args.size]
    struct.pack_into("<I", data, 0xd0, struct.unpack_from("<I", data, 0xd0)[0] & ~0x100)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(data)
    print(args.output.resolve())


if __name__ == "__main__":
    main()
