import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from bullet_probe import NativeBullet, EXE_SHA256, datum, emulate  # noqa: E402

ECL_DIR = ROOT / 'artifacts/dsh-readonly-inputs/ecl'
ROUTES = ('st_a1_3', 'st_a2_3', 'st_b1_3', 'st_b2_3', 'st_c1_3', 'st_c2_3')
MANAGER, CONTEXT, OWNER2, OWNER3, COMMAND = 0x380000, 0x300000, 0x310000, 0x320000, 0x340000
TASK, STATE = 0x330000, 0x331000


def words(raw):
    return list(struct.unpack('<%dI' % (len(raw) // 4), raw))


def signed_words(raw):
    return list(struct.unpack('<%di' % (len(raw) // 4), raw))


def text_arg(raw, index):
    length = words(raw)[index]
    data = raw[(index + 1) * 4:(index + 1) * 4 + length]
    return data.split(b'\0')[0].decode('ascii')


def load(stem):
    path = ECL_DIR / (stem + '.json')
    data = path.read_bytes()
    return json.loads(data), hashlib.sha256(data).hexdigest()


def spawn_record(command):
    raw = bytes.fromhex(command['raw'])
    length = words(raw)[0]
    name = text_arg(raw, 0)
    offset = (1 + (length + 3) // 4) * 4
    x, y, life, score, drop = struct.unpack('<ffiii', raw[offset:offset + 20])
    return {'subroutine': name, 'x': x, 'y': y, 'life': life, 'score': score, 'drop': drop,
            'offset': command['offset'], 'time': command['time']}


def command_events(commands):
    events = []
    for command in commands:
        raw = bytes.fromhex(command['raw'])
        values = signed_words(raw)
        if command['opcode'] == 412:
            events.append({'subroutine': None, 'opcode': 412, 'offset': command['offset'],
                           'time': command['time'], 'slot': values[0]})
        elif command['opcode'] == 414:
            events.append({'subroutine': None, 'opcode': 414, 'offset': command['offset'],
                           'time': command['time'], 'slot': values[0], 'threshold': values[1],
                           'limit': values[2], 'target': text_arg(raw, 3)})
        elif command['opcode'] == 427:
            events.append({'subroutine': None, 'opcode': 427, 'offset': command['offset'],
                           'time': command['time'], 'slot': values[0],
                           'life': struct.unpack('<f', raw[4:8])[0], 'color': words(raw)[2]})
        elif command['opcode'] == 421:
            events.append({'subroutine': None, 'opcode': 421, 'offset': command['offset'],
                           'time': command['time'], 'slot': values[0], 'target': text_arg(raw, 1)})
        elif command['opcode'] == 411:
            events.append({'subroutine': None, 'opcode': 411, 'offset': command['offset'],
                           'time': command['time'], 'life': values[0]})
        elif command['opcode'] == 613:
            events.append({'subroutine': None, 'opcode': 613, 'offset': command['offset'],
                           'time': command['time'], 'argument': values[0]})
        elif command['opcode'] == 11:
            events.append({'subroutine': None, 'opcode': 11, 'offset': command['offset'],
                           'time': command['time'], 'target': text_arg(raw, 0)})
    return events


def chain(script, entry, limit=64, depth=10, seen=None):
    seen = set() if seen is None else seen
    subs = {sub['name']: sub for sub in script['subroutines']}
    collected = []

    def walk(name, level):
        if name in seen or level > depth or len(collected) >= limit or name not in subs:
            return
        seen.add(name)
        for event in command_events(subs[name]['commands']):
            if len(collected) >= limit:
                return
            event = dict(event, subroutine=name)
            collected.append(event)
            if event['opcode'] in (11, 414, 421):
                walk(event['target'], level + 1)

    walk(entry, 0)
    return collected


CORE_OPCODES = {0, 1, 10, 11, 12, 13, 14, 15, 16, 17, 21, *range(40, 46), *range(50, 79), 81, 82, 83, 84, 85, 86}
STAGEWORLD_OPCODES = {22, 256, 257, 258, 259, 260, 261, 262, 263, 269, 301, 312, 400, 401, 402, 403, 404, 405, 406,
                      407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 423, 424, 427,
                      304, 305, 435, 436, 437, 438, 439, 440, 448, 500, 501, 502, 503, 504, 505, 506, 507, 509,
                      510, 512, 513, 521, 522, 523, 524, 525, 537, 526}


def reachable(script, entries):
    subs = {sub['name']: sub for sub in script['subroutines']}
    seen, opcodes = set(), {}

    def walk(name, level):
        if name in seen or name not in subs or level > 10:
            return
        seen.add(name)
        for command in subs[name]['commands']:
            raw = bytes.fromhex(command['raw'])
            opcodes.setdefault(command['opcode'], set()).add(name)
            try:
                if command['opcode'] in (11, 15):
                    walk(text_arg(raw, 0), level + 1)
                elif command['opcode'] in (414, 421):
                    walk(text_arg(raw, 3 if command['opcode'] == 414 else 1), level + 1)
            except (ValueError, IndexError):
                continue

    for entry in entries:
        walk(entry, 0)
    missing = sorted(opcode for opcode in opcodes if opcode not in CORE_OPCODES | STAGEWORLD_OPCODES)
    return {'subroutines': len(seen), 'missingHostOpcodes': missing,
            'missingHostTargets': {str(opcode): sorted(opcodes[opcode]) for opcode in missing}}


def ecl_route(main_stem):
    main_script, main_hash = load(main_stem)
    boss_script, boss_hash = load(main_stem + 'b')
    after = next(sub for sub in main_script['subroutines'] if sub['name'] == 'MainAfterMBoss')
    spawns = {}
    for command in after['commands']:
        if command['opcode'] in (256, 257, 260, 261):
            record = spawn_record(command)
            if record['subroutine'] in ('BossA', 'BossB', 'BossC'):
                spawns[record['subroutine']] = record
    registrations = {}
    for name in ('BossA', 'BossB', 'BossC'):
        sub = next(item for item in boss_script['subroutines'] if item['name'] == name)
        events = [event for event in command_events(sub['commands']) if event['opcode'] == 412]
        registrations[name] = [event['slot'] for event in events]
    death = []
    for name in ('BossDead', 'BossDead2'):
        sub = next((item for item in boss_script['subroutines'] if item['name'] == name), None)
        if sub is None:
            continue
        events = [event for event in command_events(sub['commands']) if event['opcode'] == 412]
        death.append({'subroutine': name, 'slots': [event['slot'] for event in events]})
    midboss = []
    for name in ('MBoss', 'MBossDead', 'MBossEscape'):
        sub = next((item for item in main_script['subroutines'] if item['name'] == name), None)
        if sub is None:
            continue
        events = [event for event in command_events(sub['commands']) if event['opcode'] == 412]
        midboss.append({'subroutine': name, 'slots': [event['slot'] for event in events]})
    caller = [sub['name'] for sub in main_script['subroutines']
              for event in command_events(sub['commands'])
              if event['opcode'] == 11 and event['target'] == 'MainAfterMBoss']
    return {
        'main': main_stem, 'mainSha256': main_hash, 'include': main_stem + 'b',
        'includeSha256': boss_hash, 'includes': main_script['includes'],
        'spawns': spawns, 'registrations': registrations, 'deathUnregister': death,
        'midbossRegistration': midboss, 'postMidbossCallers': sorted(set(caller)),
        'chainA': chain(boss_script, 'BossA'), 'chainB': chain(boss_script, 'BossB'),
        'chainC': chain(boss_script, 'BossC'),
        'bossPath': reachable(boss_script, ('BossA', 'BossB', 'BossC')),
        'subroutineCounts': {'main': len(main_script['subroutines']), 'include': len(boss_script['subroutines'])},
    }


def identifier(goals):
    return {event['subroutine']: event['target'] for event in goals}


def disassemble(native, start, size):
    import capstone
    code = native.read(start, size)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [['0x%08x' % item.address, item.mnemonic, item.op_str] for item in decoder.disasm(code, start)]


def handler(native, opcode):
    index = native.read(0x41c7c0 + opcode - 256, 1)[0]
    return struct.unpack('<I', native.read(0x41c554 + 4 * index, 4))[0]


def registration_cases(native):
    stop = 0x41c52c
    chunks = [native.chunk(0x41a4bb, 0x41a517), native.chunk(0x41e0c0, 0x41e0d0),
              native.chunk(0x46fed0, 0x46ff66), native.chunk(stop, stop + 2)]
    cases = []
    for label, owners, slot, flags, retained, own_context in (
            ('register-slot0', (0, 0, 0), 0, 0x81000000, 0, CONTEXT),
            ('register-slot1', (0x600000, 0, 0), 1, 0x81000000, 0, OWNER2),
            ('register-slot2', (0x600000, 0x601000, 0), 2, 0x81000000, 0, OWNER3),
            ('clear-retained', (0x600000, 0x601000, 0x602000), -1, 0x81000000, 0, CONTEXT),
            ('clear-without-flag', (0x600000, 0x601000, 0x602000), -1, 0x80000000, 1, OWNER2),
            ('slot8-writes-flags', (0x600000, 0x601000, 0x602000), 8, 0x81000000, 0, CONTEXT)):
        manager = bytearray(0x80)
        struct.pack_into('<3I', manager, 0x5c, *owners)
        struct.pack_into('<I', manager, 0x7c, 1)
        context = bytearray(0x4000)
        struct.pack_into('<I', context, 0x3f7c, TASK)
        struct.pack_into('<I', context, 0x3ee4, flags)
        struct.pack_into('<i', context, 0x3ef4, retained)
        task = struct.pack('<2I', 0, STATE)
        state = struct.pack('<2I', 0, COMMAND)
        command = struct.pack('<iHHHBBIi', 0, 412, 20, 0, 255, 1, 0, slot) + bytes(8)
        result = emulate(chunks, 0x41a4bb, [
            datum('manager', MANAGER, bytes(manager)), datum('managerGlobal', 0x4b8948, MANAGER, 'I', False),
            datum('owner', CONTEXT, bytes(context)), datum('owner2', OWNER2, bytes(context)),
            datum('owner3', OWNER3, bytes(context)), datum('task', TASK, task, writable=False),
            datum('state', STATE, state), datum('command', COMMAND, command, writable=False)],
            registers={'ebx': own_context}, stops=(stop,), budget=64)
        if result['stop'] != {'reason': 'boundary', 'address': '0x0041c52c'}:
            raise ValueError(result['stop'])
        after_manager = bytes.fromhex(result['memory']['manager']['after'])
        region = {CONTEXT: 'owner', OWNER2: 'owner2', OWNER3: 'owner3'}[own_context]
        after_owner = bytes.fromhex(result['memory'][region]['after'])
        cases.append({'case': label, 'input': {'owners': list(owners), 'slot': slot, 'flags': flags,
                     'retained': retained}, 'ownersAfter': list(struct.unpack_from('<8I', after_manager, 0x5c)),
                     'managerFlagsAfter': struct.unpack_from('<I', after_manager, 0x7c)[0],
                     'flagsAfter': struct.unpack_from('<I', after_owner, 0x3ee4)[0],
                     'retainedAfter': struct.unpack_from('<i', after_owner, 0x3ef4)[0],
                     'ownerImage': '0x%06x' % own_context})
    return cases


def wait_cases(native):
    chunks = [native.chunk(0x41b3dd, 0x41b400), native.chunk(0x41c52c, 0x41c535)]
    cases = []
    for owners in ((0, 0, 0), (5, 0, 0), (0, 0, 7), (0, 0, 0, 9, 9, 9, 9, 9), (0, 0, 0, 0, 0, 0, 0, 9)):
        manager = bytearray(0x80)
        padded = tuple(owners) + (0,) * (8 - len(owners))
        struct.pack_into('<8I', manager, 0x5c, *padded)
        result = emulate(chunks, 0x41b3dd, [
            datum('manager', MANAGER, bytes(manager), writable=False),
            datum('managerGlobal', 0x4b8948, MANAGER, 'I', False)],
            stops=(0x41c52c, 0x41c52e), budget=32)
        cases.append({'owners': list(owners), 'stop': result['stop'],
                      'result': -1 if result['stop']['address'] == '0x0041c52e' else 0})
    return cases


def cross_boss_cases(native):
    stop = 0x4708a0
    chunks = [native.chunk(0x41c42f, 0x41c4b3), native.chunk(0x41e0c0, 0x41e0d0),
              native.chunk(0x46fed0, 0x46ff66), native.chunk(0x414500, 0x41452d),
              native.chunk(0x414580, 0x4145fc), native.chunk(stop, stop + 6),
              native.chunk(0x41c52c, 0x41c52e)]
    cases = []
    scenarios = (('both-active', ((1, 4200), (-1, 0), (1, 3600)), 0x3a0000, 4200),
                 ('skip-zero-target', ((1, 0), (-1, 0), (1, 3600)), 0x3c0000, 3600),
                 ('skip-inactive', ((-1, 4200), (-1, 0), (1, 3600)), 0x3c0000, 3600),
                 ('no-target-owner', ((1, 0), (-1, 0), (1, 0)), None, None))
    for label, records, expected_owner, expected_target in scenarios:
        manager = bytearray(0x80)
        struct.pack_into('<3I', manager, 0x5c, 0x3a0000, 0x3b0000, 0x3c0000)
        images = {}
        for base, (threshold, health_target) in zip((0x3a0000, 0x3b0000, 0x3c0000), records):
            image = bytearray(0x5180)
            struct.pack_into('<i', image, 0x50c4 + 16, threshold)
            struct.pack_into('<i', image, 0x50cc + 16, health_target)
            struct.pack_into('<I', image, 0x11b4, 0)
            struct.pack_into('<I', image, 0x11ac, 0)
            struct.pack_into('<I', image, 4, base + 8)
            images[base] = image
        context = bytearray(0x4000)
        struct.pack_into('<I', context, 0x3f7c, TASK)
        struct.pack_into('<i', context, 0x3ef4, 1)
        task = struct.pack('<2I', 0, STATE)
        state = struct.pack('<2I', 0, COMMAND)
        command = struct.pack('<iHHHBBIi', 0, 613, 20, 0, 255, 1, 0, 1) + bytes(8)
        result = emulate(chunks, 0x41c42f, [
            datum('manager', MANAGER, bytes(manager)), datum('managerGlobal', 0x4b8948, MANAGER, 'I', False),
            datum('self', 0x360000, bytes(context)), datum('ownerA', 0x3a0000, bytes(images[0x3a0000])),
            datum('ownerB', 0x3b0000, bytes(images[0x3b0000])), datum('ownerC', 0x3c0000, bytes(images[0x3c0000])),
            datum('task', TASK, task, writable=False), datum('state', STATE, state),
            datum('command', COMMAND, command, writable=False)],
            registers={'ebx': 0x360000}, stops=(stop, 0x41c52c), budget=512)
        owner_a = bytes.fromhex(result['memory']['ownerA']['after'])
        owner_b = bytes.fromhex(result['memory']['ownerB']['after'])
        owner_c = bytes.fromhex(result['memory']['ownerC']['after'])
        self_image = bytes.fromhex(result['memory']['self']['after'])
        cases.append({'case': label, 'records': [list(item) for item in records], 'stop': result['stop'],
                      'esi': '0x%06x' % result['registers']['esi'], 'ecx': result['registers']['ecx'],
                      'ownerAAfter': {'flags': struct.unpack_from('<I', owner_a, 0x11a8)[0],
                                      'float8': struct.unpack_from('<f', owner_a, 8)[0],
                                      'field1018': struct.unpack_from('<i', owner_a, 0x1018)[0],
                                      'field101c': '0x%06x' % struct.unpack_from('<I', owner_a, 0x101c)[0]},
                      'ownerBAfter': {'flags': struct.unpack_from('<I', owner_b, 0x11a8)[0]},
                      'ownerCAfter': {'flags': struct.unpack_from('<I', owner_c, 0x11a8)[0],
                                      'float8': struct.unpack_from('<f', owner_c, 8)[0],
                                      'field1018': struct.unpack_from('<i', owner_c, 0x1018)[0],
                                      'field101c': '0x%06x' % struct.unpack_from('<I', owner_c, 0x101c)[0]},
                      'selfFlags': struct.unpack_from('<I', self_image, 0x3ee4)[0],
                      'expected': {'owner': expected_owner, 'healthTargetWord': expected_target}})
    return cases


def probe(executable):
    native = NativeBullet(executable)
    try:
        if hashlib.sha256(native.executable.read_bytes()).hexdigest() != EXE_SHA256:
            raise ValueError('probe requires the pinned Japanese executable')
        handlers = {opcode: handler(native, opcode) for opcode in (412, 414, 420, 427, 613)}
        if handlers != {412: 0x41a4bb, 414: 0x41b2a7, 420: 0x41b3dd, 427: 0x41a238, 613: 0x41c42f}:
            raise ValueError('unexpected original handler dispatch')
        code = {
            'dispatchTable': {'arguments': '0x41c7c0', 'handlers': '0x41c554'},
            'handlers': {str(opcode): '0x%06x' % address for opcode, address in handlers.items()},
            'registration412': disassemble(native, 0x41a4bb, 0x5c),
            'wait420': disassemble(native, 0x41b3dd, 0x22),
            'gauge427': disassemble(native, 0x41a238, 0x4e),
            'crossBoss613': disassemble(native, 0x41c42f, 0x84),
            'crossBossHelpers': {'free0x11b4': disassemble(native, 0x414500, 0x2d),
                                 'resetEnemyState': disassemble(native, 0x414580, 0x60),
                                 'restartFromHealthTarget': disassemble(native, 0x4708a0, 0x1c)},
        }
        return {
            'schema': 'th128-original-thirdstage-boss/1',
            'executable': {'sha256': EXE_SHA256,
                           'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'routes': [ecl_route(stem) for stem in ROUTES],
            'code': code,
            'cases': {'registration': registration_cases(native), 'bossWait': wait_cases(native),
                      'crossBoss': cross_boss_cases(native)},
            'previousEvidence': {'phase': 'local/research/enemy-phase-fixtures.json',
                                 'phaseControl': 'local/research/enemy-phase-control-fixtures.json',
                                 'gauge': 'local/research/boss-gauge-fixtures.json',
                                 'wait': 'local/research/boss-wait-fixtures.json'},
            'apiReturnsAssigned': False, 'windowsGameProcessLaunched': False, 'nativeCpuTrace': True,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope': 'third-stage three-fairy ECL registration/phase/gauge/death commands and bounded original 412/420/613 handlers; '
                     'handler internals of 0x414500/0x414580/0x4708a0 stopped at the timer helper boundary'
        }
    finally:
        native.image.close()


def destination(path):
    target = Path(path).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if target.parent != allowed or target.name != 'thirdstage-boss-fixtures.json':
        raise ValueError('report must be local/research/thirdstage-boss-fixtures.json')
    return target


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--report', default=str(ROOT / 'local/research/thirdstage-boss-fixtures.json'))
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    destination(arguments.report).write_text(json.dumps(report, indent=1) + '\n', encoding='utf-8', newline='\n')
    spawns = sum(len(route['spawns']) for route in report['routes'])
    print('recorded %d routes, %d fairy spawns, %d registration, %d wait and %d cross-boss original cases'
          % (len(report['routes']), spawns, len(report['cases']['registration']),
             len(report['cases']['bossWait']), len(report['cases']['crossBoss'])))
