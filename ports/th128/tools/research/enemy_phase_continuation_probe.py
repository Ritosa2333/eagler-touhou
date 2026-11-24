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
        enemy, program, records, names, headers = 0x300000, 0x380000, 0x390000, 0x3a0000, 0x3b0000
        for life, elapsed in ((6300, 100), (6299, 1799), (10000, 1800)):
            image = bytearray(0x5178)
            struct.pack_into('<I', image, 0x5144, enemy)
            struct.pack_into('<I', image, 0x11ac, program)
            struct.pack_into('<I', image, 0x11a8, 3)
            struct.pack_into('<3I', image, 0x1010, 32, 96, 8)
            for index in range(8):
                struct.pack_into('<I', image, 0x104c + 44 * index, 5)
                struct.pack_into('<4i', image, 0x50c4 + 16 * index, -1, -1, 0, 0)
            struct.pack_into('<i', image, 0x5004, life)
            struct.pack_into('<4I', image, 0x50c4, 6300, 1800, names + 8, names + 8)
            struct.pack_into('<iifII', image, 0x1434, elapsed - 1, elapsed, float(elapsed), 0x4b6f58, 1)
            struct.pack_into('<I', image, 0x13d0, 0x12345678)
            lookup = bytearray(0x90)
            struct.pack_into('<I', lookup, 8, 3)
            struct.pack_into('<I', lookup, 0x8c, records)
            text = b'Alpha\0\0\0MBoss2\0\0Zebra\0'
            table = struct.pack('<6I', names, headers, names + 8, headers + 32, names + 16, headers + 64)
            result = emulate([native.chunk(start, end) for start, end in (
                (0x416757, 0x4167ae), (0x41dea0, 0x41e0b8), (0x403280, 0x4032bd),
                (0x414500, 0x41452d), (0x414580, 0x4145fc), (0x470610, 0x470695))], 0x416757,
                [datum('enemy', enemy, bytes(image)), datum('program', program, bytes(lookup), writable=False),
                 datum('records', records, table, writable=False), datum('names', names, text, writable=False),
                 datum('headers', headers, bytes(96), writable=False), datum('hud', 0x3c0000, bytes(0x5b00)),
                 datum('hudGlobal', 0x4b8950, 0x3c0000, kind='I', writable=False),
                 datum('spell', 0x3d0000, bytes(0x84)), datum('spellGlobal', 0x4b8934, 0x3d0000, kind='I', writable=False),
                 datum('manager', 0x3e0000, bytes(0xcc)), datum('managerGlobal', 0x4b8948, 0x3e0000, kind='I', writable=False)],
                registers={'ebx': enemy + 0x11c8}, stops=(0x4167ae,), budget=4096)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            final = bytes.fromhex(result['memory']['enemy']['after'])
            result.update(input={'life': life, 'elapsed': elapsed}, after={
                'life': struct.unpack_from('<i', final, 0x5004)[0], 'phaseThreshold': struct.unpack_from('<i', final, 0x50c4)[0],
                'framePointer': struct.unpack_from('<I', final, 4)[0], 'timeBits': struct.unpack_from('<I', final, 8)[0],
                'scriptPointer': struct.unpack_from('<I', final, 12)[0], 'lookupResult': headers + 32 + 16,
                'asyncHead': struct.unpack_from('<I', final, 0x11b4)[0],
                'stackDepths': [struct.unpack_from('<I', final, 0x104c + 44 * index)[0] for index in range(8)],
                'preservedEntityVariable': struct.unpack_from('<I', final, 0x13d0)[0],
                'phaseTimer': list(struct.unpack_from('<iifII', final, 0x1434))})
            cases.append(result)
        return {'schema': 'th128-original-phase-continuation/1', 'cases': cases, 'code': native.code, 'constants': native.constants,
                'sourceExeSha256': hashlib.sha256(native.executable.read_bytes()).hexdigest(), 'apiReturnsAssigned': False,
                'scope': 'whole normal phase tail416757: real query, phase timer reset, empty async cleanup, full VM context reset, real sorted subroutine lookup and PC/time writes; stops before damage/death tail',
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    report = probe(parser.parse_args().exe)
    (ROOT / 'local/research/enemy-phase-continuation-fixtures.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print('3 original phase-query/reset/lookup/PC continuations recorded')
