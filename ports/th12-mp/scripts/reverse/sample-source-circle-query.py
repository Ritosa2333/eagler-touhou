"""Retail439ed0 Source circle comparison and write-order samples, query only.

Prepared Player projectiles are inactive; the separate406de0 Bomb query returns
zero at an explicit boundary. Original Source iteration/cadence, x87 distance,
damage/limit side effects, total cap and score update run. No source update,
weapon update, replay, candidate routine or translated game code executes.
"""
import hashlib
import json
import math
import pathlib
import random
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_WRITE
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
PLAYER, SOURCE, POSITION, BBOX = 0x2000000, 0x2008988, 0x2020000, 0x2020010
SP, STOP = 0x100f000, 0x100ff00


def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


def adjacent(value, step):
    bits = struct.unpack('<I', struct.pack('<f', value))[0]
    return struct.unpack('<f', struct.pack('<I', bits + step))[0]


def rounded_delta_formula(sx, sy, x, y, radius):
    # Input selection / diagnostic of the old candidate formula only. Native
    # fixture expected values always come from the retail executable below.
    dx, dy = f32(sx - x), f32(sy - y)
    squared = f32(dx * dx + dy * dy)
    return radius * radius >= squared, squared


class Native:
    def __init__(self):
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        for address, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                              (0x1000000, 0x10000), (PLAYER, 0x40000)]:
            self.mu.mem_map(address, size)
        self.mu.mem_write(0x400000, PE.get_memory_mapped_image())
        self.mu.reg_write(UC_X86_REG_FPCW, 0x37f)
        self.mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        self.put(0x4b4514, 'I', PLAYER)
        self.put(0x4b2ed0, 'f', 1)
        self.put(0x4d52d4, 'I', 1)
        self.writes = []
        self.distances = []
        self.floor_calls = 0
        self.bomb_calls = 0
        for address in [0x406de0, 0x43a3c4, 0x465320, 0x493290]:
            self.mu.hook_add(UC_HOOK_CODE, self.hook, begin=address, end=address)
        self.mu.hook_add(UC_HOOK_MEM_WRITE, self.write, begin=SOURCE, end=SOURCE + 128 * 0x74 - 1)

    def put(self, address, fmt, *values):
        self.mu.mem_write(address, struct.pack('<' + fmt, *values))

    def get(self, address, fmt):
        return list(struct.unpack('<' + fmt, self.mu.mem_read(address, struct.calcsize('<' + fmt))))

    def seed(self, sources):
        self.mu.mem_write(PLAYER, bytes(0xc59c))
        self.put(PLAYER + 0xa30, '2i', 0, 1)
        self.put(0x4b0c44, 'i', 0)
        for slot, source in enumerate(sources):
            pointer = SOURCE + slot * 0x74
            self.put(pointer, 'f', source['radius'])
            self.put(pointer + 0x18, '3f', *source['center'])
            self.put(pointer + 0x4c, '2i', source['previousTicks'], source['ticks'])
            self.put(pointer + 0x60, '4i', source['damage'], source['accumulated'], source['limit'], source['cadence'])
            self.put(pointer + 0x70, 'I', source['flags'])

    def snapshot(self, count):
        result = []
        for slot in range(count):
            pointer = SOURCE + slot * 0x74
            result.append({'center': self.get(pointer + 0x18, '3f'),
                           'centerBits': self.get(pointer + 0x18, '3I'),
                           'radius': self.get(pointer, 'f')[0],
                           'previousTicks': self.get(pointer + 0x4c, 'i')[0],
                           'ticks': self.get(pointer + 0x50, 'i')[0],
                           'damage': self.get(pointer + 0x60, 'i')[0],
                           'accumulated': self.get(pointer + 0x64, 'i')[0],
                           'limit': self.get(pointer + 0x68, 'i')[0],
                           'cadence': self.get(pointer + 0x6c, 'i')[0],
                           'flags': self.get(pointer + 0x70, 'I')[0]})
        return {'sources': result, 'scoreStored': self.get(0x4b0c44, 'i')[0]}

    def hook(self, mu, address, size, unused):
        stack = self.mu.reg_read(UC_X86_REG_ESP)
        if address == 0x406de0:
            self.bomb_calls += 1
            target = self.get(stack, 'I')[0]
            self.mu.reg_write(UC_X86_REG_ESP, stack + 8)
            self.mu.reg_write(UC_X86_REG_EAX, 0)
            self.mu.reg_write(UC_X86_REG_EIP, target)
        elif address == 0x43a3c4:
            pointer = self.mu.reg_read(UC_X86_REG_EBX)
            self.distances.append({'slot': (pointer - SOURCE) // 0x74,
                                   'distanceSquared': self.get(stack + 0x2c, 'f')[0]})
        else:
            self.floor_calls += 1

    def write(self, mu, access, address, size, value, unused):
        slot, offset = divmod(address - SOURCE, 0x74)
        self.writes.append({'slot': slot, 'offset': hex(offset),
                            'instruction': hex(self.mu.reg_read(UC_X86_REG_EIP)),
                            'previousBits': int.from_bytes(self.mu.mem_read(address, size), 'little'),
                            'writtenBits': value & ((1 << (size * 8)) - 1), 'size': size})

    def query(self, position, bbox, count):
        self.put(POSITION, '3f', *position)
        self.put(BBOX, '2f', *bbox)
        inputs = bytes(self.mu.mem_read(POSITION, 0x18))
        before = self.snapshot(count)
        self.writes.clear()
        self.distances.clear()
        self.floor_calls = self.bomb_calls = 0
        self.put(SP, '3I', STOP, POSITION, BBOX)
        self.mu.reg_write(UC_X86_REG_ESP, SP)
        self.mu.emu_start(0x439ed0, STOP, count=1000000)
        if self.mu.reg_read(UC_X86_REG_EIP) != STOP:
            raise RuntimeError('Original439ed0 instruction budget')
        raw = self.mu.reg_read(UC_X86_REG_EAX)
        damage = struct.unpack('<i', struct.pack('<I', raw))[0]
        after = self.snapshot(count)
        if bytes(self.mu.mem_read(POSITION, 0x18)) != inputs:
            raise RuntimeError('Original query unexpectedly modified the query coordinate/bbox')
        if self.floor_calls:
            raise RuntimeError('Original query unexpectedly executed coordinate floor')
        for old, new in zip(before['sources'], after['sources']):
            if old['centerBits'] != new['centerBits'] or old['radius'] != new['radius']:
                raise RuntimeError('Original query unexpectedly changed source center/radius')
        return {'position': self.get(POSITION, '3f'), 'bbox': self.get(BBOX, '2f'),
                'nativeDamage': damage, 'before': before, 'after': after,
                'distanceSamples': self.distances.copy(), 'writes': self.writes.copy(),
                'floorCalls': self.floor_calls, 'separateBombQueryCalls': self.bomb_calls}


