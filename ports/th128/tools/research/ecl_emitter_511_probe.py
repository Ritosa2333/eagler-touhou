import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-ecl-emitter-511/1'


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


def actual_instruction():
    catalog = json.loads((ROOT / 'local/catalog.json').read_text(encoding='utf-8'))
    raw = (ROOT / 'local' / catalog['variants']['ja']['manifest']).parent / 'raw'
    ecl = json.loads((raw.parent / 'ecl' / 'st_a1_2.json').read_text(encoding='utf-8'))
    for subroutine in ecl['subroutines']:
        if subroutine['name'] != 'MBoss1At3':
            continue
        commands = subroutine['commands']
        for command in commands:
            if command['offset'] == 408:
                return ({'subroutine': 'MBoss1At3', 'subroutineOffset': subroutine['offset'], **command},
                        [{'offset': item['offset'], 'opcode': item['opcode']}
                         for item in commands if item['offset'] <= 408])
    raise ValueError('MBoss1At3 offset408 is missing')


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        actual, pre408 = actual_instruction()
        words = bytes.fromhex(actual['raw'])
        params = list(struct.unpack('<%di' % (len(words) // 4), words))
        if actual['opcode'] != 511 or actual['parameterCount'] != 2 or params != [4, 3]:
            raise ValueError('actual MBoss1At3 offset408 is not 511(4,3)')
        index = 511 - 256
        selector = read(image, 0x41c7c0 + index, 1)[0]
        target = struct.unpack('<I', read(image, 0x41c554 + 4 * selector, 4))[0]
        if target != 0x41a717:
            raise ValueError('unexpected opcode511 dispatch target')
        code = disasm(image, 0x41a717, 0x41a7b8)
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'actualInstruction': {**actual, 'parameters': params},
            'dispatch': {
                'opcodeArgumentByteAddress': f'0x{0x41c7c0+index:08x}',
                'argumentIndex': index,
                'selector': selector,
                'handlerTableAddress': f'0x{0x41c554+4*selector:08x}',
                'target': f'0x{target:08x}',
                'code': code,
            },
            'semantics': {
                'destinationSlot': 'I(0)',
                'sourceSlot': 'I(1)',
                'copyBytes': 0x380,
                'copyUnit': 'sdword',
                'auxiliaryCopies': ['+0x3ca0 + slot*12, 3 dwords', '+0x3d60 + slot*12, 3 dwords'],
                'selectedActual': {'destination': 4, 'source': 3},
                'doesNotEmitBullet': True,
            },
            'auxiliaryMapping': {
                'offsetX/offsetY': '+3ca0/+3ca4 (native 503/523 writes)',
                'absoluteX/absoluteY/absoluteOrigin': '+3d60/+3d64 plus flag (native 525 writes)',
                'thirdComponents': '+3ca8/+3d68 are not separately typed; supported 500/503/523/525 do not write nonzero, reset leaves zero',
                'actualPre408': pre408,
                'actualSourceAuxZero': True,
                'actualSourceReason': 'MBoss1At3 source slot3 pre-408 uses 500 reset and no 503/525; both auxiliary XY pairs stay zero',
            },
            'currentOwner': {
                'emitters': 'std::array<bullet::Emitter,16> at StageWorld Entity',
                'copy': 'whole typed Emitter assignment, including mapped offsetX/Y and absoluteX/Y/flags and extension records',
                'missing': '511 registration/host handler',
            },
            'limitations': [
                'No original EXE process execution.',
                'Auxiliary native arrays +0x3ca0/+0x3d60 are outside the typed Emitter subset.',
            ],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/ecl-emitter-511-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/ecl-emitter-511-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('MBoss1At3 offset408 511(4,3); dispatch selector=%d target=0x41a717; copy 0x380 source->destination'
          % fixture['dispatch']['selector'])


if __name__ == '__main__':
    main()
