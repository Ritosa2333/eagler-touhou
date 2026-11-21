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
        index = native.read(0x41c7c0 + 410 - 256, 1)[0]
        if struct.unpack('<I', native.read(0x41c554 + index * 4, 4))[0] != 0x41a45e:
            raise ValueError('unexpected original410 dispatch')
        chunks = [native.chunk(0x41a45e, 0x41a472), native.chunk(0x41e0c0, 0x41e0d0),
                  native.chunk(0x46fed0, 0x46ff66), native.chunk(HANDLER_STOP, HANDLER_STOP + 2)]
        cases = []
        before = [8, *range(11, 21), 0x42400000, 0x42400000]
        for value in (-2147483648, -1, 0, 1, 2, 8, 17, 2147483647):
            context = bytearray(0x4000)
            struct.pack_into('<I', context, 0x3f7c, TASK)
            struct.pack_into('<13I', context, 0x3e54, *before)
            state = bytearray(0x1020)
            struct.pack_into('<I', state, 4, COMMAND)
            command = struct.pack('<iHHHBBIi', 0, 410, 20, 0, 255, 1, 0, value)
            result = emulate(chunks, 0x41a45e,
                [datum('context', CONTEXT, bytes(context)), datum('state', STATE, bytes(state)),
                 datum('task', TASK, struct.pack('<2I', 0, STATE), writable=False),
                 datum('command', COMMAND, command, writable=False)],
                registers={'ebx': CONTEXT}, stops=(HANDLER_STOP,), budget=128)
            if result['stop'] != {'reason': 'boundary', 'address': '0x0041c52c'}:
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['context']['after'])
            result.update(value=value, beforeWords=before, afterWords=list(struct.unpack_from('<13I', after, 0x3e54)))
            cases.append(result)
        return {'schema': 'th128-original-enemy410-drop/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'cases': cases, 'code': native.code, 'apiReturnsAssigned': False, 'windowsGameProcessLaunched': False,
            'nativeCpuTrace': False, 'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope': 'complete unmasked410 base-drop setter with real integer parameter helper; additional words/spread unchanged; no allocator/API/callback returns'}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    (ROOT / 'local/research/enemy-drop410-fixtures.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8', newline='\n')
    print('eight original410 base-drop setters; additional drop words and spread unchanged')
