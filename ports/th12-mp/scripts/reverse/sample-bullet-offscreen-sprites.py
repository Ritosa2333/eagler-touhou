"""Actual409900 raw-sprite bounds and complete4099e0 sprite-change tails.

Synthetic ANM sprite tables/script isolate the dimensions used by the original
tail. Actual binding, sprite selection, bytecode update and4099e0 execute with
no game instruction substitutions. Direct4099e0 does not include40a020's age
advance; this fixture compares bounds, phase, raw sprite and ANM-tail visits.
"""
import json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
sys.path.insert(0, str(ROOT / 'scripts/reverse'))
from native_anm_resource import check_original, EXE_SHA256, install_resource
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *
EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                      (0x1000000, 0x10000), (0x2000000, 0x1000000)]: mu.mem_map(address, size)
mu.mem_write(0x400000, PE.get_memory_mapped_image())
mu.reg_write(UC_X86_REG_FPCW, 0x37f)
mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
BULLET, PLAYER, RESOURCE, POSITION, SP, STOP = 0x2000000, 0x2010000, 0x2600000, 0x2020000, 0x100e000, 0x100f000
def put(at, fmt, *values): mu.mem_write(at, struct.pack('<' + fmt, *values))
def get(at, fmt): return list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
def f32(value): return struct.unpack('<f', struct.pack('<f', value))[0]
def adjacent(value, direction):
    raw = struct.unpack('<I', struct.pack('<f', value))[0]
    if value < 0: direction = -direction
    return struct.unpack('<f', struct.pack('<I', raw + direction))[0]
def call(address, arguments=(), registers=None):
    put(SP, 'I' * (len(arguments) + 1), STOP, *arguments)
    mu.reg_write(UC_X86_REG_ESP, SP)
    for register, value in (registers or {}).items(): mu.reg_write(register, value)
    mu.emu_start(address, STOP, count=100000)
    if mu.reg_read(UC_X86_REG_EIP) != STOP: raise RuntimeError(f'Original bullet bounds budget {address:x}')
    return mu.reg_read(UC_X86_REG_EAX)
points = []
for width, height in [(16., 14.), (30., 30.), (64., 64.), (8., 8.), (14., 16.), (0., 0.), (.000001, .000001)]:
    width, height = f32(width), f32(height)
    for edge, center in [('left', (-192. - width / 2, 120.)), ('right', (192. + width / 2, 120.)),
                         ('top', (0., -64. - height / 2)), ('bottom', (0., 448. + height / 2))]:
        center = list(map(f32, center))
        axis = 0 if edge in ['left', 'right'] else 1
        for direction in [-1, 0, 1]:
            position = center.copy()
            if direction: position[axis] = adjacent(position[axis], direction)
            put(POSITION, '3f', *position, 0.)
            put(SP, 'Iff', STOP, width, height)
            mu.reg_write(UC_X86_REG_ESP, SP)
            mu.reg_write(UC_X86_REG_ECX, POSITION)
            mu.emu_start(0x409900, STOP, count=1000)
            points.append({'edge': edge, 'position': position, 'dimensions': [width, height],
                           'outside': bool(mu.reg_read(UC_X86_REG_EAX))})
for dimensions in [[16., 14.], [30., 30.]]:
    put(POSITION, '3f', 201.15533447265625, 0., 0.)
    put(SP, 'Iff', STOP, *dimensions)
    mu.reg_write(UC_X86_REG_ESP, SP)
    mu.reg_write(UC_X86_REG_ECX, POSITION)
    mu.emu_start(0x409900, STOP, count=1000)
    points.append({'edge': 'actual4152', 'position': [201.15533447265625, 0.], 'dimensions': dimensions,
                   'outside': bool(mu.reg_read(UC_X86_REG_EAX))})

def instruction(op, words=(), time=0, mask=0):
    return struct.pack('<HHhH', op & 65535, 8 + len(words) * 4, time, mask) + struct.pack('<' + 'i' * len(words), *words)
