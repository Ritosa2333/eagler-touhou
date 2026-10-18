"""Read-only TH20 asset recovery using pinned, locally built Touhou Toolkit.

Example: python tools/asset_recover.py --game-dir E:/path/to/game
Every output is contained within the reconstruction project. The input game is
only opened for reading. Recovered scripts are game DSLs, not original C++.
"""
from __future__ import annotations
import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
from pathlib import Path, PureWindowsPath
import re
import subprocess
import time

ROOT = Path(__file__).resolve().parents[1]
PIN = "892114a0fcaa0bbdaaecf3cb4ad56f758683fb40"

def digest(path: Path) -> str:
    with path.open("rb") as f:
        return hashlib.file_digest(f, "sha256").hexdigest()

def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game-dir", type=Path, required=True)
    parser.add_argument("--skip-extract", action="store_true")
    parser.add_argument("--workers", type=int, default=4)
    args = parser.parse_args()
    game = args.game_dir.resolve(strict=True)
    archive = game / "th20.dat"
    raw = ROOT / "assets/raw"
    recovered = ROOT / "scripts/recovered"
    report = ROOT / "reports"
    logs = report / "assets_logs"
    for path in [raw, recovered, logs]:
        path.mkdir(parents=True, exist_ok=True)
    build = ROOT / "third_party/thtk/build"
    binaries = {tool: build / tool / "Release" / f"{tool}.exe"
                for tool in ["thdat", "thecl", "thanm", "thstd", "thmsg"]}
    binaries["asset_index"] = ROOT / "tools/asset_index/build/Release/asset_extract_index.exe"
    for tool, binary in binaries.items():
        if not binary.is_file():
            raise SystemExit(f"Missing {binary}; run tools/asset_build_thtk.ps1 first")
    records = []

    def run(tool: str, command: list, tag: str, cwd=ROOT, stdout_path=None):
        argv = [str(binaries[tool]), *map(str, command)]
        start = time.monotonic()
        proc = subprocess.run(argv, cwd=cwd, stdout=subprocess.PIPE,
                              stderr=subprocess.PIPE, timeout=300)
        (logs / f"{tag}.stderr.txt").write_bytes(proc.stderr)
        if stdout_path is not None:
            stdout_path.write_bytes(proc.stdout)
        elif proc.stdout:
            (logs / f"{tag}.stdout.txt").write_bytes(proc.stdout)
        record = {"tag": tag, "tool": tool, "argv": argv, "cwd": str(cwd),
                  "returncode": proc.returncode, "stderr_bytes": len(proc.stderr),
                  "seconds": round(time.monotonic() - start, 3)}
        records.append(record)
        return proc

    archive_before = digest(archive)
    listing = run("thdat", ["-l20", archive], "archive_list")
    if listing.returncode:
        raise SystemExit("Archive listing failed; see reports/assets_logs")
    listing_text = listing.stdout.decode("utf-8", errors="strict")
    index_listing = run("asset_index", [archive], "archive_index")
    if index_listing.returncode:
        raise SystemExit("Archive index listing failed")
    index_rows = [line.split("\t") for line in index_listing.stdout.decode("utf-8").splitlines()]
    entries = []
    for line in listing_text.splitlines()[1:]:
        match = re.match(r"^(.*?)\s+(\d+)\s+(\d+)\s*$", line)
        if not match:
            raise SystemExit(f"Unexpected archive listing: {line!r}")
        name, size, stored = match.groups()
        parsed = PureWindowsPath(name)
        if parsed.is_absolute() or parsed.drive or ".." in parsed.parts:
            raise SystemExit(f"Unsafe archive entry path: {name!r}")
        entries.append({"name": name, "bytes": int(size), "stored_bytes": int(stored)})
    if len(index_rows) != len(entries):
        raise SystemExit("Archive index/count disagreement")
    for entry, row in zip(entries, index_rows):
        index, name, offset, size, stored = row
        if (name, int(size), int(stored)) != (entry["name"], entry["bytes"], entry["stored_bytes"]):
            raise SystemExit("Archive listing/index metadata disagreement")
        entry["index"] = int(index)
        entry["archive_offset"] = int(offset)
    if not args.skip_extract:
        extraction = run("thdat", ["-x20", archive, "-C", raw], "archive_extract")
        if extraction.returncode:
            raise SystemExit("Archive extraction failed; see reports/assets_logs")
    name_counts = Counter(e["name"] for e in entries)
    for entry in entries:
        target = raw / entry["name"]
        if not target.is_file() or target.stat().st_size != entry["bytes"]:
            raise SystemExit(f"Missing/wrong-sized extraction: {target}")
        if name_counts[entry["name"]] > 1:
            duplicates = ROOT / "assets/duplicate_entries"
            duplicates.mkdir(exist_ok=True)
            target = duplicates / f"{entry['index']:04d}_{entry['name']}"
            extracted = run("asset_index", [archive, entry["index"], target], f"duplicate_index_{entry['index']}")
            if extracted.returncode or target.stat().st_size != entry["bytes"]:
                raise SystemExit("Duplicate archive index extraction failed")
            entry["independent_extraction"] = str(target.relative_to(ROOT))
        entry["sha256"] = digest(target)

    roundtrips = []
    def recover(path: Path):
        kind = path.suffix.lstrip(".").lower()
        dest = recovered / kind
        dest.mkdir(exist_ok=True)
        output = dest / (path.name + ".txt")
        if kind == "ecl":
            run("thecl", ["-jd20", path, output], path.name + "_decompile")
            raw_dest = recovered / "ecl_raw"
            raw_dest.mkdir(exist_ok=True)
            raw_output = raw_dest / (path.name + ".txt")
            proc = run("thecl", ["-jrd20", path, raw_output], path.name + "_raw")
            compiler = "thecl"
            compile_flags = "-jsc20"
        elif kind == "anm":
            listed = run("thanm", ["-uul20", path], path.name + "_decompile", stdout_path=output)
            texture_dir = ROOT / "assets/textures" / path.stem
            texture_dir.mkdir(parents=True, exist_ok=True)
            extracted = run("thanm", ["-uux20", path], path.name + "_textures", cwd=texture_dir)
            if listed.returncode == 0 and extracted.returncode == 0:
                rt_dir = recovered / "roundtrip/anm"
                rt_dir.mkdir(parents=True, exist_ok=True)
                rebuilt = rt_dir / path.name
                compiled = run("thanm", ["-uuc20", rebuilt, output], path.name + "_roundtrip", cwd=texture_dir)
                info = {"name": path.name, "kind": kind, "compiled": compiled.returncode == 0,
                        "original_sha256": digest(path)}
                if compiled.returncode == 0:
                    info["rebuilt_sha256"] = digest(rebuilt)
                    info["byte_identical"] = info["original_sha256"] == info["rebuilt_sha256"]
                    info["original_bytes"] = path.stat().st_size
                    info["rebuilt_bytes"] = rebuilt.stat().st_size
                    if not info["byte_identical"]:
                        redump = rt_dir / (path.name + ".redump.txt")
                        rd = run("thanm", ["-uul20", rebuilt], path.name + "_redump", stdout_path=redump)
                        info["canonical_dump_identical"] = rd.returncode == 0 and redump.read_bytes() == output.read_bytes()
                roundtrips.append(info)
            return
        else:
            compiler = "th" + kind
            ending = kind == "msg" and path.stem.startswith(("e", "staff"))
            flags = "-ed20" if ending else "-d20"
            compile_flags = "-ec20" if ending else "-c20"
            proc = run(compiler, [flags, path, output], path.name + "_decompile")
            raw_output = output
            # thmsg uses original Shift-JIS bytes. Keep compiler source byte-for-byte
            # and create a separate Unicode view for modern editors.
            if kind == "msg" and output.exists():
                text_path = dest / (path.name + ".utf8.txt")
                text_path.write_text(output.read_bytes().decode("cp932", errors="replace"), encoding="utf-8")
        if proc.returncode != 0:
            return
        rt_dir = recovered / "roundtrip" / kind
        rt_dir.mkdir(parents=True, exist_ok=True)
        rebuilt = rt_dir / path.name
        compiled = run(compiler, [compile_flags, raw_output, rebuilt], path.name + "_roundtrip")
        info = {"name": path.name, "kind": kind, "compiled": compiled.returncode == 0,
                "original_sha256": digest(path)}
        if compiled.returncode == 0 and rebuilt.is_file():
            info["rebuilt_sha256"] = digest(rebuilt)
            info["byte_identical"] = info["original_sha256"] == info["rebuilt_sha256"]
            info["original_bytes"] = path.stat().st_size
            info["rebuilt_bytes"] = rebuilt.stat().st_size
            if not info["byte_identical"]:
                redump = rt_dir / (path.name + ".redump.txt")
                flags = "-jrd20" if kind == "ecl" else ("-ed20" if kind == "msg" and path.stem.startswith(("e", "staff")) else "-d20")
                rd = run(compiler, [flags, rebuilt, redump], path.name + "_redump")
                info["canonical_dump_identical"] = rd.returncode == 0 and redump.read_bytes() == raw_output.read_bytes()
        roundtrips.append(info)

    selected = sorted({raw / item["name"] for item in entries
                if Path(item["name"]).suffix.lower() in {".ecl", ".anm", ".std", ".msg"}})
    with ThreadPoolExecutor(max_workers=args.workers) as pool:
        list(pool.map(recover, selected))
    archive_after = digest(archive)
    textures = list((ROOT / "assets/textures").rglob("*"))
    texture_files = [p for p in textures if p.is_file()]
    result = {
        "input_directory": str(game), "archive_sha256_before": archive_before,
        "archive_sha256_after": archive_after, "original_archive_unchanged": archive_before == archive_after,
        "thtk_revision": PIN, "thtk_source": "https://github.com/thpatch/thtk",
        "thtk_compatibility_patch": "tools/asset_thtk_th20.patch",
        "thtk_compatibility_patch_sha256": digest(ROOT / "tools/asset_thtk_th20.patch"),
        "entry_count": len(entries), "unique_file_count": len({e["name"] for e in entries}),
        "duplicate_names": [n for n, count in Counter(e["name"] for e in entries).items() if count > 1],
        "duplicate_payloads_identical": {name: len({e["sha256"] for e in entries if e["name"] == name}) == 1
                                         for name, count in name_counts.items() if count > 1},
        "extracted_bytes": sum(e["bytes"] for e in entries),
        "extensions": dict(sorted(Counter(Path(e["name"]).suffix.lower() for e in entries).items())),
        "texture_files": len(texture_files), "texture_extensions": dict(Counter(p.suffix.lower() for p in texture_files)),
        "source_roundtrip_count": len(roundtrips),
        "source_roundtrip_byte_identical_count": sum(bool(r.get("byte_identical")) for r in roundtrips),
        "entries": entries, "roundtrips": sorted(roundtrips, key=lambda r: r["name"]),
        "commands": sorted(records, key=lambda r: r["tag"]),
        "failed_commands": [r for r in records if r["returncode"] != 0],
        "warning_commands": [r["tag"] for r in records if r["stderr_bytes"] > 0],
    }
    (report / "assets_recovery.json").write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding="utf-8")
    print(json.dumps({**{k: result[k] for k in ["entry_count", "unique_file_count", "extensions", "texture_files", "original_archive_unchanged"]}, "failed_command_count": len(result["failed_commands"]), "warning_command_count": len(result["warning_commands"])}, ensure_ascii=False, indent=2))

if __name__ == "__main__":
    main()
