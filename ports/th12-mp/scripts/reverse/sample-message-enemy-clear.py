"""Actual414c60 repeated clear and actual ECL418/MSG constructor integration.

The literal test actors/SCPT/MSG are synthetic inputs. Enemy clearing, ANM
allocation/binding/children, random calls and the tested ECL/MSG path execute
from the pinned executable. No game logic or random result is intercepted.
The integration uses the provider's declared selected-manager host environment;
it is neither a whole-game nor a GPU/audio oracle. Existing fixtures are kept.
"""
import hashlib, json, pathlib, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT.parent / 'tools/python'))
import pefile
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_INVALID
from unicorn.x86_const import *
from native_anm_resource import check_original, EXE_SHA256, install_resource

EXE = ROOT.parent / 'th12/th12.exe'
check_original(EXE)
PE = pefile.PE(str(EXE))
BULLET = (ROOT / 'local/retail/bullet.anm').read_bytes()


def primary_snapshot(mu, manager, read):
    result, node = [], read(manager + 0x8856b8, 'I')[0]
    while node:
        vm, node = read(node, 'II')
        result.append({'script': read(vm + 0x3ea, 'h')[0],
                       'clock': read(vm + 0x6c, 'i')[0],
                       'layer': read(vm + 0x20, 'i')[0],
                       'engine': read(vm + 0x430, '3f'),
                       'position': read(vm + 0x424, '3f'),
                       'scale': read(vm + 0x40, '2f'),
                       'attached': bool(read(vm + 0x18, 'I')[0])})
        if len(result) > 1000:
            raise RuntimeError('Actual ANM primary list exceeded fixture bound')
    return result


def isolated_case(name, flags, script=84, count=1):
    mu = Uc(UC_ARCH_X86, UC_MODE_32)
    for base, size in [(0x400000, (PE.OPTIONAL_HEADER.SizeOfImage + 4095) & ~4095),
                       (0x1000000, 0x10000), (0x2000000, 0x1000000)]:
        mu.mem_map(base, size)
    mu.mem_write(0x400000, PE.get_memory_mapped_image())
    manager, resource, em = 0x2000000, 0x2a00000, 0x2c00000
    stack, stop = 0x100e000, 0x100f000
    put = lambda at, fmt, *v: mu.mem_write(at, struct.pack('<' + fmt, *v))
    read = lambda at, fmt: list(struct.unpack('<' + fmt, mu.mem_read(at, struct.calcsize('<' + fmt))))
    install_resource(mu, BULLET, resource, 0)
    put(0x4ce8cc, 'I', manager)
    put(0x4b43dc, 'I', em)
    put(em + 0x40, 'I', resource)
    # Retail412d70..412d98 initializes this real Timer's clock pointer;
    # 414c60 advances it through464a80 after walking the linked enemies.
    put(em + 0x50, 'iifII', -1, 0, 0, 0x4b2ed0, 1)
    put(0x4b2ed0, 'f', 1)
    put(0x4ce568, 'II', 0x1234, 0)
    put(0x4ce560, 'II', 0xbeef, 0)
    mu.reg_write(UC_X86_REG_FPCW, 0x37f)
    mu.reg_write(UC_X86_REG_MXCSR, 0x1f80)
    actors = []
    for index in range(count):
        enemy, node = 0x2b00000 + index * 0x3000, 0x2d00000 + index * 16
        actors.append(enemy)
        put(enemy + 0x26bc, 'ii', script, 0)
        put(enemy + 0x26f8, 'I', flags)
        put(enemy + 0x1074, '3f', -20 + index * 7, 120 + index * 3, 7)
        put(node, 'II', enemy, node + 16 if index + 1 < count else 0)
    put(em + 0x68, 'I', 0x2d00000 if count else 0)
    events, pass_index = [], 0

    def hook(mu, address, size, unused):
        if address == 0x454d10:
            esp = mu.reg_read(UC_X86_REG_ESP)
            caller, vm, bound_script = read(esp, 'III')
            # Children bind recursively as well. Keep both the actual root
            # caller and actual child constructor calls, without replacing them.
            events.append({'pass': pass_index, 'entry': hex(address),
                           'caller': hex(caller), 'script': bound_script,
                           'rngBefore': read(0x4ce568, 'II'),
                           'enemyFlagsBefore': read(mu.reg_read(UC_X86_REG_EBP) + 0x26f8, 'I')[0]
                           if caller == 0x414d6c else None})
        elif address == 0x453e20:
            events.append({'pass': pass_index, 'entry': hex(address),
                           'caller': hex(read(mu.reg_read(UC_X86_REG_ESP), 'I')[0])})
    mu.hook_add(UC_HOOK_CODE, hook, begin=0x454d10, end=0x454d10)
    mu.hook_add(UC_HOOK_CODE, hook, begin=0x453e20, end=0x453e20)
    def fault(mu, access, address, size, value, unused):
        print(json.dumps({'case': name, 'eip': hex(mu.reg_read(UC_X86_REG_EIP)),
                          'address': hex(address), 'size': size,
                          'registers': {n: hex(mu.reg_read(r)) for n, r in [
                              ('eax', UC_X86_REG_EAX), ('ebx', UC_X86_REG_EBX),
                              ('ecx', UC_X86_REG_ECX), ('edx', UC_X86_REG_EDX),
                              ('esi', UC_X86_REG_ESI), ('edi', UC_X86_REG_EDI),
                              ('ebp', UC_X86_REG_EBP), ('esp', UC_X86_REG_ESP)]}}))
        return False
    mu.hook_add(UC_HOOK_MEM_INVALID, fault)

    def snapshot():
        return {'gameSeedCounter': read(0x4ce568, 'II'),
                'displaySeedCounter': read(0x4ce560, 'II'),
                'clearTimer': read(em + 0x50, 'iifII'),
                'enemyFlags': [read(enemy + 0x26f8, 'I')[0] for enemy in actors],
                'primary': primary_snapshot(mu, manager, read)}
    frames = [snapshot()]
    for pass_index in (1, 2):
        put(stack, 'I', stop)
        mu.reg_write(UC_X86_REG_ESP, stack)
        mu.emu_start(0x414c60, stop, count=2000000)
        if mu.reg_read(UC_X86_REG_EIP) != stop:
            raise RuntimeError('Actual414c60 did not return within fixture budget')
        frames.append(snapshot())
    return {'name': name, 'initialFlags': flags, 'deathScript': script,
            'enemyCount': count, 'frames': frames, 'actualBindCalls': events}


