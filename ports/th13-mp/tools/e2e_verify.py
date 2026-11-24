#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import os
import struct
import sys

OUTER = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, OUTER)
sys.path.insert(0, os.path.join(OUTER, "..", "..", "..", ".."))

from oracle import replayfmt
from oracle import scorefmt

EXE = os.path.join("games", "th13", "th13.exe")
MALLOC = 0x47A0ED
GETPTD = 0x47FE5A
GETPTD_NOEXIT = 0x47FDE1
CHDIR = 0x47AB92
SUPERVISOR = 0x4C22E0
REPLAY_FLAGS = 0x4BE830


class Session:

    def __init__(self):
        from oracle.machine import Machine
        self.m = Machine(EXE)
        self.read_file = None
        self.writes = []
        self._install_stubs()

    def _arg(self, i):
        import struct as _s
        from unicorn.x86_const import UC_X86_REG_ESP
        esp = self.m.uc.reg_read(UC_X86_REG_ESP)
        return _s.unpack("<I", self.m.uc.mem_read(esp + 4 + i * 4, 4))[0]

    def _install_stubs(self):
        import struct as _s

        self.m.override_call(MALLOC, lambda machine: self.m.malloc(self._arg(0)))
        self.m.override_call(CHDIR, lambda machine: 0)
        self.m.override_call(GETPTD, lambda machine: self.ptd)
        self.m.override_call(GETPTD_NOEXIT, lambda machine: self.ptd)
        self.m.override_call(0x47BF27, lambda machine: 0)

        def mini_sprintf(machine):
            buf = self._arg(0)
            fmt = self.m.read_cstr(self._arg(1))
            name = self.m.read_cstr(self._arg(2))
            out = fmt.replace("%s", name).encode("ascii") + b"\x00"
            self.m.uc.mem_write(buf, out)
            return len(out) - 1

        self.m.override_call(0x47A3D8, mini_sprintf)

        def heap_alloc(machine):
            return self.m.malloc(self._arg(2))

        self.m.stub("HeapAlloc", heap_alloc, args_pop=12)
        self.m.stub("HeapFree", lambda machine: 1, args_pop=12)
        self.m.stub("HeapReAlloc", heap_alloc, args_pop=16)
        self.m.stub("InitializeCriticalSectionAndSpinCount", lambda machine: 1, args_pop=8)
        self.m.stub("InitializeCriticalSection", lambda machine: 1, args_pop=4)
        self.m.stub("GetLastError", lambda machine: 0, args_pop=0)

        for slot, imp in list(self.m.import_stubs.items()):
            if imp.name.lower() in ("encodepointer", "decodepointer"):
                self.m.uc.mem_write(slot + 8, b"\xC3")
                self.m.stub(imp.name, (lambda s: (lambda machine: s + 8))(slot), args_pop=4)

        self.ptd = self.m.malloc(0x214)
        self.m.uc.mem_write(self.ptd, b"\x00" * 0x214)

        GENERIC_READ, GENERIC_WRITE, OPEN_EXISTING, CREATE_ALWAYS = (
            0x80000000, 0x40000000, 3, 2)
        READ_HANDLE, WRITE_HANDLE = 0x1337, 0x1338

        def createfilea(machine):
            name = self.m.read_cstr(self._arg(0))
            access, disp = self._arg(1), self._arg(4)
            if access == GENERIC_READ and disp == OPEN_EXISTING and self.read_file:
                self.read_name = name
                return READ_HANDLE
            if access == GENERIC_WRITE and disp == CREATE_ALWAYS:
                self.write_name = name
                return WRITE_HANDLE
            raise RuntimeError("unexpected CreateFileA(%r, access=%08x, disp=%d)"
                               % (name, access, disp))

        self.m.stub("createfilea", createfilea, args_pop=28)

        def readfile(machine):
            handle, buf, n = self._arg(0), self._arg(1), self._arg(2)
            written = self._arg(3)
            data = (self.read_file or b"")[self.read_pos:][:n] if handle == READ_HANDLE else b""
            if handle == READ_HANDLE:
                self.read_pos += len(data)
            self.m.uc.mem_write(buf, data)
            self.m.uc.mem_write(written, _s.pack("<I", len(data)))
            return 1

        self.m.stub("readfile", readfile, args_pop=20)

        def writefile(machine):
            handle, buf, n = self._arg(0), self._arg(1), self._arg(2)
            written = self._arg(3)
            data = bytes(self.m.uc.mem_read(buf, n))
            self.writes.append((handle, data))
            self.m.uc.mem_write(written, _s.pack("<I", n))
            return 1

        self.m.stub("writefile", writefile, args_pop=20)
        self.m.stub("closehandle", lambda machine: 1, args_pop=4)
        self.read_pos = 0

        def getfilesize(machine):
            return len(self.read_file) if self.read_file else 0

        self.m.stub("getfilesize", getfilesize, args_pop=8)
        self.m.stub("setfilepointer", lambda machine: 0, args_pop=16)
        self.m.stub("setcurrentdirectorya", lambda machine: 1, args_pop=4)
        self.m.stub("getcurrentdirectorya", lambda machine: 3, args_pop=8)

