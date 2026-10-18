"""Independent raw-bit differential check against selected original x86 routines.

Only Timer object bytes and tick return bits are compared. Floating-point status
flags, exceptions, concurrency, and full gameplay are outside this test's scope.
The original rate is written directly from uint32 bits. C++ add uses a uint32
adapter, so Python/ctypes cannot quiet a signaling NaN while marshaling a float.
"""
from pathlib import Path
import argparse
import ctypes as ct
import hashlib
import itertools
import json
import random
import struct
import sys

ROOT = Path(__file__).resolve().parents[1]
for dep in ('binary_python', 'emulation_python'):
    sys.path.insert(0, str(ROOT / '.cache' / dep))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import (UC_X86_REG_ESP, UC_X86_REG_ECX, UC_X86_REG_EAX,
    UC_X86_REG_EIP, UC_X86_REG_EFLAGS, UC_X86_REG_FPCW, UC_X86_REG_FPTAG,
    UC_X86_REG_MXCSR)

EXPECTED_SHA256 = 'a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897'
SOURCE = Path(r'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe')
STACK, OBJECT, STOP = 0x1000000, 0x2000000, 0x3000000
DEFAULT_MXCSR = 0x1f80


class Timer(ct.Structure):
    _fields_ = [('previous', ct.c_int32), ('current', ct.c_int32),
                ('current_f', ct.c_float), ('flags', ct.c_uint32)]


