import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

from bullet_probe import EXE_SHA256, NativeBullet, address, datum, emulate

OUTPUT = ROOT / 'local' / 'research' / 'msg22-control-fixtures.json'
REGISTRY = 0x4d5a68
REGISTRY_SIZE = 0x4300
FTOL_FLAG = 0x4dcd68
CLOCK = 0x4b6f58
DEBUG_FLAG = 0x4d33f8
EMPTY_STRING = 0x4a28bd
SLICES = {
    'msg22Helper': (0x434a20, 0x434a70),
    'ftolHelper': (0x48fac0, 0x48fadc),
    'soundQueuePush': (0x458ff0, 0x45908e),
    'soundQueueConsume': (0x458640, 0x458930),
    'fadeOutCase': (0x4588b5, 0x4588ef),
    'fadeUpdate': (0x436100, 0x4361de),
    'volumeHelper': (0x46d290, 0x46d30a),
}
STRINGS = {
    'queueName': 0x4a28bd,
    'fadeLog': 0x4a5a1c,
    'bgmFormat': 0x4a2e08,
    'bgmData': 0x4a2e44,
    'dummyName': 0x4a2fac,
}
CONSTANTS = {
    'framesPerSecond': 0x4a65d0,
    'volumeDivisor': 0x4a6590,
}


def read_string(native, start):
    payload = native.read(start, 128)
    end = payload.find(b'\0')
    return payload[:end if end >= 0 else len(payload)]


def queue_case(native, base, clock):
    result = emulate(
        [native.chunk(0x434a20, 0x434a70), native.chunk(0x48fac0, 0x48fadc),
         native.chunk(0x458ff0, 0x45908e)],
        0x434a20,
        [datum('registry', REGISTRY, bytes(REGISTRY_SIZE)),
         datum('ftolFlag', FTOL_FLAG, struct.pack('<I', 1), writable=False),
         datum('clock', CLOCK, struct.pack('<f', clock), writable=False),
         datum('debugFlag', DEBUG_FLAG, struct.pack('<I', 0), writable=False),
         datum('emptyString', EMPTY_STRING, b'\0', writable=False)],
        registers={'edi': EMPTY_STRING},
        arguments=struct.pack('<f', base),
        budget=128,
    )
    image = bytes.fromhex(result['memory']['registry']['after'])
    slot = image[0x21ac:0x21b8]
    queue = {
        'operation': struct.unpack_from('<i', slot, 0)[0],
        'arg': struct.unpack_from('<i', slot, 4)[0],
        'field8': struct.unpack_from('<i', slot, 8)[0],
        'name': slot[0xc:].split(b'\0', 1)[0].decode('ascii'),
    }
    return {'base': base, 'clock': clock, 'stop': result['stop'], 'queue': queue,
            'instructionsExecuted': result['instructionsExecuted']}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    native = NativeBullet(arguments.exe)
    try:
        for start, end in SLICES.values():
            native.chunk(start, end)
        table = native.read(0x458b4c, 32)
        strings = {name: read_string(native, start).decode('ascii', 'replace')
                  for name, start in STRINGS.items()}
        constants = {name: struct.unpack('<d', native.read(start, 8))[0]
                     for name, start in CONSTANTS.items()}
        cases = [queue_case(native, base, clock) for base, clock in
                 ((2.0, 1.0), (8.0, 1.0), (2.0, 0.5), (8.0, 0.7),
                  (2.0, 0.0), (2.0, 2.0), (2.0, -1.0))]
        fixture = {
            'schema': 'th128-msg22-control-fixtures/1',
            'executable': {'sha256': EXE_SHA256,
                           'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
                           'bytes': native.executable.stat().st_size,
                           'imageBase': '0x00400000'},
            'strings': strings,
            'constants': constants,
            'queue': {'registry': address(REGISTRY), 'stride': 0x10c, 'capacity': 31,
                      'firstSlot': address(REGISTRY + 0x21ac),
                      'firstOffset': 0x21ac, 'consumer': '0x458640'},
            'operationTable': {
                'address': '0x00458b4c',
                'sha256': hashlib.sha256(table).hexdigest(),
                'targets': [address(struct.unpack_from('<I', table, offset)[0])
                            for offset in range(0, len(table), 4)],
            },
            'cases': cases,
            'slices': native.code,
            'nativeCpuTrace': False,
            'windowsGameProcessLaunched': False,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        }
        if fixture['executable']['sha256'] != fixture['executable']['sha256After']:
            raise ValueError('executable changed during read-only probe')
        for case in cases:
            if case['stop'].get('reason') != 'returned':
                raise ValueError('queue emulation did not return: ' + repr(case['stop']))
            if case['queue']['operation'] != 5 or case['queue']['name'] != '':
                raise ValueError('unexpected sound queue record')
        OUTPUT.parent.mkdir(parents=True, exist_ok=True)
        OUTPUT.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8', newline='\n')
        print(f'msg22 control fixture: {len(cases)} queue cases, '
              f'{len(fixture["slices"])} slices, {OUTPUT.stat().st_size} bytes')
    finally:
        native.image.close()


if __name__ == '__main__':
    main()
