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
import math
from pathlib import Path
import shutil
import struct
import subprocess
import tempfile

from PIL import Image

from touhou_formats import extract_pbg3_entry, extract_pbg4_entry, extract_pbgz_entry


ARTWORK_BY_GAME = {
    "th06": (
        "th06-card.webp",
        "favicon.ico",
        "pwa/icon-192.png",
        "pwa/icon-512.png",
        "pwa/icon-maskable-512.png",
        "pwa/apple-touch-icon.png",
    ),
    "th07": ("th07-card.webp",),
    "th075": ("th075-card.webp",),
    "th08": ("th08-card.webp",),
    "th09": ("th09-card.webp",),
    "th10": ("th10-card.webp",),
    # TH20's archive has no reader here. Its card must be supplied by the host
    # through --override-dir, like the TH11 card.
    "th20": ("th20-card.webp",),
    # TH11's archive has no reader here. Use an explicit, locally supplied card
    # override (for example its existing Launcher th11-card.png) for publication.
    "th11": ("th11-card.webp",),
    # TH15's title is layered artwork in the retail title.anm, not a screenshot.
    "th15": ("th15-card.webp",),
    "th12": ("th12-card.webp",),
    "th13": ("th13-card.webp",),
}
# Launcher cards are presentation derivatives, not archival copies of the
# original title artwork.  The UI darkens/crops them heavily and Lighthouse's
# public-site audit consistently identifies the generated cards as the largest
# avoidable first-load image cost.  Keep method=6 for the best encoder search,
# but use a web-facing quality target rather than the previous near-source
# setting.  Original game bytes are never rewritten.
WEBP_QUALITY = 55
WEBP_METHOD = 6
SITE_ICON_SPECS = {
    "pwa/icon-192.png": (192, 1.0, None),
    "pwa/icon-512.png": (512, 1.0, None),
    "pwa/icon-maskable-512.png": (512, 0.70, (16, 16, 15, 255)),
    "pwa/apple-touch-icon.png": (180, 0.80, (16, 16, 15, 255)),
}


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
    elif name.endswith(".png"):
        if len(data) < 24 or data[:8] != b"\x89PNG\r\n\x1a\n":
            raise ValueError(f"{name}: expected a PNG file")
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