def synthetic_bank(sprite):
    header = bytearray(80)
    struct.pack_into('<IHH', header, 0, 7, 2, 1)
    struct.pack_into('<HH', header, 10, 64, 64)
    struct.pack_into('<2I', header, 64, 80, 100)
    struct.pack_into('<iI', header, 72, 0, 120)
    sprites = struct.pack('<I4f', 0, 0., 0., 30., 30.) + struct.pack('<I4f', 1, 0., 0., 16., 14.)
    code = instruction(6, (10000, 1), mask=1)
    if sprite: code += instruction(3, (0,)) + instruction(3, (1,), time=1)
    code += instruction(63, time=1 if sprite else 0) + instruction(-1)
    return bytes(header) + sprites + code

tail_visits = 0
def observe(uc, address, size, user):
    global tail_visits
    tail_visits += 1
mu.hook_add(UC_HOOK_CODE, observe, begin=0x455630, end=0x455630)
put(0x4b2ed0, 'f', 1.)
put(0x4b4514, 'I', PLAYER)
put(0x4b43e4, 'I', 0)
put(0x4ce8cc, 'I', 0x2500000)
put(PLAYER + 0xa28, 'i', 1)
put(PLAYER + 0x9cc, '6f', 4999., 4999., 0., 5001., 5001., 0.)
lifecycles = []
for name, has_sprite in [('large-spawn-to-small-body', True), ('missing-sprite', False)]:
    mu.mem_write(BULLET, bytes(0x9f8))
    raw = synthetic_bank(has_sprite)
    install_resource(mu, raw, RESOURCE)
    call(0x454d10, (BULLET + 8, 0), {UC_X86_REG_ECX: RESOURCE})
    put(BULLET, 'I', 2)
    put(BULLET + 0x532, 'H', 2)
    put(BULLET + 0x4bc, '3f', 201.15533447265625, 0., 0.)
    put(BULLET + 0x4dc, '2f', 4., 4.)
    put(BULLET + 0x4e8, 'i', 9)
    put(BULLET + 0x524, 'i', -1)
    put(BULLET + 0x520, 'i', 0)
    def snap(pass_number):
        sprite = get(BULLET + 0x3fc, 'I')[0]
        return {'pass': pass_number, 'phase': get(BULLET + 0x532, 'H')[0],
                'sprite': get(BULLET + 8 + 0x3e4, 'h')[0],
                'dimensions': [get(sprite + 0x38, 'f')[0], get(sprite + 0x34, 'f')[0]] if sprite else None,
                'tailVisits': tail_visits, 'clock': get(BULLET + 8 + 0x6c, 'i')[0]}
    tail_visits = 0
    frames = [snap(0)]
    for pass_number in range(1, 4):
        if get(BULLET + 0x532, 'H')[0] != 0: call(0x4099e0, (BULLET,))
        frames.append(snap(pass_number))
    lifecycles.append({'name': name, 'position': [201.15533447265625, 0.], 'type': 9,
                        'phase': 2, 'age': 9, 'syntheticBankHex': raw.hex(), 'frames': frames})
output = ROOT / 'tests/fixtures/bullet-offscreen-sprites-original.json'
output.write_text(json.dumps({'schema': 'th12-original-bullet-offscreen-sprites/1',
    'originalExeSha256': EXE_SHA256, 'wholeGameOracle': False, 'gpuExecuted': False,
    'scope': 'Actual409900 queries; complete4099e0 with actual454d10/454b80/455630 on '
             'synthetic ANM(sprite30x30→16x14 and no sprite) and far-away Player. '
             'Read-only455630 hook counts visits; no game instructions replaced. '
             '40a020 manager age advance is outside this direct-update fixture.',
    'points': points, 'lifecycles': lifecycles}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'points': len(points),
                  'lifecycles': {r['name']: r['frames'] for r in lifecycles}}))
