import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, datum, emulate


def probe(executable):
    native = NativeBullet(executable)
    try:
        cases = []
        bullet, center = 0x390000, 0x3a0000
        for distance, radius in ((0., 640.), (641., 640.), (642., 640.), (0., 0.)):
            image = bytearray(0x11b8)
            struct.pack_into('<3f', image, 0x9d8, distance, 100, .1)
            struct.pack_into('<3f', image, 0x9e4, .5, 1.5, 0)
            struct.pack_into('<f', image, 0x9f8, 2.4)
            struct.pack_into('<i', image, 0xa1c, -1)
            struct.pack_into('<H', image, 0xa2a, 1)
            result = emulate([native.chunk(0x40c190, 0x40c393)], 0x40c190,
                [datum('bullet', bullet, bytes(image)), datum('center', center, struct.pack('<3f', 0, 100, .1), writable=False),
                 datum('clock', 0x4b6f58, struct.pack('<f', 1), writable=False),native.constant(0x4a65a8,8),
                 datum('managerPointer',0x4b8930,bullet-0x64,kind='I',writable=False)],
                arguments=struct.pack('<If', center, radius),
                stops=(0x40c377,), budget=512)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            final = bytes.fromhex(result['memory']['bullet']['after'])
            result.update(distance=distance, radius=radius, positionBits=list(struct.unpack_from('<3I', final, 0x9d8)),
                          motionState=struct.unpack_from('<H', final, 0xa2a)[0], pending=struct.unpack_from('<H', final, 0x400)[0],
                          lifeTimer=list(struct.unpack_from('<iifII', final, 0x1184)))
            cases.append(result)
        empty = emulate([native.chunk(0x42a7c0, 0x42a820)], 0x42a7c0,
            [datum('laserPointer', 0x4b8a60, 0x3c0000, kind='I', writable=False), datum('lasers', 0x3c0000, bytes(0x5e8)),
             datum('center', center, struct.pack('<3f', 0, 100, .1), writable=False)],
            registers={'esi': center}, arguments=struct.pack('<fii', 640., 1, 1), budget=256)
        if empty['stop']['reason'] != 'returned':
            raise ValueError(empty['stop'])
        return {'schema': 'th128-original-radius-cancel/1', 'cases': cases, 'emptyLaserReturn': empty,
                'code': native.code, 'constants': native.constants, 'apiReturnsAssigned': False,
                'sourceExeSha256': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
                'scope': 'real per-bullet 40c190 scalar/IRQ/half-step/state4/timer branch with no registered callbacks or cancel effect; real empty laser manager return',
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    report = probe(parser.parse_args().exe)
    (ROOT / 'local/research/bullet-radius-cancel-fixtures.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print('original radius-selected IRQ/state4 cancellation and empty laser return recorded')