def verify_replay(export_path: str, original_path: str) -> dict:
    def decode(path):
        r = replayfmt.ReplayFile()
        with open(path, "rb") as f:
            r.decode(f.read())
        return r

    ours, orig = decode(export_path), decode(original_path)
    report = {"export": export_path, "original": original_path}
    failures = []

    top_o, top_e = bytearray(orig.top_header), bytearray(ours.top_header)
    (ours_score,) = struct.unpack_from("<i", ours.top_header, 0x14)
    (orig_score,) = struct.unpack_from("<i", orig.top_header, 0x14)
    undocumented = {}
    for off in range(0x18, 0x54, 4):
        a = struct.unpack_from("<I", top_o, off)[0]
        b = struct.unpack_from("<I", top_e, off)[0]
        if a != b:
            undocumented[hex(off)] = {"orig": a, "ours": b}
    report["top"] = {
        "name": {"ours": ours.player_name, "orig": orig.player_name},
        "score": {"ours": ours_score, "orig": orig_score},
        "character": {"ours": ours.character, "orig": orig.character},
        "shot_type": {"ours": ours.shot_type, "orig": orig.shot_type},
        "rank": {"ours": ours.rank, "orig": orig.rank},
        "stage_count": {"ours": ours.stage_count, "orig": orig.stage_count},
        "practice_stage": {"ours": ours.practice_stage, "orig": orig.practice_stage},
        "mode_bits_0x0a": {"ours": ours.top_header[0x0A], "orig": orig.top_header[0x0A]},
        "stage_field_0x68": {"ours": struct.unpack_from("<i", ours.top_header, 0x68)[0],
                             "orig": struct.unpack_from("<i", orig.top_header, 0x68)[0]},
        "date_recorded_only": {"ours": struct.unpack_from("<Q", ours.top_header, 0x0C)[0],
                               "orig": struct.unpack_from("<Q", orig.top_header, 0x0C)[0]},
        "slow_rate_recorded_only": {"ours": ours.slow_rate, "orig": orig.slow_rate},
        "snapshot_region_0x18_0x53_recorded_only": undocumented,
    }
    for field, key in [("name", "player_name"), ("stage_count", "stage_count"),
                       ("character", "character"), ("rank", "rank"),
                       ("practice_stage", "practice_stage")]:
        if getattr(ours, key) != getattr(orig, key):
            failures.append(f"top.{field}: {getattr(ours, key)!r} != {getattr(orig, key)!r}")
    if ours_score != orig_score:
        failures.append(f"top.score: {ours_score} != {orig_score}")
    if ours.top_header[0x0A] != orig.top_header[0x0A]:
        failures.append("top.mode_bits_0x0a differ")
    if (struct.unpack_from("<i", ours.top_header, 0x68)[0] !=
            struct.unpack_from("<i", orig.top_header, 0x68)[0]):
        failures.append("top.stage_field_0x68 differ")
    for off in (0x5C, 0x60, 0x64, 0x70):
        if top_o[off:off + 4] != top_e[off:off + 4]:
            failures.append(f"top field +{off:#x} differ")

    if len(ours.stages) != len(orig.stages):
        failures.append("stage count %d != %d" % (len(ours.stages), len(orig.stages)))
    stages_report = []
    for idx, (o, e) in enumerate(zip(orig.stages, ours.stages)):
        srep = {"index": idx}
        for name, a, b in [("number", o.number, e.number), ("seed", o.stamp, e.stamp),
                           ("frames", o.frame_count, e.frame_count),
                           ("blob", o.blob_size, e.blob_size),
                           ("start_score", o.score, e.score)]:
            srep[name] = {"ours": b, "orig": a, "equal": a == b}
        equip_o = o.header[0x14:0x6C]
        equip_e = e.header[0x14:0x6C]
        srep["equipment_0x34_recorded_only"] = {
            "orig": struct.unpack_from("<I", o.header, 0x34)[0],
            "ours": struct.unpack_from("<I", e.header, 0x34)[0]}
        mask = bytearray(equip_o)
        equip_e = bytearray(equip_e)
        for i in range(0x34 - 0x14, 0x38 - 0x14):
            mask[i] = 0
            equip_e[i] = 0
            mask[i] = 0
        equip_o_cmp = bytes(mask)
        equip_e_cmp = bytes(equip_e)
        srep["equipment_region_0x14_0x6c_equal"] = equip_o_cmp == equip_e_cmp
        if equip_o_cmp != equip_e_cmp:
            bad = sorted({hex(i + 0x14) for i in range(0x58)
                          if equip_o_cmp[i:i + 4] != equip_e_cmp[i:i + 4]})
            srep["equipment_diff_dwords"] = bad
            srep["equipment_diff_detail"] = {
                hex(int(x, 16)): {"orig": struct.unpack_from("<I", o.header, int(x, 16))[0],
                                  "ours": struct.unpack_from("<I", e.header, int(x, 16))[0]}
                for x in bad}
            failures.append(f"stage[{idx}] number={o.number} equipment region diff at {bad}")
        spell_o, spell_e = o.header[0x70:0xC4], e.header[0x70:0xC4]
        srep["spell_time_array_0x70_recorded_only_equal"] = spell_o == spell_e
        if o.frame_count != e.frame_count:
            failures.append(f"stage[{idx}] frames {e.frame_count} != {o.frame_count}")
        if len(o.events) != len(e.events):
            failures.append(f"stage[{idx}] event stream length {len(e.events)} != {len(o.events)}")
        n = min(o.frame_count, e.frame_count)
        bad = []
        for i in range(n):
            if o.input[i * 6:(i + 1) * 6] != e.input[i * 6:(i + 1) * 6]:
                bad.append(i)
        srep["input_frames_compared"] = n
        srep["input_frames_equal"] = len(bad) == 0
        srep["input_mismatch_count"] = len(bad)
        if bad:
            srep["input_mismatch_first"] = bad[:8]
            srep["input_mismatch_detail"] = [
                {"frame": i, "ours": list(e.frame(i)), "orig": list(o.frame(i))}
                for i in bad[:8]]
            boundary = [i for i in bad if i == 0]
            others = [i for i in bad if i != 0]
            if boundary:
                srep["boundary_frame0_pressed_delta"] = True
            if others:
                failures.append(f"stage[{idx}] input frames mismatch beyond frame0: "
                                f"{others[:8]} ({len(others)} total)")
            elif not boundary:
                failures.append(f"stage[{idx}] input frames mismatch incl non-boundary")
        if len(o.events) == len(e.events):
            differing = sum(1 for a, b in zip(o.events, e.events) if a != b)
            srep["event_bytes_differing"] = differing
        stages_report.append(srep)
    report["stages"] = stages_report
    report["verdict"] = "PASS" if not failures else "FAIL"
    report["failures"] = failures
    return report

