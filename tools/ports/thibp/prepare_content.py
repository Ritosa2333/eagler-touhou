"""Prepare local 1.10b data for the ph3-modern Runtime resource bridge."""

import argparse
import json
import shutil
import struct
from pathlib import Path


SCRIPT_ROOT = Path("script/Th_IBP")
CONTENT_MAGIC = b"HSOBFS01"
CONTENT_SCHEMA = "eagler-hsob-content/1"


def content_files(root):
    yield Path("th_dnh.def")
    for directory in (Path("resource"), Path("script/default_system"), SCRIPT_ROOT):
        for path in sorted((root / directory).rglob("*")):
            if not path.is_file():
                continue
            relative = path.relative_to(root)
            if relative.is_relative_to(SCRIPT_ROOT / "data") or relative.is_relative_to(SCRIPT_ROOT / "replay"):
                continue
            if path.suffix.lower() not in {".exe", ".dll", ".lnk", ".log"}:
                yield relative


def translated_scripts(chinese, japanese, files):
    if japanese is None:
        return {}
    result = {}
    for path in files:
        if path.suffix.lower() != ".dnh":
            continue
        source = (chinese / path).read_bytes()
        if source.startswith(b"\xff\xfe") or source == (japanese / path).read_bytes():
            continue
        result[path] = b"\xff\xfe" + source.decode("gbk").encode("utf-16le")
    return result


def pack(root, output, *, japanese=None):
    files = tuple(content_files(root))
    if SCRIPT_ROOT / "Th_IBP.dnh" not in files:
        raise ValueError(f"Missing the original package entry under {root}")
    translations = translated_scripts(root, japanese, files)
    entries = []
    offset = 0
    for path in files:
        size = len(translations[path]) if path in translations else (root / path).stat().st_size
        entries.append({"path": "/game/" + path.as_posix(), "offset": offset, "bytes": size})
        offset += size
    manifest = json.dumps({"schema": CONTENT_SCHEMA, "files": entries}, separators=(",", ":")).encode("utf-8")
    with output.open("xb") as stream:
        stream.write(CONTENT_MAGIC)
        stream.write(struct.pack("<I", len(manifest)))
        stream.write(manifest)
        for path in files:
            if path in translations:
                stream.write(translations[path])
            else:
                with (root / path).open("rb") as source:
                    shutil.copyfileobj(source, stream)
    return len(files), len(translations)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("game", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--japanese", type=Path, help="Original CP932 installation for detecting GBK translations")
    args = parser.parse_args()
    files, translations = pack(args.game, args.output, japanese=args.japanese)
    print(f"Prepared {files} files with {translations} GBK scripts converted to UTF-16LE: {args.output}")


if __name__ == "__main__":
    main()
