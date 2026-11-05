"""Original EnemyManager last successful registration ID and ECL455 polling.

Synthetic SCPT executes actual412990,413190,467170,419a20/41a000 and412500.
Only selected Enemy18 passes run; no gameplay result or RNG is substituted.
This is independent local CPU evidence, never a full replay or render oracle.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]

def i(n): return struct.pack('<i', n)
def f(n): return struct.pack('<f', n)
def ins(op, args=(), time=0, mask=0):
    raw = b''.join(args)
    return struct.pack('<iHHHBBI', time, op, len(raw) + 16, mask, 63, len(args), 0) + raw
def string(value):
    raw = value.encode('ascii') + b'\0'
    raw += bytes((-len(raw)) & 3)
    return i(len(raw)) + raw
def birth(name, time=0):
    return ins(256, [string(name), f(0), f(0), i(10), i(0), i(0)], time)
def readlast(slot, time=0):
    return [ins(42, [i(-9931)], time, 1), ins(43, [i(slot * 4)], time, 1)]
def hold(): return [ins(402, [i(0xa0)]), ins(83, [i(100000)]), ins(10)]
def scpt(bodies):
    names = b''.join(name.encode('ascii') + b'\0' for name, _ in bodies)
    start = (36 + 4 * len(bodies) + len(names) + 3) & ~3
    raw = bytearray(start); raw[:4] = b'SCPT'
    struct.pack_into('<H', raw, 4, 1); struct.pack_into('<H', raw, 16, len(bodies))
    names_at = 36 + 4 * len(bodies); raw[names_at:names_at + len(names)] = names
    entries = {}
    for index, (name, code) in enumerate(bodies):
        header = len(raw); struct.pack_into('<I', raw, 36 + index * 4, header)
        entries[name] = (names_at, header); names_at += len(name) + 1
        raw += b'ECLH' + bytes(12) + b''.join(code)
    return bytes(raw), entries
def polling():
    code = [ins(402, [i(0xa0)])]; labels = {}; jumps = []
    def label(name): labels[name] = sum(map(len, code))
    def jump(op, target):
        jumps.append((len(code), target)); code.append(ins(op, [i(0), i(0)]))
    label('spawn')
    code += [birth('Anchor')] + readlast(0) + [ins(83, [i(2)]),
             ins(301, [i(8), i(0), f(64), f(100)])]
    label('poll')
    code += [ins(455, [i(4), i(0)], mask=3), ins(42, [i(4)], mask=1)]
    jump(13, 'spawn')
    code += [ins(83, [i(1)])]; jump(12, 'poll')
    offsets = []; at = 0
    for command in code: offsets.append(at); at += len(command)
    for index, target in jumps:
        op = struct.unpack_from('<H', code[index], 4)[0]
        code[index] = ins(op, [i(labels[target] - offsets[index]), i(0)])
    return code

birth_cases = [
 ('first-and-successive-constructor', [('main', [ins(402, [i(0xa0)])] + readlast(0) +
    [birth('Leaf')] + readlast(1) + [birth('Leaf')] + readlast(2) + [ins(83, [i(100000)]), ins(10)]),
    ('Leaf', hold())], 3),
 ('successive-later-ticks', [('main', [ins(402, [i(0xa0)])] + readlast(0, 1) + [birth('Leaf', 1)] +
    readlast(1, 1) + readlast(2, 2) + [birth('Leaf', 2)] + readlast(3, 2) +
    [ins(83, [i(100000)], 2), ins(10, time=2)]), ('Leaf', hold())], 5),
 ('nested-constructor-registration', [('main', [ins(402, [i(0xa0)]), birth('Outer')] + readlast(0) +
    [ins(83, [i(100000)]), ins(10)]), ('Outer', [ins(402, [i(0xa0)]), birth('Inner')] +
    readlast(0) + [ins(83, [i(100000)]), ins(10)]), ('Inner', hold())], 3),
]
cases = [(name, bodies, ticks, 1) for name, bodies, ticks in birth_cases]
cases += [('anchor-lifetime-poll-while-moving-difficulty' + str(difficulty),
           [('main', polling()), ('Anchor', [ins(402, [i(0xa0)]), ins(83, [i(6)]), ins(1), ins(1)])],
           18, difficulty) for difficulty in range(4)]
records = []; getter_cases = []
for name, bodies, ticks, difficulty in cases:
    # Native locals and expression stack share backing storage. Reserve the
    # four local words using actual op40 before any push/pop; omission would
    # overwrite A with the stack's type tag0x69 and invalidate polling.
    bodies = [(routine, [ins(40, [i(16)])] + code) for routine, code in bodies]
    saved = sys.argv; sys.argv = [str(PROVIDER), '--frames', '0', '--seed', '1234']
    ns = {'__file__': str(PROVIDER), '__name__': 'original_last_enemy_id'}
    try: exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
    finally: sys.argv = saved
    from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_WRITE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_ECX, UC_X86_REG_EDI, UC_X86_REG_EAX
    mu, put, get, call = (ns[key] for key in ['v', 'put', 'get', 'call'])
    # Explicit controlled initial difficulty, before412990/467170. The provider
    # has no standalone difficulty CLI; every Actor constructor reads this real
    # native global and assigns its original difficulty instruction-mask byte.
    put(0x4b0ca8, 'i', difficulty)
    raw, entries = scpt(bodies); base = 0x4300000; mu.mem_write(base, raw)
    for routine, (text, header) in entries.items(): ns['routines'][routine] = (base + text, base + header)
    put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
    for index, (_, (text, header)) in enumerate(sorted(ns['routines'].items())):
        put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, header)
    events = []; tick = -1
    def observe(mu, address, size, unused):
        row = {'inputTick': tick, 'entry': hex(address)}
        if address == 0x412b70:
            row['registeredId'] = mu.reg_read(UC_X86_REG_EAX)
        else:
            row['caller'] = hex(get(mu.reg_read(UC_X86_REG_ESP), 'I')[0])
            if address == 0x412500: row['requestedId'] = mu.reg_read(UC_X86_REG_ECX)
            if address in [0x419a20, 0x41a000]: row['reference'] = get(mu.reg_read(UC_X86_REG_ESP) + 4, 'i')[0]
        events.append(row)
    for entry in [0x412b70, 0x412500, 0x419a20, 0x41a000]:
        mu.hook_add(UC_HOOK_CODE, observe, begin=entry, end=entry)
    request = 0x2070000; put(request, 'fffiiiII', 0, 0, 0, 0, 0, 10000, 0, 0)
    call(0x412990, base + entries['main'][0], request)
    def capture(t):
        enemies, node = [], get(ns['MANAGER'] + 0x68, 'I')[0]
        while node:
            enemy, node = get(node, 'II'); ctx = get(enemy + 4, 'I')[0]; pc = get(ctx + 4, 'I')[0]
            possible = [(header, routine) for routine, (_, header) in entries.items() if base + header + 16 <= pc]
            header, routine = max(possible); local_base = ctx + 8 + get(ctx + 0x100c, 'I')[0]
            enemies.append({'id': get(enemy + 0x27b4, 'I')[0], 'routine': routine,
              'pcOffset': pc - base - header - 16, 'time': get(ctx, 'f')[0],
              'localsWords': list(get(local_base, '4I')), 'position': list(get(enemy + 0x1074, '2f')),
              'absolutePosition': list(get(enemy + 0x10a8, '2f')),
              'flags': get(enemy + 0x26f8, 'I')[0],
              'difficultyMask': get(enemy + 0x1024, 'B')[0]})
        return {'inputTick': t, 'lastRegisteredId': get(ns['MANAGER'] + 0x78, 'I')[0],
                'nativeDifficulty': get(0x4b0ca8, 'i')[0],
                'nextId': get(ns['MANAGER'] + 0x74, 'I')[0],
                'gameSeedCounter': list(get(0x4ce568, 'II')), 'enemies': enemies}
    frames = [capture(-1)]
    for tick in range(ticks):
        ns['frame'] = tick; call(0x413190, ns['MANAGER']); frames.append(capture(tick))
    records.append({'name': name, 'syntheticEclHex': raw.hex(), 'seed': 1234,
                    'difficulty': difficulty,
                    'frames': frames, 'actualEntries': events})
    if not getter_cases:
        # Controlled raw manager field probes, separated from the Enemy passes.
        # This verifies int words above the float-exact range too.
        for value in [0, 1, 17, 16777217, 0x80000001, 0xffffffff]:
            put(ns['MANAGER'] + 0x78, 'I', value)
            integer = call(0x419a20, (-9931) & 0xffffffff)
            call(0x41a000, (-9931) & 0xffffffff)
            # The original result is ST0. Store it using an isolated fstp seam.
            mu.mem_write(0x10fd000, b'\xd9\x1d\x00\xd0\x0f\x02\xc3')
            call(0x10fd000)
            getter_cases.append({'managerWord': value, 'integerWord': integer,
                                 'floatValue': get(0x20fd000, 'f')[0]})

output = ROOT / 'tests/fixtures/last-enemy-id-original.json'
output.write_text(json.dumps({'schema': 'th12-original-last-enemy-id/1',
 'originalExeSha256': ns['SHA'], 'wholeGameOracle': False, 'gpuExecuted': False,
 'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
 'providerBootstrapSha256': hashlib.sha256(BOOTSTRAP.encode()).hexdigest(),
 'scope': 'Actual412990 full synchronous constructors,413190 Enemy18 and467170 ECL256/42/43/455/83/301. '
          'Initial synthetic difficulty0..3 is written to4b0ca8 before any constructor; each capture '
          'records that native global and the actual Actor+1024 instruction-mask byte. '
          'EnemyManager+78 is written at412b70 after successful registration; nested child registrations '
          'complete before parent registration. Native419a20 -9931 dispatches419dae raw int,41a000 '
          'dispatches41a41b fild. Native412500 scans linked IDs including marked actors until removal. '
          'Only Enemy18 dispatches; no Player16, Bomb17, Bullet21, ANM27, scene transitions or rendering. '
          'Getter-only controlled fields and fstp seam do not substitute any game instruction/result. '
          'No replay bytes or accepted golden is changed.',
 'getterCases': getter_cases, 'records': records}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'cases': len(records), 'getterCases': len(getter_cases),
  'frameCounts': [len(row['frames']) for row in records],
  'pollingBirths': {row['difficulty']: [event for event in row['actualEntries'] if event['entry'] == '0x412b70']
                   for row in records if row['name'].startswith('anchor-lifetime-poll')}}))
