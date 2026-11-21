import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-ecl-global-9946/1'
INTEGER_ENTRY = 0x41c950
INTEGER_TABLE = 0x41cfc4
INTEGER_TARGET = 0x41cc08
FLOAT_ENTRY = 0x41d2c0
FLOAT_TABLE = 0x41daa8
FLOAT_TARGET = 0x41d8e5


def read(image, address, size):
    data = bytes(image.get_data(address - 0x400000, size))
    if len(data) != size:
        raise ValueError('truncated original read')
    return data


def disasm(image, start, end):
    code = read(image, start, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [{'address': f'0x{item.address:08x}', 'bytes': item.bytes.hex(),
             'text': f'{item.mnemonic} {item.op_str}'.strip()}
            for item in decoder.disasm(code, start)]


def main_sub03():
    catalog = json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf-8'))
    raw = (ROOT / 'local' / catalog['variants']['ja']['manifest']).parent / 'raw'
    ecl = json.loads((raw.parent / 'ecl' / 'st_a1_2.json').read_text(encoding='utf-8'))
    for subroutine in ecl['subroutines']:
        if subroutine['name'] != 'MainSub03':
            continue
        for command in subroutine['commands']:
            if command['offset'] == 152:
                return {'subroutine': 'MainSub03', 'subroutineOffset': subroutine['offset'],
                        'instructionOffset': 152, **command}
        raise ValueError('MainSub03 offset152 is missing')
    raise ValueError('MainSub03 is missing')


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        scope = main_sub03()
        first = int.from_bytes(bytes.fromhex(scope['raw']), 'little', signed=True)
        if scope['opcode'] != 42 or scope['mask'] != 1 or first != -9946:
            raise ValueError('actual MainSub03 offset152 is not a -9946 read')
        integer_table = read(image, INTEGER_TABLE, 0x5c * 4)
        integer_target = struct.unpack_from('<I', integer_table, (first + 10000) * 4)[0]
        if integer_target != INTEGER_TARGET:
            raise ValueError('unexpected integer -9946 dispatch target')
        integer_values = [
            {'managerC0': value, 'preservedValue': value,
             'proof': 'static identity at 0x41cc0e: mov eax,[edx+0xc0]; no arithmetic'}
            for value in (0, 1, 2)
        ]
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'actualInstruction': {**scope, 'signedReference': first},
            'nativeReader': {
                'integerEntry': '0x41c950',
                'integerTable': '0x41cfc4',
                'integerTarget': '0x41cc08',
                'integerDisassembly': disasm(image, INTEGER_TARGET, 0x41cc1b),
                'floatEntry': '0x41d2c0',
                'floatTable': '0x41daa8',
                'floatTarget': '0x41d8e5',
                'floatDisassembly': disasm(image, FLOAT_TARGET, 0x41d8f6),
                'managerPointer': '0x4b8948',
                'field': 'manager+0xc0',
                'integerOperation': 'mov eax,[manager+0xc0] -> integer',
                'floatOperation': 'fild dword [manager+0xc0] -> float',
                'integerValues': integer_values,
                'nodeCountLifecycle': {
                    'incrementSpawn': '0x415231',
                    'decrementUnlink': '0x4152c8',
                    'decrementUnlink2': '0x415f25',
                },
            },
            'valueDomain': {
                'integerReader': '0x41cc08 returns integer manager+0xc0',
                'floatReader': '0x41d8e5 returns fild manager+0xc0',
                'opcode42': 'integer request',
                'opcode44': 'float request',
                'minimum': 0,
                'stageEntityCountIncludesRoot': True,
                'comparisonInActualEcl': 'unless ([-9946] <= 1)',
            },
            'stageWorldCurrent': {
                'referenceReader': 'StageWorld.cpp reference()',
                'missing': '-9946 returns std::nullopt before this fix',
                'owner': 'entities_ active/linked enemy nodes; no new global float field',
                'candidateCount': 'active entities_ count',
            },
            'limitations': [
                'No original EXE process execution.',
                'Node count is manager lifecycle evidence; no full-stage replay.',
            ],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/ecl-global-9946-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/ecl-global-9946-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('st_a1_2 MainSub03 offset152 opcode42 -9946; int 0x41c950/0x41cfc4->0x41cc08 count; float 0x41d8e5; lifecycle 0x415231/0x4152c8')


if __name__ == '__main__':
    main()
