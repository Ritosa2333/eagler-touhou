"""Native437980 circle hit/graze geometry and Player gate fixture.

All query instructions execute from the fixed retail EXE. 438370 only records
the boundary call; its death transition/ANM effects are independently exercised
in sample-special-enemy-geometry.py's actual collision cases. No game file or
accepted golden is changed.
"""
import argparse
import hashlib
import json
import math
import pathlib
import struct
import sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_FPCW, UC_X86_REG_FPTAG, UC_X86_REG_FPSW
SHA = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
parser = argparse.ArgumentParser()
parser.add_argument('--output', type=pathlib.Path,
                    default=ROOT / 'tests/fixtures/player-circle-native.json')
args = parser.parse_args()
exe = ROOT.parent / 'th12/th12.exe'
if hashlib.sha256(exe.read_bytes()).hexdigest() != SHA:
    raise SystemExit('Wrong EXE version; no native code executed')
pe = pefile.PE(str(exe))
v = Uc(UC_ARCH_X86, UC_MODE_32)
for address, size in [(0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                      (0x1000000, 0x10000), (0x2000000, 0x40000)]:
    v.mem_map(address, size)
v.mem_write(0x400000, pe.get_memory_mapped_image())
PLAYER, HUD, SHT, CENTER, SP, STOP = 0x2000000, 0x2010000, 0x2020000, 0x2030000, 0x100e000, 0x100f000


def put(address, fmt, *values):
    v.mem_write(address, struct.pack('<' + fmt, *values))


def get(address, fmt):
    return list(struct.unpack('<' + fmt, v.mem_read(address, struct.calcsize('<' + fmt))))


def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


misses = 0


def observe(vm, address, size, unused):
    global misses
    misses += 1
    sp = v.reg_read(UC_X86_REG_ESP)
    target = get(sp, 'I')[0]
    v.reg_write(UC_X86_REG_ESP, sp + 4)
    v.reg_write(UC_X86_REG_EIP, target)


v.hook_add(UC_HOOK_CODE, observe, begin=0x438370, end=0x438370)


def sample(name, radius=8, hit=2, dx=0, dy=0, state=1, invuln=0,
           flags=0, dialogue=False, has_hud=True):
    global misses
    origin = [-44.625, 137.25, 9]
    position = [f32(origin[0] + dx), f32(origin[1] + dy), -7]
    put(0x4b4514, 'I', PLAYER)
    put(0x4b43e4, 'I', HUD if has_hud else 0)
    put(HUD + 0x6d30, 'i', int(dialogue))
    put(PLAYER + 0x97c, 'fff', *position)
    put(PLAYER + 0xa2c, 'I', SHT)
    put(SHT + 4, 'f', hit)
    # These deliberately differ from the circle SHT radius.
    put(PLAYER + 0x9e4, 'ff', 99, 77)
    put(PLAYER + 0xa28, 'i', state)
    put(PLAYER + 0xc404, 'i', invuln)
    put(PLAYER + 0xc414, 'I', flags)
    put(CENTER, 'fff', *origin)
    put(SP, 'If', STOP, radius)
    v.reg_write(UC_X86_REG_ESP, SP)
    v.reg_write(UC_X86_REG_EAX, CENTER)
    v.reg_write(UC_X86_REG_FPCW, 0x37f)
    v.reg_write(UC_X86_REG_FPTAG, 0xffff)
    v.reg_write(UC_X86_REG_FPSW, 0)
    misses = 0
    v.emu_start(0x437980, STOP, count=20000)
    if v.reg_read(UC_X86_REG_EIP) != STOP:
        raise RuntimeError('Native query budget exhausted')
    code = v.reg_read(UC_X86_REG_EAX)
    if code not in (0, 1, 2):
        raise RuntimeError('Unexpected native circle return')
    return {'name': name, 'center': origin, 'radius': f32(radius),
            'player': {'position': position, 'circleHitRadius': f32(hit), 'hitWidth': 99,
                       'hitHeight': 77, 'state': state, 'invulnerability': invuln,
                       'flags': flags, 'dialogue': dialogue if has_hud else False},
            'nativeReturn': code, 'calls438370': misses, 'hudPointerPresent': has_hud}


cases = []
for radius in [0, 1, 4, 8, 16, 40, 100, 200, -8]:
    for hit in [2, 3.5, 3]:
        hit_edge = math.sqrt(radius * radius + hit * hit)
        graze_edge = math.sqrt(radius * radius + (hit + max(40, f32(radius / 2.5))) ** 2)
        for distance in [0, hit_edge - .00002, hit_edge, hit_edge + .00002,
                         graze_edge - .00002, graze_edge, graze_edge + .00002, 500]:
            cases.append(sample(f'radius{radius}_hit{hit}_distance{distance}', radius, hit, dx=distance))
        for dx, dy in [(3, 4), (24, 41), (-32, -40), (31, 37), (10, 80)]:
            cases.append(sample(f'radius{radius}_hit{hit}_at{dx}_{dy}', radius, hit, dx=dx, dy=dy))
for state in range(5):
    for invuln in [-1, 0, 1, 120]:
        for flags in [0, 2, 4, 6]:
            for dialogue in [False, True]:
                for distance in [0, 25, 200]:
                    cases.append(sample(f'gate_{state}_{invuln}_{flags}_{dialogue}_{distance}',
                                        dx=distance, state=state, invuln=invuln,
                                        flags=flags, dialogue=dialogue))
for invuln in [0, 1]:
    cases.append(sample(f'null_hud_invuln{invuln}', has_hud=False, invuln=invuln))
# Exact float32 equality cases prove strict edges without sqrt input rounding.
for distance in [1.99999, 2, 2.00001, 41.99999, 42, 42.00001]:
    cases.append(sample(f'exact_radius0_distance{distance}', radius=0, hit=2, dx=distance))
for distance in [57.99999, 58, 58.00001]:
    cases.append(sample(f'exact_radius40_graze{distance}', radius=40, hit=2, dx=distance))
result = {'schema': 'th12-player-circle-native-v1', 'originalExeSha256': SHA,
          'entry': '00437980', 'wholeGameOracle': False,
          'boundaries': ['Synthetic Player/HUD/SHT storage;actual437980 instructions execute',
                         '438370 records its call only;death window/visual effects not executed in this fixture'],
          'cases': cases}
args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(args.output), 'cases': len(cases),
                  'returns': {code: sum(c['nativeReturn'] == code for c in cases) for code in [0, 1, 2]},
                  'calls438370': sum(c['calls438370'] for c in cases)}))
