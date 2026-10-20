"""Execute retail 1.00b callbacks 41aa70 / 41b5f0 / 41bf50.

This is an independent synthetic subsystem fixture, not a full-game golden.
It executes the EXE's loops, x87 arithmetic, sqrt/atan2 and the real system
D3DX9_40 Vec2Normalize. Host text creation and ANM handle lookup only record
their boundary inputs. The DLL's real SSE2 table initializer is selected
explicitly; its Windows registry/CPU autodetection is outside this fixture.
"""
import argparse
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import *

EXE_SHA = '99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
DLL_SHA = '16fd909aeb68d0d1aca8529dc7f78880b97d6649d70ce8d03a2c858bc28e216b'
parser = argparse.ArgumentParser()
parser.add_argument('--output', type=pathlib.Path,
                    default=ROOT / 'artifacts/reverse/special-enemy-callbacks-native.json')
parser.add_argument('--dll', type=pathlib.Path,
                    default=pathlib.Path(r'C:\Windows\SysWOW64\D3DX9_40.dll'))
args = parser.parse_args()
exe = ROOT.parent / 'th12/th12.exe'
if hashlib.sha256(exe.read_bytes()).hexdigest() != EXE_SHA:
    raise SystemExit('Wrong original EXE version; no native code executed')
if hashlib.sha256(args.dll.read_bytes()).hexdigest() != DLL_SHA:
    raise SystemExit('Wrong D3DX9_40 dependency version; no native code executed')
