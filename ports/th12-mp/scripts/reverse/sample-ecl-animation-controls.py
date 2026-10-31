"""Original ECL277/278/279/281/444 operand and animation-tail experiments.

Run original Enemy constructors, high-opcode dispatch, operand readers and ANM
resource binding. Synthetic ECL is added only to research memory. The passive
provider boundaries omit Player hurt/body/audio; none replaces these handlers.
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

def ins(op, payload=(), time=0, mask=0):
    raw = b''.join(payload)
    return struct.pack('<iHHHBBI', time, op, 16 + len(raw), mask, 63, len(payload), 0) + raw

def program(code):
    name = b'Probe\0'
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

bind = [ins(258, [word(1)]), ins(259, [word(0), word(0)]),
        ins(259, [word(1), word(1)])]
hold = ins(83, [word(100000)])
cases = [
    ('rotation-positive', bind, [ins(277, [word(0), floating(1.25)])]),
    ('rotation-unwrapped', bind, [ins(277, [word(1), floating(8)])]),
    ('rotation-reference', bind, [ins(277, [word(0), floating(-9999)], mask=2)]),
    ('rotation-missing-reference', [], [ins(277, [word(13), floating(-9999)], mask=2)]),
    ('scale-literal', bind, [ins(278, [word(1), floating(2.25)])]),
    ('scale-reference-twice', bind, [ins(278, [word(1), floating(-9999)], mask=2)]),
    ('scale-missing-reference', [], [ins(278, [word(13), floating(-9999)], mask=2)]),
    ('offset-unbound', [], [ins(279, [word(1), floating(7), floating(-9)])]),
    ('offset-after-bind', bind, [ins(279, [word(1), floating(7), floating(-9)])]),
    ('offset-before-bind', [], [ins(279, [word(1), floating(7), floating(-9)]),
                                ins(258, [word(1)]), ins(259, [word(1), word(1)])]),
    ('parent-slot0', bind, [ins(281, [word(1), word(0)])]),
    ('parent-disabled', bind, [ins(281, [word(1), word(-1)])]),
    ('offset-parent-combined', bind,
     [ins(279, [word(1), floating(7), floating(-9)]), ins(281, [word(1), word(0)])]),
    *[(f'flag444-{n}', [], [ins(444, [word(n)])]) for n in [-1, 0, 1, 2, 3]],
    ('flag444-clears-previous', [ins(402, [word(0x4000000)])], [ins(444, [word(0)])]),
]

records = []
for name, prefix, commands in cases:
    saved = sys.argv
    sys.argv = [str(PROVIDER), '--frames', '1', '--seed', '1234']
    ns = {'__file__': str(PROVIDER), '__name__': 'native_ecl_animation_controls'}
    try:
        exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
    finally:
        sys.argv = saved
    v, put, get, call = (ns[n] for n in ['v', 'put', 'get', 'call'])
    base = 0x4300000
    raw, header = program(prefix + commands + [hold])
    v.mem_write(base, raw)
    ns['routines']['Probe'] = (base + 40, base + header)
    put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
    for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
        put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, routine)
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ECX, UC_X86_REG_ESP
    current = [0]
    events = []

    def vm_pointer(handle):
        manager = get(0x4ce8cc, 'I')[0]
        for at in [manager + 0x8856b8, manager + 0x8856c0]:
            node = get(at, 'I')[0]
            while node:
                vm, next_node = get(node, 'II')
                if get(vm, 'I')[0] == handle:
                    return vm
                node = next_node
        return 0

    def capture(label):
        state = current[0]
        handles = get(state + 0xe0, '14I')
        vms = []
        for slot, handle in enumerate(handles):
            vm = vm_pointer(handle) if handle else 0
            if vm:
                vms.append({'slot': slot, 'rotation': get(vm + 0x24, '3f'),
                            'scale': get(vm + 0x40, '2f'),
                            'scriptPosition': get(vm + 0x424, '3f'),
                            'enginePosition': get(vm + 0x430, '3f'),
                            'flags': get(vm + 0x47c, 'I')[0]})
        offsets = [list(get(state + 0x120 + i * 12, '3f')) for i in range(14)]
        return {'label': label, 'flags': get(state + 0x16b8, 'I')[0],
                'position': list(get(state + 0x34, '3f')), 'offsets': offsets,
                'parents': list(get(state + 0x1e0, '14i')), 'vms': vms,
                'rng': list(get(0x4ce568, 'II'))}

    def trace(machine, address, size, data):
        if address == 0x413840:
            current[0] = get(machine.reg_read(UC_X86_REG_ESP) + 4, 'I')[0]
        elif address in [0x415bf1, 0x415c33, 0x415c93, 0x415ce5, 0x41899e]:
            events.append(capture(f'before-{address:08x}'))
        elif address in [0x415c2e, 0x415c8e, 0x415ce0, 0x415d08, 0x4189bb,
                         0x4141bb, 0x414338]:
            events.append(capture(f'after-{address:08x}'))
    for address in [0x413840, 0x415bf1, 0x415c33, 0x415c93, 0x415ce5, 0x41899e,
                    0x415c2e, 0x415c8e, 0x415ce0, 0x415d08, 0x4189bb,
                    0x4141bb, 0x414338]:
        v.hook_add(UC_HOOK_CODE, trace, begin=address, end=address)
    request = 0x2070000
    put(request, 'fffiiiII', 25, 100, 0, 0, 0, 100, 0, 0)
    enemy = call(0x412990, base + 40, request)
    current[0] = enemy + 0x1040
    records.append({'name': name, 'syntheticEclHex': raw.hex(), 'events': events,
                    'final': capture('after-constructor')})

target = ROOT / 'tests/fixtures/ecl-animation-controls-native.json'
target.write_text(json.dumps({'schema': 'th12-ecl-animation-controls-native/1',
                              'originalExeSha256': ns['SHA'], 'seed': 1234,
                              'scope': 'real Enemy birth, ECL operand readers/dispatch and animation-position tail using original ANM',
                              'partial': True, 'wholeGameOracle': False,
                              'boundaries': ['native-enemy-provider passive Player collision/hurt/audio boundaries',
                                             'constructed Enemy spawn request and synthetic ECL in research memory'],
                              'cases': records}, indent=2) + '\n')
print(json.dumps({'output': str(target), 'cases': len(records),
                  'handlerSnapshots': sum(len(row['events']) for row in records)}))
