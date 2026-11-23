#!/usr/bin/env python3
"""Unicorn oracle：直接执行原版 th16.exe 的 RNG 函数，导出 golden 序列。

用法：python ports/th16/tools/oracle/rng-oracle.py [--exe games/th16/original/th16.exe] [--out ports/th16/tests/golden/rng.json]

执行 0x402b70（step16）、0x402be0（step32）与 0x402c70/0x402cb0/0x402cf0（浮点化）。
.data 全零 → DAT_004c10b6 为 0，不进入临界区分支。浮点结果经返回桩 fstp dword 按 float 落位（IEEE 位型记录）。
"""
import argparse, json, struct, sys, pathlib
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *

ROOT = pathlib.Path(__file__).resolve().parents[4]
STEP16, STEP32, UNIT, SIGNED, ANGLE = 0x402B70, 0x402BE0, 0x402C70, 0x402CB0, 0x402CF0
SENTINEL = 0x00500000  # 返回地址：映射一页，命中即停
RNG_OBJ = 0x00510000   # 伪 RNG 对象：u16 seed, u16 pad, u32 calls
STACK = 0x00600000


def load(exe: pathlib.Path):
    pe = pefile.PE(str(exe))
    base = pe.OPTIONAL_HEADER.ImageBase
    size = (pe.OPTIONAL_HEADER.SizeOfImage + 0xFFFF) & ~0xFFFF
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    uc.mem_map(base, size)
    uc.mem_write(base, pe.header)
    for s in pe.sections:
        uc.mem_write(base + s.VirtualAddress, s.get_data())
    uc.mem_map(SENTINEL, 0x1000)
    # 返回桩：fstp dword [RNG_OBJ+8]（x87 按 float 落位，与原版把 float10 结果存入 float 变量一致）；hlt
    uc.mem_write(SENTINEL, bytes([0xD9, 0x1D]) + struct.pack('<I', RNG_OBJ + 8) + bytes([0xF4]))
    uc.mem_map(RNG_OBJ, 0x1000)
    uc.mem_map(STACK, 0x10000)
    return uc


def call(uc, fn, seed, calls):
    uc.mem_write(RNG_OBJ, struct.pack('<HHI', seed, 0, calls))
    uc.reg_write(UC_X86_REG_ECX, RNG_OBJ)  # __fastcall this
    uc.reg_write(UC_X86_REG_ESP, STACK + 0x8000)
    uc.mem_write(STACK + 0x8000, struct.pack('<I', SENTINEL))
    uc.emu_start(fn, SENTINEL + 7, count=10000)
    eax = uc.reg_read(UC_X86_REG_EAX)
    new_seed, _, new_calls, f32 = struct.unpack('<HHII', uc.mem_read(RNG_OBJ, 12))
    return eax, new_seed, new_calls, f32


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=str(ROOT / 'games/th16/original/th16.exe'))
    ap.add_argument('--out', default=str(ROOT / 'ports/th16/tests/golden/rng.json'))
    ap.add_argument('--steps', type=int, default=64)
    a = ap.parse_args()
    uc = load(pathlib.Path(a.exe))
    golden = {'source': 'th16.exe v1.00a sha256 c11776019f083978e66027e7394dafb1fb9543afca986f28049a49417e341929',
              'functions': {'step16': hex(STEP16), 'step32': hex(STEP32), 'unit': hex(UNIT), 'signed_unit': hex(SIGNED), 'angle': hex(ANGLE)},
              'sequences': []}
    for seed in (0, 1, 0x1234, 0x9630, 0xFFFF, 0xBEEF, 0x4e21):
        s16 = []
        cur, calls = seed, 0
        for _ in range(a.steps):
            eax, cur, calls, _ = call(uc, STEP16, cur, calls)
            s16.append([eax & 0xFFFF, cur, calls])
        s32 = []
        cur, calls = seed, 0
        for _ in range(a.steps):
            eax, cur, calls, _ = call(uc, STEP32, cur, calls)
            s32.append([eax, cur, calls])
        floats = {}
        for name, fn in (('unit', UNIT), ('signed_unit', SIGNED), ('angle', ANGLE)):
            cur, calls = seed, 0
            seq = []
            for _ in range(16):
                _, cur, calls, f32 = call(uc, fn, cur, calls)
                seq.append([f32, cur, calls])
            floats[name] = seq
        golden['sequences'].append({'seed': seed, 'step16': s16, 'step32': s32, **floats})
    out = pathlib.Path(a.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(golden, indent=1) + '\n')
    print(f'wrote {out} ({len(golden["sequences"])} seeds × {a.steps} steps)')


if __name__ == '__main__':
    main()