def source(center=(0, 0, 0), radius=56, damage=7, accumulated=0, limit=999999, ticks=11, previous=10, cadence=4, flags=3):
    return {'center': list(center), 'radius': radius, 'damage': damage,
            'accumulated': accumulated, 'limit': limit, 'ticks': ticks,
            'previousTicks': previous, 'cadence': cadence, 'flags': flags}


native = Native()
cases = []
for radius in [0., 5., 32., 56.]:
    for x in [radius, adjacent(radius, 1), adjacent(radius, -1) if radius else 0.]:
        for bbox in [[0, 0], [24, 24], [1000, 1000]]:
            sources = [source(radius=radius)]
            native.seed(sources)
            cases.append({'name': 'axis-radius-boundary', 'sources': sources,
                          'queries': [native.query([x, 0, 0], bbox, 1)]})
for ticks, previous, cadence, flags in [(12, 11, 4, 3), (12, 12, 4, 3), (11, 10, 4, 3),
                                      (11, 10, 1, 3), (11, 11, 1, 3), (0, 0, 4, 3),
                                      (-4, -5, 4, 3), (-3, -4, 4, 3), (11, 10, 4, 2)]:
    sources = [source(ticks=ticks, previous=previous, cadence=cadence, flags=flags)]
    native.seed(sources)
    cases.append({'name': 'cadence-and-active-gate', 'sources': sources,
                  'queries': [native.query([0, 0, 0], [24, 24], 1)]})
