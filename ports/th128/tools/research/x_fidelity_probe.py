import argparse
import hashlib
import json
import struct
from pathlib import Path

import capstone
import pefile

ROOT = Path(__file__).resolve().parents[2]
EXE_SHA256 = 'ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4'
SCHEMA = 'th128-x-fidelity-native/1'


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
        ten = struct.unpack('<f', read(image, 0x4a6a60, 4))[0]
        two_hundred = struct.unpack('<f', read(image, 0x4a69ec, 4))[0]
        if ten != 10.0 or two_hundred != 200.0:
            raise ValueError('unexpected perfect protection constants')
        fixture = {
            'schema': SCHEMA,
            'sourceExeSha256': digest,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'launchesOriginalExecutable': False,
            'activation': {
                'entry': '0x407a70',
                'slice': disasm(image, 0x407a70, 0x407b20),
                'playerProtectionFields': {
                    '+0x438d8': 'previous=199', '+0x438dc': 'current=200',
                    '+0x438e0': 'fractional=200.0/0x4a69ec',
                    '+0x438e4': 'clock=0x4b6f58', '+0x438e8': 'flags bit0'},
            },
            'phase120': {
                'slice': disasm(image, 0x407c50, 0x407d80),
                'protectWriteBeforeFreeze': {
                    '+0x438d8': 'previous=9', '+0x438dc': 'current=10',
                    '+0x438e0': 'fractional=10.0/0x4a6a60',
                    '+0x438e4': 'clock=0x4b6f58'},
                'protectOrder': '0x407c92 timer10 setup, 0x407d77 freeze call',
            },
            'constants': {'protection200': two_hundred, 'phaseFreezeProtection10': ten},
            'limitations': ['Static read-only slices; no original process.',
                            'Phase120 manager/effect fields are recorded as remaining gaps, not guessed.'],
        }
        return fixture
    finally:
        image.close()


def main(argv=None):
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    parser.add_argument('--out', default=str(ROOT / 'local/research/x-fidelity-native-fixtures.json'))
    args = parser.parse_args(argv)
    destination = Path(args.out).resolve()
    allowed = (ROOT / 'local/research').resolve()
    if destination.parent != allowed or destination.suffix != '.json' or destination.drive.lower() == 'e:':
        raise ValueError('fixture destination must be local/research/x-fidelity-native-fixtures.json')
    fixture = probe(Path(args.exe))
    destination.write_text(json.dumps(fixture, indent=1, ensure_ascii=False) + '\n', encoding='utf-8')
    print('X activation protect200 @407a70; phase120 protect10 @407c92 before freeze @407d77')


if __name__ == '__main__':
    main()
