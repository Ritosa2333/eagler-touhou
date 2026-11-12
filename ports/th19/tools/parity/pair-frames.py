"""Pair real Web/native screenshots at identical simulated frames (requires Pillow)."""
import argparse
import hashlib
import json
from pathlib import Path
import struct

from PIL import Image, ImageDraw, ImageFont


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("web_capture", type=Path)
    parser.add_argument("native_capture", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--frames", nargs="+", type=int, required=True)
    parser.add_argument("--font", type=Path)
    args = parser.parse_args()
    report = json.loads((args.web_capture / "report.json").read_text(encoding="utf8"))
    native_trace = args.native_capture / "native-present.jsonl"
    if hashlib.sha256(native_trace.read_bytes()).hexdigest() != report["nativeTraceSha256"]:
        parser.error("Web report and original capture have different trace hashes")
    samples = {row["frame"]: row for row in report["samples"]}
    # Slot identities and vector order differ between the engines. Compare the
    # complete kind/color/coordinate multiset for each field, as check-web does.
    native_bullets = {}
    native_index = {(row.get("originalRound", 1) - 1, row.get("originalRoundFrame", frame)): frame
                    for frame, row in samples.items()}
    for line in native_trace.open(encoding="utf8"):
        original = json.loads(line)
        key = (original.get("segment"), original.get("frame"))
        if key in native_index and "bulletPositions" in original:
            native_bullets[native_index[key]] = original["bulletPositions"]
    bullet_differences = set()
    positions_path = args.web_capture / "web-bullet-positions.jsonl"
    if positions_path.is_file():
        normalize = lambda fields: [sorted(tuple(value & 0xffffffff for value in bullet) for bullet in field)
                                    for field in fields]
        for line in positions_path.open(encoding="utf8"):
            web = json.loads(line)
            frame = web["frame"]
            if frame in native_bullets and normalize(web["bulletPositions"]) != normalize(native_bullets[frame]):
                bullet_differences.add(frame)
    font_path = args.font or next((path for path in [
        Path("C:/Windows/Fonts/msyh.ttc"),
        Path("/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc"),
    ] if path.is_file()), None)
    if not font_path:
        parser.error("Supply a Chinese font with --font")
    title_font = ImageFont.truetype(str(font_path), 26)
    font = ImageFont.truetype(str(font_path), 20)
    args.output.mkdir(parents=True, exist_ok=True)
    evidence = []
    for frame in args.frames:
        row = samples[frame]
        left = args.web_capture / f"web-step-{frame}.png"
        native_round = row.get("originalRound", 1)
        native_frame = row.get("originalRoundFrame", frame)
        right = args.native_capture / f"native-segment-{native_round - 1}-frame-{native_frame}.bmp"
        if not left.is_file() or not right.is_file():
            parser.error(f"Missing frame {frame} image; recapture with --image-frames {frame}")
        errors = []
        for index, label in enumerate(["主 RNG", "动画 RNG"]):
            if row["animationRng"][index] != row["originalRng"][index]:
                errors.append(label)
        if row.get("controls") != row.get("originalControls"):
            errors.append("CPU 输入")
        if row.get("positionBits") != row.get("originalPositionBits"):
            errors.append("自机位置")
        if frame in bullet_differences:
            errors.append("敌弹状态")
        status = "、".join(errors) + "不一致" if errors else "所列状态一致"
        canvas = Image.new("RGB", (1944, 920), "#131925")
        draw = ImageDraw.Draw(canvas)
        frame_label = f"累计 {frame} 帧 · 第 {native_round} 轮 {native_frame} 帧" if report.get("continuous") else f"第 {frame} 帧"
        names = {0: "灵梦", 1: "魔理沙"}
        characters = report["fixture"]["characters"]
        matchup = " vs ".join(names.get(character, str(character)) for character in characters)
        draw.text((16, 10), f"{matchup} CPU vs CPU · {frame_label} · {status}", font=title_font, fill="white")
        for side, (source, x, label, color) in enumerate([
            (left, 8, "左侧 网页版", "#80caff"), (right, 984, "右侧 原版", "#a3e3b2"),
        ]):
            draw.text((x + 8, 50), label, font=title_font, fill=color)
            rng = row["animationRng" if side == 0 else "originalRng"]
            controls = row["controls" if side == 0 else "originalControls"]
            bits = row["positionBits" if side == 0 else "originalPositionBits"]
            positions = [struct.unpack("<f", struct.pack("<I", value & 0xffffffff))[0] for value in bits]
            draw.text((x + 8, 90), f"主 RNG {rng[0]} · 副 RNG {rng[1]} · 输入 {controls}", font=font, fill="white")
            draw.text((x + 8, 122), f"自机坐标 {positions}", font=font, fill="white")
            with Image.open(source) as image:
                if image.width * 3 != image.height * 4:
                    parser.error(f"Expected a 4:3 game image: {source}")
                canvas.paste(image.convert("RGB").resize((960, 720), Image.Resampling.LANCZOS), (x, 168))
        different_frames = sum(
            sample["animationRng"] != sample["originalRng"]
            or (sample.get("originalControls") is not None and sample.get("controls") != sample["originalControls"])
            or (sample.get("originalPositionBits") is not None and sample.get("positionBits") != sample["originalPositionBits"])
            or sample["frame"] in bullet_differences
            for sample in report["samples"] if sample["frame"] <= frame
        )
        draw.text((16, 892), f"同一模拟帧 · 截至本帧累计状态差异 {different_frames} 帧 · 原始截图按 4:3 等比例显示", font=font, fill="#b2bfce")
        destination = args.output / f"frame-{frame}.png"
        canvas.save(destination)
        evidence.append({"frame": frame, "status": status, "differentFramesThroughCurrent": different_frames, "image": destination.name,
                         "webImageSha256": hashlib.sha256(left.read_bytes()).hexdigest(),
                         "nativeImageSha256": hashlib.sha256(right.read_bytes()).hexdigest()})
    (args.output / "pairs.json").write_text(json.dumps({
        "wasmSha256": report["wasmSha256"], "nativeTraceSha256": report["nativeTraceSha256"],
        "pairs": evidence,
    }, indent=2, ensure_ascii=False) + "\n", encoding="utf8")


if __name__ == "__main__":
    main()
