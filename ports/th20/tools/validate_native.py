"""Differential tests: original x86 instructions in Unicorn versus built C++ DLL.

Only selected pure/core routines execute in an emulator; no original process is
started, and no original file is modified. This does NOT validate full gameplay.
"""
from pathlib import Path
import argparse
import atexit
import ctypes as ct
import hashlib
import json
import random
import struct
import sys
from datetime import datetime, timezone

ROOT = Path(__file__).resolve().parents[1]
for dep in ('binary_python', 'emulation_python'):
    sys.path.insert(0, str(ROOT / '.cache' / dep))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import (UC_X86_REG_ESP, UC_X86_REG_ECX,
    UC_X86_REG_EAX, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW,
    UC_X86_REG_FPTAG, UC_X86_REG_MXCSR)

SOURCE = Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders')
EXPECTED_SHA256 = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
STACK, OBJECT, STOP = 0x1000000, 0x2000000, 0x3000000

class Timer(ct.Structure):
    _fields_ = [('previous', ct.c_int32), ('current', ct.c_int32),
                ('current_f', ct.c_float), ('flags', ct.c_uint32)]

def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--exe', type=Path, default=SOURCE / 'th20.exe')
    ap.add_argument('--dll', type=Path, default=ROOT / 'build/native_recovered/Release/th20_native_recovered.dll')
    ap.add_argument('--output', type=Path, default=ROOT / 'reports/native_validation.json')
    args = ap.parse_args()
    raw = args.exe.read_bytes()
    if hashlib.sha256(raw).hexdigest() != EXPECTED_SHA256:
        raise ValueError('Native addresses belong to a different executable revision')
    pe = pefile.PE(data=raw)
    if pe.OPTIONAL_HEADER.ImageBase != 0x400000 or pe.FILE_HEADER.Machine != 0x14c:
        raise ValueError('Expected fixed TH20 PE32 revision at preferred base 0x400000')
    evidence = {
        'lcg_step': (0x422cb0, 0x422d35), 'lcg_next': (0x4235f0, 0x423613),
        'timer_reset': (0x423f50, 0x423f79), 'timer_mode': (0x423fe0, 0x424006),
        'timer_set': (0x423f80, 0x423fda), 'timer_add': (0x4530f0, 0x453217),
        'timer_tick': (0x4533b0, 0x4534f4),
    }
    # Version/address guard: full hash is set to the analyzed specimen.
    manifest = ROOT / 'reports' / 'source_manifest.json'
    if manifest.exists():
        expected = next(f['sha256'] for f in json.loads(manifest.read_text('utf-8'))['files'] if f['name'] == 'th20.exe')
        if hashlib.sha256(raw).hexdigest() != expected:
            raise ValueError('EXE hash differs from analyzed source manifest')
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    uc.mem_map(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
    uc.mem_write(0x400000, pe.get_memory_mapped_image())
    for address in (STACK, OBJECT, STOP):
        uc.mem_map(address, 0x10000)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, 0x37f)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    initial = uc.context_save()

    def execute(address, state=b'', params=b'', rate=None):
        uc.context_restore(initial)
        uc.mem_write(OBJECT, state or bytes(16))
        uc.mem_write(0x5aefe0, struct.pack('<I', OBJECT + 0x100 if rate is not None else 0))
        if rate is not None:
            uc.mem_write(OBJECT + 0x100, struct.pack('<f', rate))
        sp = STACK + 0x8000
        uc.mem_write(sp, struct.pack('<I', STOP) + params)
        uc.reg_write(UC_X86_REG_ESP, sp)
        uc.reg_write(UC_X86_REG_ECX, OBJECT)
        uc.emu_start(address, STOP, count=10000)
        # A count-limited execution must have reached its actual return address.
        from unicorn.x86_const import UC_X86_REG_EIP
        if uc.reg_read(UC_X86_REG_EIP) != STOP:
            raise RuntimeError(f'Instruction budget exhausted at {address:x}')
        return uc.reg_read(UC_X86_REG_EAX), bytes(uc.mem_read(OBJECT, len(state) or 16))

    lib = ct.CDLL(str(args.dll.resolve()))
    lib.th20_lcg_step.argtypes = [ct.c_uint32]
    lib.th20_lcg_step.restype = ct.c_uint32
    lib.th20_lcg_next.argtypes = [ct.POINTER(ct.c_uint32)]
    lib.th20_lcg_next.restype = ct.c_uint32
    lib.th20_timer_reset.argtypes = [ct.POINTER(Timer)]
    lib.th20_timer_mode.argtypes = [ct.POINTER(Timer), ct.c_uint32]
    lib.th20_timer_set.argtypes = [ct.POINTER(Timer), ct.c_int32]
    lib.th20_timer_add.argtypes = [ct.POINTER(Timer), ct.c_float, ct.POINTER(ct.c_float)]
    lib.th20_timer_tick.argtypes = [ct.POINTER(Timer), ct.POINTER(ct.c_float)]
    lib.th20_timer_tick.restype = ct.c_int32
    for n in ('reset', 'mode', 'set', 'add'):
        getattr(lib, 'th20_timer_' + n).restype = None
    lib.th20_get_mxcsr.argtypes = []
    lib.th20_get_mxcsr.restype = ct.c_uint32
    lib.th20_set_mxcsr.argtypes = [ct.c_uint32]
    lib.th20_set_mxcsr.restype = None
    saved_mxcsr = lib.th20_get_mxcsr()
    atexit.register(lib.th20_set_mxcsr, saved_mxcsr)
    lib.th20_set_mxcsr(0x1f80)
    rng = random.Random(0x20250914)
    counts, failures = {k: 0 for k in evidence}, []
    def compare(name, expected, got, context):
        counts[name] += 1
        if expected != got and len(failures) < 100:
            failures.append({'function': name, 'expected': str(expected), 'actual': str(got), 'input': str(context)})

    seeds = [0, 1, 2, 48271, 0x7ffffffe, 0x7fffffff, 0x80000000, 0xffffffff]
    seeds += [rng.getrandbits(32) for _ in range(10000)]
    for seed in seeds:
        original, _ = execute(0x422cb0, params=struct.pack('<I', seed))
        compare('lcg_step', original, lib.th20_lcg_step(seed), seed)
        original, state = execute(0x4235f0, struct.pack('<I', seed))
        updated = ct.c_uint32(seed)
        got = lib.th20_lcg_next(ct.byref(updated))
        compare('lcg_next', (original, state.hex()), (got, bytes(updated).hex()), seed)

    values = [-2147483648, -16777217, -1, 0, 1, 60, 16777217, 2147483647]
    floats = [-float('inf'), -2147483648.0, -123.25, -0.0, 0.0, 0.5,
              16777216.0, 2147483648.0, float('inf'), float('nan')]
    rates = [None, -2.0, 0.0, 0.25, 0.99, 0.99000007, 1.0, 1.0099999,
             1.01, 2.0, float('inf'), float('nan')]
    states = [struct.pack('<iifI', rng.choice(values), rng.choice(values), rng.choice(floats), flags)
              for flags in [0, 1, 2, 3, 6, 7, 0xfffffffe, 0xffffffff] for _ in range(16)]
    states += [struct.pack('<iifI', rng.randint(-1000, 1000), rng.randint(-1000, 1000),
                rng.uniform(-1000, 1000), rng.getrandbits(32)) for _ in range(128)]
    for state in states:
        original = execute(0x423f50, state)[1]
        timer = Timer.from_buffer_copy(state)
        lib.th20_timer_reset(ct.byref(timer))
        compare('timer_reset', original.hex(), bytes(timer).hex(), state.hex())
        for mode in [0, 1, 2, 3, 4, 0xffffffff]:
            original = execute(0x423fe0, state, struct.pack('<I', mode))[1]
            timer = Timer.from_buffer_copy(state)
            lib.th20_timer_mode(ct.byref(timer), mode)
            compare('timer_mode', original.hex(), bytes(timer).hex(), (state.hex(), mode))
        for value in values:
            original = execute(0x423f80, state, struct.pack('<i', value))[1]
            timer = Timer.from_buffer_copy(state)
            lib.th20_timer_set(ct.byref(timer), value)
            compare('timer_set', original.hex(), bytes(timer).hex(), (state.hex(), value))
        for rate in rates:
            rate_c = ct.c_float(rate) if rate is not None else None
            rate_p = ct.byref(rate_c) if rate_c is not None else None
            original_ret, original = execute(0x4533b0, state, rate=rate)
            timer = Timer.from_buffer_copy(state)
            got = lib.th20_timer_tick(ct.byref(timer), rate_p)
            compare('timer_tick', (original_ret, original.hex()), (got & 0xffffffff, bytes(timer).hex()), (state.hex(), rate))
            for delta in [-0.5, 0.0, 1.0, 17.25]:
                original = execute(0x4530f0, state, struct.pack('<f', delta), rate)[1]
                timer = Timer.from_buffer_copy(state)
                lib.th20_timer_add(ct.byref(timer), delta, rate_p)
                compare('timer_add', original.hex(), bytes(timer).hex(), (state.hex(), delta, rate))
    report = {
        'status': 'passed' if not failures else 'failed',
        'scope': 'Seven isolated recovered routines, not complete gameplay; default SSE/x87 environment only',
        'time_utc': datetime.now(timezone.utc).isoformat(),
        'source_exe': str(args.exe.resolve()), 'source_sha256': hashlib.sha256(raw).hexdigest(),
        'dll': str(args.dll.resolve()), 'dll_sha256': hashlib.sha256(args.dll.read_bytes()).hexdigest(),
        'emulator': 'Unicorn x86-32, actual PE machine instructions; no instruction replacement/hooks',
        'fp_environment': {'mxcsr_initial_control': '0x1f80', 'host_saved_mxcsr': hex(saved_mxcsr),
            'status_flags_compared': False, 'host_restored_on_exit': True},
        'comparisons': counts, 'total': sum(counts.values()), 'failures': failures,
        'evidence': {name: {'va': hex(a), 'end_exclusive': hex(b),
            'machine_code_sha256': hashlib.sha256(pe.get_data(a - 0x400000, b-a)).hexdigest()}
            for name, (a, b) in evidence.items()},
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    lib.th20_set_mxcsr(saved_mxcsr)
    print(json.dumps({'status': report['status'], 'total': report['total'], 'failures': failures[:3]}))
    return 1 if failures else 0

if __name__ == '__main__':
    raise SystemExit(main())
