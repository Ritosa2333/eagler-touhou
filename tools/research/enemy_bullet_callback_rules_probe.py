import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-enemy-bullet-callback-rules/1'


def read(image, address, size):
    data = bytes(image.get_data(address - 0x400000, size))
    if len(data) != size:
        raise ValueError('truncated original read')
    return data


def disasm(image, start, end):
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    return [{'address': f'0x{item.address:08x}', 'bytes': item.bytes.hex(),
             'text': f'{item.mnemonic} {item.op_str}'.strip()}
            for item in decoder.disasm(read(image, start, end - start), start)]


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        selector_table = struct.unpack('<5I', read(image, 0x4b1e54, 20))
        threshold = struct.unpack('<f', read(image, 0x4a6820, 4))[0]
        pi = struct.unpack('<d', read(image, 0x4a6580, 8))[0]
        tau = struct.unpack('<d', read(image, 0x4a6578, 8))[0]
        if selector_table != (0, 0x41e730, 0x41e860, 0x41e9b0, 0x41ea40):
            raise ValueError('unexpected 529 callback table')
        if threshold != 9216.0 or pi != 3.1415927410125732 or tau != 6.2831854820251465:
            raise ValueError('unexpected selector2 constants')
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'selectorTable': {'address': '0x4b1e54', 'values': list(selector_table)},
            'selector2': {
                'entry': '0x41e860', 'end': '0x41e9b0',
                'slice': disasm(image, 0x41e860, 0x41e9b0),
                'managerGlobal': '0x4b8930', 'poolOffset': 0x64,
                'stride': 0x11b8, 'capacity': 2000,
                'activeMask': 'byte[bullet+0] bit0', 'motionField': '+0xa2a == 1',
                'positionFields': ['bullet+0x9d8', 'bullet+0x9dc'],
                'entityFields': ['enemy+0x34', 'enemy+0x38', 'enemy+0x24c', 'enemy+0x250'],
                'distanceSquaredThreshold': threshold,
                'distanceComparison': 'strict < threshold; equal/above skipped',
                'branchFlags': {
                    'zeroDifference': 'fcomp 0 vs delta sets C3; 0x41e929 test ah,0x41 is nonzero',
                    'aboveEntity': '41e922 by>ey falls through; delta>=0 cannot take je41e94f -> 41e92e subtract; delta<0 je -> 41e94f add',
                    'equalOrBelowEntity': '41e927 by<=ey jumps to 41e93c; delta<0 falls to 41e941 subtract, delta>=0 jumps to 41e94f add',
                },
                'capacityContract': 'typed span size must be <= 2000; over-capacity rejected, sparse live span permitted within bound',
                'unsupportedNormalization': 'loops over 32 or nonfinite active bullet/entity state must reject the plan; no unchanged fake success',
                'angleHelper': '0x41ee90', 'atan2Helper': '0x49007a',
                'velocityHelper': '0x41f750', 'velocityFields': ['bullet+0x9e4', 'bullet+0x9e8'],
                'speedField': 'bullet+0x9f0', 'angleField': 'bullet+0x9f4',
                'angleNormalization': {'pi': pi, 'tau': tau},
                'var4': '-9981 / enemy+0x1414 / state+0x24c',
                'var5': '-9980 / enemy+0x1418 / state+0x250',
                'variableUpdate': 'var4 += var5 once after full 2000-slot pass',
                'callbackReturn': 0,
            },
            'selector0': {'callback': 0, 'rule': 'clear entity +0x3fa0; no bullet pass or var4 advance'},
            'selector3': {
                'entry': '0x41e9b0', 'notImplemented': True,
                'requiredOwner': 'existing Birth/Visual in-place type25 conversion; +4e8 callback/+4ec owner replacement',
            },
            'limitations': ['Static read-only slice; no original EXE execution.',
                            'The pure component is not linked to StageWorld/Manager.'],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/enemy-bullet-callback-rules-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/enemy-bullet-callback-rules-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('selector2 41e860..41e9b0 threshold=9216 strict<; stride11b8 cap2000; var4+=var5; selector3 dependency recorded')


if __name__ == '__main__':
    main()
