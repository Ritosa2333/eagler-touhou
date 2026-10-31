"""Actual40d560 conversion viewport and40caa0 circle geometry/birth paths.

Constructed CPU fixtures, not uninterrupted replay or accepted goldens. Each
circle has one active native physical bullet with cancel-effect script -1, an
empty Enemy list, and an empty original item pool. Original cancellation,
item9 constructor, timer, velocity and cursor logic execute without stubs.
"""
import json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
from native_anm_resource import check_original, EXE_SHA256
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
MANAGER, ITEMS, BONUS, ENEMIES, POSITION = 0x2000000, 0x3000000, 0x2900000, 0x2901000, 0x2902000
BULLET, SP, STOP = MANAGER + 0x64, 0x100e000, 0x100f000
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                      (0x1000000, 0x10000), (MANAGER, 0x1800000)]: mu.mem_map(address, size)
mu.mem_write(0x400000, PE.get_memory_mapped_image())
mu.reg_write(UC_X86_REG_FPCW, 0x37f)
mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
def put(at, fmt, *values): mu.mem_write(at, struct.pack('<' + fmt, *values))
def get(at, fmt): return list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
def call(at, args=(), registers=None):
    put(SP, 'I' * (1 + len(args)), STOP, *args)
    mu.reg_write(UC_X86_REG_ESP, SP)
    for register, value in (registers or {}).items(): mu.reg_write(register, value)
    mu.emu_start(at, STOP, count=1000000)
    if mu.reg_read(UC_X86_REG_EIP) != STOP: raise RuntimeError('Original cancellation budget')
    return mu.reg_read(UC_X86_REG_EAX)
def f32(value): return struct.unpack('<f', struct.pack('<f', value))[0]
points = []
for x in [-194, -193.99998474121094, -192, 0, 192, 193.99998474121094, 194]:
    for y in [-2, -1.9999998807907104, -1.9409205913543701, 0, 448, 449.9999694824219, 450]:
        put(POSITION, '3f', x, y, 0)
        put(SP, 'Iff', STOP, 2, 2)
        mu.reg_write(UC_X86_REG_ESP, SP)
        mu.reg_write(UC_X86_REG_ECX, POSITION)
        mu.emu_start(0x40d560, STOP, count=1000)
        points.append({'position': [x, y], 'visible': mu.reg_read(UC_X86_REG_EAX) == 0})
put(0x4b2ed0, 'f', 1)
put(0x4b43c8, 'I', MANAGER)
put(0x4b43cc, 'I', BONUS)
put(0x4b43dc, 'I', ENEMIES)
put(0x4b44f0, 'I', ITEMS)
circles = []
for origin, radius, hitbox in [((0, 128), 10, 4), ((0, 128), 180, 6), ((147.93, 78.94), 30.6, 4)]:
    origin = [f32(v) for v in origin]
    radius, hitbox = f32(radius), f32(hitbox)
    reach = f32(radius + hitbox * .5)
    for dx, dy in [(0, 0), (reach, 0), (reach + .00002, 0), (-reach, 0), (-reach - .00002, 0),
                   (0, reach), (0, reach + .00002), (0, -reach), (reach * .6, reach * .8),
                   (reach * .6 + .00002, reach * .8)]:
        position = [f32(origin[0] + dx), f32(origin[1] + dy)]
        mu.mem_write(BULLET, bytes(0x9f8))
        mu.mem_write(ITEMS + 0x17afd4, bytes(0x9d8))
        put(ITEMS + 0x666fd8, 'ii', 0, 0)
        put(BULLET + 0x532, 'H', 1)
        put(BULLET + 0x524, 'i', -1)
        put(BULLET + 0x4bc, '3f', *position, .1)
        put(BULLET + 0x4dc, '2f', hitbox, hitbox)
        put(POSITION, '3f', *origin, .1)
        put(SP, 'Ifii', STOP, radius, 1, 0)
        mu.reg_write(UC_X86_REG_ESP, SP)
        mu.reg_write(UC_X86_REG_EBX, POSITION)
        mu.emu_start(0x40caa0, STOP, count=1000000)
        if mu.reg_read(UC_X86_REG_EIP) != STOP: raise RuntimeError('Circle budget')
        item = ITEMS + 0x17afd4
        circles.append({'position': position, 'origin': origin, 'radius': radius, 'hitbox': hitbox,
                        'cancelled': get(BULLET + 0x532, 'H')[0] == 3,
                        'converted': get(item + 0x9b0, 'i')[0] != 0,
                        'pointPosition': get(item + 0x96c, '2f'), 'pointDelay': get(item + 0x9c0, 'i')[0],
                        'pointCursor': get(ITEMS + 0x666fd8, 'i')[0]})
output = ROOT / 'tests/fixtures/bullet-cancellation-geometry-native.json'
output.write_text(json.dumps({'schema': 'th12-native-bullet-cancellation-geometry/1',
    'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False, 'gpuExecuted': False,
    'boundary': 'Actual40d560 with2px extents and complete40caa0/40c8b0/4273f0/414f40. One synthetic slot0 bullet per circle, phase1, countdown0, cancel-effect script-1, blank Enemy list and item9 pool. No instruction replacements, no accepted replay golden.',
    'points': points, 'circles': circles}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'pointCases': len(points), 'circleCases': len(circles), 'output': str(output)}))
