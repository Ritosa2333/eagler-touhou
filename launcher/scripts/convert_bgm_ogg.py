#!/usr/bin/env python3
"""Build the optional production OGG payload from deployer-owned originals.

This is the host-assembly entry point.  It deliberately lives with the host
packager so a prebuilt Runtime Release does not require any game source
repository.  Archive decoding is delegated to ``touhou_formats``; this module
only owns PCM slicing, Vorbis encoding, and production-output verification.
"""

from __future__ import annotations

from argparse import ArgumentParser
from hashlib import sha256
import json
from pathlib import Path
from struct import Struct

from touhou_formats import extract_pbg4_entry, extract_pbgz_entry


OGG_CRC_POLYNOMIAL = 0x04C11DB7
BGM_FORMAT = Struct("<16s i I i i H H I I H H H 2x")
EXPECTED_PCM_FORMAT = (1, 2, 44100, 176400, 4, 16)
SUPPORTED_GAMES = ("th06", "th07", "th08")


def _progress(game: str, current: int, total: int, label: str, detail: str = "") -> None:
    width = 24
    ratio = min(1.0, max(0.0, current / total if total else 1.0))
    filled = round(width * ratio)
    bar = "#" * filled + "-" * (width - filled)
    suffix = f"  {detail}" if detail else ""
    print(
        f"[OGG {game.upper()}] [{bar}] {current:02d}/{total:02d} {ratio * 100:5.1f}%  {label}{suffix}",
        flush=True,
    )


def _soundfile():
    try:
        import soundfile as sf
    except ImportError as error:
        raise RuntimeError(
            "OGG conversion requires the deployment Python dependencies; "
            "install host/requirements.txt"
        ) from error
    return sf


def _ogg_crc(page: bytes | bytearray) -> int:
    crc = 0
    for value in page:
        crc ^= value << 24
        for _ in range(8):
            crc = (
                ((crc << 1) ^ OGG_CRC_POLYNOMIAL) & 0xFFFFFFFF
                if crc & 0x80000000
                else (crc << 1) & 0xFFFFFFFF
            )
    return crc


def _pin_ogg_serial(path: Path, serial: int) -> None:
    data = bytearray(path.read_bytes())
    offset = 0
    while offset < len(data):
        if offset + 27 > len(data) or data[offset : offset + 4] != b"OggS":
            raise RuntimeError(f"invalid Ogg page in {path} at {offset}")
        segments = data[offset + 26]
        header_end = offset + 27 + segments
        if header_end > len(data):
            raise RuntimeError(f"truncated Ogg segment table in {path}")
        page_end = header_end + sum(data[offset + 27 : header_end])
        if page_end > len(data):
            raise RuntimeError(f"truncated Ogg page in {path}")
        data[offset + 14 : offset + 18] = serial.to_bytes(4, "little")
        data[offset + 22 : offset + 26] = b"\0\0\0\0"
        data[offset + 22 : offset + 26] = _ogg_crc(data[offset:page_end]).to_bytes(4, "little")
        offset = page_end
    path.write_bytes(data)


def _load_baseline(path: Path, game: str) -> dict:
    baseline = json.loads(path.read_text(encoding="utf-8"))
    if (
        baseline.get("schema") != "eagler-touhou/ogg-server-baseline/1"
        or baseline.get("game") != game
        or not isinstance(baseline.get("files"), dict)
    ):
        raise RuntimeError(f"invalid {game.upper()} OGG production baseline: {path}")
    return baseline


def _verify_output(path: Path, expected: dict, frames: int) -> None:
    sf = _soundfile()
    _pin_ogg_serial(path, int(expected["serial"], 0))
    info = sf.info(path)
    if info.frames != frames or info.samplerate != 44100 or info.channels != 2:
        raise RuntimeError(f"OGG verification failed: {path}")
    payload = path.read_bytes()
    digest = sha256(payload).hexdigest()
    if len(payload) != expected["bytes"] or digest != expected["sha256"]:
        raise RuntimeError(
            f"OGG production baseline mismatch: {path.name} "
            f"({len(payload)} bytes, {digest})"
        )


def _matches_expected_output(path: Path, expected: dict) -> bool:
    if not path.is_file():
        return False
    try:
        payload = path.read_bytes()
    except OSError:
        return False
    return len(payload) == expected["bytes"] and sha256(payload).hexdigest() == expected["sha256"]


def _encode_wav(source: Path, destination: Path, quality: float, expected: dict) -> None:
    sf = _soundfile()
    with sf.SoundFile(source, "r") as input_file:
        if input_file.samplerate != 44100 or input_file.channels != 2:
            raise RuntimeError(
                f"unsupported BGM format: {source} "
                f"({input_file.samplerate} Hz, {input_file.channels} channels)"
            )
        frames = input_file.frames
        with sf.SoundFile(
            destination,
            "w",
            samplerate=44100,
            channels=2,
            format="OGG",
            subtype="VORBIS",
            compression_level=quality,
        ) as output_file:
            while True:
                block = input_file.read(65536, dtype="float32", always_2d=True)
                if not len(block):
                    break
                output_file.write(block)
    _verify_output(destination, expected, frames)


def _convert_th06(original: Path, output: Path, baseline: dict, quality: float) -> None:
    bgm = original / "bgm"
    total = len(baseline["files"])
    for index, (output_name, expected) in enumerate(baseline["files"].items(), start=1):
        destination = output / output_name
        if _matches_expected_output(destination, expected):
            _progress("th06", index, total, destination.name, f"CACHED, {destination.stat().st_size} bytes")
            continue
        source = bgm / Path(output_name).with_suffix(".wav").name
        if not source.is_file():
            raise FileNotFoundError(f"required TH06 BGM WAV not found: {source}")
        _encode_wav(source, destination, quality, expected)
        _progress("th06", index, total, destination.name, f"{destination.stat().st_size} bytes")


