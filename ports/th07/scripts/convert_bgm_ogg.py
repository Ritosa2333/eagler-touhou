#!/usr/bin/env python3
"""Extract TH07's BGM format table and create sample-accurate OGG tracks."""

from argparse import ArgumentParser
from hashlib import sha256
import json
import os
from pathlib import Path
from struct import Struct
import sys

def load_extract_pbg4_entry(host_root: Path):
    shared_tools = host_root.expanduser().resolve() / "scripts"
    helper = shared_tools / "touhou_formats.py"
    if not helper.is_file():
        raise RuntimeError(f"Host checkout is missing scripts/touhou_formats.py: {host_root}")
    if str(shared_tools) not in sys.path:
        sys.path.insert(0, str(shared_tools))
    from touhou_formats import extract_pbg4_entry
    return extract_pbg4_entry

SHARED_TOOLS = Path(__file__).resolve().parents[2] / "eagler-touhou" / "scripts"
if str(SHARED_TOOLS) not in sys.path:
    sys.path.insert(0, str(SHARED_TOOLS))
from touhou_formats import extract_pbg4_entry


OGG_CRC_POLYNOMIAL = 0x04C11DB7


def ogg_crc(page: bytes | bytearray) -> int:
    crc = 0
    for value in page:
        crc ^= value << 24
        for _ in range(8):
            crc = ((crc << 1) ^ OGG_CRC_POLYNOMIAL) & 0xFFFFFFFF if crc & 0x80000000 else (crc << 1) & 0xFFFFFFFF
    return crc


def pin_ogg_serial(path: Path, serial: int) -> None:
    data = bytearray(path.read_bytes())
    offset = 0
    while offset < len(data):
        if offset + 27 > len(data) or data[offset : offset + 4] != b"OggS":
            raise RuntimeError(f"invalid Ogg page in {path} at {offset}")
        segments = data[offset + 26]
        header_end = offset + 27 + segments
        if header_end > len(data):
            raise RuntimeError(f"truncated Ogg segment table in {path}")
        payload_size = sum(data[offset + 27 : header_end])
        page_end = header_end + payload_size
        if page_end > len(data):
            raise RuntimeError(f"truncated Ogg page in {path}")
        data[offset + 14 : offset + 18] = serial.to_bytes(4, "little")
        data[offset + 22 : offset + 26] = b"\0\0\0\0"
        checksum = ogg_crc(data[offset:page_end])
        data[offset + 22 : offset + 26] = checksum.to_bytes(4, "little")
        offset = page_end
    path.write_bytes(data)


def load_server_baseline(path: Path) -> dict:
    baseline = json.loads(path.read_text(encoding="utf-8"))
    if baseline.get("schema") != "eagler-touhou/ogg-server-baseline/1" or baseline.get("game") != "th07":
        raise RuntimeError(f"invalid TH07 OGG server baseline: {path}")
    return baseline


BGM_FORMAT = Struct("<16s i I i i H H I I H H H 2x")


def main() -> None:
    parser = ArgumentParser()
    parser.add_argument(
        "--host-root", type=Path,
        default=Path(os.environ["TH_EAGLER_HOST_ROOT"]) if os.environ.get("TH_EAGLER_HOST_ROOT") else None,
        help="eagler-touhou checkout (or set TH_EAGLER_HOST_ROOT)",
    )
    parser.add_argument("--archive", type=Path, default=Path("assets/th07.dat"))
    parser.add_argument("--pcm", type=Path, default=Path("assets/thbgm.dat"))
    parser.add_argument("--output", type=Path, default=Path("assets-ogg/bgm-ogg"))
    parser.add_argument("--quality", type=float, default=0.55)
    parser.add_argument("--baseline", type=Path, default=Path(__file__).with_name("ogg_server_baseline.json"))
    args = parser.parse_args()
    if args.host_root is None:
        parser.error("--host-root or TH_EAGLER_HOST_ROOT is required")
    if not 0.0 <= args.quality <= 1.0:
        parser.error("--quality must be between 0 and 1")
    try:
        import soundfile as sf
    except ImportError as error:
        parser.error(f"soundfile is required for OGG conversion: {error}")
    extract_pbg4_entry = load_extract_pbg4_entry(args.host_root)
    baseline = load_server_baseline(args.baseline)
    if args.quality != float(baseline.get("quality")):
        parser.error(f"--quality must remain {baseline['quality']} to preserve the production OGG baseline")

    fmt_data = extract_pbg4_entry(args.archive, "thbgm.fmt")
    args.output.mkdir(parents=True, exist_ok=True)
    for offset in range(0, len(fmt_data) - BGM_FORMAT.size + 1, BGM_FORMAT.size):
        fields = BGM_FORMAT.unpack_from(fmt_data, offset)
        name = fields[0].split(b"\0", 1)[0].decode("ascii")
        if not name:
            break
        start, intro_length, total_length = fields[1], fields[3], fields[4]
        format_tag, channels, sample_rate, byte_rate, block_align, bits, _ = fields[5:]
        if (format_tag, channels, sample_rate, byte_rate, block_align, bits) != (1, 2, 44100, 176400, 4, 16):
            raise RuntimeError(f"unsupported format for {name}")
        if start < 0 or intro_length < 0 or total_length <= intro_length or total_length % 4:
            raise RuntimeError(f"invalid offsets for {name}")

        destination = args.output / Path(name).with_suffix(".ogg").name
        frame_count = total_length // 4
        with sf.SoundFile(args.pcm, "r", samplerate=44100, channels=2, subtype="PCM_16", endian="LITTLE", format="RAW") as input_file:
            input_file.seek(start // 4)
            remaining = frame_count
            with sf.SoundFile(destination, "w", samplerate=44100, channels=2, format="OGG", subtype="VORBIS", compression_level=args.quality) as output_file:
                while remaining:
                    block = input_file.read(min(65536, remaining), dtype="float32", always_2d=True)
                    if not len(block):
                        raise RuntimeError(f"truncated PCM data for {name}")
                    output_file.write(block)
                    remaining -= len(block)
        expected = baseline["files"].get(destination.name)
        if not expected:
            raise RuntimeError(f"missing production OGG baseline for {destination.name}")
        pin_ogg_serial(destination, int(expected["serial"], 0))
        info = sf.info(destination)
        if info.frames != frame_count or info.channels != 2 or info.samplerate != 44100:
            raise RuntimeError(f"OGG verification failed: {destination}")
        payload = destination.read_bytes()
        digest = sha256(payload).hexdigest()
        if len(payload) != expected["bytes"] or digest != expected["sha256"]:
            raise RuntimeError(
                f"OGG production baseline mismatch: {destination.name} "
                f"({len(payload)} bytes, {digest})"
            )
        print(f"{name}: intro={intro_length // 4}, frames={frame_count}, bytes={destination.stat().st_size}")


if __name__ == "__main__":
    main()
