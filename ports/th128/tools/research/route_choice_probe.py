import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
sys.path.insert(0, str(Path(__file__).resolve().parent))

from bullet_probe import EXE_SHA256, NativeBullet, STACK_POINTER, address, datum, emulate
from th128_assets.formats import parse_anm

MESSAGE = 0x3d0000
MESSAGE_SIZE = 0x1b0
KEY_PRESSED = 0x4db21c
FRONT = ROOT / 'artifacts' / 'dsh-readonly-inputs' / 'front.anm'
OUTPUT = ROOT / 'local' / 'research' / 'route-choice-fixtures.json'
SLICES = {
    'messageUpdate': (0x4231f0, 0x423683),
    'messageChoiceBranch': (0x4232bd, 0x423683),
    'msg30Handler': (0x42486a, 0x424928),
    'msg30AnmCall': (0x424883, 0x424891),
    'handleLookup': (0x467420, 0x467479),
    'queuedInterrupt': (0x467480, 0x4674e7),
    'spriteSelect': (0x459220, 0x4593ac),
    'soundEnqueue': (0x458410, 0x458498),
    'timerStart': (0x427420, 0x427497),
    'timerReset': (0x4274a0, 0x4274c0),
}
FIELDS = {'handle': 0x6c, 'frame': 0x70, 'state': 0x74, 'choice': 0x78,
          'applied': 0x7c, 'base': 0x80, 'text': 0x148}


def message_bytes(values=None, handle=0):
    image = bytearray(MESSAGE_SIZE)
    struct.pack_into('<I', image, FIELDS['handle'], handle)
    for name, value in (values or {}).items():
        struct.pack_into('<i', image, FIELDS[name], value)
    return bytes(image)


def message_fields(result):
    after = bytes.fromhex(result['memory']['message']['after'])
    return {name: struct.unpack_from('<i', after, offset)[0] for name, offset in FIELDS.items()}


def fields_record(before, after):
    return {name: {'before': before.get(name), 'after': after.get(name)}
            for name in FIELDS if name in before or name in after}


def choice_case(native, start, entry, fields, pressed, stops, handle):
    before = {name: fields.get(name, 0) for name in FIELDS}
    before['handle'] = handle
    result = emulate(
        [native.chunk(start, 0x423683)],
        entry,
        [datum('message', MESSAGE, message_bytes(fields, handle)),
         datum('pressed', KEY_PRESSED, struct.pack('<I', pressed), writable=False)],
        registers={'esi': MESSAGE, 'ebx': 0},
        stops=stops,
        budget=256,
    )
    after = message_fields(result)
    return {
        'input': {'pressed': pressed, 'fields': before},
        'stop': result['stop'],
        'registerEbx': result['registers']['ebx'],
        'stackHead': result['stackHead'],
        'fields': fields_record(before, after),
        'instructionsExecuted': result['instructionsExecuted'],
    }


def intro_case(native):
    before = {'frame': 41, 'state': 0, 'choice': 0, 'applied': 0, 'base': 0, 'handle': 0x1234abcd}
    result = emulate(
        [native.chunk(0x4232e4, 0x423327)],
        0x4232e4,
        [datum('message', MESSAGE, message_bytes(before, before['handle']))],
        registers={'esi': MESSAGE, 'ebx': 0},
        stops=[0x423321],
        budget=64,
    )
    after = message_fields(result)
    return {'input': before, 'stop': result['stop'],
            'fields': fields_record(before, after),
            'instructionsExecuted': result['instructionsExecuted']}


def msg30_args(native):
    result = emulate(
        [native.chunk(0x424883, 0x424891)],
        0x424883,
        [],
        registers={'ebp': 0x10a000},
        stops=[0x424891],
        budget=16,
    )
    return {'stop': result['stop'],
            'argumentsAtCall': list(struct.unpack_from('<4I', bytes.fromhex(result['stackHead']), 0))}