def word(n):
    return struct.pack('<i', n)


def floating(n):
    return struct.pack('<f', n)


def ecl(op, args=(), time=0):
    raw = b''.join(args)
    return struct.pack('<iHHHBBI', time, op, len(raw) + 16, 0, 63, len(args), 0) + raw


def string_arg(value):
    raw = value.encode('ascii') + b'\0'
    raw += bytes((-len(raw)) & 3)
    return word(len(raw)) + raw


def scpt(routines):
    names = b''.join(name.encode('ascii') + b'\0' for name, _ in routines)
    first = (36 + 4 * len(routines) + len(names) + 3) & ~3
    raw = bytearray(first)
    raw[:4] = b'SCPT'
    struct.pack_into('<H', raw, 4, 1)
    struct.pack_into('<H', raw, 16, len(routines))
    raw[36 + 4 * len(routines):36 + 4 * len(routines) + len(names)] = names
    for index, (_, code) in enumerate(routines):
        struct.pack_into('<I', raw, 36 + 4 * index, len(raw))
        raw += b'ECLH' + bytes(12) + b''.join(code)
    return bytes(raw)


def integration_case(count, arm_flags=0x500, name=None):
    provider = ROOT / 'scripts/reverse/native-enemy-provider.py'
    bootstrap = provider.read_text(encoding='utf-8').split('SPAWN=0x2070000;')[0]
    saved = sys.argv
    sys.argv = [str(provider), '--frames', '0', '--native-player', '--native-items',
                '--native-bullets', '--native-lasers', '--active-collision',
                '--instruction-budget', '20000000', '--input',
                str(ROOT / 'artifacts/reverse/enemy-input-bomb-lifecycle-sanae-b.json'),
                '--output', str(ROOT / 'artifacts/reverse/message-enemy-clear-bootstrap.json')]
    ns = {'__file__': str(provider), '__name__': 'original_message_enemy_clear'}
    try:
        exec(compile(bootstrap, str(provider), 'exec'), ns)
    finally:
        sys.argv = saved
    mu, put, get, call = (ns[key] for key in ['v', 'put', 'get', 'call'])
    main = [ecl(402, [word(0xa0)])]
    main += [ecl(257, [string_arg('ArmProbe'), floating(-20 + index * 7),
                         floating(120 + index * 3), word(1000), word(0), word(0)])
             for index in range(count)]
    main += [ecl(83, [word(1)]), ecl(418, [word(0)]), ecl(83, [word(10000)]), ecl(10)]
    # Same protected+force bits as actual stage3 arms; do not bind a live VM,
    # so the exact default death84 production path is isolated.
    arm = [ecl(402, [word(arm_flags)]), ecl(83, [word(10000)]), ecl(10)]
    raw = scpt([('main', main), ('ArmProbe', arm)])
    message = struct.pack('<III', 1, 12, 256) + struct.pack('<HBB', 100, 0, 0)
    message_base, ecl_base = 0x43f0000, 0x4300000
    mu.mem_write(message_base, message)
    put(ns['stage_manager'] + 0x6d34, 'I', message_base)
    mu.mem_write(ecl_base, raw)
    name_offset = 36 + 4 * 2
    for index in range(2):
        end = raw.index(0, name_offset)
        routine_name = raw[name_offset:end].decode('ascii')
        header = struct.unpack_from('<I', raw, 36 + 4 * index)[0]
        ns['routines'][routine_name] = (ecl_base + name_offset, ecl_base + header)
        name_offset = end + 1
    put(ns['PROGRAM'] + 8, 'I', len(ns['routines']))
    for index, (_, (name_address, header)) in enumerate(sorted(ns['routines'].items())):
        put(ns['PROGRAM'] + 0x100 + index * 8, 'II', name_address, header)

    events, phase = [], 'constructor'

    def hook(mu, address, size, unused):
        esp = mu.reg_read(UC_X86_REG_ESP)
        row = {'phase': phase, 'entry': hex(address), 'caller': hex(get(esp, 'I')[0]),
               'gameSeedCounter': list(get(0x4ce568, 'II'))}
        if address == 0x454d10:
            row['script'] = get(esp + 8, 'i')[0]
            if row['caller'] == '0x414d6c':
                enemy = mu.reg_read(UC_X86_REG_EBP)
                row['enemyId'] = get(enemy + 0x27b4, 'I')[0]
                row['enemyFlagsBefore'] = get(enemy + 0x26f8, 'I')[0]
        elif address == 0x40cf60:
            row['convert'] = get(esp + 4, 'I')[0]
        elif address in (0x40d230, 0x428750):
            row['ebx'] = mu.reg_read(UC_X86_REG_EBX)
            row['edi'] = mu.reg_read(UC_X86_REG_EDI)
        events.append(row)
    for entry in [0x414c60, 0x454d10, 0x40d230, 0x40cf60, 0x428750, 0x453e20]:
        mu.hook_add(UC_HOOK_CODE, hook, begin=entry, end=entry)
    request = 0x2070000
    put(request, 'fffiiiII', 0, 0, 0, 0, 0, 10000, 0, 0)
    main_enemy = call(0x412990, ns['routines']['main'][0], request)

    def snapshot(tick):
        enemies, node = [], get(ns['MANAGER'] + 0x68, 'I')[0]
        while node:
            enemy, node = get(node, 'II')
            context = get(enemy + 4, 'I')[0]
            enemies.append({'id': get(enemy + 0x27b4, 'I')[0],
                            'flags': get(enemy + 0x26f8, 'I')[0],
                            'position': list(get(enemy + 0x1074, '3f')),
                            'pc': get(context + 4, 'I')[0]})
            if len(enemies) > 20:
                raise RuntimeError('Actual integration enemy list exceeded fixture bound')
        animations = primary_snapshot(mu, 0x5000000, get)
        death = [vm for vm in animations if vm['script'] in (84, 85, 86, 93)]
        return {'inputTick': tick, 'gameSeedCounter': list(get(0x4ce568, 'II')),
                'displaySeedCounter': list(get(0x4ce560, 'II')), 'enemies': enemies,
                'deathAnimations': death,
                'messageActive': bool(get(ns['stage_manager'] + 0x6d30, 'I')[0])}
    frames = [snapshot(-1)]
    for tick in range(3):
        phase = 'input' + str(tick)
        ns['frame'] = tick
        for address in (0x4d49d0, 0x4d49d4, 0x4d49dc, 0x4d49e0):
            put(address, 'I', 0)
        mu.reg_write(UC_X86_REG_EDI, 0x5000000)
        call(0x4610d0)
        call(0x436ba0, ns['PLAYER'])
        mu.reg_write(UC_X86_REG_EAX, ns['bomb_manager'])
        call(0x406ce0)
        call(0x413190, ns['MANAGER'])
        mu.reg_write(UC_X86_REG_EAX, ns['ufo_manager'])
        call(0x44a2d0)
        mu.reg_write(UC_X86_REG_EDI, ns['laser_manager'])
        call(0x428270)
        mu.reg_write(UC_X86_REG_ESI, ns['BULLETS'])
        call(0x40a020)
        call(0x425c20, ns['item_manager'])
        mu.reg_write(UC_X86_REG_EDI, ns['bonus_manager'])
        call(0x40db40)
        mu.reg_write(UC_X86_REG_EDI, ns['stage_manager'])
        call(0x41e020)
        mu.reg_write(UC_X86_REG_EDI, 0x5000000)
        call(0x460fe0)
        frames.append(snapshot(tick))
    # Native addresses and IDs are diagnostic only, never compared as portable
    # identities. The regression compares RNG and actual ordered death poses.
    return {'name': name or 'forced-arms-' + str(count), 'enemyCount': count,
            'armFlags': arm_flags, 'syntheticEclHex': raw.hex(),
            'syntheticMsgHex': message.hex(), 'character': 2, 'shot': 1,
            'difficulty': 3, 'seed': ns['args'].seed, 'frames': frames,
            'actualEntryCalls': events}


