import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, EXE_SHA256, CONTEXT, TASK, STATE, COMMAND, HANDLER_STOP, datum
from enemy_probe import emulate


def probe(executable):
    native = NativeBullet(executable)
    try:
        index = native.read(0x41c7c0 + 404 - 256, 1)[0]
        if struct.unpack('<I', native.read(0x41c554 + index * 4, 4))[0] != 0x419f25:
            raise ValueError('unexpected original404 handler dispatch')
        registration = []
        chunks = [native.chunk(0x419f25, 0x419f79), native.chunk(0x41e110, 0x41e11e),
                  native.chunk(0x46ff70, 0x470026), native.chunk(HANDLER_STOP, HANDLER_STOP + 2)]
        for area in ((0, 144, 288, 64), (0, 128, 280, 64), (12.25, -4.75, 15.5, 30.125)):
            context = bytearray(0x4000)
            struct.pack_into('<I', context, 0x3f7c, TASK)
            state = bytearray(0x1020)
            struct.pack_into('<I', state, 4, COMMAND)
            command = struct.pack('<iHHHBBI4f', 0, 404, 32, 0, 255, 4, 0, *area)
            result = emulate(chunks, 0x419f25,
                [datum('context', CONTEXT, bytes(context)), datum('state', STATE, bytes(state)),
                 datum('task', TASK, struct.pack('<2I', 0, STATE), writable=False),
                 datum('command', COMMAND, command, writable=False)],
                registers={'ebx': CONTEXT}, stops=(HANDLER_STOP,), budget=256)
            if result['stop'] != {'reason': 'boundary', 'address': '0x0041c52c'}:
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['context']['after'])
            result.update(input={'area': list(area)}, after={'area': list(struct.unpack_from('<4f', after, 0x3e28)),
                'flags': struct.unpack_from('<I', after, 0x3ee4)[0]})
            registration.append(result)
        chunks = [native.chunk(0x416030, 0x41615c), native.chunk(0x46ae80, 0x46af07),
                  native.chunk(0x48fdf0, 0x48ff11)]
        combinations = []
        standard = (0, 144, 288, 64)
        vectors = [((0, 140, 0), (0, 0, 0), (0, 0, 0), standard, 0x40000),
                   ((-200, 100, 0), (10, -5, 0), (50, 50, 0), standard, 0x40000),
                   ((200, 200, 0), (-10, 5, 0), (-50, -50, 0), standard, 0x40000),
                   ((-144, 112, 0), (0, 0, 0), (0, 0, 0), standard, 0x40000),
                   ((144, 176, 0), (0, 0, 0), (0, 0, 0), standard, 0x40000),
                   ((-0.001, 120.001, .125), (0, 0, .25), (.001, -.001, .5), standard, 0x40000),
                   ((300, 300, 1), (20, 30, 2), (0, 0, 0), standard, 0),
                   ((.2, -.2, 0), (.1, -.1, 0), (1000, -1000, 0), (0, 0, 0, 0), 0x40000),
                   ((10, -20, .5), (.25, -.5, -.125), (-2, 3, 1), (12.25, -4.75, 15.5, 30.125), 0x40000),
                   ((20, 20, 0), (0, 0, 0), (0, 0, 0), (0, 0, -10, -20), 0x40000),
                   ((-20, -20, 0), (0, 0, 0), (0, 0, 0), (0, 0, -10, -20), 0x40000),
                   ((144.009, 176.009, 0), (0, 0, 0), (0, 0, 0), standard, 0x40000),
                   ((-143.999, 112.009, 0), (0, 0, 0), (0, 0, 0), standard, 0x40000),
                   ((60000, -60000, 4), (-5, 5, -2), (2, 2, 0), (10, 20, 300, 100), 0x40000)]
        for absolute, relative, combined, area, flags in vectors:
            context = bytearray(0x4000)
            struct.pack_into('<3f', context, 0x68, *absolute)
            struct.pack_into('<3f', context, 0x9c, *relative)
            struct.pack_into('<3f', context, 0x34, *combined)
            struct.pack_into('<4f', context, 0x3e28, *area)
            struct.pack_into('<I', context, 0x3ee4, flags)
            result = emulate(chunks, 0x416030,
                [datum('context', CONTEXT, bytes(context)), native.constant(0x4a6500, 0x800),
                 datum('declaredSse2Floor', 0x4dbc20, struct.pack('<I', 1), writable=False)],
                registers={'esi': CONTEXT}, budget=2048)
            if result['stop']['reason'] != 'returned':
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['context']['after'])
            result.update(input={'absolute': list(absolute), 'relative': list(relative), 'combined': list(combined),
                'area': list(area), 'flags': flags}, after={'absolute': list(struct.unpack_from('<3I', after, 0x68)),
                'relative': list(struct.unpack_from('<3I', after, 0x9c)), 'combined': list(struct.unpack_from('<3I', after, 0x34)),
                'velocity': list(struct.unpack_from('<3I', after, 0x40)), 'area': list(struct.unpack_from('<4f', after, 0x3e28)),
                'flags': struct.unpack_from('<I', after, 0x3ee4)[0]})
            combinations.append(result)
        return {'schema': 'th128-original-enemy404-clamp/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'registrationCases': registration, 'combineCases': combinations, 'code': native.code, 'constants': native.constants,
            'apiReturnsAssigned': False, 'windowsGameProcessLaunched': False, 'nativeCpuTrace': False,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope': 'complete unmasked404 registration and full416030 combine/integrate/floor/clamp/absolute rebuild returns with declared SSE2 floor state and explicit preallocated finite vectors; no host/API callbacks'}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    (ROOT / 'local/research/enemy-clamp-fixtures.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8', newline='\n')
    print('three original404 registrations and14 complete combine/floor/clamp/rebuild helper returns')
