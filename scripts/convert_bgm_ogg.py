#!/usr/bin/env python3
"""Generate the optional OGG Web payload without modifying source WAV files."""

from argparse import ArgumentParser
from pathlib import Path

import soundfile as sf


def main() -> None:
    parser = ArgumentParser()
    parser.add_argument("--input", type=Path, default=Path("assets/bgm"))
    parser.add_argument("--output", type=Path, default=Path("assets-ogg/bgm"))
    parser.add_argument("--quality", type=float, default=0.55)
    args = parser.parse_args()

    if not 0.0 <= args.quality <= 1.0:
        parser.error("--quality must be between 0 and 1")
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
        info = sf.info(destination)
        if info.frames != source_frames or info.samplerate != 44100 or info.channels != 2:
            raise RuntimeError(f"OGG verification failed: {destination}")
        print(f"{source.name} -> {destination.name} ({destination.stat().st_size} bytes)")


if __name__ == "__main__":
    main()