def _iter_bgm_entries(fmt_data: bytes):
    for offset in range(0, len(fmt_data) - BGM_FORMAT.size + 1, BGM_FORMAT.size):
        fields = BGM_FORMAT.unpack_from(fmt_data, offset)
        name = fields[0].split(b"\0", 1)[0].decode("ascii")
        if not name:
            return
        start, intro_length, total_length = fields[1], fields[3], fields[4]
        pcm_format = fields[5:11]
        if tuple(pcm_format) != EXPECTED_PCM_FORMAT:
            raise RuntimeError(f"unsupported PCM format for {name}: {tuple(pcm_format)}")
        if start < 0 or intro_length < 0 or total_length <= intro_length or total_length % 4:
            raise RuntimeError(f"invalid BGM offsets for {name}")
        yield name, start, intro_length, total_length


def _encode_raw_segment(
    pcm: Path,
    destination: Path,
    start_bytes: int,
    total_bytes: int,
    quality: float,
    expected: dict,
) -> None:
    sf = _soundfile()
    frames = total_bytes // 4
    with sf.SoundFile(
        pcm,
        "r",
        samplerate=44100,
        channels=2,
        subtype="PCM_16",
        endian="LITTLE",
        format="RAW",
    ) as input_file:
        input_file.seek(start_bytes // 4)
        remaining = frames
        with sf.SoundFile(
            destination,
            "w",
            samplerate=44100,
            channels=2,
            format="OGG",
            subtype="VORBIS",
            compression_level=quality,
        ) as output_file:
            while remaining:
                block = input_file.read(min(65536, remaining), dtype="float32", always_2d=True)
                if not len(block):
                    raise RuntimeError(f"truncated PCM data while producing {destination.name}")
                output_file.write(block)
                remaining -= len(block)
    _verify_output(destination, expected, frames)


def _convert_packed_game(game: str, original: Path, output: Path, baseline: dict, quality: float) -> None:
    total = len(baseline["files"])
    cached_names = {
        name for name, expected in baseline["files"].items()
        if _matches_expected_output(output / name, expected)
    }
    if len(cached_names) == total:
        for index, name in enumerate(baseline["files"], start=1):
            destination = output / name
            _progress(game, index, total, destination.name, f"CACHED, {destination.stat().st_size} bytes")
        return

    if game == "th07":
        archive = original / "th07.dat"
        fmt_data = extract_pbg4_entry(archive, "thbgm.fmt")
    elif game == "th08":
        archive = original / "th08.dat"
        fmt_data = extract_pbgz_entry(archive, "bgm/thbgm.fmt")
    else:
        raise AssertionError(game)
    pcm = original / "thbgm.dat"
    if not archive.is_file() or not pcm.is_file():
        raise FileNotFoundError(f"{game.upper()} requires {archive.name} and thbgm.dat under {original}")

    produced: set[str] = set()
    for index, (name, start, intro_length, total_length) in enumerate(_iter_bgm_entries(fmt_data), start=1):
        output_name = Path(name).with_suffix(".ogg").name
        expected = baseline["files"].get(output_name)
        if not expected:
            raise RuntimeError(f"missing production OGG baseline for {output_name}")
        destination = output / output_name
        if output_name in cached_names:
            produced.add(output_name)
            _progress(game, index, total, destination.name, f"CACHED, {destination.stat().st_size} bytes")
            continue
        _encode_raw_segment(pcm, destination, start, total_length, quality, expected)
        produced.add(output_name)
        _progress(
            game,
            index,
            total,
            destination.name,
            f"intro={intro_length // 4}, frames={total_length // 4}, bytes={destination.stat().st_size}",
        )

    expected_names = set(baseline["files"])
    if produced != expected_names:
        raise RuntimeError(
            f"{game.upper()} OGG track set mismatch: produced={sorted(produced)} baseline={sorted(expected_names)}"
        )


def main() -> None:
    parser = ArgumentParser(description="Build verified TH06/TH07/TH08 OGG payloads for host assembly")
    parser.add_argument("--list-games", action="store_true", help="print the format-adapter game ids and exit")
    parser.add_argument("--game", choices=SUPPORTED_GAMES)
    parser.add_argument("--original-dir", type=Path)
    parser.add_argument("--output", type=Path)
    parser.add_argument("--quality", type=float, default=0.55)
    parser.add_argument("--baseline", type=Path)
    args = parser.parse_args()

    if args.list_games:
        print(json.dumps(list(SUPPORTED_GAMES)))
        return
    if not args.game or args.original_dir is None or args.output is None:
        parser.error("--game, --original-dir and --output are required unless --list-games is used")

    baseline_path = args.baseline or Path(__file__).resolve().parents[1] / "host" / "ogg-baselines" / f"{args.game}.json"
    baseline = _load_baseline(baseline_path, args.game)
    if args.quality != float(baseline.get("quality")):
        parser.error(f"--quality must remain {baseline['quality']} to preserve the production OGG baseline")
    original = args.original_dir.resolve()
    if not original.is_dir():
        parser.error(f"original directory not found: {original}")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)

    if args.game == "th06":
        _convert_th06(original, output, baseline, args.quality)
    else:
        _convert_packed_game(args.game, original, output, baseline, args.quality)


if __name__ == "__main__":
    main()
