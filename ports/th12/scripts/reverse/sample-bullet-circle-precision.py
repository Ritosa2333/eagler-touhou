"""Actual437980 circle boundaries, with unrounded x87 coordinate differences.

The complete query executes without instruction substitutions. Synthetic Player
storage has positive invulnerability, so actual hit geometry returns 1 while its
native gate avoids invoking the downstream miss scene. This is a local geometry
oracle, not a replay or a whole-game oracle.
"""
import json, math, pathlib, random, struct, sys
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
mu = Uc(UC_ARCH_X86, UC_MODE_32)
for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                      (0x1000000, 0x10000), (0x2000000, 0x30000)]:
    mu.mem_map(address, size)
mu.mem_write(0x400000, PE.get_memory_mapped_image())
mu.reg_write(UC_X86_REG_FPCW, 0x37f)
mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
PLAYER, SHT, POSITION, SP, STOP = 0x2000000, 0x2010000, 0x2011000, 0x100e000, 0x100f000
def put(at, fmt, *values): mu.mem_write(at, struct.pack('<' + fmt, *values))
def get(at, fmt): return list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
def f32(value): return struct.unpack('<f', struct.pack('<f', value))[0]
def bits(value): return struct.unpack('<I', struct.pack('<f', value))[0]
def offset_ulp(value, count):
    raw = bits(value)
    if value < 0: count = -count
    return struct.unpack('<f', struct.pack('<I', raw + count))[0]
def expected(center, player, hit, radius, early_rounding=False):
    dx, dy = (player[0] - center[0]), (player[1] - center[1])
    if early_rounding: dx, dy = f32(dx), f32(dy)
    distance = f32(dx * dx + dy * dy)
    if hit * hit + radius * radius > distance: return 1
    graze = max(40., f32(radius / 2.5))
    if (hit + graze) ** 2 + radius * radius > distance: return 2
    return 0

captured = []
def read_distance(uc, address, size, user):
    captured.append(get(mu.reg_read(UC_X86_REG_ESP) + 4, 'f')[0])
mu.hook_add(UC_HOOK_CODE, read_distance, begin=0x4379b1, end=0x4379b1)
put(0x4b4514, 'I', PLAYER)
put(0x4b43e4, 'I', 0)
put(PLAYER + 0xa2c, 'I', SHT)
put(PLAYER + 0xa28, 'i', 1)
put(PLAYER + 0xc404, 'i', 1)
put(PLAYER + 0xc414, 'I', 0)
rows = []
def sample(label, center, player, hit, radius=14.):
    center, player = list(map(f32, center)), list(map(f32, player))
    put(PLAYER + 0x97c, '3f', *player, 0)
    put(SHT + 4, 'f', hit)
    put(POSITION, '3f', *center, 0)
    put(SP, 'II', STOP, bits(radius))
    mu.reg_write(UC_X86_REG_ESP, SP)
    mu.reg_write(UC_X86_REG_EAX, POSITION)
    captured.clear()
    mu.emu_start(0x437980, STOP, count=100000)
    if mu.reg_read(UC_X86_REG_EIP) != STOP or len(captured) != 1:
        raise RuntimeError('Circle query instruction budget or distance capture')
    code = mu.reg_read(UC_X86_REG_EAX)
    assert code == expected(center, player, hit, radius), (label, code, center, player, hit)
    assert captured[0] == f32((player[0] - center[0]) ** 2 + (player[1] - center[1]) ** 2)
    rows.append({'case': label, 'center': center, 'player': player, 'radius': radius,
                 'playerHit': hit, 'distanceSquared': captured[0], 'nativeReturn': code,
                 'earlyRoundedReturn': expected(center, player, hit, radius, True)})

for hit in [2., 3.5, 3.]:
    for name, point in [('hit-equality', (14., hit)), ('graze-equality', (hit + 40., 14.)),
                        ('center', (0., 0.)), ('outside', (60., 60.))]:
        for shift in [-1, 0, 1] if 'equality' in name else [0]:
            point2 = (offset_ulp(point[0], shift), point[1])
            sample(f'hit{hit}-{name}-{shift}', (0., 120.), (point2[0], point2[1] + 120.), hit)

# Deterministically select disagreements in the two threshold calculations.
# Every selected position is then tested by actual437980, independent of CPP.
rng = random.Random(0x437980)
for hit in [2., 3.5, 3.]:
    for boundary in ['hit', 'graze']:
        threshold = 14. ** 2 + (hit if boundary == 'hit' else hit + 40.) ** 2
        found = 0
        for attempt in range(200000):
            center = [f32(rng.uniform(-100., 100.)), f32(rng.uniform(40., 400.))]
            angle = rng.uniform(-math.pi, math.pi)
            player = [f32(center[0] + math.cos(angle) * math.sqrt(threshold)),
                      f32(center[1] + math.sin(angle) * math.sqrt(threshold))]
            if expected(center, player, hit, 14.) == expected(center, player, hit, 14., True): continue
            sample(f'hit{hit}-{boundary}-precision-{found}', center, player, hit)
            found += 1
            if found == 8: break
        assert found == 8, (hit, boundary, found)

output = ROOT / 'tests/fixtures/bullet-circle-precision-original.json'
output.write_text(json.dumps({'schema': 'th12-original-bullet-circle-precision/1',
    'originalExeSha256': EXE_SHA256, 'entry': '0x437980', 'wholeGameOracle': False,
    'boundary': 'Complete original437980 query without game instruction substitutions. '
                'Read-only4379b1 captures the distance store. Synthetic Player/SHT: '
                'state1, invulnerability1, flags0, no dialogue; original hit branch '
                'returns1 without calling downstream438370. Radius14 covers both '
                'effect2 types26/27; three original character hit radii2/3.5/3.',
    'rows': rows}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'cases': len(rows), 'changed': sum(r['nativeReturn'] != r['earlyRoundedReturn'] for r in rows),
                  'returns': {code: sum(r['nativeReturn'] == code for r in rows) for code in [0, 1, 2]},
                  'output': str(output)}))
