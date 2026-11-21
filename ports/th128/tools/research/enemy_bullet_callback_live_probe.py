import argparse
import hashlib
import json
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-enemy-bullet-callback-live/1'


def read(image, address, size):
    data = bytes(image.get_data(address - 0x400000, size))
    if len(data) != size:
        raise ValueError('truncated original read')
    return data


def probe(executable):
    binary = Path(executable).read_bytes()
    digest = hashlib.sha256(binary).hexdigest()
    if digest != EXE_SHA256:
        raise ValueError('pinned Japanese TH128 executable required')
    image = pefile.PE(data=binary)
    try:
        table = json.loads(
            (ROOT / 'local/research/enemy-bullet-callback-rules-fixtures.json').read_text(encoding='utf-8'))
        if table['selectorTable']['values'] != [0, 0x41e730, 0x41e860, 0x41e9b0, 0x41ea40]:
            raise ValueError('existing selector table fixture mismatch')
        decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        dispatch = [{'address': f'0x{item.address:08x}', 'text': f'{item.mnemonic} {item.op_str}'.strip()}
                    for item in decoder.disasm(read(image, 0x4164e1, 0x4164ff - 0x4164e1), 0x4164e1)]
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'dispatch': dispatch,
            'order': '0x4164e1 vm run; 0x4164ee callback slot after vm; 0x4164f8 ECX=entity callback',
            'callbackSlot': 'entity+0x3fa0', 'callbackReturnNonzero': '0x416575',
            'selectorTable': table['selectorTable'],
            'manager': {'global': '0x4b8930', 'poolOffset': 0x64, 'stride': 0x11b8, 'capacity': 2000},
            'bulletFields': {'active': '+0 bit0', 'motion': '+0xa2a', 'position': ['+0x9d8', '+0x9dc'],
                             'velocity': ['+0x9e4', '+0x9e8'], 'speed': '+0x9f0', 'angle': '+0x9f4'},
            'entityFields': {'variables4': '+0x24c', 'variables5': '+0x250'},
            'limitations': ['Static read-only probe; no original process.',
                            'StageWorld/Manager integration is the new owner; selector3 remains unsupported.'],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/enemy-bullet-callback-live-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/enemy-bullet-callback-live-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('callback dispatch post-vm=0x4164ee slot=entity+3fa0; selector2=0x41e860; manager=2000x11b8')


if __name__ == '__main__':
    main()