matrix = [('default', 0), ('already-marked', 0x1000000)]
for bit in [0x20, 0x80, 0x400, 0x200000, 0x400000]:
    matrix += [('protected-' + hex(bit), bit), ('force-' + hex(bit), bit | 0x100)]
isolated = [isolated_case(name, flags) for name, flags in matrix]
isolated += [isolated_case('death-script-' + str(script), 0, script) for script in [81, 87, 90, -1]]
isolated += [isolated_case('six-stage3-arm-bits', 0x2000851d, count=6),
             isolated_case('ten-stage3-arm-bits', 0x2000851d, count=10)]
integration = [integration_case(count) for count in [0, 1, 6, 10]]
integration += [integration_case(1, 0, 'unprotected-arm'),
                integration_case(1, 0x6004a0, 'protected-without-force'),
                integration_case(1, 0x6005a0, 'protected-with-force')]
output = ROOT / 'tests/fixtures/message-enemy-clear-original.json'
data = {'schema': 'th12-original-message-enemy-clear/1', 'originalExeSha256': EXE_SHA256,
        'resourceSha256': hashlib.sha256(BULLET).hexdigest(), 'wholeGameOracle': False,
        'samplerSha256': hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest(),
        'providerBootstrapSourceSha256': hashlib.sha256((ROOT / 'scripts/reverse/native-enemy-provider.py').read_bytes()).hexdigest(),
        'gpuExecuted': False, 'audibleOutputExecuted': False,
        'scope': 'Actual414c60 called consecutively without Enemy18 removal; actual454d10/455630 '
                 'and child factories/RNG execute normally. Integration executes native Enemy birth/18, '
                 'ECL418, MSG41fbe0/41f900/41fcc0 and ANM8/27 plus selected Player16/Bomb17/UFO19/'
                 'Laser20/Bullet21/Item22/Spell23/HUD25 managers, with provider declared host-only '
                 'resource/heap/audio boundaries. Synthetic literal actors/SCPT/MSG, no GPU or full '
                 'application startup. No mocked death visual, extra random burn or rewritten game result.',
        'assemblyEvidence': {'constructorClear': '41fba3→40d230; laser vtable+14(0,0);41fbcc→414c60',
                             'eclClear': '418825→40cf60(0);41882e→428750(EBX0,EDI0);418833→414c60',
                             'selection': '414c8b/a0 or414c8f/600400 protected,414c96/100 override; '
                                          'marked1000000 is not a skip branch',
                             'visual': '414d67→454d10;414d70→461250;414d92→flags|1000000; '
                                       'no453e20 or412140 call in414c60'},
        'isolated': isolated, 'integration': integration}
output.write_text(json.dumps(data, indent=2) + '\n', encoding='utf-8')
print(json.dumps({'output': str(output), 'isolatedCases': len(isolated),
                  'integrationCases': len(integration),
                  'integrationCalls': [[row['enemyCount'], [f['gameSeedCounter'][1] for f in row['frames']]]
                                       for row in integration]}))