def msg30_fields(native, timer):
    before = {'handle': 0x12345678, 'frame': 99, 'state': 99, 'choice': 99, 'applied': 99,
              'base': timer}
    result = emulate(
        [native.chunk(0x424899, 0x4248be)],
        0x424899,
        [datum('message', MESSAGE, message_bytes(before, 0))],
        registers={'esi': MESSAGE, 'edx': 0x1234abcd, 'edi': 0},
        stops=[0x4248be],
        budget=64,
    )
    after = message_fields(result)
    return {'timer': timer, 'stop': result['stop'], 'fields': fields_record(before, after)}


def commit_case(native, entry, stop, before, registers):
    result = emulate(
        [native.chunk(entry, 0x423635)],
        entry,
        [datum('message', MESSAGE, message_bytes(before, before.get('handle', 0)))],
        registers={'esi': MESSAGE, **registers},
        stops=[stop],
        budget=64,
    )
    after = message_fields(result)
    return {'input': before, 'stop': result['stop'],
            'fields': fields_record(before, after),
            'instructionsExecuted': result['instructionsExecuted']}


def cleanup_case(native, before, stop):
    result = emulate(
        [native.chunk(0x423648, 0x423683)],
        0x423648,
        [datum('message', MESSAGE, message_bytes(before, before.get('handle', 0)))],
        registers={'esi': MESSAGE, 'ebx': 0},
        stops=[stop],
        budget=64,
    )
    after = message_fields(result)
    return {'input': before, 'stop': result['stop'],
            'registerEbx': result['registers']['ebx'],
            'stackHead': result['stackHead'],
            'fields': fields_record(before, after),
            'instructionsExecuted': result['instructionsExecuted']}


def cleanup_commit(native):
    before = {'handle': 0x12345678, 'frame': 40, 'state': 3, 'choice': 1, 'applied': 0, 'base': 3}
    result = emulate(
        [native.chunk(0x42365c, 0x42366d)],
        0x42365c,
        [datum('message', MESSAGE, message_bytes(before, before['handle']))],
        registers={'esi': MESSAGE, 'ebx': 1},
        stops=[0x423668],
        budget=16,
    )
    after = message_fields(result)
    return {'input': before, 'stop': result['stop'], 'fields': fields_record(before, after)}


