import argparse
import hashlib
import json
import os
import subprocess
import sys
import tempfile
import unicodedata
from pathlib import Path

from fontTools.ttLib import TTFont


def cmap_codepoints(path):
    font = TTFont(path, lazy=True)
    try:
        codepoints = set()
        for table in font["cmap"].tables:
            codepoints.update(table.cmap)
        return codepoints
    finally:
        font.close()


def readable_codepoints(text):
    normalized = unicodedata.normalize("NFC", text)
    return {
        ord(character)
        for character in normalized
        if ord(character) >= 0x20 and not 0x7F <= ord(character) <= 0x9F
    }


def write_subset(source, requested, output):
    output = Path(output)
    output.parent.mkdir(parents=True, exist_ok=True)
    temporary = None
    try:
        with tempfile.NamedTemporaryFile(
            "w", encoding="utf-8", suffix=".txt", delete=False
        ) as text_file:
            text_file.write("".join(chr(codepoint) for codepoint in sorted(requested)))
            temporary = text_file.name
        command = [
            sys.executable,
            "-m",
            "fontTools.subset",
            str(source),
            f"--text-file={temporary}",
            f"--output-file={output}",
            "--layout-features=*",
            "--name-IDs=*",
            "--name-legacy",
            "--name-languages=*",
            "--notdef-glyph",
            "--notdef-outline",
            "--recommended-glyphs",
            "--no-ignore-missing-unicodes",
            "--no-prune-unicode-ranges",
            "--flavor=woff2",
        ]
        subprocess.run(command, check=True)
    finally:
        if temporary:
            os.unlink(temporary)


def ranges(codepoints):
    values = sorted(codepoints)
    if not values:
        return ""
    result = []
    start = previous = values[0]
    for value in values[1:]:
        if value == previous + 1:
            previous = value
            continue
        result.append((start, previous))
        start = previous = value
    result.append((start, previous))
    return ",".join(
        f"U+{start:04X}" if start == end else f"U+{start:04X}-{end:04X}"
        for start, end in result
    )


def css_block(family, weight, output, url_prefix):
    codepoints = cmap_codepoints(output)
    name = Path(output).name
    prefix = url_prefix.rstrip("/")
    return "\n".join(
        [
            "@font-face{",
            f'font-family:"{family}";',
            "font-style:normal;",
            f"font-weight:{weight};",
            "font-display:swap;",
            f'src:url("{prefix}/{name}") format("woff2");',
            f"unicode-range:{ranges(codepoints)};",
            "}",
        ]
    )


def sha256(path):
    digest = hashlib.sha256()
    with open(path, "rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main():
    parser = argparse.ArgumentParser(
        description=(
            "Split an existing WOFF2 into disjoint critical and deferred cmap subsets. "
            "Run once per weight; use --append-css for subsequent weights."
        )
    )
    parser.add_argument("--font", required=True, help="Existing complete WOFF2 input")
    parser.add_argument("--critical-text-file", required=True)
    parser.add_argument("--critical-output", required=True)
    parser.add_argument("--deferred-output", required=True)
    parser.add_argument("--family", default="ET Chill Round")
    parser.add_argument("--weight", type=int, required=True)
    parser.add_argument("--url-prefix", default="assets/fonts")
    parser.add_argument("--css-output")
    parser.add_argument("--deferred-css-output")
    parser.add_argument(
        "--append-css",
        action="store_true",
        help="Append the generated @font-face block instead of replacing --css-output",
    )
    args = parser.parse_args()

    source = Path(args.font)
    critical_output = Path(args.critical_output)
    deferred_output = Path(args.deferred_output)
    if not source.is_file():
        raise SystemExit(f"font input does not exist: {source}")
    if critical_output.resolve() == deferred_output.resolve():
        raise SystemExit("critical and deferred outputs must be different files")

    original = cmap_codepoints(source)
    requested = set(range(0x20, 0x7F))
    requested.update(readable_codepoints(Path(args.critical_text_file).read_text("utf-8")))
    critical = requested & original
    deferred = original - critical
    if critical & deferred or critical | deferred != original:
        raise SystemExit("critical/deferred cmap partition is not disjoint and complete")

    write_subset(source, critical, critical_output)
    write_subset(source, deferred, deferred_output)
    critical_result = cmap_codepoints(critical_output)
    deferred_result = cmap_codepoints(deferred_output)
    if critical_result != critical or deferred_result != deferred:
        raise SystemExit(
            "fontTools subset output cmap differs from the requested partition"
        )
    if critical_result & deferred_result or critical_result | deferred_result != original:
        raise SystemExit("generated cmap partition is not disjoint and complete")

    for destination, output in (
        (args.css_output, critical_output),
        (args.deferred_css_output, deferred_output),
    ):
        if destination:
            css_output = Path(destination)
            css_output.parent.mkdir(parents=True, exist_ok=True)
            mode = "a" if args.append_css else "w"
            with css_output.open(mode, encoding="utf-8", newline="\n") as target:
                if args.append_css and css_output.stat().st_size:
                    target.write("\n")
                target.write(css_block(args.family, args.weight, output, args.url_prefix))
                target.write("\n")

    print(
        json.dumps(
            {
                "input": str(source),
                "weight": args.weight,
                "originalCmap": len(original),
                "criticalCmap": len(critical_result),
                "deferredCmap": len(deferred_result),
                "criticalBytes": critical_output.stat().st_size,
                "deferredBytes": deferred_output.stat().st_size,
                "criticalSha256": sha256(critical_output),
                "deferredSha256": sha256(deferred_output),
                "criticalOutput": str(critical_output),
                "deferredOutput": str(deferred_output),
            },
            ensure_ascii=False,
            separators=(",", ":"),
        )
    )


if __name__ == "__main__":
    main()