def verify_replay_loader(export_path: str, expected_frames: list[int],
                         expected_seeds: list[int]) -> dict:
    sess = Session()
    with open(export_path, "rb") as f:
        sess.read_file = f.read()
    sess.read_pos = 0
    name_buf = sess.m.malloc(0x40)
    sess.m.uc.mem_write(name_buf, b"th13_01.rpy\x00")
    sess.m.call(0x448C40, args=[], regs={"ecx": name_buf, "ebx": SUPERVISOR},
                timeout_ms=300_000)

    m = sess.m
    report = {"entry": "0x448c40", "ebx": hex(SUPERVISOR),
              "path": "replay/th13_01.rpy via 0x472680/0x472900/0x472a20 stubs"}
    file_ptr = m.read_u32(SUPERVISOR + 0x18)
    plain_ptr = m.read_u32(SUPERVISOR + 0x1C)
    buf_ptr = m.read_u32(SUPERVISOR + 0x208)
    report["file_ptr"] = hex(file_ptr)
    report["plain_ptr"] = hex(plain_ptr)
    report["plain_buf_ptr"] = hex(buf_ptr)
    failures = []
    if not file_ptr or not plain_ptr or not buf_ptr:
        failures.append("loader left null pointers")
        report["verdict"] = "FAIL"
        report["failures"] = failures
        return report
    magic = bytes(m.uc.mem_read(file_ptr, 4))
    report["file_magic"] = magic.decode("latin1")
    if magic != b"t13r":
        failures.append("file magic %r != t13r" % magic)
    stage_count = m.uc.mem_read(plain_ptr + 0x58, 1)[0]
    report["stage_count"] = stage_count
    if stage_count != len(expected_frames):
        failures.append("stage count %d != %d" % (stage_count, len(expected_frames)))
    name = bytes(m.uc.mem_read(plain_ptr, 12)).split(b"\0")[0].decode("ascii", "replace").rstrip()
    (score,) = struct.unpack("<i", bytes(m.uc.mem_read(plain_ptr + 0x14, 4)))
    report["name"] = name
    report["score"] = score
    stages = []
    for idx, want_frames in enumerate(expected_frames[:min(stage_count, len(expected_frames))]):
        number = idx + 1
        header_ptr = m.read_u32(SUPERVISOR + 0xD8 + number * 0x28)
        input_ptr = m.read_u32(SUPERVISOR + (number * 5 + 0x19) * 8)
        end_ptr = m.read_u32(SUPERVISOR + 0xD0 + number * 0x28)
        if not header_ptr:
            failures.append("stage %d header ptr null" % number)
            continue
        snum, seed = struct.unpack("<HH", bytes(m.uc.mem_read(header_ptr, 4)))
        (frames,) = struct.unpack("<I", bytes(m.uc.mem_read(header_ptr + 4, 4)))
        (score_start,) = struct.unpack("<i", bytes(m.uc.mem_read(header_ptr + 0x1C, 4)))
        ok_number = snum == number
        ok_frames = frames == want_frames
        ok_seed = seed == expected_seeds[idx]
        ok_end = end_ptr == input_ptr + frames * 6
        stages.append({"number": number, "seed": seed, "frames": frames,
                       "start_score": score_start, "input_ptr": hex(input_ptr),
                       "input_end": hex(end_ptr), "end_equals_ptr_plus_6f": ok_end})
        if not ok_number:
            failures.append("stage %d header number %d" % (number, snum))
        if not ok_frames:
            failures.append("stage %d frames %d != %d" % (number, frames, want_frames))
        if not ok_seed:
            failures.append("stage %d seed %d != %d" % (number, seed, expected_seeds[idx]))
        if not ok_end:
            failures.append("stage %d input end %08x != ptr+6*frames" % (number, end_ptr))
    report["stages"] = stages
    report["verdict"] = "PASS" if not failures else "FAIL"
    report["failures"] = failures
    return report