for sources in [[source(limit=15)], [source(damage=40, limit=45), source(damage=60, limit=70)],
                [source(damage=-3, accumulated=10, limit=8)], [source(damage=0, accumulated=10, limit=8)]]:
    native.seed(sources)
    cases.append({'name': 'repeated-query-and-budget', 'sources': sources,
                  'queries': [native.query([0, 0, 0], [24, 24], len(sources)) for _ in range(4)]})

# Choose finite gameplay-range inputs that distinguish subtraction rounded
# before squaring from the original x87 expression. Selection is not an oracle.
selector = random.Random(12061327)
for _ in range(100000):
    sx, sy, x, y = [f32(selector.uniform(-200, 200)) for _ in range(4)]
    unrounded_sum = f32((sx - x) ** 2 + (sy - y) ** 2)
    radius = f32(math.sqrt(unrounded_sum))
    predicted_hit, predicted_sum = rounded_delta_formula(sx, sy, x, y, radius)
    if predicted_sum == unrounded_sum or predicted_hit == (radius * radius >= unrounded_sum):
        continue
    sources = [source(center=[sx, sy, 0], radius=radius)]
    native.seed(sources)
    query = native.query([x, y, 0], [24, 24], 1)
    cases.append({'name': 'premature-delta-rounding', 'sources': sources,
                  'oldRoundedDeltaDiagnostic': {'damage': 7 if predicted_hit else 0,
                                                'distanceSquared': predicted_sum}, 'queries': [query]})
    if sum(case['name'] == 'premature-delta-rounding' for case in cases) == 16:
        break

result = {'schema': 'th12-native-source-circle-query/1', 'originalExeSha256': EXE_SHA256,
          'partial': True, 'acceptedReplayGolden': False, 'wholeGameOracle': False,
          'provider': 'Original439ed0 Source circle branch43a3a4..43a3cf, cadence/budget/cap/score; original query only',
          'preparedInputs': ['Inactive256 friendly projectile slots', 'Prepared1..2 active Source records, stride74 atPlayer+8988',
                             'Player+a30=0/a34=1 opens the query epoch gate', 'FPCW037f/MXCSR1f80/SSE2 available'],
          'externalBoundaries': ['406de0 separate Bomb subsystem damage returns0; no Source or geometry boundary replaced'],
          'addresses': {'epochGate': '0x439ef2', 'sourceActiveGate': '0x43a21f',
                        'cadence': '0x43a228..0x43a235', 'circleFlagDispatch': '0x43a23b',
                        'circleDistance': '0x43a3a4..0x43a3c0', 'comparison': '0x43a3c8..0x43a3cf',
                        'totalAdd': '0x43a3d4', 'accumulatedWrite': '0x43a3d8',
                        'limitCompare': '0x43a3de', 'damageZeroWrite': '0x43a3e3',
                        'damageCap80': '0x43a40b..0x43a418', 'scoreWrite': '0x43a452'},
          'sourceOffsets': {'radius': '0x00', 'centerXYZ': '0x18', 'previousTicks': '0x4c',
                            'ticks': '0x50', 'damage': '0x60', 'accumulated': '0x64',
                            'limit': '0x68', 'cadence': '0x6c', 'flags': '0x70'},
          'cases': cases}
output = ROOT / 'tests/fixtures/source-circle-query-native.json'
output.write_text(json.dumps(result, indent=2, allow_nan=False) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': len(cases),
                  'queries': sum(len(case['queries']) for case in cases),
                  'prematureDeltaRoundingCases': sum(case['name'] == 'premature-delta-rounding' for case in cases),
                  'mismatchExamples': [{ 'nativeDamage': case['queries'][0]['nativeDamage'],
                                         'oldRoundedDeltaDamage': case['oldRoundedDeltaDiagnostic']['damage'],
                                         'nativeDistance': case['queries'][0]['distanceSamples']}
                                        for case in cases if case['name'] == 'premature-delta-rounding'][:2]}))
