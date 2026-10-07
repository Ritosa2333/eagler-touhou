"""Read-only retail 530/531 selector1 execution and dependency inspection.

Uses the independent original-world provider to keep the real Player, SHT,
Bomb/bonus/ANM resources, collision death transition and graze effects. The
controlled enemy/primary VM are fixture objects. Some hurt cases explicitly
replace 439ed0 returns to expose the outer damage curve; other cases execute
the complete original query, including Source and friendly-shot side effects.
"""
import argparse
import contextlib
import copy
import hashlib
import io
import json
import math
import pathlib
import runpy
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
SHA = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
parser = argparse.ArgumentParser()
parser.add_argument('--output', type=pathlib.Path,
                    default=ROOT / 'tests/fixtures/special-enemy-geometry-native.json')
args = parser.parse_args()
if hashlib.sha256((ROOT.parent / 'th12/th12.exe').read_bytes()).hexdigest() != SHA:
    raise SystemExit('Wrong EXE version; no native code executed')
provider = ROOT / 'scripts/reverse/native-enemy-provider.py'
sys.argv = [str(provider), '--frames', '0', '--input',
            str(ROOT / 'artifacts/reverse/world-input-demo2.json'),
            '--native-player', '--native-bullets', '--native-items', '--active-collision',
            '--output', str(ROOT / 'artifacts/reverse/special-geometry-bootstrap.json')]
with contextlib.redirect_stdout(io.StringIO()):
    loaded = runpy.run_path(str(provider))
g = loaded['call'].__globals__
v, put, get, call = [g[n] for n in ['v', 'put', 'get', 'call']]
from unicorn import UC_HOOK_CODE
from unicorn.x86_const import UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDI, UC_X86_REG_ESI, UC_X86_REG_ESP