def is_nan_bits(bits):
    return bits & 0x7f800000 == 0x7f800000 and bits & 0x007fffff != 0


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--exe', type=Path, default=SOURCE)
    ap.add_argument('--dll', type=Path, default=ROOT / 'build/native_recovered/Release/th20_native_recovered.dll')
    ap.add_argument('--output', type=Path, default=ROOT / 'reports/native_edges_validation.json')
    args = ap.parse_args()
    raw = args.exe.read_bytes()
    digest = hashlib.sha256(raw).hexdigest()
    if digest != EXPECTED_SHA256:
        raise ValueError('Native addresses belong to a different executable revision')
    pe = pefile.PE(data=raw)
    uc = Uc(UC_ARCH_X86, UC_MODE_32)
    uc.mem_map(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)
    uc.mem_write(0x400000, pe.get_memory_mapped_image())
    for address in (STACK, OBJECT, STOP):
        uc.mem_map(address, 0x10000)
    uc.reg_write(UC_X86_REG_EFLAGS, 2)
    uc.reg_write(UC_X86_REG_FPCW, 0x37f)
    uc.reg_write(UC_X86_REG_FPTAG, 0xffff)
    uc.reg_write(UC_X86_REG_MXCSR, DEFAULT_MXCSR)
    initial = uc.context_save()
    lib = ct.CDLL(str(args.dll.resolve()))
    lib.th20_get_mxcsr.argtypes = []
    lib.th20_get_mxcsr.restype = ct.c_uint32
    lib.th20_set_mxcsr.argtypes = [ct.c_uint32]
    lib.th20_set_mxcsr.restype = None
    lib.th20_timer_add_bits.argtypes = [ct.POINTER(Timer), ct.c_uint32, ct.POINTER(ct.c_float)]
    lib.th20_timer_add_bits.restype = None
    lib.th20_timer_tick.argtypes = [ct.POINTER(Timer), ct.POINTER(ct.c_float)]
    lib.th20_timer_tick.restype = ct.c_int32
    host_mxcsr_saved = lib.th20_get_mxcsr()

    def execute(name, state, rate_bits, delta_bits):
        uc.context_restore(initial)
        uc.mem_write(OBJECT, state)
        uc.mem_write(0x5aefe0, struct.pack('<I', OBJECT + 0x100 if rate_bits is not None else 0))
        if rate_bits is not None:
            uc.mem_write(OBJECT + 0x100, struct.pack('<I', rate_bits))
        params = b'' if name == 'timer_tick' else struct.pack('<I', delta_bits)
        uc.mem_write(STACK + 0x8000, struct.pack('<I', STOP) + params)
        uc.reg_write(UC_X86_REG_ESP, STACK + 0x8000)
        uc.reg_write(UC_X86_REG_ECX, OBJECT)
        uc.emu_start(0x4533b0 if name == 'timer_tick' else 0x4530f0, STOP, count=10000)
        if uc.reg_read(UC_X86_REG_EIP) != STOP:
            raise RuntimeError('Original routine did not reach its return address')
        expected = bytes(uc.mem_read(OBJECT, 16))
        expected_ret = uc.reg_read(UC_X86_REG_EAX) if name == 'timer_tick' else None
        timer = Timer.from_buffer_copy(state)
        rate_storage = ct.c_uint32(rate_bits) if rate_bits is not None else None
        rate_p = ct.cast(ct.byref(rate_storage), ct.POINTER(ct.c_float)) if rate_storage is not None else None
        lib.th20_set_mxcsr(DEFAULT_MXCSR)
        if name == 'timer_tick':
            actual_ret = lib.th20_timer_tick(ct.byref(timer), rate_p) & 0xffffffff
        else:
            lib.th20_timer_add_bits(ct.byref(timer), delta_bits, rate_p)
            actual_ret = None
        return expected, expected_ret, bytes(timer), actual_ret

    patterns = [0, 0x80000000, 1, 0x80000001, 0x007fffff, 0x00800000,
        0x3f800000, 0x7f7fffff, 0xff7fffff, 0x7f800000, 0xff800000,
        0x7f800001, 0x7fa12345, 0x7fc01234, 0xffc01234]
    counts = {'timer_add': 0, 'timer_tick': 0}
    failures, failure_count, nan_payload_only_count = [], 0, 0

    def compare(name, current_bits, rate_bits, delta_bits, integer=7):
        nonlocal failure_count, nan_payload_only_count
        state = struct.pack('<iiII', -1, integer, current_bits, 1)
        expected, expected_ret, actual, actual_ret = execute(name, state, rate_bits, delta_bits)
        counts[name] += 1
        if expected == actual and expected_ret == actual_ret:
            return
        failure_count += 1
        expected_float = struct.unpack_from('<I', expected, 8)[0]
        actual_float = struct.unpack_from('<I', actual, 8)[0]
        nan_only = (expected[:8] == actual[:8] and expected[12:] == actual[12:]
            and expected_ret == actual_ret and is_nan_bits(expected_float) and is_nan_bits(actual_float))
        nan_payload_only_count += int(nan_only)
        failures.append({'function': name, 'current_bits': hex(current_bits),
            'rate_bits': hex(rate_bits) if rate_bits is not None else None,
            'delta_bits': hex(delta_bits) if delta_bits is not None else None,
            'original_object': expected.hex(), 'cpp_object': actual.hex(),
            'original_return_bits': expected_ret, 'cpp_return_bits': actual_ret,
            'nan_payload_only': nan_only})

    try:
        for current, rate in itertools.product(patterns, [None] + patterns):
            compare('timer_tick', current, rate, None)
            for delta in patterns:
                compare('timer_add', current, rate, delta)
        rng = random.Random(0x20e11)
        for _ in range(3000):
            current, rate, delta = [rng.getrandbits(32) for _ in range(3)]
            compare('timer_add', current, rate, delta)
            compare('timer_tick', current, rate, None)
    finally:
        lib.th20_set_mxcsr(host_mxcsr_saved)
    result = {
        'status': 'passed' if not failure_count else 'failed',
        'scope': 'Timer object bytes and tick return bits only; FP status flags, exceptions and concurrency are not compared',
        'source_sha256': digest, 'dll': str(args.dll.resolve()),
        'dll_sha256': hashlib.sha256(args.dll.read_bytes()).hexdigest(),
        'emulator': 'Unicorn x86-32 running original instructions including helper calls',
        'fp_environment': {'emulator_mxcsr_initial': hex(DEFAULT_MXCSR),
            'host_mxcsr_before_each_call': hex(DEFAULT_MXCSR),
            'host_mxcsr_saved_and_restored': hex(host_mxcsr_saved), 'emulator_x87_control': '0x37f'},
        'raw_float_marshaling': 'uint32_t delta adapter and raw uint32_t rate pointer; no Python float conversions',
        'patterns_hex': [hex(p) for p in patterns], 'random_seed': '0x20e11', 'random_triples': 3000,
        'comparisons': counts, 'total': sum(counts.values()), 'failure_count': failure_count,
        'nan_payload_only_count': nan_payload_only_count, 'failures_first_100': failures[:100],
        'failures_all': failures,
        'interpretation': 'Any mismatch is reported. NaN payload differences need an independent hardware oracle before attributing them to the C++ implementation or emulator.'
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2), encoding='utf-8')
    print(json.dumps({k: result[k] for k in ('status', 'total', 'failure_count', 'nan_payload_only_count')}))
    return 1 if failure_count else 0


if __name__ == '__main__':
    raise SystemExit(main())
