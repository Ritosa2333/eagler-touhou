import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, datum, emulate


def probe(executable):
    native = NativeBullet(executable)
    try:
        context, task, state, command = 0x300000, 0x310000, 0x320000, 0x330000
        cases = []
        for rank in range(5):
            image = bytearray(0x4000)
            struct.pack_into('<I', image, 0x3f7c, task)
            locals_image = bytearray(0x1020)
            struct.pack_into('<I', locals_image, 4, command)
            payload = struct.pack('<iHHHBBI5f', 0, 436, 36, 1, 255, 5, 0, 4., 1., 2., 4., 8.)
            chunks = [native.chunk(start, end) for start, end in (
                (0x41a5e2, 0x41a67f), (0x41e110, 0x41e11e), (0x46ff70, 0x470026),
                (0x41e120, 0x41e182), (0x48fac0, 0x48fadc), (0x41c52a, 0x41c52c))]
            result = emulate(chunks, 0x41a5e2, [
                datum('context', context, bytes(image)), datum('task', task, struct.pack('<2I', 0, state), writable=False),
                datum('state', state, bytes(locals_image)), datum('command', command, payload, writable=False),
                datum('difficulty', 0x4b4d0c, rank, writable=False), datum('ftolCpu', 0x4dcd68, 1, writable=False),
                native.constant(0x41c93c, 20)], registers={'ebx': context}, stops=(0x41c52c,), budget=2048)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            final = bytes.fromhex(result['memory']['state']['after'])
            result.update(difficulty=rank, destination=4, valueBits=struct.unpack_from('<I', final, 12)[0])
            cases.append(result)
        return {'schema': 'th128-boss-ecl436/1', 'cases': cases, 'code': native.code, 'constants': native.constants,
                'sourceExeSha256': hashlib.sha256(native.executable.read_bytes()).hexdigest(), 'apiReturnsAssigned': False,
                'scope': 'original436 difficulty selection, real float argument and local-destination helpers, original x87 integer conversion and final fstp store',
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    report = probe(parser.parse_args().exe)
    (ROOT / 'local/research/boss-ecl-fixtures.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print('5 original436 difficulty branches and actual floating local stores recorded')
