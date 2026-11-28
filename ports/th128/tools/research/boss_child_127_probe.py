#!/usr/bin/env python3
"""Extract the real bullet.anm S127/S126/S121 topology evidence (read-only)."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GENERATION = ROOT / "local/generations/8c71c316a4b24e68827f9fc560242050/ja"
METADATA = GENERATION / "animations/bullet.json"
BULLET = GENERATION / "raw/bullet.anm"
ASCII = GENERATION / "raw/ascii.anm"
OUTPUT = ROOT / "local/research/boss-child-127-fixtures.json"


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def command(command):
    return {
        "time": command["time"],
        "opcode": command["opcode"],
        "mask": command["mask"],
        "raw": command["raw"],
    }


def script(metadata, index):
    value = metadata["scripts"][index]
    return {
        "index": value["index"],
        "entry": value.get("entry"),
        "offset": value.get("offset"),
        "commands": [command(item) for item in value["commands"]],
    }


def main():
    metadata = json.loads(METADATA.read_text(encoding="utf-8"))
    scripts = {index: script(metadata, index) for index in (127, 126, 121)}
    bullet_bytes = BULLET.read_bytes()
    script_offset = metadata["scripts"][127]["offset"]
    header = bytes.fromhex("58000c0000000000")
    header_at = bullet_bytes.find(header, script_offset, script_offset + 128)
    child126_offset = header_at + 8 if header_at >= 0 else -1
    if child126_offset < 0 or bullet_bytes[child126_offset:child126_offset + 4] != (126).to_bytes(4, "little"):
        raise SystemExit("S127 opcode88 child126 offset was not found")
    fixture = {
        "schema": "th128-boss-child-127-fixtures/1",
        "source": {
            "metadata": str(METADATA.relative_to(ROOT)).replace("\\", "/"),
            "bullet": str(BULLET.relative_to(ROOT)).replace("\\", "/"),
            "bulletSha256": sha256(BULLET),
            "ascii": str(ASCII.relative_to(ROOT)).replace("\\", "/"),
            "asciiSha256": sha256(ASCII),
            "child126Op88FileOffset": child126_offset,
        },
        "root127": {
            "script": scripts[127],
            "spawns": [{"time": 0, "child": 126}, {"time": 1, "child": 121}],
            "end": 62,
            "noSprite": True,
        },
        "child126": {
            "script": scripts[126],
            "layer": 16,
            "mode": 16,
            "geometryCount": 36,
            "sprite": 313,
            "spriteBank": 11,
            "end": 40,
        },
        "child121": {
            "script": scripts[121],
            "layer": 16,
            "mode": 1,
            "sprite": 452,
            "spriteBank": 0,
            "end": 40,
        },
        "ascii313": metadata["sprites"][313],
        "notes": [
            "S127 op88 t0->126 and t1->121 with flags0 detached0 offset0 bank0.",
            "S126 never spawns; S121 never spawns; only one attached layer is supported.",
            "S126 op3=-1 maps through runtime.whiteSpriteBank=11 and ASCII313; it is not ignored.",
        ],
    }
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(json.dumps(fixture, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(OUTPUT)


if __name__ == "__main__":
    main()
