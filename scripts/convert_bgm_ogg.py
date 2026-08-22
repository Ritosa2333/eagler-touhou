#!/usr/bin/env python3
"""Generate the optional OGG Web payload without modifying source WAV files."""

from argparse import ArgumentParser
from hashlib import sha256
import json
from pathlib import Path

import soundfile as sf


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
    if baseline.get("schema") != "eagler-touhou/ogg-server-baseline/1" or baseline.get("game") != "th06":
        raise RuntimeError(f"invalid TH06 OGG server baseline: {path}")
    return baseline


def main() -> None:
    parser = ArgumentParser()
    parser.add_argument("--input", type=Path, default=Path("assets/bgm"))
    parser.add_argument("--output", type=Path, default=Path("assets-ogg/bgm"))
    parser.add_argument("--quality", type=float, default=0.55)
    parser.add_argument("--baseline", type=Path, default=Path(__file__).with_name("ogg_server_baseline.json"))
    args = parser.parse_args()

    if not 0.0 <= args.quality <= 1.0:
        parser.error("--quality must be between 0 and 1")
    baseline = load_server_baseline(args.baseline)
    if args.quality != float(baseline.get("quality")):
        parser.error(f"--quality must remain {baseline['quality']} to preserve the production OGG baseline")
    sources = sorted(args.input.glob("*.wav"))
    if not sources:
        parser.error(f"no WAV files found under {args.input}")
    args.output.mkdir(parents=True, exist_ok=True)

    for source in sources:
        destination = args.output / source.with_suffix(".ogg").name
        with sf.SoundFile(source, "r") as input_file:
            if input_file.samplerate != 44100 or input_file.channels != 2:
                raise RuntimeError(
                    f"unsupported BGM format: {source} "
                    f"({input_file.samplerate} Hz, {input_file.channels} channels)"
                )
            source_frames = input_file.frames
            with sf.SoundFile(
                destination,
                "w",
                samplerate=input_file.samplerate,
                channels=input_file.channels,
                format="OGG",
                subtype="VORBIS",
                compression_level=args.quality,
            ) as output_file:
                while True:
                    block = input_file.read(65536, dtype="float32", always_2d=True)
                    if not len(block):
                        break
                    output_file.write(block)
        expected = baseline["files"].get(destination.name)
        if not expected:
            raise RuntimeError(f"missing production OGG baseline for {destination.name}")
        pin_ogg_serial(destination, int(expected["serial"], 0))
        info = sf.info(destination)
        if info.frames != source_frames or info.samplerate != 44100 or info.channels != 2:
            raise RuntimeError(f"OGG verification failed: {destination}")
        payload = destination.read_bytes()
        digest = sha256(payload).hexdigest()
        if len(payload) != expected["bytes"] or digest != expected["sha256"]:
            raise RuntimeError(
                f"OGG production baseline mismatch: {destination.name} "
                f"({len(payload)} bytes, {digest})"
            )
        print(f"{source.name} -> {destination.name} ({destination.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
