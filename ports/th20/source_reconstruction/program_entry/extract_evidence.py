"""Read-only source evidence extraction; generated C++ contains TEXT DATA only.

No original instruction bytes are compiled, embedded, mapped, or executed by
the source module. Its assembler excerpts are documentation, not build inputs.
"""
from pathlib import Path
import argparse
import csv
import hashlib
import json
import re
import struct

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
EXPECTED = "a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897"
SELECTED = {
    0x5435e0: "PE entry / CRT security cookie entry",
    0x543461: "MSVC CRT startup and invocation of WinMain",
    0x543cd2: "MSVC security cookie initialization",
    0x41e7d0: "WinMain, corrected four-argument WINAPI ABI",
    0x419c20: "unlimited frame scheduler",
    0x419de0: "software timed frame scheduler",
    0x41a030: "present paced frame scheduler",
    0x41cb10: "clock source, unresolved lock and uint64 conversion dependencies",
    0x41cc70: "display mode getter",
    0x41d080: "reset flag getter",
    0x41dcc0: "draw counter setter",
    0x41de00: "reset flag setter",
    0x41de30: "reset delay setter",
    0x412730: "Direct3D device getter",
    0x415800: "D3DPRESENT_PARAMETERS.Windowed getter",
    0x41a200: "release Direct3D object",
    0x41a240: "release Direct3D device",
    0x41b480: "SetForegroundWindow wrapper",
    0x41b490: "restore system settings",
    0x412540: "proven side-effect-free constant-zero function",
    0x4111e0: "proven side-effect-free constant-minus-one function",
    0x40c6b0: "proven five-byte no-op, not printf",
}

def csv_rows(name):
    with (ROOT / name).open(encoding="utf-8-sig", newline="") as stream:
        return list(csv.DictReader(stream))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("exe", type=Path)
    args = parser.parse_args()
    raw = args.exe.read_bytes()
    if hashlib.sha256(raw).hexdigest() != EXPECTED:
        raise SystemExit("Original specimen SHA-256 mismatch")
    pe = struct.unpack_from("<I", raw, 0x3c)[0]
    sections = struct.unpack_from("<H", raw, pe + 6)[0]
    opt_size = struct.unpack_from("<H", raw, pe + 20)[0]
    sec_at = pe + 24 + opt_size
    def at_va(va, length):
        rva = va - 0x400000
        for i in range(sections):
            _, start, size, offset = struct.unpack_from("<4I", raw, sec_at + i * 40 + 8)
            if start <= rva and rva + length <= start + size:
                return offset + rva - start
        raise ValueError(f"Address has no file-backed section: {va:#x}")
    def cstring(va):
        at = at_va(va, 1)
        return raw[at:raw.index(0, at)]
    strings = {
        "start": 0x56c948, "cannot_save": 0x56c988,
        "function_allocation_site": 0x56c9b4,
        "sprite_allocation_site": 0x56c9f4,
        "restart": 0x56ca3c,
    }
    header = ["#pragma once", "// Original CP932 string data, not executable bytes.",
              "// Generated once from the verified specimen; no extraction runs at build time.",
              "namespace th20::source::program_entry::text_constants {"]
    for name, va in strings.items():
        value = cstring(va)
        literal = "".join(f"\\x{byte:02x}" for byte in value)
        header.append(f'inline constexpr char {name}[] = "{literal}"; // 0x{va:08x}')
    header += ["}", ""]
    (HERE / "text_constants.hpp").write_text("\n".join(header), encoding="utf-8")

    metadata = {int(row["entry_va"], 16): row for row in csv_rows("analysis/ghidra/functions.csv")}
    ranges = {}
    for row in csv_rows("analysis/ghidra/function_ranges.csv"):
        function = int(row["function_entry"], 16)
        if function in SELECTED:
            ranges.setdefault(function, []).append((int(row["range_start"], 16), int(row["range_end_inclusive"], 16)))
    def owner(address):
        for function, parts in ranges.items():
            if any(begin <= address <= end for begin, end in parts):
                return function
        return None
    edges = []
    for row in csv_rows("analysis/binary/calls.csv"):
        caller = owner(int(row["from_va"], 16))
        if caller is not None:
            target = int(row["to_va"], 16)
            edges.append({"caller": f"0x{caller:08x}", "site": row["from_va"],
                          "target": row["to_va"], "kind": row["kind"], "symbol": row["symbol"],
                          "target_pseudocode": metadata.get(target, {}).get("pseudocode_file")})
    # entry's second edge is a tail JMP rather than CALL, recorded explicitly.
    edges.append({"caller": "0x005435e0", "site": "0x005435e5", "target": "0x00543461", "kind": "tail_jump", "symbol": "CRT startup"})
    graph = {
        "source_sha256": EXPECTED,
        "scope": "Exact statically resolved calls in selected entry/scheduler functions; indirect object/vtable/callback targets require further recovery",
        "roots": ["0x005435e0", "0x00543461", "0x0041e7d0"],
        "nodes": [{"address": f"0x{va:08x}", "role": role,
                   "ranges": [[f"0x{a:08x}", f"0x{b:08x}"] for a, b in ranges[va]],
                   "pseudocode": metadata[va]["pseudocode_file"]} for va, role in SELECTED.items()],
        "edges": edges,
        "crt_initializers": {},
    }
    for name, start, end in [("C", 0x56c418, 0x56c43c), ("C++", 0x56c384, 0x56c414)]:
        graph["crt_initializers"][name] = [
            {"slot": f"0x{slot:08x}", "target": f"0x{struct.unpack_from('<I', raw, at_va(slot, 4))[0]:08x}"}
            for slot in range(start, end, 4)
        ]
    (HERE / "entry_call_graph.json").write_text(json.dumps(graph, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    bodies = {va: [] for va in SELECTED}
    with (ROOT / "analysis/binary/disassembly.asm").open(encoding="utf-8") as stream:
        for line in stream:
            match = re.match(r"^([0-9a-fA-F]{8})\s", line)
            if match:
                function = owner(int(match.group(1), 16))
                if function is not None:
                    bodies[function].append(line.rstrip())
    evidence = HERE / "evidence"
    evidence.mkdir(exist_ok=True)
    for va, body in bodies.items():
        (evidence / f"{va:08x}.asm").write_text(
            f"; {SELECTED[va]}\n; source SHA-256 {EXPECTED}\n" + "\n".join(body) + "\n", encoding="utf-8")
    constants = {f"0x{va:08x}": {"bytes": raw[at_va(va, 8):at_va(va, 8)+8].hex(),
                "double": struct.unpack_from("<d", raw, at_va(va, 8))[0]}
                 for va in [0x56c468, 0x56c470, 0x56cd70, 0x56cd88, 0x56cdd0, 0x56cdd8]}
    (evidence / "constants.json").write_text(json.dumps(constants, indent=2) + "\n", encoding="utf-8")
    print(f"Selected functions: {len(SELECTED)}; call edges: {len(edges)}; CP932 strings: {len(strings)}")

if __name__ == "__main__":
    main()
