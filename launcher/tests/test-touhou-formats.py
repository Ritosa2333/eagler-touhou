#!/usr/bin/env python3

from hashlib import sha256
import sys


from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
sys.path.insert(0, str(ROOT / "tests" / "support"))
sys.path.insert(0, str(ROOT / "scripts"))

from touhou_formats import (
    decode_rgba_png,
    extract_pbg3_entry,
    extract_pbg4_entry,
    extract_pbgz_entry,
    extract_th07_png,
)
from workspace_layout import workspace_path


th06_archive = workspace_path("th06", "assets", "紅魔郷TL.DAT")
th07_archive = workspace_path("th07", "assets", "th07.dat")

title06 = extract_pbg3_entry(th06_archive, "title00.jpg")
assert len(title06) == 106331
assert sha256(title06).hexdigest() == "1dbf9df8dd4520b48e19d374cdcfb55a003a95f36363bc982cba5315a0ddb90c"

new07 = extract_pbg4_entry(th07_archive, "thbgm.fmt")
assert len(new07) == 1057
assert sha256(new07).hexdigest() == "c5de4bacc59810c66cff4a8f6bd00eaf9d62512070401b911bd0e18a70e22c4f"
assert callable(extract_pbgz_entry)

hitbox = extract_th07_png(th07_archive, "etama.anm", "data/etama/etama2.png")
width, height, rgba = decode_rgba_png(hitbox)
assert (width, height) == (256, 256)
assert len(rgba) == 256 * 256 * 4
assert sha256(rgba).hexdigest() == "f9c0423b7ddd516c36a85f3e9f31694c17a75b159080e5edd19beaef4c538a97"

print("Touhou pure-Python formats: PASS")
