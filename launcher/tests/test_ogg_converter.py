"""L3/module checks for the host-owned OGG conversion metadata/parser.

No audio encoding is performed and ``soundfile`` is intentionally not needed.
The expensive full-track baseline remains a release/integration gate.
"""

from __future__ import annotations

import importlib.util
from hashlib import sha256
from pathlib import Path
import sys
from tempfile import TemporaryDirectory


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
SPEC = importlib.util.spec_from_file_location("eagler_ogg_converter", ROOT / "scripts" / "convert_bgm_ogg.py")
assert SPEC and SPEC.loader
module = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(module)


for game, minimum_tracks in (("th06", 17), ("th07", 20), ("th08", 21)):
    baseline = module._load_baseline(ROOT / "host" / "ogg-baselines" / f"{game}.json", game)
    assert baseline["quality"] == 0.55
    assert len(baseline["files"]) >= minimum_tracks
    for name, identity in baseline["files"].items():
        assert name.endswith(".ogg")
        assert identity["bytes"] > 0
        assert len(identity["sha256"]) == 64
        assert int(identity["serial"], 0) >= 0


entry = module.BGM_FORMAT.pack(
    b"th07_01.wav\0\0\0\0\0",
    400,
    0,
    800,
    4000,
    1,
    2,
    44100,
    176400,
    4,
    16,
    0,
)
terminator = bytes(module.BGM_FORMAT.size)
parsed = list(module._iter_bgm_entries(entry + terminator))
assert parsed == [("th07_01.wav", 400, 800, 4000)]

with TemporaryDirectory(prefix="eagler-ogg-cache-") as temporary:
    cached = Path(temporary) / "cached.ogg"
    payload = b"verified-cache-fixture"
    cached.write_bytes(payload)
    expected = {"bytes": len(payload), "sha256": sha256(payload).hexdigest()}
    assert module._matches_expected_output(cached, expected)
    cached.write_bytes(payload + b"!")
    assert not module._matches_expected_output(cached, expected)

print("Host OGG converter metadata/parser: PASS")
