import json
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw


def compare(original, candidate):
    if original.shape != candidate.shape:
        raise ValueError(f"Capture sizes differ: {original.shape} vs {candidate.shape}")
    difference = np.abs(original - candidate)
    changed = np.any(difference != 0, axis=2)
    y, x = np.where(changed)
    bounds = [int(x.min()), int(y.min()), int(x.max()) + 1, int(y.max()) + 1] if x.size else None
    return difference.astype(np.uint8), {
        "size": [original.shape[1], original.shape[0]],
        "changedPixels": int(x.size),
        "changedFraction": float(changed.mean()),
        "meanAbsoluteChannelDifference": float(difference.mean()),
        "maxChannelDifference": int(difference.max()),
        "bounds": bounds,
    }

if len(sys.argv) != 4:
    raise SystemExit("Usage: compare-frames.py <Web capture> <native capture> <output directory>")
run = Path(sys.argv[1])
original = Path(sys.argv[2])
output = Path(sys.argv[3])
output.mkdir(parents=True, exist_ok=True)
size = (960, 720)


def native_image(frame):
    base = original / f"native-segment-0-frame-{frame}"
    return next((base.with_suffix(suffix) for suffix in (".png", ".bmp") if base.with_suffix(suffix).is_file()), None)


frames = sorted(int(path.stem.removeprefix("web-step-")) for path in run.glob("web-step-*.png"))
frames = [frame for frame in frames if native_image(frame)]
if not frames:
    raise ValueError("No shared screenshot frames")
panel_frames = [frames[0], frames[-1]]
metrics = {}
preview = Image.new("RGB", (1440, 774), "#111827")
label = ImageDraw.Draw(preview)
for frame in frames:
    with Image.open(native_image(frame)) as source:
        before = source.convert("RGB")
    with Image.open(run / f"web-step-{frame}.png") as source:
        after = source.convert("RGB")
    assert before.size == size
    input_size = after.size
    after = after.resize(size, Image.Resampling.BOX)
    difference, stats = compare(np.asarray(before, dtype=np.int16), np.asarray(after, dtype=np.int16))
    diff = Image.fromarray(difference)
    diff.save(output / f"pixel-diff-{frame}.png")
    metrics[frame] = {"webCaptureSize": list(input_size), **stats}
    if frame in panel_frames:
        y = 0 if frame == panel_frames[0] else 387
        for x, image, title in zip((0,480,960), (before, after, diff), ("Original", "Web", "Absolute RGB difference")):
            label.text((x+10, y+7), f"Frame {frame}   |   {title}", fill="white")
            preview.paste(image.resize((480,360),Image.Resampling.BOX), (x,y+27))
preview.save(output / "opening-visual-comparison.png")
(output / "pixel-diff.json").write_text(json.dumps({
    "normalization": "Original 960x720, web resized to 960x720 using BOX",
    "frames": metrics
}, indent=2) + "\n", encoding="utf-8")
for frame, stats in metrics.items():
    print(frame, round(stats["changedFraction"]*100,2), round(stats["meanAbsoluteChannelDifference"],2))
