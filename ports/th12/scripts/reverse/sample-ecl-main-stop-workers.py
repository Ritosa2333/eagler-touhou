"""Original primary ECL1/10 immediately stops an actor's remaining workers.

Literal SCPT is synthetic input. Actual Enemy412990/Enemy18,468d90 coroutine
traversal,467170 opcodes and501 bullet/ANM births run unchanged. No thread
result, random call, emitter or animation is replaced. Not a whole-game oracle.
"""
import hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]
PROVIDER = ROOT / 'scripts/reverse/native-enemy-provider.py'
BOOTSTRAP = PROVIDER.read_text(encoding='utf-8').split('if args.routine not in routines:')[0]

def word(n): return struct.pack('<i', n)
def floating(n): return struct.pack('<f', n)
def ins(op, args=(), time=0, mask=0):
    raw = b''.join(args)
    return struct.pack('<iHHHBBI', time, op, len(raw) + 16, mask, 63, len(args), 0) + raw
def string(value):
    raw = value.encode('ascii') + b'\0'
    raw += bytes((-len(raw)) & 3)
    return word(len(raw)) + raw
def program(bodies):
    names = b''.join(name.encode('ascii') + b'\0' for name, _ in bodies)
    start = (36 + 4 * len(bodies) + len(names) + 3) & ~3
    raw = bytearray(start)
    raw[:4] = b'SCPT'
    struct.pack_into('<H', raw, 4, 1)
    struct.pack_into('<H', raw, 16, len(bodies))
    names_at = 36 + 4 * len(bodies)
    raw[names_at:names_at + len(names)] = names
    entries = {}
    for index, (name, code) in enumerate(bodies):
        header = len(raw)
        struct.pack_into('<I', raw, 36 + 4 * index, header)
        entries[name] = (names_at, header)
        names_at += len(name) + 1
        raw += b'ECLH' + bytes(12) + b''.join(code)
    return bytes(raw), entries

def worker():
    code = []
    # All three actual original global getters run. Their stored values allow
    # the regression to verify genuine random evaluation, not a counter burn.
    for index, global_id in enumerate([-9999., -9987., -9998.]):
        code += [ins(44, [floating(global_id)], mask=1), ins(45, [floating(index * 4)], mask=1)]
    code += [ins(500, [word(0)]), ins(507, [word(0), word(3)]),
             ins(502, [word(0), word(0), word(4)]),
             ins(505, [word(0), floating(0), floating(0)]),
             ins(506, [word(0), word(1), word(1)]), ins(501, [word(0)]),
             ins(83, [word(100000)]), ins(10)]
    return code