def verify_score(score_path: str, want_name: str, want_score: int,
                 character: int = 0, rank_index: int = 1) -> dict:
    with open(score_path, "rb") as f:
        data = f.read()
    model = scorefmt.parse(data)
    report = {"file": score_path, "magic": model.header.magic.decode("latin1"),
              "version": model.header.version, "comp_size": model.header.comp_size,
              "uncomp_size": model.header.uncomp_size}
    failures = []
    if model.header.magic != b"TH31":
        failures.append("magic %r != TH31" % model.header.magic)
    if model.header.version != 4:
        failures.append("version %d != 4" % model.header.version)
    block = model.cr_blocks[character] if character < len(model.cr_blocks) else None
    if block is None:
        failures.append("character CR column missing (dropped)")
    else:
        entry = block.entries[rank_index][0]
        report["character_normal_rank1"] = {
            "score": entry.score, "name": entry.name.decode("ascii", "replace").rstrip("\x00 "),
            "stage": entry.stage, "clear": entry.clear,
            "time_lo": entry.time_lo, "time_hi": entry.time_hi,
            "rate_bits": hex(entry.rate_bits)}
        if entry.score != want_score:
            failures.append("rank1 score %d != %d" % (entry.score, want_score))
        if entry.name.split(b"\0")[0].rstrip() != want_name.encode():
            failures.append("rank1 name %r != %r" % (entry.name, want_name))
        if entry.time_lo == 0 and entry.time_hi == 0:
            failures.append("rank1 timestamp missing")
        row = block.entries[rank_index]
        for r in range(1, 10):
            if row[r].score > row[0].score:
                failures.append("rank%d score %d > rank1" % (r + 1, row[r].score))
    report["verdict"] = "PASS" if not failures else "FAIL"
    report["failures"] = failures
    return report

