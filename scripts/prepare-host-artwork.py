"""Prepare publication-sensitive Launcher artwork for one host assembly.

The source repository and Runtime Release intentionally do not own original
Touhou artwork.  A host build materializes the small UI-facing subset from the
deployer's original game directories, while an optional override directory can
replace any normalized output file with custom artwork.
"""

from __future__ import annotations

from argparse import ArgumentParser
from hashlib import sha256
from io import BytesIO
import json
from pathlib import Path
import shutil
import subprocess
import tempfile

from PIL import Image

from touhou_formats import extract_pbg3_entry, extract_pbg4_entry, extract_pbgz_entry, extract_pe_icon


ARTWORK_BY_GAME = {
    "th06": ("th06-card.webp", "th06.ico"),
    "th07": ("th07-card.webp",),
    "th08": ("th08-card.webp",),
    "th10": ("th10-card.webp",),
}
# Launcher cards are presentation derivatives, not archival copies of the
# original title artwork.  The UI darkens/crops them heavily and Lighthouse's
# public-site audit consistently identifies the generated cards as the largest
# avoidable first-load image cost.  Keep method=6 for the best encoder search,
# but use a web-facing quality target rather than the previous near-source
# setting.  Original game bytes are never rewritten.
WEBP_QUALITY = 55
WEBP_METHOD = 6


def _validate_artwork(name: str, data: bytes) -> None:
    if name.endswith(".webp"):
        if len(data) < 16 or data[:4] != b"RIFF" or data[8:12] != b"WEBP":
            raise ValueError(f"{name}: expected a WebP file")
    elif name.endswith(".ico"):
        if len(data) < 22 or data[:4] != b"\x00\x00\x01\x00":
            raise ValueError(f"{name}: expected a Windows ICO file")
        count = int.from_bytes(data[4:6], "little")
        if count <= 0 or len(data) < 6 + count * 16:
            raise ValueError(f"{name}: invalid Windows ICO directory")
    else:
        raise ValueError(f"unsupported host artwork output: {name}")


def _encode_webp(source: bytes, label: str) -> tuple[bytes, tuple[int, int]]:
    try:
        with Image.open(BytesIO(source)) as image:
            image.load()
            if image.width <= 0 or image.height <= 0:
                raise ValueError(f"{label}: invalid image dimensions")
            has_alpha = "A" in image.getbands()
            normalized = image.convert("RGBA" if has_alpha else "RGB")
            output = BytesIO()
            normalized.save(
                output,
                format="WEBP",
                quality=WEBP_QUALITY,
                method=WEBP_METHOD,
            )
            data = output.getvalue()
            _validate_artwork(f"{label}.webp", data)
            return data, normalized.size
    except OSError as error:
        raise ValueError(f"{label}: unsupported or corrupt image") from error


def _find_th06_executable(root: Path) -> Path:
    for name in ("th06.exe", "東方紅魔郷.exe", "紅魔郷.exe"):
        candidate = root / name
        if candidate.is_file():
            return candidate
    raise FileNotFoundError(
        f"TH06 icon extraction requires th06.exe/東方紅魔郷.exe under {root}; "
        "provide a custom th06.ico override if the executable is intentionally absent"
    )


def _extract_th06_title(root: Path) -> bytes:
    archive = root / "紅魔郷TL.DAT"
    if not archive.is_file():
        raise FileNotFoundError(f"TH06 title archive not found: {archive}")
    return extract_pbg3_entry(archive, "title00.jpg")


