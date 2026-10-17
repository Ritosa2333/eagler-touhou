"""Verify that the installed th20.exe is the analysis image this repo pins.

The repo contract pins the oracle by whole-file SHA-256 (reports/source_manifest.json).
Some installations ship the same 1.00c build with non-code bytes rewritten
(patch/repack), which changes the file hash without changing any instruction.
This tool reports both facts separately so code-level oracle comparisons are
never justified by an assumption:

  * whole-file SHA-256 match against the manifest
  * byte-for-byte match of every opcode byte in analysis/binary/disassembly.asm
    (the disassembled code range of the pinned image) against the installed EXE

Usage:
    python tools/oracle_code_check.py [PATH_TO_th20.exe] [--json OUT.json]

Exit code 0 when the code range matches; 1 otherwise. A whole-file mismatch is
reported but does not by itself fail the run, because the disassembly is the
authority for the recovered instructions.
"""
import argparse
import hashlib
import json
import re
import struct
import sys
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
DEFAULT_EXE = Path(r"C:\Program Files (x86)\上海アリス幻樂団\東方錦上京\th20.exe")
DISASSEMBLY = REPO / "analysis" / "binary" / "disassembly.asm"
MANIFEST = REPO / "reports" / "source_manifest.json"

LINE = re.compile(r"^([0-9a-fA-F]{8})\s+((?:[0-9a-fA-F]{2} )+)")


def manifest_sha() -> str:
    data = json.loads(MANIFEST.read_text(encoding="utf-8"))
    for entry in data["files"]:
        if entry["name"] == "th20.exe":
            return entry["sha256"]
    raise SystemExit("th20.exe missing from reports/source_manifest.json")


def load_pe(path: Path):
    data = path.read_bytes()
    e_lfanew = struct.unpack_from("<I", data, 0x3C)[0]
    if data[e_lfanew:e_lfanew + 4] != b"PE\0\0":
        raise SystemExit(f"{path} is not a PE image")
    coff = e_lfanew + 4
    nsec, = struct.unpack_from("<H", data, coff + 2)
    opt_size, = struct.unpack_from("<H", data, coff + 16)
    opt = coff + 20
    magic, = struct.unpack_from("<H", data, opt)
    image_base = (struct.unpack_from("<I", data, opt + 28)[0] if magic == 0x10B
                  else struct.unpack_from("<Q", data, opt + 24)[0])
    sections = []
    sec = opt + opt_size
    for i in range(nsec):
        off = sec + i * 40
        name = data[off:off + 8].rstrip(b"\0").decode("latin1")
        vsize, vaddr, rawsize, rawptr = struct.unpack_from("<IIII", data, off + 8)
        sections.append((name, vaddr, vsize, rawptr, rawsize))
    return data, image_base, sections


def va_byte(data, sections, image_base, va):
    rva = va - image_base
    for _name, vaddr, vsize, rawptr, rawsize in sections:
        if vaddr <= rva < vaddr + max(vsize, rawsize):
            return data[rawptr + (rva - vaddr)]
    raise KeyError(hex(va))


def reference_bytes(path: Path):
    """Return {VA: opcode byte} for every byte printed in the listing."""
    out = {}
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = LINE.match(line)
        if not m:
            continue
        va = int(m.group(1), 16)
        for i, token in enumerate(m.group(2).split()):
            out[va + i] = int(token, 16)
    if not out:
        raise SystemExit(f"No opcode bytes parsed from {path}")
    return out


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("exe", nargs="?", default=str(DEFAULT_EXE))
    ap.add_argument("--json")
    args = ap.parse_args()

    exe = Path(args.exe)
    if not exe.is_file():
        print(f"missing oracle image: {exe}")
        return 2

    data, image_base, sections = load_pe(exe)
    expected_sha = manifest_sha()
    actual_sha = hashlib.sha256(data).hexdigest()

    ref = reference_bytes(DISASSEMBLY)
    lo, hi = min(ref), max(ref)
    diff = 0
    first = None
    for va in range(lo, hi + 1):
        try:
            actual = va_byte(data, sections, image_base, va)
        except KeyError:
            continue
        if ref[va] != actual:
            diff += 1
            if first is None:
                first = (va, ref[va], actual)

    report = {
        "exe": str(exe),
        "image_base": hex(image_base),
        "file_sha256": actual_sha,
        "manifest_sha256": expected_sha,
        "whole_file_matches_manifest": actual_sha == expected_sha,
        "code_range": [hex(lo), hex(hi)],
        "code_bytes_compared": sum(1 for va in range(lo, hi + 1) if va in ref),
        "code_bytes_differing": diff,
        "first_code_difference": None if first is None else
            {"va": hex(first[0]), "reference": first[1], "installed": first[2]},
    }
    print(json.dumps(report, indent=2))
    if args.json:
        Path(args.json).write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    return 0 if diff == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