ENEMY = call(0x46c9ea, 0x2800)
ES = ENEMY + 0x1040
VM = call(0x46c9ea, 0x4b4)
PLAYER = g['PLAYER']
SOURCE = PLAYER + 0x8988
FRIENDLY = PLAYER + 0xa58
SPEC = call(0x46c9ea, 52)
REGIONS = [(0x400000, (g['pe'].OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
           (0x2000000, 0x200000), (0x3000000, (g['heap'] - 0x3000000 + 0x1ffff) & ~0xffff),
           (0x5000000, 0x100000), (0x6000000, (g['resource_base'] - 0x6000000 + 0xffff) & ~0xffff)]
memory = [(a, bytes(v.mem_read(a, size))) for a, size in REGIONS]
registers = v.context_save()
saved = {name: copy.deepcopy(g[name]) for name in ['heap', 'resource_base', 'resource_by_name',
                                                 'births', 'bullet_ids', 'next_bullet_id', 'last_spawn_request']}
queries, collision_calls, grazes, misses, polar_calls = [], [], [], [], []
query_returns = None
collision_returns = None


def f32(value):
    return struct.unpack('<f', struct.pack('<f', value))[0]


def reset():
    global query_returns, collision_returns
    for address, data in memory:
        v.mem_write(address, data)
    v.context_restore(registers)
    for name, value in saved.items():
        g[name] = copy.deepcopy(value)
    g['events'].clear()
    g['intercepts'].clear()
    g['frame'] = 0
    for rows in [queries, collision_calls, grazes, misses, polar_calls]:
        rows.clear()
    query_returns = collision_returns = None
    put(ES + 0xe0, 'I', 0x76543210)
    put(VM + 0x24, 'fff', 9, 3, 0)
    put(VM + 0x40, 'ff', 7, 64)
    put(VM + 0x58, 'ff', 99, 101)
    put(VM + 0x47c, 'I', 0x400)
    put(PLAYER + 0xa30, 'ii', 0, 1)
    put(PLAYER + 0xa28, 'i', 1)
    put(PLAYER + 0xc404, 'i', 0)
    put(PLAYER + 0xc414, 'I', 0)
    put(0x4b0c44, 'i', 0)
    put(0x4b0cdc, 'i', 0)
    put(0x4b0c78, 'i', 1000000)
    put(0x4ce568, 'H', 1234)
    put(0x4ce56c, 'I', 0)
    # Fixture callers own their old stack contents. The helper only writes
    # XY, so initialize the scratch stack and retain raw Z separately.
    v.mem_write(g['SP'] - 0x2000, bytes(0x2000))


def source_state():
    return {'damage': get(SOURCE + 0x60, 'i')[0],
            'accumulatedDamage': get(SOURCE + 0x64, 'i')[0],
            'limit': get(SOURCE + 0x68, 'i')[0], 'flags': get(SOURCE + 0x70, 'I')[0]}


def state():
    return {'playerState': get(PLAYER + 0xa28, 'i')[0],
            'scoreStored': get(0x4b0c44, 'i')[0], 'graze': get(0x4b0cdc, 'i')[0],
            'pivStored': get(0x4b0c78, 'i')[0], 'source': source_state(),
            'friendlyPhase': get(FRIENDLY + 0x48, 'i')[0],
            'rng': get(0x4ce568, 'H') + get(0x4ce56c, 'I')}


def animation():
    return {'rotationX': get(VM + 0x24, 'f')[0], 'rotationY': get(VM + 0x28, 'f')[0],
            'rotationZ': get(VM + 0x2c, 'f')[0], 'scaleX': get(VM + 0x40, 'f')[0],
            'scaleY': get(VM + 0x44, 'f')[0], 'spriteWidth': get(VM + 0x58, 'f')[0],
            'spriteHeight': get(VM + 0x5c, 'f')[0], 'flags': get(VM + 0x47c, 'I')[0]}


def observe(vm, address, size, unused):
    sp = v.reg_read(UC_X86_REG_ESP)
    if address == 0x461920 and get(sp + 4, 'I')[0] == 0x76543210:
        g['return_from_call'](4, VM)
    elif address == 0x41c580:
        angle, radius = get(sp + 4, 'ff')
        polar_calls.append({'angle': angle, 'radius': radius})
    elif address == 0x439ed0:
        center, extent = get(sp + 4, 'II')
        queries.append({'position': get(center, 'fff'), 'size': get(extent, 'ff'),
                        'before': state()})
        if query_returns is not None:
            g['return_from_call'](8, query_returns[len(queries) - 1])
    elif address == 0x41ac7c:
        queries[-1]['nativeReturn'] = v.reg_read(UC_X86_REG_EAX)
        queries[-1]['after'] = state()
    elif address == 0x437980:
        circle = get(v.reg_read(UC_X86_REG_EAX), 'fff')
        collision_calls.append({'kind': 'circle', 'position': circle, 'radius': get(sp + 4, 'f')[0],
                                'before': state()})
        if collision_returns is not None:
            g['return_from_call'](4, collision_returns[0])
    elif address == 0x41b549:
        collision_calls[-1]['nativeReturn'] = v.reg_read(UC_X86_REG_EAX)
        collision_calls[-1]['after'] = state()
    elif address == 0x437a80:
        collision_calls.append({'kind': 'line', 'position': get(v.reg_read(UC_X86_REG_EAX), 'fff'),
                                'angle': get(sp + 4, 'f')[0], 'transverseSize': get(sp + 8, 'f')[0],
                                'longitudinalSize': get(sp + 12, 'f')[0], 'before': state()})
        if collision_returns is not None:
            g['return_from_call'](12, collision_returns[1])
    elif address == 0x41b5aa:
        collision_calls[-1]['nativeReturn'] = v.reg_read(UC_X86_REG_EAX)
        collision_calls[-1]['after'] = state()
    elif address == 0x4391c0:
        grazes.append({'position': get(get(sp + 4, 'I')[0], 'fff'), 'before': state(),
                       'afterCircleQuery': len(collision_calls) == 1})
    elif address == 0x438370:
        misses.append({'before': state(), 'afterCircleQuery': len(collision_calls) == 1})


for address in [0x461920, 0x41c580, 0x439ed0, 0x41ac7c, 0x437980, 0x41b549,
                0x437a80, 0x41b5aa, 0x4391c0, 0x438370]:
    v.hook_add(UC_HOOK_CODE, observe, begin=address, end=address)


def seed_enemy(position, health=500, maximum=1000, flags=0, age=0, width=16, length=100):
    put(ES + 0x34, 'fff', *position)
    put(ES + 0x1608, 'ii', health, maximum)
    put(ES + 0x16b8, 'I', flags)
    put(ES + 0x270, 'i', age)
    put(ES + 0x24c, 'ff', width, length)


def hurt_case(name, health, maximum, angle, radius, position=(0, 0, 0),
              returns=None, source=None, friendly=False):
    global query_returns
    reset()
    seed_enemy(position, health, maximum)
    put(VM + 0x2c, 'f', angle)
    put(VM + 0x44, 'f', radius)
    query_returns = returns
    if source is not None:
        put(SOURCE, 'f', 1000)
        put(SOURCE + 0x18, 'fff', *position)
        put(SOURCE + 0x4c, 'ii', 4, 5)
        put(SOURCE + 0x60, 'iii', source['damage'], 0, source['limit'])
        put(SOURCE + 0x6c, 'iI', 4, 3)
    if friendly:
        # A real type0 shot is consumed by its first overlapping query.
        put(SPEC + 0x1d, 'B', 0)
        put(FRIENDLY, 'ii', 3, 4)
        put(FRIENDLY + 0x14, 'fff', *position)
        put(FRIENDLY + 0x48, 'i', 1)
        put(FRIENDLY + 0x60, 'i', 7)
        put(FRIENDLY + 0x68, 'ff', 2000, 2000)
        put(FRIENDLY + 0x74, 'I', SPEC)
    before = state()
    initial = animation()
    v.reg_write(UC_X86_REG_ECX, ES)
    returned = call(0x41aba0, budget=20000000)
    if len(queries) != 31:
        raise RuntimeError('Native custom hurt did not execute all31 queries')
    return {'name': name, 'input': {'health': health, 'maximumHealth': maximum,
            'position': list(position), 'animation': initial,
            'queryReturnFixture': returns, 'sourceFixture': source, 'friendlyFixture': friendly},
            'before': before, 'damage': returned, 'after': state(), 'animationAfter': animation(),
            'queries': copy.deepcopy(queries), 'polarCalls': copy.deepcopy(polar_calls)}


hurt = []
for total in [0, 1, 29, 30, 31, 96, 97, 100, 200, 313, 314, 2480]:
    base, remainder = divmod(total, 31)
    values = [base + (i < remainder) for i in range(31)]
    hurt.append(hurt_case(f'curve_total_{total}', 500, 1000, .7, 64, returns=values))
for health, angle, radius in [(0, 0, 32), (250, -math.pi, 64), (1000, math.pi, 100),
                              (-100, 7, -20), (750, math.pi * 80, 1)]:
    hurt.append(hurt_case(f'geometry_hp_{health}_angle_{angle}', health, 1000, angle, radius,
                         position=[-44.625, 137.25, 0], returns=[i % 4 for i in range(31)]))
for damage, limit in [(2, 999999), (10, 25), (100, 999999)]:
    hurt.append(hurt_case(f'actual_source_damage{damage}_limit{limit}', 500, 1000, .7, 64,
                         source={'damage': damage, 'limit': limit}))
hurt.append(hurt_case('actual_friendly_shot_consumed_once', 500, 1000, .7, 64, friendly=True))


def collision_case(name, angle, localx, localy, flags=0x200, age=0,
                   player_state=1, invuln=0, player_flags=0, dialogue=False, returns=None):
    global collision_returns
    reset()
    origin = [-17.25, 61.5, 0]
    angle = f32(angle)
    seed_enemy(origin, flags=flags, age=age)
    put(VM + 0x2c, 'f', angle)
    player_pos = [f32(origin[0] + math.cos(angle) * localx - math.sin(angle) * localy),
                  f32(origin[1] + math.sin(angle) * localx + math.cos(angle) * localy), 0]
    put(PLAYER + 0x97c, 'fff', *player_pos)
    put(PLAYER + 0xa28, 'i', player_state)
    put(PLAYER + 0xc404, 'i', invuln)
    put(PLAYER + 0xc414, 'I', player_flags)
    put(g['stage_manager'] + 0x6d30, 'i', int(dialogue))
    collision_returns = returns
    before = state()
    initial = animation()
    v.reg_write(UC_X86_REG_ECX, ES)
    returned = call(0x41b4d0, budget=20000000)
    if len(collision_calls) != 2 or returned:
        raise RuntimeError('Native collision callback call count / return differs')
    return {'name': name, 'input': {'position': origin, 'animation': initial,
            'flags': flags, 'age': age, 'privateFloat0': 16, 'privateFloat1': 100,
            'playerPosition': player_pos, 'playerState': player_state, 'invuln': invuln,
            'playerFlags': player_flags, 'dialogue': dialogue, 'queryReturnFixture': returns},
            'before': before, 'nativeReturn': returned, 'after': state(),
            'animationAfter': animation(), 'queries': copy.deepcopy(collision_calls),
            'grazes': copy.deepcopy(grazes), 'misses': copy.deepcopy(misses),
            'polarCalls': copy.deepcopy(polar_calls)}


collisions = []
for angle in [0, math.pi / 4, math.pi / 2, -math.pi / 2, math.pi]:
    for localx, localy in [(24, 0), (50, 0), (24, 41), (50, 20), (50, 39.999),
                           (50, 64), (110, 0), (-40, 0), (500, 500)]:
        collisions.append(collision_case(f'actual_angle{angle}_at{localx}_{localy}', angle, localx, localy))
for age in [0, 1, 2, 3, 19, 20, 21, -3]:
    collisions.append(collision_case(f'two_graze_age{age}', .7, 200, 200, age=age, returns=[2, 2]))
collisions.append(collision_case('two_graze_flag_disabled', .7, 200, 200, flags=0, returns=[2, 2]))
for player_state, invuln, player_flags, dialogue in [(1, 120, 0, False), (0, 0, 0, False),
    (2, 0, 0, False), (3, 0, 0, False), (4, 0, 0, False), (1, 0, 2, False), (1, 0, 0, True)]:
    collisions.append(collision_case(f'actual_player_gate_{player_state}_{invuln}_{player_flags}_{dialogue}',
                                    0, 24, 0, player_state=player_state, invuln=invuln,
                                    player_flags=player_flags, dialogue=dialogue))
result = {'schema': 'th12-special-enemy-geometry-native-v1', 'originalExeSha256': SHA,
          'entries': {'hurt530_1': '0041aba0', 'collision531_1': '0041b4d0',
                      'polar': '0041c580 x87 FSINCOS', 'normalize': '004646e0'},
          'wholeGameOracle': False,
          'bootstrapFiles': g['files'],
          'boundaries': ['Synthetic EnemyState/primary VM;461920 resolves only its fixture handle',
                         'outer curve fixtures replace439ed0 only when queryReturnFixture is present',
                         'collision-return fixtures replace queries only when queryReturnFixture is present',
                         'all actual cases execute439ed0/437980/437a80/438370/4391c0 and original ANM bytecode',
                         'provider intercepts host allocation/file/resource loading/audio output'],
          'limits': ['geometry helpers write XY only;unused stack Z is retained as raw evidence, not portable input',
                     'real Enemy18 outer flag/body-immunity guards are not exercised by direct callback entry',
                     'missing primary animation/maxHP0 invalid native accesses are excluded'],
          'hurtCases': hurt, 'collisionCases': collisions}
args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(args.output), 'hurtCases': len(hurt),
                  'damageQueries': sum(len(c['queries']) for c in hurt),
                  'collisionCases': len(collisions), 'collisionQueries': len(collisions) * 2,
                  'grazeCalls': sum(len(c['grazes']) for c in collisions),
                  'missCalls': sum(len(c['misses']) for c in collisions)}))