def _extract_th10_title(root: Path, thdat: str, thanm: str) -> bytes:
    # The retail title.anm places sprite 73 at (0,0), sprite 74 at (512,0).
    # Reuse thtk's maintained THA1/ANM readers rather than another archive codec.
    archive = (root / "th10.dat").resolve()
    with tempfile.TemporaryDirectory(prefix="eagler-th10-artwork-") as directory:
        def run(arguments: list[str]) -> None:
            result = subprocess.run(arguments, cwd=directory, capture_output=True,
                                    creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
            if result.returncode:
                raise ValueError(result.stderr.decode("utf-8", errors="replace"))
        run([thdat, "-x", "10", str(archive), "title.anm"])
        run([thanm, "-x", "title.anm", "title/title00a.png", "title/title00b.png"])
        with Image.open(Path(directory) / "title/title00a.png") as left, Image.open(Path(directory) / "title/title00b.png") as right:
            if left.size != (512, 480) or right.size != (128, 480):
                raise ValueError("Unexpected TH10 title texture dimensions")
            combined = Image.new("RGB", (640, 480))
            combined.paste(left.convert("RGB"), (0, 0))
            combined.paste(right.convert("RGB"), (512, 0))
            output = BytesIO()
            combined.save(output, format="PNG")
            return output.getvalue()


def _default_artwork(game: str, name: str, roots: dict[str, Path], thdat: str = "thdat", thanm: str = "thanm") -> bytes:
    root = roots.get(game)
    if root is None:
        raise FileNotFoundError(
            f"{game}: original directory is required unless every host artwork file is overridden"
        )
    if name == "th06.ico":
        return extract_pe_icon(_find_th06_executable(root))
    if name == "th06-card.webp":
        return _extract_th06_title(root)
    if name == "th07-card.webp":
        return extract_pbg4_entry(root / "th07.dat", "title00.jpg")
    if name == "th08-card.webp":
        return extract_pbgz_entry(root / "th08.dat", "title/title00.png")
    if name == "th10-card.webp":
        return _extract_th10_title(root, thdat, thanm)
    raise AssertionError(name)


def _card_override(override_dir: Path | None, game: str) -> tuple[Path | None, bool]:
    if override_dir is None:
        return None, False
    direct = override_dir / f"{game}-card.webp"
    if direct.is_file():
        return direct, True
    for suffix in (".png", ".jpg", ".jpeg", ".webp"):
        candidate = override_dir / f"{game}-card{suffix}"
        if candidate.is_file():
            return candidate, suffix == ".webp"
    return None, False


def prepare_host_artwork(
    output: Path,
    games: list[str],
    roots: dict[str, Path],
    override_dir: Path | None = None,
    thdat: str = "thdat",
    thanm: str = "thanm",
) -> dict[str, object]:
    output = output.resolve()
    if output.exists():
        shutil.rmtree(output)
    output.mkdir(parents=True)
    entries: list[dict[str, object]] = []
    for game in games:
        for name in ARTWORK_BY_GAME[game]:
            if name.endswith(".webp"):
                override, direct_webp = _card_override(override_dir, game)
            else:
                override = override_dir / name if override_dir else None
                direct_webp = False
            dimensions: tuple[int, int] | None = None
            source_bytes = None
            if override is not None and override.is_file():
                raw = override.read_bytes()
                if direct_webp:
                    _validate_artwork(name, raw)
                    # Decode once as validation while preserving caller bytes.
                    try:
                        with Image.open(BytesIO(raw)) as image:
                            image.load()
                            dimensions = image.size
                    except OSError as error:
                        raise ValueError(f"{override}: invalid WebP override") from error
                    data = raw
                elif name.endswith(".webp"):
                    data, dimensions = _encode_webp(raw, override.name)
                else:
                    data = raw
                    _validate_artwork(name, data)
                source = "override"
                source_bytes = len(raw)
            else:
                try:
                    raw = _default_artwork(game, name, roots, thdat, thanm)
                    if name.endswith(".webp"):
                        data, dimensions = _encode_webp(raw, f"{game}-title00")
                    else:
                        data = raw
                        _validate_artwork(name, data)
                    source = "original-extraction"
                    source_bytes = len(raw)
                except (FileNotFoundError, ValueError, EOFError, OSError) as error:
                    entries.append({
                        "game": game,
                        "file": name,
                        "source": "unavailable",
                        "reason": str(error),
                    })
                    continue
            (output / name).write_bytes(data)
            entry: dict[str, object] = {
                "game": game,
                "file": name,
                "source": source,
                "bytes": len(data),
                "sha256": sha256(data).hexdigest(),
            }
            if source_bytes is not None:
                entry["sourceBytes"] = source_bytes
            if dimensions is not None:
                entry["width"], entry["height"] = dimensions
            entries.append(entry)
    report = {"format": "eagler-touhou/host-artwork/1", "games": games, "files": entries}
    (output / "host-artwork.json").write_text(json.dumps(report, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    return report


def _main() -> None:
    parser = ArgumentParser(description="Prepare extracted/custom Launcher artwork for host assembly")
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--games", required=True, help="comma-separated subset of th06,th07,th08,th10")
    parser.add_argument("--th06-dir", type=Path)
    parser.add_argument("--th07-dir", type=Path)
    parser.add_argument("--th08-dir", type=Path)
    parser.add_argument("--th10-dir", type=Path)
    parser.add_argument("--override-dir", type=Path)
    parser.add_argument("--thdat", default="thdat")
    parser.add_argument("--thanm", default="thanm")
    args = parser.parse_args()
    games = [value.strip().lower() for value in args.games.split(",") if value.strip()]
    if not games or len(games) != len(set(games)) or any(game not in ARTWORK_BY_GAME for game in games):
        parser.error("--games must be a non-empty unique subset of th06,th07,th08,th10")
    roots = {
        game: getattr(args, f"{game}_dir").resolve()
        for game in games
        if getattr(args, f"{game}_dir") is not None
    }
    if args.override_dir is not None and not args.override_dir.is_dir():
        parser.error(f"override directory not found: {args.override_dir}")
    report = prepare_host_artwork(
        args.output,
        games,
        roots,
        args.override_dir.resolve() if args.override_dir else None,
        args.thdat,
        args.thanm,
    )
    print(json.dumps(report, ensure_ascii=False))


if __name__ == "__main__":
    _main()