pe, dll = pefile.PE(str(exe)), pefile.PE(str(args.dll))
v = Uc(UC_ARCH_X86, UC_MODE_32)
for address, size in [(0, 0x10000),
                      (0x400000, (pe.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                      (0x1000000, 0x10000), (0x2000000, 0x1000000),
                      (0x10000000, (dll.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095)]:
    v.mem_map(address, size)
v.mem_write(0x400000, pe.get_memory_mapped_image())
v.mem_write(0x10000000, dll.get_memory_mapped_image())
BM, ENEMY, BOSS, FONT, EM, VM = 0x2000000, 0x2600000, 0x2700000, 0x2800000, 0x2820000, 0x2830000
ES, RUNNER, CTX, CODE, VT = ENEMY + 0x1040, 0x2840000, 0x2850000, 0x2860000, 0x2870000
SP, STOP = 0x100e000, 0x100f000


def put(address, fmt, *values):
    v.mem_write(address, struct.pack('<' + fmt, *values))


def get(address, fmt):
    return list(struct.unpack('<' + fmt, v.mem_read(address, struct.calcsize('<' + fmt))))


def ret(pop=0, value=0):
    sp = v.reg_read(UC_X86_REG_ESP)
    target = get(sp, 'I')[0]
    v.reg_write(UC_X86_REG_EAX, value)
    v.reg_write(UC_X86_REG_ESP, sp + 4 + pop)
    v.reg_write(UC_X86_REG_EIP, target)


popups, lookups, traces, normalizations = [], [], [], []


def observe(vm, address, size, unused):
    if address == 0x4015c0:
        sp = v.reg_read(UC_X86_REG_ESP)
        popups.append({
            'displayedValue': get(sp + 8, 'i')[0],
            'screenPosition': get(v.reg_read(UC_X86_REG_EBX), 'fff'),
            'font': get(FONT + 0x18f98, 'i')[0],
            'color': get(FONT + 0x18f80, 'I')[0],
            'scale': get(FONT + 0x18f84, 'ff'),
            'alignment': get(FONT + 0x18fa4, 'ii'),
            'nativeTextFlag': get(FONT + 0x18f9c, 'i')[0],
            'format': bytes(v.mem_read(get(sp + 4, 'I')[0], 3)).decode('ascii').rstrip('\0'),
        })
        ret()
    elif address == 0x461920:
        handle = get(v.reg_read(UC_X86_REG_ESP) + 4, 'I')[0]
        lookups.append(handle)
        if handle != 7:
            raise RuntimeError('The original linked-line callback dereferences a null VM; excluded invalid fixture')
        ret(4, VM)
    elif address == 0x101fded0:
        sp = v.reg_read(UC_X86_REG_ESP)
        normalizations.append({'input': get(get(sp + 8, 'I')[0], 'ff'),
                               'implementation': hex(get(0x103f8104, 'I')[0])})
    elif address in (0x41aa70, 0x41b5f0, 0x41bf50, 0x4178bf, 0x41962b, 0x468bea):
        traces.append(hex(address))


for address in [0x4015c0, 0x461920, 0x101fded0, 0x41aa70, 0x41b5f0,
                0x41bf50, 0x4178bf, 0x41962b, 0x468bea]:
    v.hook_add(UC_HOOK_CODE, observe, begin=address, end=address)


def call(address, arguments=(), registers=None):
    put(SP, 'I' * (1 + len(arguments)), STOP, *arguments)
    v.reg_write(UC_X86_REG_ESP, SP)
    for register, value in (registers or {}).items():
        v.reg_write(register, value)
    try:
        v.emu_start(address, STOP, count=1000000)
    except Exception as error:
        raise RuntimeError(f'{error}; entry={address:x}, eip={v.reg_read(UC_X86_REG_EIP):x}') from error
    if v.reg_read(UC_X86_REG_EIP) != STOP:
        raise RuntimeError(f'Native instruction budget exhausted at {v.reg_read(UC_X86_REG_EIP):x}')
    return v.reg_read(UC_X86_REG_EAX)


def reset():
    v.mem_write(BM, bytes(0x4debdc))
    for address, size in [(ENEMY, 0x2800), (BOSS, 0x2800), (FONT, 0x19000),
                          (EM, 0x100), (VM, 0x4b4), (CTX, 0x1100), (RUNNER, 0x100)]:
        v.mem_write(address, bytes(size))
    for address, value in [(0x4b43c8, BM), (0x4b43dc, EM), (0x4b43b8, FONT),
                           (0x4ce8cc, 0x2880000)]:
        put(address, 'I', value)
    put(0x4d52d4, 'I', 1)
    put(0x4d52dc, 'I', 1)
    put(0x4b0c44, 'i', 0)
    put(0x4ce568, 'H', 1234)
    put(0x4ce56c, 'I', 0)
    put(EM + 0x1c, 'I', BOSS)
    put(ES + 0xe0, 'I', 7)
    put(VM + 0x58, 'ff', 13, 17)
    put(VM + 0x47c, 'I', 0x400)
    put(FONT + 0x18f80, 'Iff', 0xffffffff, 1, 1)
    put(FONT + 0x18fa4, 'ii', 1, 1)
    v.reg_write(UC_X86_REG_FPCW, 0x37f)
    v.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    v.reg_write(UC_X86_REG_FPTAG, 0xffff)
    v.reg_write(UC_X86_REG_FPSW, 0)
    # The EXE imports the genuine DLL export. Run its genuine SSE2 function
    # table initializer, without mocking vector arithmetic or the caller.
    put(0x4982e4, 'I', 0x101fded0)
    call(0x103e18fe, [0x103f8048])
    if get(0x103f8104, 'I')[0] != 0x103e0517:
        raise RuntimeError('Unexpected actual D3DX SSE2 Vec2Normalize entry')
    popups.clear()
    lookups.clear()
    traces.clear()
    normalizations.clear()


def seed_enemy(position=(0, 0, 0), age=0, private=(0, 0, 0, 0)):
    put(ES + 0x34, 'fff', *position)
    put(ES + 0x270, 'i', age)
    put(ES + 0x23c, '4i', *private)


def seed_bullet(row):
    slot = row['slot']
    p = BM + 0x64 + slot * 0x9f8
    put(p, 'I', row.get('objectFlags', 1))
    put(p + 0x532, 'H', row.get('phase', 1))
    put(p + 0x50c, 'i', row.get('tag', 0))
    put(p + 0x4bc, 'fff', *row.get('position', (0, 0, 0)))
    put(p + 0x4c8, 'fff', *row.get('velocity', (0, 0, 0)))
    put(p + 0x4d4, 'ff', row.get('nominalSpeed', 2), row.get('heading', 1))


def bullet(slot):
    p = BM + 0x64 + slot * 0x9f8
    return {'slot': slot, 'objectFlags': get(p, 'I')[0], 'phase': get(p + 0x532, 'H')[0],
            'tag': get(p + 0x50c, 'i')[0], 'position': get(p + 0x4bc, 'fff'),
            'velocity': get(p + 0x4c8, 'fff'), 'nominalSpeed': get(p + 0x4d4, 'f')[0],
            'heading': get(p + 0x4d8, 'f')[0]}


def snapshot(slots):
    boss = get(EM + 0x1c, 'I')[0]
    return {'privateIntegers': get(ES + 0x23c, '4i'),
            'primaryBossRewardCounter': get(boss + 0x1288, 'i')[0],
            'scoreStored': get(0x4b0c44, 'i')[0],
            'bullets': [bullet(slot) for slot in slots],
            'animation': {'spriteWidth': get(VM + 0x58, 'f')[0],
                          'spriteHeight': get(VM + 0x5c, 'f')[0], 'flags': get(VM + 0x47c, 'I')[0]},
            'fontAfter': {'color': get(FONT + 0x18f80, 'I')[0],
                          'scale': get(FONT + 0x18f84, 'ff'),
                          'alignment': get(FONT + 0x18fa4, 'ii'),
                          'font': get(FONT + 0x18f98, 'i')[0],
                          'nativeTextFlag': get(FONT + 0x18f9c, 'i')[0]},
            'rng': get(0x4ce568, 'H') + get(0x4ce56c, 'I')}


def case(name, selector, rows, position=(0, 0, 0), age=0, private=(0, 0, 0, 0),
         score=0, counter=0, repeat=1, alias=False):
    reset()
    seed_enemy(position, age, private)
    if alias:
        put(EM + 0x1c, 'I', ENEMY)
        put(ES + 0x248, 'i', counter)
    else:
        put(BOSS + 0x1288, 'i', counter)
    put(0x4b0c44, 'i', score)
    for row in rows:
        seed_bullet(row)
    slots = [row['slot'] for row in rows]
    before = snapshot(slots)
    steps = []
    for _ in range(repeat):
        native_return = call({1: 0x41aa70, 2: 0x41b5f0, 6: 0x41bf50}[selector],
                             registers={UC_X86_REG_ECX: ES})
        steps.append({'nativeReturn': native_return, 'after': snapshot(slots)})
    return {'name': name, 'selector': selector, 'enemyPosition': list(position),
            'enemyAge': age, 'aliasPrimaryBoss': alias, 'before': before, 'steps': steps,
            'popups': list(popups), 'animationLookups': list(lookups),
            'normalizations': list(normalizations), 'trace': list(traces)}


cases = []
rows = [
    {'slot': 0, 'position': [32, 0, 11], 'velocity': [0, 2, 7]},
    {'slot': 1, 'position': [0, -96, 11], 'velocity': [.25, -.5, 7]},
    {'slot': 2, 'position': [70, 70, 11], 'velocity': [-.1, .2, 7]},
    {'slot': 3, 'position': [128, 0, 11], 'velocity': [0, 2, 7]},
    {'slot': 4, 'position': [127.999, 0, 11], 'velocity': [0, 2, 7]},
    {'slot': 5, 'position': [128.001, 0, 11], 'velocity': [0, 2, 7]},
    {'slot': 6, 'position': [0, 0, 11], 'velocity': [.1, -.2, 7]},
    {'slot': 7, 'position': [1, 0, 11], 'velocity': [.1, -.2, 7], 'objectFlags': 2},
    {'slot': 8, 'position': [1, 0, 11], 'velocity': [.1, -.2, 7], 'phase': 2},
    {'slot': 9, 'position': [1, 0, 11], 'velocity': [.1, -.2, 7], 'phase': 3},
    {'slot': 1999, 'position': [3, 4, 11], 'velocity': [1.1, -2.2, 7], 'objectFlags': 0x101},
]
cases.append(case('attraction_filters_radius_zero_and_last_pool_slot', 1, rows))
cases.append(case('attraction_translated_origin_64_calls', 1, [
    {'slot': 17, 'position': [-10.125, 83.75, 3], 'velocity': [.91, -.23, 5],
     'nominalSpeed': 2.35, 'heading': -.3}], position=[-44.625, 137.25, 9], repeat=64))
cases.append(case('attraction_tiny_distance_d3dx_zero_threshold', 1, [
    {'slot': 0, 'position': [1e-8, 0, 0], 'velocity': [0, 0, 1]},
    {'slot': 1, 'position': [1e-6, 0, 0], 'velocity': [0, 0, 1]}]))
for index in range(15):
    cases.append(case(f'extra_score_table_{index}', 2, [], position=[-17.25, 61.5, 2],
                      private=[1, 12, 99, 0], score=12345, counter=index))
cases.append(case('extra_score_cap', 2, [], private=[1, 0, 0, 0], score=999999995, counter=14))
for trigger in [0, 2, -1]:
    cases.append(case(f'extra_score_popup_only_trigger_{trigger}', 2, [],
                      private=[trigger, 3, 0, 0], score=234, counter=11))
cases.append(case('extra_score_primary_boss_alias_and_16_calls', 2, [],
                  private=[1, 9, 0, 0], counter=14, repeat=16, alias=True))
for age in [0, 19, 20, 21, 100]:
    cases.append(case(f'linked_line_missing_at_age_{age}', 6, [], age=age,
                      private=[0, 0, 123, 0]))
cases.append(case('linked_line_first_enabled_alive_matching_tag', 6, [
    {'slot': 0, 'position': [500, 500, 9], 'tag': 123, 'objectFlags': 0},
    {'slot': 1, 'position': [400, 400, 9], 'tag': 123, 'phase': 2},
    {'slot': 2, 'position': [300, 300, 9], 'tag': 124},
    {'slot': 3, 'position': [3, 4, 9], 'tag': 123},
    {'slot': 4, 'position': [30, 40, 9], 'tag': 123}], age=100, private=[0, 0, 123, 0]))
cases.append(case('linked_line_negative_tag_last_slot_and_origin', 6, [
    {'slot': 1999, 'position': [-20.5, 137.25, 88], 'tag': -1}],
    position=[-44.625, 137.25, 9], age=0, private=[0, 0, -1, 0]))

tag_dispatch = []
for cursor, tag, active, concurrent in [(0, 123, 0, 0), (0, -1, 0, 0),
                                        (17, 0x7fffffff, 0, 0), (17, -0x80000000, 0, 0),
                                        (0, 456, 8, 0), (0, 456, 8, 1)]:
    reset()
    seed_bullet({'slot': 0, 'position': [3, 4, 5], 'velocity': [6, 7, 8],
                 'nominalSpeed': 9, 'heading': .25, 'tag': 99})
    ptr = BM + 0x64
    put(ptr + 0x528, 'I', active)
    put(ptr + 0x548, 'i', cursor)
    put(ptr + 0x550 + cursor * 24, 'ffiiII', 2, 3, tag, 4, 0x200000, concurrent)
    before = {'bullet': bullet(0), 'active': active, 'cursor': cursor}
    value = call(0x40aa60, registers={UC_X86_REG_ECX: ptr})
    tag_dispatch.append({'input': {'cursor': cursor, 'tag': tag, 'active': active,
                                  'concurrent': concurrent}, 'before': before,
                         'after': {'bullet': bullet(0), 'active': get(ptr + 0x528, 'I')[0],
                                   'cursor': get(ptr + 0x548, 'i')[0],
                                   'rng': get(0x4ce568, 'H') + get(0x4ce56c, 'I')}})

# ECL534 ignores callback 6's nonzero return and advances the actual VM.
reset()
seed_enemy(age=20, private=[0, 0, 999, 0])
put(ES, 'I', VT)
put(VT, 'I', 0x4154d0)
put(ES + 0x174c, 'I', RUNNER)
put(RUNNER + 4, 'I', CTX)


def instruction(opcode, arguments=()):
    return (struct.pack('<iHHHBBI', 0, opcode, 16 + 4 * len(arguments), 0, 255,
                        len(arguments), 0) + struct.pack('<' + 'I' * len(arguments), *arguments))


v.mem_write(CODE, instruction(534, [6]) + instruction(83, [10000]) + instruction(10))
put(CTX, 'fI', 0, CODE)
put(CTX + 0x1008, 'I', 256)
put(CTX + 0x1014, 'I', ES)
put(CTX + 0x101c, 'I', 255)
dispatch_return = call(0x467170, [0x3f800000], {UC_X86_REG_EAX: CTX})
dispatch = {'opcode': 534, 'selector': 6, 'enemyAge': 20,
            'vmReturn': dispatch_return, 'pcOffset': get(CTX + 4, 'I')[0] - CODE,
            'trace': list(traces)}
if dispatch['pcOffset'] != 40 or dispatch_return != 0:
    raise RuntimeError('Original 534 VM progression differs from expected boundary')
result = {'schema': 'th12-special-enemy-native-v1', 'originalExeSha256': EXE_SHA,
          'dependency': {'filename': args.dll.name, 'sha256': DLL_SHA,
                         'normalizer': 'actual D3DX9_40 SSE2 dispatcher, initialized by 103e18fe'},
          'entries': {'1': '0041aa70', '2': '0041b5f0', '6': '0041bf50'},
          'boundaryFixtures': ['FontManager 4015c0 records popup; no glyph creation',
                               'ANM lookup 461920 returns a valid fixture VM for handle 7',
                               'DLL CPU/registry autodetection bypassed by its own SSE2 table initializer'],
          'notCovered': ['full-game scheduling', 'D3DX scalar/3DNow CPU branches',
                         'missing primary boss or invalid reward table index', 'missing animation handle'],
          'cases': cases, 'tagDispatch': tag_dispatch, 'immediateDispatch': dispatch}
args.output.parent.mkdir(parents=True, exist_ok=True)
args.output.write_text(json.dumps(result, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(args.output), 'cases': len(cases),
                  'nativeCallbackCalls': sum(len(row['steps']) for row in cases),
                  'immediateDispatch': dispatch}))