def _encode_site_icon(source: bytes, name: str) -> tuple[bytes, tuple[int, int]]:
    size, scale, background = SITE_ICON_SPECS[name]
    try:
        with Image.open(BytesIO(source)) as image:
            image.load()
            normalized = image.convert("RGBA")
            target_size = max(1, round(size * scale))
            resized = normalized.resize((target_size, target_size), Image.Resampling.NEAREST)
            canvas = Image.new("RGBA", (size, size), background or (0, 0, 0, 0))
            offset = ((size - target_size) // 2, (size - target_size) // 2)
            canvas.alpha_composite(resized, offset)
            output = BytesIO()
            canvas.save(output, format="PNG", optimize=True)
            data = output.getvalue()
            _validate_artwork(name, data)
            return data, canvas.size
    except OSError as error:
        raise ValueError(f"favicon.ico: unsupported or corrupt site icon") from error


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


def _extract_th15_title(root: Path, thdat: str, thanm: str) -> bytes:
    archive = (root / "th15.dat").resolve()
    textures = ["title/title_bk00.png", "title/title_ch00.png",
                "title/title_logob.png", "title/title_logo.png"]
    with tempfile.TemporaryDirectory(prefix="eagler-th15-artwork-") as directory:
        for arguments in ([thdat, "-x", "15", str(archive), "title.anm"],
                          [thanm, "-x", "title.anm", *textures]):
            result = subprocess.run(arguments, cwd=directory, capture_output=True,
                                    creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
            if result.returncode:
                raise ValueError(result.stderr.decode("utf-8", errors="replace"))
        def texture(name: str, size: tuple[int, int]) -> Image.Image:
            with Image.open(Path(directory) / "title" / name) as image:
                if image.size != size:
                    raise ValueError(f"Unexpected TH15 title texture dimensions: {name}")
                return image.convert("RGBA")
        combined = texture("title_bk00.png", (1280, 960))
        # thanm merges the background tiles and preserves archive image offsets.
        # Crop the logo padding before placing sprites at their settled title
        # positions (scripts 94/95). Only artwork layers, never menu/copyright UI.
        crest = texture("title_logob.png", (1194, 838)).crop((414, 58, 1194, 838))
        logo = texture("title_logo.png", (1264, 812)).crop((664, 82, 1264, 812))
        combined.alpha_composite(crest, (458, 58))
        combined.alpha_composite(logo, (650, 85))
        combined.alpha_composite(texture("title_ch00.png", (820, 960)), (0, 0))
        output = BytesIO()
        combined.convert("RGB").save(output, format="PNG")
        return output.getvalue()


def _th13_texture_rgba(width: int, height: int, pixel_format: int, pixels: bytes) -> bytes:
    count = width * height
    if pixel_format == 1:
        if len(pixels) != count * 4:
            raise ValueError("TH13 BGRA8888 payload size mismatch")
        out = bytearray(count * 4)
        for index in range(count):
            b, g, r, a = pixels[index * 4:index * 4 + 4]
            out[index * 4:index * 4 + 4] = bytes((r, g, b, a))
        return bytes(out)
    if pixel_format == 4:
        if len(pixels) != count * 3:
            raise ValueError("TH13 BGR888 payload size mismatch")
        out = bytearray(count * 4)
        for index in range(count):
            b, g, r = pixels[index * 3:index * 3 + 3]
            out[index * 4:index * 4 + 4] = bytes((r, g, b, 255))
        return bytes(out)
    if pixel_format in (2, 3, 5, 6):
        if len(pixels) != count * 2:
            raise ValueError("TH13 16-bit texture payload size mismatch")
        out = bytearray(count * 4)
        for index in range(count):
            value = pixels[index * 2] | (pixels[index * 2 + 1] << 8)
            if pixel_format == 5:
                out[index * 4:index * 4 + 4] = bytes((
                    ((value >> 8) & 15) * 17, ((value >> 4) & 15) * 17,
                    (value & 15) * 17, ((value >> 12) & 15) * 17))
            elif pixel_format == 2:
                out[index * 4:index * 4 + 4] = bytes((
                    (((value >> 10) & 31) * 255 + 15) // 31,
                    (((value >> 5) & 31) * 255 + 15) // 31,
                    ((value & 31) * 255 + 15) // 31,
                    255 if value & 0x8000 else 0))
            elif pixel_format == 6:
                out[index * 4:index * 4 + 4] = bytes((
                    (((value >> 5) & 7) * 255 + 3) // 7,
                    (((value >> 2) & 7) * 255 + 3) // 7,
                    (value & 3) * 85, (value >> 8) & 0xFF))
            else:
                out[index * 4:index * 4 + 4] = bytes((
                    (((value >> 11) & 31) * 255 + 15) // 31,
                    (((value >> 5) & 63) * 255 + 31) // 63,
                    ((value & 31) * 255 + 15) // 31, 255))
        return bytes(out)
    if pixel_format == 7:
        if len(pixels) != count:
            raise ValueError("TH13 A8 payload size mismatch")
        out = bytearray(count * 4)
        for index, alpha in enumerate(pixels):
            out[index * 4:index * 4 + 4] = bytes((255, 255, 255, alpha))
        return bytes(out)
    raise ValueError(f"unsupported TH13 texture format {pixel_format}")


def _iter_th13_anm_textures(anm: bytes):
    # ANM v8 entry layout used by TH13's AnmResource.cpp. TH13 title.anm has
    # two distinct title/title00a.png entries (512x512 at source_x=0 and
    # 128x512 at source_x=512), so a name-only thanm extraction is ambiguous.
    base = 0
    while base < len(anm):
        if base + 64 > len(anm):
            raise ValueError("truncated TH13 ANM header")
        magic, sprites, scripts = struct.unpack_from("<IHH", anm, base)
        if magic != 8:
            raise ValueError(f"unsupported TH13 ANM version {magic}")
        stride = struct.unpack_from("<I", anm, base + 36)[0]
        chunk_size = stride or len(anm) - base
        table_end = 64 + sprites * 4 + scripts * 8
        if chunk_size < table_end or chunk_size > len(anm) - base:
            raise ValueError("invalid TH13 ANM entry bounds")
        def within(offset: int, size: int) -> bool:
            return table_end <= offset <= chunk_size and 0 <= size <= chunk_size - offset
        name_offset = struct.unpack_from("<I", anm, base + 16)[0]
        if not within(name_offset, 1):
            raise ValueError("invalid TH13 ANM texture name offset")
        end = anm.find(b"\0", base + name_offset, base + chunk_size)
        if end < 0:
            raise ValueError("unterminated TH13 ANM texture name")
        name = anm[base + name_offset:end].decode("ascii")
        source_x = struct.unpack_from("<h", anm, base + 20)[0]
        texture_offset = struct.unpack_from("<I", anm, base + 28)[0]
        has_data = anm[base + 32]
        if sprites:
            sprite_offset = struct.unpack_from("<I", anm, base + 64)[0]
            if not within(sprite_offset, 20):
                raise ValueError("invalid TH13 ANM sprite bounds")
            sprite_x, sprite_y, sprite_w, sprite_h = struct.unpack_from(
                "<ffff", anm, base + sprite_offset + 4)
            if not all(math.isfinite(value) for value in (sprite_x, sprite_y, sprite_w, sprite_h)):
                raise ValueError("invalid TH13 ANM sprite coordinates")
        else:
            sprite_x = sprite_y = sprite_w = sprite_h = 0.0
        if has_data:
            if not within(texture_offset, 16):
                raise ValueError("invalid TH13 THTX bounds")
            absolute = base + texture_offset
            if anm[absolute:absolute + 4] != b"THTX":
                raise ValueError("invalid TH13 THTX header")
            pixel_format, width, height, size = struct.unpack_from("<HHHI", anm, absolute + 6)
            if not width or not height or not within(texture_offset + 16, size):
                raise ValueError("invalid TH13 texture payload bounds")
            pixels = anm[absolute + 16:absolute + 16 + size]
            yield (name, source_x, (sprite_x, sprite_y, sprite_w, sprite_h),
                   width, height, pixel_format, pixels)
        if not stride:
            break
        base += stride


def _compose_th13_title(anm: bytes) -> bytes:
    layers = [(source_x, rect, width, height, pixel_format, pixels)
              for name, source_x, rect, width, height, pixel_format, pixels
              in _iter_th13_anm_textures(anm)
              if name.replace("\\", "/").lower() == "title/title00a.png"]
    if not layers:
        raise FileNotFoundError("TH13 title/title00a.png not found in title.anm")
    layers.sort(key=lambda layer: layer[0])
    expected = [(0, (0.0, 0.0, 512.0, 480.0), 512, 512),
                (512, (0.0, 0.0, 128.0, 480.0), 128, 512)]
    if [(x, rect, width, height) for x, rect, width, height, _, _ in layers] != expected:
        raise ValueError("Unexpected TH13 title texture layout")
    canvas = Image.new("RGB", (640, 480))
    for source_x, (sprite_x, sprite_y, sprite_w, sprite_h), width, height, pixel_format, pixels in layers:
        rgba = _th13_texture_rgba(width, height, pixel_format, pixels)
        with Image.frombytes("RGBA", (width, height), rgba) as image:
            crop = image.crop((int(sprite_x), int(sprite_y),
                               int(sprite_x + sprite_w), int(sprite_y + sprite_h))).convert("RGB")
            canvas.paste(crop, (int(source_x + sprite_x), int(sprite_y)))
    output = BytesIO()
    canvas.save(output, format="PNG")
    return output.getvalue()


def _extract_th13_title(root: Path, thdat: str, thanm: str) -> bytes:
    archive = (root / "th13.dat").resolve()
    with tempfile.TemporaryDirectory(prefix="eagler-th13-artwork-") as directory:
        result = subprocess.run([thdat, "-x", "13", str(archive), "title.anm"],
                                cwd=directory, capture_output=True,
                                creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
        if result.returncode:
            raise ValueError(result.stderr.decode("utf-8", errors="replace"))
        return _compose_th13_title((Path(directory) / "title.anm").read_bytes())


def _default_artwork(game: str, name: str, roots: dict[str, Path], thdat: str = "thdat", thanm: str = "thanm") -> bytes:
    if name == "favicon.ico":
        return (Path(__file__).resolve().parent.parent / "favicon.ico").read_bytes()
    root = roots.get(game)
    if root is None:
        raise FileNotFoundError(
            f"{game}: original directory is required unless every host artwork file is overridden"
        )
    if name == "th06-card.webp":
        return _extract_th06_title(root)
    if name == "th07-card.webp":
        return extract_pbg4_entry(root / "th07.dat", "title00.jpg")
    if name == "th08-card.webp":
        return extract_pbgz_entry(root / "th08.dat", "title/title00.png")
    if name == "th09-card.webp":
        return extract_pbgz_entry(root / "th09.dat", "title00.png")
    if name == "th10-card.webp":
        return _extract_th10_title(root, thdat, thanm)
    if name == "th075-card.webp":
        for candidate in (root / "th075-card.webp", root / "th075_web/launcher/public/assets/th075-card.png"):
            if candidate.is_file():
                return candidate.read_bytes()
        raise FileNotFoundError("Supply the TH075 development card or an artwork override")
    if name == "th15-card.webp":
        return _extract_th15_title(root, thdat, thanm)
    if name == "th13-card.webp":
        return _extract_th13_title(root, thdat, thanm)
    if name in ("th11-card.webp", "th20-card.webp"):
        raise FileNotFoundError(f"{game}: supply {name} through --override-dir; no retail artwork reader is available")
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
    prepared_artwork_dirs: dict[str, Path] | None = None,
) -> dict[str, object]:
    output = output.resolve()
    if output.exists():
        shutil.rmtree(output)
    output.mkdir(parents=True)
    entries: list[dict[str, object]] = []
    for game in games:
        for name in ARTWORK_BY_GAME[game]:
            derived_site_icon = name in SITE_ICON_SPECS
            if name.endswith(".webp"):
                override, direct_webp = _card_override(override_dir, game)
            elif derived_site_icon:
                override = override_dir / name if override_dir else None
                direct_webp = False
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
            elif derived_site_icon:
                icon_override = override_dir / "favicon.ico" if override_dir else None
                if icon_override is not None and icon_override.is_file():
                    raw = icon_override.read_bytes()
                    source = "override-derived"
                else:
                    raw = _default_artwork("th06", "favicon.ico", roots, thdat, thanm)
                    source = "site-brand-derived"
                _validate_artwork("favicon.ico", raw)
                data, dimensions = _encode_site_icon(raw, name)
                source_bytes = len(raw)
            else:
                try:
                    prepared_root = (prepared_artwork_dirs or {}).get(game)
                    raw = (prepared_root / name).read_bytes() if prepared_root is not None else _default_artwork(game, name, roots, thdat, thanm)
                    if prepared_root is not None and name.endswith(".webp"):
                        _validate_artwork(name, raw)
                        with Image.open(BytesIO(raw)) as image:
                            image.load()
                            dimensions = image.size
                        data = raw
                    elif name.endswith(".webp"):
                        data, dimensions = _encode_webp(raw, f"{game}-title00")
                    else:
                        data = raw
                        _validate_artwork(name, data)
                    source = "prepared-content" if prepared_root is not None else "site-brand" if name == "favicon.ico" else "original-extraction"
                    source_bytes = len(raw)
                except (FileNotFoundError, ValueError, EOFError, OSError) as error:
                    entries.append({
                        "game": game,
                        "file": name,
                        "source": "unavailable",
                        "reason": str(error),
                    })
                    continue
            target = output / name
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
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
    parser.add_argument("--list-games", action="store_true", help="print the registered artwork format-adapter games as JSON and exit")
    parser.add_argument("--output", type=Path)
    parser.add_argument("--games", help="comma-separated subset of registered artwork adapters")
    parser.add_argument(
        "--game-dir",
        action="append",
        default=[],
        metavar="GAME=DIR",
        help="original game directory; repeat once per selected game",
    )
    parser.add_argument("--override-dir", type=Path)
    parser.add_argument("--prepared-artwork-dir", action="append", default=[], metavar="GAME=DIR")
    parser.add_argument("--thdat", default="thdat")
    parser.add_argument("--thanm", default="thanm")
    args = parser.parse_args()
    if args.list_games:
        print(json.dumps({
            "games": list(ARTWORK_BY_GAME),
            "files": {game: list(files) for game, files in ARTWORK_BY_GAME.items()},
        }, ensure_ascii=False))
        return
    if args.output is None or not args.games:
        parser.error("--output and --games are required unless --list-games is used")
    games = [value.strip().lower() for value in args.games.split(",") if value.strip()]
    if not games or len(games) != len(set(games)) or any(game not in ARTWORK_BY_GAME for game in games):
        parser.error(f"--games must be a non-empty unique subset of {','.join(ARTWORK_BY_GAME)}")
    roots: dict[str, Path] = {}
    for value in args.game_dir:
        if "=" not in value:
            parser.error(f"--game-dir must use GAME=DIR: {value}")
        game, directory = value.split("=", 1)
        game = game.strip().lower()
        if game not in games:
            parser.error(f"--game-dir references an unselected or unknown game: {game}")
        if game in roots:
            parser.error(f"duplicate --game-dir for {game}")
        if not directory.strip():
            parser.error(f"--game-dir requires a directory for {game}")
        roots[game] = Path(directory).resolve()
    if args.override_dir is not None and not args.override_dir.is_dir():
        parser.error(f"override directory not found: {args.override_dir}")
    prepared_artwork_dirs: dict[str, Path] = {}
    for value in args.prepared_artwork_dir:
        game, separator, directory = value.partition("=")
        if not separator or game not in games or game in prepared_artwork_dirs or not directory.strip():
            parser.error(f"invalid or duplicate --prepared-artwork-dir: {value}")
        location = Path(directory).resolve()
        if not location.is_dir():
            parser.error(f"prepared artwork directory not found: {location}")
        prepared_artwork_dirs[game] = location
    report = prepare_host_artwork(
        args.output,
        games,
        roots,
        args.override_dir.resolve() if args.override_dir else None,
        args.thdat,
        args.thanm,
        prepared_artwork_dirs,
    )
    print(json.dumps(report, ensure_ascii=False))


if __name__ == "__main__":
    _main()
