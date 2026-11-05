"""Execute native ECL312/313 random-movement branches and their motion tails.

Synthetic ECL and initial player/spawn positions are fixture inputs. Original
operand readers, RNG16/32, angular guards, interpolators and Enemy updates run.
No native gameplay result is substituted; this is not a whole replay oracle.
"""
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]


def word(value):
    return struct.pack('<i', value)


def floating(value):
    return struct.pack('<f', value)


def instruction(op, payload=()):
    raw = b''.join(payload)
    return struct.pack('<iHHHBBI', 0, op, 16 + len(raw), 0, 63, len(payload), 0) + raw


def program(code):
    name = b'main\0'
    start = (40 + len(name) + 3) & ~3
    header = b'ECLH' + bytes(12) + b''.join(code)
    raw = bytearray(start + len(header))
    raw[:4] = b'SCPT'
    struct.pack_into('<HH', raw, 4, 1, 0)
    struct.pack_into('<H', raw, 16, 1)
    struct.pack_into('<I', raw, 36, start)
    raw[40:40 + len(name)] = name
    raw[start:] = header
    return bytes(raw), start


branches = [
    ('outside-left', -81, 160, 10),
    ('left-boundary', -80, 160, 10),
    ('player-right', 0, 160, 10),
    ('player-left', 0, 160, -10),
    ('player-equal', 0, 160, 0),
    ('right-boundary', 80, 160, -10),
    ('outside-right', 81, 160, -10),
    ('outside-top', 0, 119, 10),
    ('top-boundary', 0, 120, 10),
    ('bottom-boundary', 0, 200, -10),
    ('outside-bottom', 0, 201, -10),
]
rows = []
for op in [312, 313]:
    for seed in [1, 2, 4, 42, 1234, 32768]:
        for name, x, y, player_x in branches:
            saved = sys.argv
            sys.argv = [str(PROVIDER), '--frames', '0', '--seed', str(seed), '--without-anm']
            ns = {'__file__': str(PROVIDER), '__name__': 'native_random_movement_fixture'}
            try:
                exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
            finally:
                sys.argv = saved
            v, put, get, call = (ns[n] for n in ['v', 'put', 'get', 'call'])
            put(ns['PLAYER'] + 0x97c, 'fff', player_x, 400, 0)
            code = [instruction(300, [floating(x), floating(y)]),
                    instruction(404, [floating(0), floating(160), floating(320), floating(160)]),
                    instruction(op, [word(6), word(0), floating(5)]),
                    instruction(83, [word(100000)])]
            raw, header = program(code)
            base = 0x4300000
            v.mem_write(base, raw)
            ns['routines']['main'] = (base + 40, base + header)
            put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
            for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
                put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, routine)
            from unicorn import UC_HOOK_CODE
            from unicorn.x86_const import UC_X86_REG_EBX, UC_X86_REG_ESP
            angular = []

            def capture_angle(machine, address, size, data):
                state = machine.reg_read(UC_X86_REG_EBX)
                angular.append({'angle': get(machine.reg_read(UC_X86_REG_ESP) + 0x18, 'f')[0],
                                'clamp': get(state + 0x15f4, '4f'),
                                'rngSeed': get(0x4ce568, 'H')[0],
                                'rngCalls': get(0x4ce56c, 'I')[0]})

            v.hook_add(UC_HOOK_CODE, capture_angle, begin=0x417799, end=0x417799)
            request = 0x2070000
            put(request, 'fffiiiII', 0, 0, 0, 0, 0, 10000, 0, 0)
            enemy = call(0x412990, base + 40, request)
            state = enemy + 0x1040
            if len(angular) != 1:
                raise RuntimeError('Expected exactly one native random movement instruction')
            frames = []
            for tick in range(8):
                call(0x413190, ns['MANAGER'])
                frames.append({'inputTick': tick, 'position': get(state + 0x34, '2f'),
                               'absolute': get(state + 0x68, '2f'),
                               'relative': get(state + 0x9c, '2f'),
                               'angle': get(state + 0x84, 'f')[0],
                               'speed': get(state + 0x80, 'f')[0],
                               'relativeAngle': get(state + 0xb8, 'f')[0],
                               'relativeSpeed': get(state + 0xb4, 'f')[0],
                               'rngSeed': get(0x4ce568, 'H')[0],
                               'rngCalls': get(0x4ce56c, 'I')[0]})
            rows.append({'name': f'op{op}-seed{seed}-{name}', 'op': op, 'seed': seed,
                         'playerX': player_x, 'syntheticEclHex': raw.hex(),
                         'selected': angular[0], 'frames': frames})

target = ROOT / 'tests/fixtures/ecl-random-movement-native.json'
target.write_text(json.dumps({'schema': 'th12-ecl-random-movement-native/1',
                              'originalExeSha256': ns['SHA'], 'partial': True,
                              'wholeGameOracle': False,
                              'scope': 'Original ECL404/312/313, RNG, angular guards, motion interpolators and Enemy manager with synthetic ECL and initial player position. No graphics/player/input scheduler oracle.',
                              'creationGuardPrimed': False, 'cases': rows}, indent=2) + '\n')
print(json.dumps({'output': str(target), 'cases': len(rows),
                  'frames': sum(len(row['frames']) for row in rows)}))
