import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))

from bullet_probe import EXE_SHA256, NativeBullet, address, datum, emulate

OUTPUT = ROOT / 'local' / 'research' / 'stage-control-fixtures.json'
STAGE = 0x4d2e68
STAGE_SIZE = 0x9c4
FLAGS = STAGE + 0x590
SLICES = {
    'request': (0x411c70, 0x411c8e),
    'constructor': (0x436210, 0x436239),
    'constructorFlags': (0x436221, 0x436230),
    'stageFinalize': (0x4334b0, 0x43350f),
    'stageFinalizeClear': (0x4334f1, 0x4334fb),
    'stageStateMachine': (0x4358e0, 0x4359e1),
    'state12': (0x435a71, 0x435aa2),
    'stateCommit': (0x4359ae, 0x4359e1),
    'destroySession': (0x426980, 0x4269a6),
    'createSession': (0x426910, 0x42697a),
}


def stage_image(flags):
    image = bytearray(0x600)
    struct.pack_into('<I', image, 0x590, flags)
    return bytes(image)


def request_case(native, flags, requested):
    result = emulate(
        [native.chunk(0x411c70, 0x411c8e)],
        0x411c70,
        [datum('stage', STAGE, stage_image(flags))],
        registers={'eax': STAGE, 'ecx': requested},
        budget=16,
    )
    after = bytes.fromhex(result['memory']['stage']['after'])
    return {'flags': flags, 'requested': requested, 'stop': result['stop'],
            'pending': struct.unpack_from('<i', after, 0x558)[0],
            'instructionsExecuted': result['instructionsExecuted']}


def init_case(native, existing):
    result = emulate(
        [native.chunk(0x436221, 0x436230)],
        0x436221,
        [datum('flags', FLAGS, struct.pack('<I', existing))],
        stops=[0x436230],
        budget=16,
    )
    after = bytes.fromhex(result['memory']['flags']['after'])
    return {'existing': existing, 'stop': result['stop'],
            'flags': struct.unpack_from('<I', after, 0)[0]}


def clear_case(native, existing):
    result = emulate(
        [native.chunk(0x4334f1, 0x4334fb)],
        0x4334f1,
        [datum('flags', FLAGS, struct.pack('<I', existing))],
        stops=[0x4334fb],
        budget=16,
    )
    after = bytes.fromhex(result['memory']['flags']['after'])
    return {'existing': existing, 'stop': result['stop'],
            'flags': struct.unpack_from('<I', after, 0)[0]}


def constructor_case(native):
    result = emulate(
        [native.chunk(0x436210, 0x43621c)],
        0x436210,
        [],
        stops=[0x43621c],
        budget=16,
    )
    return {'stop': result['stop'],
            'stackHead': result['stackHead'],
            'arguments': [struct.unpack_from('<I', bytes.fromhex(result['stackHead']), offset)[0]
                          for offset in range(0, 12, 4)]}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    native = NativeBullet(arguments.exe)
    try:
        for start, end in SLICES.values():
            native.chunk(start, end)
        requests = [request_case(native, flags, 12) for flags in (0, 0x2000, 0x8000)]
        fixture = {
            'schema': 'th128-stage-control-fixtures/1',
            'executable': {'sha256': EXE_SHA256,
                           'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
                           'bytes': native.executable.stat().st_size,
                           'imageBase': '0x00400000'},
            'stage': {'address': address(STAGE), 'size': STAGE_SIZE,
                      'flags': address(FLAGS), 'flagsOffset': 0x590,
                      'currentOffset': 0x554, 'pendingOffset': 0x558,
                      'sessionFlagOffset': 0x564},
            'request': {'target': 12, 'collapsed': 2, 'lockMask': 0x2000},
            'initialization': {
                'memsetSize': 0x9c4,
                'initialFlags': 0x4240,
                'constructor': constructor_case(native),
                'cases': [init_case(native, value) for value in (0, 0x8000, 0x2000)],
                'clear': [clear_case(native, value) for value in (0x6240, 0x8240)],
            },
            'session': {'global': '0x004b8954', 'field': 0x74,
                        'create': '0x426910', 'destroy': '0x426980'},
            'requests': requests,
            'slices': native.code,
            'nativeCpuTrace': False,
            'windowsGameProcessLaunched': False,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        }
        if fixture['executable']['sha256'] != fixture['executable']['sha256After']:
            raise ValueError('executable changed during read-only probe')
        if [case['pending'] for case in requests] != [12, 2, 12]:
            raise ValueError('unexpected 411c70 request state')
        if fixture['initialization']['cases'][0]['flags'] != 0x4240:
            raise ValueError('unexpected constructor flags')
        OUTPUT.parent.mkdir(parents=True, exist_ok=True)
        OUTPUT.write_text(json.dumps(fixture, indent=2) + '\n', encoding='utf-8', newline='\n')
        print(f'stage control fixture: {len(requests)} request cases, '
              f'{len(fixture["slices"])} slices, {OUTPUT.stat().st_size} bytes')
    finally:
        native.image.close()


if __name__ == '__main__':
    main()
