"""Actual40a618..40a6aa emitter radius placement, before bullet movement.

Synthetic shooter origins/radii/angles are inputs. Original464640 normalization
and40d640 polar arithmetic execute; no gameplay result is substituted. This
isolated CPU geometry fixture does not exercise allocation, ANM or rendering.
"""
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
from native_anm_resource import check_original, EXE_SHA256
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32
from unicorn.x86_const import *

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for at, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                 (0x1000000, 0x10000), (0x2000000, 0x10000)]:
    mu.mem_map(at, size)
mu.mem_write(0x400000, PE.get_memory_mapped_image())
BULLET, SHOOTER, STACK = 0x2000000, 0x2002000, 0x100e000


def put(at, fmt, *values):
    mu.mem_write(at, struct.pack('<' + fmt, *values))


def get(at, fmt):
    return list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))


rows = []
for origin in [(0, 128), (95.02999877929688, 128.0500030517578), (-143.25, 13.75)]:
    for radius in [0, 10, 64]:
        for angle in [-7.5, -3.1415927410125732, -.16517972946166992, 0, .75, 3.5, 7.5]:
            mu.mem_write(BULLET, bytes(0xa00))
            mu.mem_write(SHOOTER, bytes(0x214))
            mu.mem_write(STACK - 0x100, bytes(0x200))
            put(SHOOTER + 4, '3f', *origin, 0)
            put(SHOOTER + 0x20, 'f', radius)
            put(STACK + 0x10, 'f', 0)
            put(STACK + 0x28, 'f', angle)
            for register, value in [(UC_X86_REG_ESP, STACK), (UC_X86_REG_EBX, BULLET),
                                    (UC_X86_REG_EBP, SHOOTER), (UC_X86_REG_FPCW, 0x37f),
                                    (UC_X86_REG_FPSW, 0), (UC_X86_REG_FPTAG, 0xffff),
                                    (UC_X86_REG_MXCSR, 0x1f80)]:
                mu.reg_write(register, value)
            mu.emu_start(0x40a618, 0x40a6aa, count=2000)
            if mu.reg_read(UC_X86_REG_EIP) != 0x40a6aa:
                raise RuntimeError('Original radius placement instruction budget')
            rows.append({'origin': get(SHOOTER + 4, '2f'),
                         'radius': get(SHOOTER + 0x20, 'f')[0],
                         'rawAngle': get(STACK + 0x28, 'f')[0],
                         'storedAngle': get(BULLET + 0x4d8, 'f')[0],
                         'position': get(BULLET + 0x4bc, '2f')})
out = ROOT / 'tests/fixtures/bullet-radius-birth-native.json'
out.write_text(json.dumps({'schema': 'th12-native-bullet-radius-birth/1',
    'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False,
    'source': 'Actual40a618..40a6aa, including464640 and40d640. Synthetic geometry inputs; allocation/ANM/rendering excluded.',
    'rows': rows}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'cases': len(rows), 'output': str(out)}))