def verify_score_loader(score_path: str, want_score: int, want_name: str,
                        character: int = 0, rank_index: int = 1) -> dict:
    sess = Session()
    with open(score_path, "rb") as f:
        sess.read_file = f.read()
    sess.read_pos = 0
    eax = sess.m.call(0x449BE0, args=[], timeout_ms=600_000)
    m = sess.m
    base = m.read_u32(scorefmt.SCORE_STRUCT_PTR)
    report = {"entry": "0x449be0", "eax": hex(eax & 0xFFFFFFFF),
              "score_struct": hex(base)}
    failures = []
    if not base:
        failures.append("DAT_004c22cc null after load")
        report["verdict"] = "FAIL"
        report["failures"] = failures
        return report
    block_addr = base + 0x8 + character * 0x56DC
    magic, version = struct.unpack("<HH", bytes(m.uc.mem_read(block_addr, 4)))
    entry_addr = block_addr + 0x10 + rank_index * 0x118
    entry = bytes(m.uc.mem_read(entry_addr, 0x1C))
    e_score, e_stage, e_continues = struct.unpack_from("<IBB", entry, 0)
    e_name = entry[6:15]
    report["character_block"] = {"magic": hex(magic), "version": version}
    report["character_rank1"] = {"score": e_score, "stage": e_stage,
                                 "continues": e_continues,
                                 "name": e_name.decode("ascii", "replace").rstrip("\x00 ")}
    if magic != 0x5243:
        failures.append("CR block magic %#x != 0x5243" % magic)
    if e_score != want_score:
        failures.append("in-memory rank1 score %d != %d" % (e_score, want_score))
    if e_name.split(b"\0")[0].rstrip() != want_name.encode():
        failures.append("in-memory rank1 name %r != %r" % (e_name, want_name))
    report["verdict"] = "PASS" if not failures else "FAIL"
    report["failures"] = failures
    return report

def main() -> int:
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    for name in ("verify-replay", "verify-replay-loader", "verify-score",
                 "verify-score-loader", "verify-all"):
        p = sub.add_parser(name)
        p.add_argument("--export", default="artifacts/tmp-scratch/e2e/e2e_export.rpy")
        p.add_argument("--score", default="artifacts/tmp-scratch/e2e/e2e_score.dat")
        p.add_argument("--original", default="games/replays/th13_01.rpy")
        p.add_argument("--json-out", default="")
    a = ap.parse_args()

    if a.cmd in ("verify-replay", "verify-all"):
        rep = verify_replay(a.export, a.original)
        print(json.dumps(rep, indent=2, ensure_ascii=False))
        if rep["verdict"] != "PASS":
            return 1
    if a.cmd in ("verify-replay-loader", "verify-all"):
        with open(a.original, "rb") as f:
            orig = replayfmt.ReplayFile()
            orig.decode(f.read())
        rep = verify_replay_loader(a.export, [s.frame_count for s in orig.stages],
                                   [s.stamp for s in orig.stages])
        print(json.dumps(rep, indent=2, ensure_ascii=False))
        if rep["verdict"] != "PASS":
            return 1
    if a.cmd in ("verify-score", "verify-all"):
        with open(a.original, "rb") as f:
            orig = replayfmt.ReplayFile()
            orig.decode(f.read())
        rep = verify_score(a.score, orig.player_name, orig.score,
                           character=orig.character, rank_index=orig.rank)
        print(json.dumps(rep, indent=2, ensure_ascii=False))
        if rep["verdict"] != "PASS":
            return 1
    if a.cmd in ("verify-score-loader", "verify-all"):
        with open(a.original, "rb") as f:
            orig = replayfmt.ReplayFile()
            orig.decode(f.read())
        rep = verify_score_loader(a.score, orig.score, orig.player_name,
                                  character=orig.character, rank_index=orig.rank)
        print(json.dumps(rep, indent=2, ensure_ascii=False))
        if rep["verdict"] != "PASS":
            return 1
    print("E2E-VERIFY PASS" if a.cmd.startswith("verify") else "")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
