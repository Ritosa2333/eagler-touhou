"""Render the touhou98 yin-yang glyph into the bundled site ICO and preview."""

from pathlib import Path
from tempfile import TemporaryDirectory

from fontTools.ttLib import TTFont
from PIL import Image, ImageDraw, ImageFont


def main() -> None:
    root = Path(__file__).resolve().parent.parent
    with TemporaryDirectory() as temporary:
        font = TTFont(root / "public/assets/fonts/touhou98.woff2")
        if 0x262F not in font.getBestCmap():
            raise ValueError("touhou98 has no yin-yang glyph")
        font.flavor = None
        ttf = Path(temporary) / "touhou98.ttf"
        font.save(ttf)
        face = ImageFont.truetype(str(ttf), 224)
        left, top, right, bottom = face.getbbox("☯")
        image = Image.new("RGBA", (256, 256), (255, 255, 255, 0))
        draw = ImageDraw.Draw(image)
        draw.text(((256 - right + left) // 2 - left,
                   (256 - bottom + top) // 2 - top), "☯", font=face, fill="white")
        image.save(root / "favicon.ico", format="ICO",
                   sizes=[(16, 16), (24, 24), (32, 32), (48, 48),
                          (64, 64), (128, 128), (256, 256)])
        image.save(root / "docs/assets/eagler-touhou-icon.png")


if __name__ == "__main__":
    main()