records = []
for kind in ['main-delete', 'main-return', 'async-delete', 'async-return']:
    saved = sys.argv
    sys.argv = [str(PROVIDER), '--frames', '0', '--seed', '1234']
    ns = {'__file__': str(PROVIDER), '__name__': 'original_ecl_main_stop_workers'}
    try:
        exec(compile(BOOTSTRAP, str(PROVIDER), 'exec'), ns)
    finally:
        sys.argv = saved
    from unicorn import UC_HOOK_CODE
    from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EDI, UC_X86_REG_EAX
    mu, put, get, call = (ns[key] for key in ['v', 'put', 'get', 'call'])
    terminating_op = 1 if kind.endswith('delete') else 10
    main = [ins(402, [word(0xa0)]), ins(15, [string('WorkerB')]), ins(15, [string('WorkerA')]),
            ins(83, [word(1 if kind.startswith('main-') else 100000)])]
    if kind.startswith('main-'):
        main += [ins(terminating_op)]
    main += [ins(10)]
    bodies = [('main', main), ('WorkerA', worker() if kind.startswith('main-') else [ins(terminating_op)]),
              ('WorkerB', worker())]
    raw, entries = program(bodies)
    base = 0x4300000
    mu.mem_write(base, raw)
    for name, (text, header) in entries.items():
        ns['routines'][name] = (base + text, base + header)
    put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
    for index, (_, (text, routine)) in enumerate(sorted(ns['routines'].items())):
        put(ns['PROGRAM'] + 0x100 + index * 8, 'II', text, routine)
    events, phase = [], 'constructor'

    def observe(mu, address, size, unused):
        esp = mu.reg_read(UC_X86_REG_ESP)
        row = {'phase': phase, 'entry': hex(address), 'caller': hex(get(esp, 'I')[0]),
               'rngBefore': list(get(0x4ce568, 'II'))}
        if address == 0x468e14:
            # A basic-block boundary is not a callee entry: ESP holds saved
            # registers here. Record its real primary result, not a caller.
            row['caller'] = None
            row['primaryContextReturn'] = mu.reg_read(UC_X86_REG_EAX)
        if address == 0x467170:
            owner = mu.reg_read(UC_X86_REG_EDI)
            context = get(owner + 4, 'I')[0]
            pc = get(context + 4, 'I')[0]
            possible = [(header, name) for name, (_, header) in entries.items() if base + header + 16 <= pc]
            header, routine = max(possible)
            row.update({'routine': routine, 'pcOffset': pc - base - header - 16})
        elif address == 0x40b5e0:
            row['kind'] = 'actualEmitter'
        events.append(row)
    for entry in [0x467170, 0x468e14, 0x40b5e0, 0x464440]:
        mu.hook_add(UC_HOOK_CODE, observe, begin=entry, end=entry)
    request = 0x2070000
    put(request, 'fffiiiII', 25, 100, 0, 0, 0, 10000, 0, 0)
    call(0x412990, base + entries['main'][0], request)

    def capture(tick):
        enemies, node = [], get(ns['MANAGER'] + 0x68, 'I')[0]
        while node:
            enemy, node = get(node, 'II')
            threads, thread = [], enemy + 0x1030
            while thread:
                context = get(thread, 'I')[0]
                pc = get(context + 4, 'I')[0]
                possible = [(header, name) for name, (_, header) in entries.items() if base + header + 16 <= pc]
                header, routine = max(possible)
                local_base = context + 8 + get(context + 0x100c, 'I')[0]
                threads.append({'routine': routine, 'pcOffset': pc - base - header - 16,
                                'time': get(context, 'f')[0], 'locals': list(get(local_base, '3f'))})
                thread = get(thread + 4, 'I')[0]
            enemies.append({'id': get(enemy + 0x27b4, 'I')[0], 'contexts': threads})
        bullets = []
        for slot in range(4):
            pointer = ns['BULLETS'] + 0x64 + slot * 0x9f8
            state = get(pointer + 0x532, 'H')[0]
            if state:
                bullets.append({'slot': slot, 'phase': state, 'typeColor': list(get(pointer + 0x9f4, 'HH')),
                                'position': list(get(pointer + 0x4bc, '3f'))})
        return {'inputTick': tick, 'gameSeedCounter': list(get(0x4ce568, 'II')),
                'enemies': enemies, 'bullets': bullets}
    frames = [capture(-1)]
    for tick in range(4):
        phase = 'input' + str(tick)
        ns['frame'] = tick
        call(0x413190, ns['MANAGER'])
        frames.append(capture(tick))
    records.append({'kind': kind, 'syntheticEclHex': raw.hex(), 'seed': 1234,
                    'frames': frames, 'actualCalls': events})
output = ROOT / 'tests/fixtures/ecl-main-stop-workers-original.json'
output.write_text(json.dumps({'schema': 'th12-original-ecl-main-stop-workers/1',
    'originalExeSha256': ns['SHA'], 'wholeGameOracle': False, 'gpuExecuted': False,
    'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
    'providerBootstrapSha256': hashlib.sha256(BOOTSTRAP.encode('utf-8')).hexdigest(),
    'scope': 'Actual Enemy412990/Enemy18,468d90,467170 ECL1/10/15/44/45/501, original global '
             'random getters and original bullet/ANM births. Synthetic literal SCPT and selected '
             'startup/resource/heap/audio host boundaries. No Player/Bomb/Bullet21/ANM27 scheduler '
             'or GPU execution. Primary nonzero return468dbc immediately468e14→413c68/413c13; '
             'async return468dc4 unlinks only that context and resumes its cached successor.',
    'records': records}, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'summary': [[row['kind'],
    [[frame['inputTick'], frame['gameSeedCounter'][1], len(frame['enemies']), len(frame['bullets'])]
     for frame in row['frames']]] for row in records]}))
