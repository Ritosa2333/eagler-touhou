"""Download original PC-98 packaging scans and prepare local Launcher cards."""
from argparse import ArgumentParser
from datetime import datetime, timezone
from hashlib import sha256
from io import BytesIO
import json
from pathlib import Path
from urllib.parse import quote
from urllib.request import Request, urlopen

from PIL import Image, ImageOps


ROOT = Path(__file__).resolve().parents[2]
# Full resolution packaging scans, identified through THBWiki's imageinfo API.
SOURCES = {
    "th01": ("东方灵异传", "1/13", (1499, 2134), 5082489),
    "th02": ("东方封魔录", "5/55", (1525, 2136), 5539381),
    "th03": ("东方梦时空", "5/5d", (1522, 2137), 5936208),
    "th04": ("东方幻想乡", "f/fd", (2156, 1485), 5100288),
}


def main():
    parser = ArgumentParser(description=__doc__)
    parser.add_argument("--games", default=",".join(SOURCES))
    parser.add_argument("--output", type=Path, default=ROOT / "games/host-artwork")
    parser.add_argument("--download-only", action="store_true")
    args = parser.parse_args()
    games = args.games.split(",")
    if not games or len(set(games)) != len(games) or any(g not in SOURCES for g in games):
        parser.error("Choose th01,th02,th03,th04")
    originals = args.output / "originals"
    originals.mkdir(parents=True, exist_ok=True)
    for game in games:
        title, upload_path, dimensions, expected_bytes = SOURCES[game]
        filename = title + "cover.png"
        url = f"https://upload.thbwiki.cc/{upload_path}/{quote(filename)}"
        source = originals / f"{game}-cover.png"
        if source.exists():
            data = source.read_bytes()
        else:
            request = Request(url, headers={"User-Agent": "EaglerTouhou-LocalArtwork/1.0"})
            with urlopen(request, timeout=90) as response:
                data = response.read(32 * 1024 * 1024)
        if len(data) != expected_bytes:
            raise ValueError(f"{game}: source size changed; inspect the new scan before publishing")
        with Image.open(BytesIO(data)) as image:
            image.load()
            if image.format != "PNG" or image.size != dimensions:
                raise ValueError(f"{game}: unexpected scan dimensions or format")
            source.write_bytes(data)
            record = {
                "game": game, "kind": "original-packaging-scan", "provider": "THBWiki",
                "filePage": "https://thwiki.cc/" + quote("文件:" + filename),
                "url": url, "sourceFile": source.name, "width": image.width,
                "height": image.height, "bytes": len(data), "sha256": sha256(data).hexdigest(),
                "preparedAt": datetime.now(timezone.utc).isoformat(),
            }
            if not args.download_only:
                card = ImageOps.exif_transpose(image).convert("RGB")
                card.thumbnail((960, 960), Image.Resampling.LANCZOS)
                encoded = BytesIO()
                card.save(encoded, "WEBP", quality=88, method=6)
                card_bytes = encoded.getvalue()
                record["card"] = {"file": f"{game}-card.webp", "width": card.width,
                                  "height": card.height, "sha256": sha256(card_bytes).hexdigest()}
                (args.output / f"{game}-card.webp").write_bytes(card_bytes)
                if game in SOURCES:
                    port_artwork = ROOT / "ports" / game / "artwork"
                    port_artwork.mkdir(parents=True, exist_ok=True)
                    (port_artwork / f"{game}-card.webp").write_bytes(card_bytes)
                    (port_artwork / "cover-source.json").write_text(
                        json.dumps(record, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            (originals / f"{game}-source.json").write_text(
                json.dumps(record, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
            print(json.dumps(record, ensure_ascii=True), flush=True)


if __name__ == "__main__":
    main()