def front_scripts():
    raw = FRONT.read_bytes()
    parsed = parse_anm(raw)
    scripts = {}
    for index in (166, 167, 168, 169):
        script = parsed['scripts'][index]
        scripts[str(index)] = {
            'index': script['index'],
            'sourceId': script['sourceId'],
            'entry': script['entry'],
            'offset': script['offset'],
            'commands': [{'time': item['time'], 'opcode': item['opcode'],
                          'mask': item['mask'], 'raw': item['raw']}
                         for item in script['commands']],
        }
    spawn = [item for item in scripts['169']['commands']
             if item['time'] == 40 and item['opcode'] == 88]
    return raw, parsed, scripts, spawn


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    native = NativeBullet(arguments.exe)
    try:
        front, parsed, scripts, spawn = front_scripts()
        for start, end in SLICES.values():
            native.chunk(start, end)
        table = native.read(0x4249f8, 16)
        emulations = {
            'intro': intro_case(native),
            'selectUp': choice_case(native, 0x423557, 0x423557,
                                    {'frame': 8, 'state': 1, 'choice': 2, 'applied': 0, 'base': 3},
                                    0x10, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'selectDown': choice_case(native, 0x423557, 0x423557,
                                      {'frame': 8, 'state': 1, 'choice': 2, 'applied': 0, 'base': 3},
                                      0x20, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'selectBoth': choice_case(native, 0x423557, 0x423557,
                                      {'frame': 8, 'state': 1, 'choice': 2, 'applied': 0, 'base': 3},
                                      0x30, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'selectNone': choice_case(native, 0x423557, 0x423557,
                                      {'frame': 8, 'state': 1, 'choice': 2, 'applied': 0, 'base': 3},
                                      0, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'selectLocked': choice_case(native, 0x423557, 0x423557,
                                        {'frame': 7, 'state': 2, 'choice': 0, 'applied': 0, 'base': 3},
                                        0x10, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'selectRepeat': choice_case(native, 0x423557, 0x423557,
                                        {'frame': 8, 'state': 2, 'choice': 0, 'applied': 0, 'base': 3},
                                        0x10, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'confirmUp': choice_case(native, 0x423557, 0x423557,
                                     {'frame': 8, 'state': 2, 'choice': 0, 'applied': 0, 'base': 3},
                                     0x80201, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'confirmDown': choice_case(native, 0x423557, 0x423557,
                                       {'frame': 8, 'state': 2, 'choice': 1, 'applied': 0, 'base': 3},
                                       0x80201, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'confirmLocked': choice_case(native, 0x423557, 0x423557,
                                         {'frame': 3, 'state': 2, 'choice': 1, 'applied': 0, 'base': 3},
                                         0x80201, [0x4235dd, 0x423615, 0x42366d], 0x12345678),
            'selectCommit': commit_case(native, 0x4235e2, 0x4235ee,
                                        {'handle': 0x12345678, 'frame': 8, 'state': 2,
                                         'choice': 0, 'applied': 2, 'base': 3}, {}),
            'confirmCommit': commit_case(native, 0x42361a, 0x42362d,
                                         {'handle': 0x12345678, 'frame': 8, 'state': 2,
                                          'choice': 1, 'applied': 0, 'base': 3}, {}),
            'cleanupDecision': cleanup_case(native,
                                            {'handle': 0x12345678, 'frame': 40, 'state': 3,
                                             'choice': 1, 'applied': 0, 'base': 3}, 0x423657),
            'cleanupLocked': cleanup_case(native,
                                          {'handle': 0x12345678, 'frame': 39, 'state': 3,
                                           'choice': 1, 'applied': 0, 'base': 3}, 0x42366d),
            'cleanupCommit': cleanup_commit(native),
            'msg30AnmCall': msg30_args(native),
            'msg30Fields': [msg30_fields(native, timer) for timer in (0, 5, -5)],
        }
        fixture = {
            'schema': 'th128-route-choice-fixtures/1',
            'executable': {'sha256': EXE_SHA256,
                           'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
                           'bytes': native.executable.stat().st_size,
                           'imageBase': '0x00400000'},
            'front': {'path': str(FRONT.relative_to(ROOT)).replace('\\', '/'),
                      'sha256': hashlib.sha256(front).hexdigest(),
                      'bytes': len(front),
                      'scripts': len(parsed['scripts']),
                      'sprites': len(parsed['sprites'])},
            'scripts': scripts,
            'spawnCommands': spawn,
            'fields': {name: address(value) for name, value in FIELDS.items()},
            'input': {'up': 0x10, 'down': 0x20, 'confirm': 0x80201,
                      'pressedGlobal': address(KEY_PRESSED)},
            'stageSprites': {
                '1': {'167': 0x70, '168': 0x71},
                '6': {'167': 0x6f, '168': 0x71},
                '11': {'167': 0x6f, '168': 0x70},
            },
            'stageSpriteSites': {
                '1': ['0x423504', '0x42349b'],
                '6': ['0x423447', '0x42349b'],
                '11': ['0x423387', '0x4233d7'],
            },
            'interrupts': {'selectBase': 7, 'confirmBase': 9, 'cleanup': 1,
                           'delivery': '0x467480'},
            'choiceJumpTable': {
                'address': '0x004249f8',
                'sha256': hashlib.sha256(table).hexdigest(),
                'targets': [address(struct.unpack_from('<I', table, offset)[0])
                            for offset in range(0, len(table), 4)],
            },
            'emulations': emulations,
            'slices': native.code,
            'nativeCpuTrace': False,
            'windowsGameProcessLaunched': False,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        }
        if fixture['executable']['sha256'] != fixture['executable']['sha256After']:
            raise ValueError('executable changed during read-only probe')
        if [item['raw'] for item in spawn] != ['a6000000', 'a7000000', 'a8000000']:
            raise ValueError('front169 spawn commands do not match 166/167/168')
        OUTPUT.parent.mkdir(parents=True, exist_ok=True)
        OUTPUT.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8', newline='\n')
        print(f'route choice fixture: {len(fixture["slices"])} slices, '
              f'{len(fixture["emulations"])} emulations, {OUTPUT.stat().st_size} bytes')
    finally:
        native.image.close()


if __name__ == '__main__':
    main()
