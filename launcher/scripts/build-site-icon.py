"""Build site icon derivatives and the standalone brand initializer."""

from pathlib import Path
from PIL import Image


def main() -> None:
    root = Path(__file__).resolve().parent.parent
    image = Image.open(root / "public/assets/brand/portrait.png").convert("RGBA")
    image.resize((256, 256), Image.Resampling.LANCZOS).save(root / "public/assets/favicon.png")
    image.save(root / "favicon.ico", format="ICO", sizes=[(16,16),(24,24),(32,32),(48,48),(64,64),(128,128),(256,256)])
    image.save(root / "docs/assets/eagler-touhou-icon.png")
    (root / "docs/assets/eagler-touhou-wordmark.svg").write_bytes((root / "public/assets/brand/face-F02B.svg").read_bytes())


if __name__ == "__main__":
    main()
