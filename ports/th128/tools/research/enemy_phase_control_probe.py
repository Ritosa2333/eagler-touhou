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
        context, task, state, command = 0x300000, 0x310000, 0x320000, 0x330000
        for opcode, start, end in ((413, 0x41b4cf, 0x41b4e1), (415, 0x41a517, 0x41a531)):
            image = bytearray(0x4000)
            struct.pack_into('<I', image, 0x3f7c, task)
            offset = 0x26c if opcode == 413 else 0x3ea0
            struct.pack_into('<iifII', image, offset, 8, 9, 9., 0x4b6f58, 1)
            locals_image = bytearray(0x1020)
            struct.pack_into('<I', locals_image, 4, command)
            payload = struct.pack('<iHHHBBIi', 0, opcode, 20, 0, 255, 1, 0, 60)
            result = emulate([native.chunk(start, end), native.chunk(0x403280, 0x4032bd), native.chunk(0x41e0c0, 0x41e0d0),
                              native.chunk(0x46fed0, 0x46ff66),native.chunk(0x41c52c,0x41c52e)], start,
                [datum('context', context, bytes(image)), datum('task', task, struct.pack('<2I', 0, state), writable=False),
                 datum('state', state, bytes(locals_image)), datum('command', command, payload, writable=False)],
                registers={'ebx': context}, stops=(0x41c52c,), budget=512)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            final = bytes.fromhex(result['memory']['context']['after'])
            result.update(opcode=opcode, timer=list(struct.unpack_from('<iifII', final, offset)))
            cases.append(result)
        inactive=[]
        for flags in (0,0x10,0x40):
            result=emulate([native.chunk(0x40e350,0x40e4de)],0x40e350,
                [datum('spellGlobal',0x4b8934,0x3a0000,kind='I',writable=False),
                 datum('spell',0x3a0000,bytes(0x7c)+struct.pack('<I',flags))],budget=128)
            if result['stop']['reason']!='returned':raise ValueError(result['stop'])
            result.update(flags=flags);inactive.append(result)
        references = []
        target = struct.unpack('<I', native.read(0x41cfc4 + 14 * 4, 4))[0]
        if target != 0x41ca48:
            raise ValueError('unexpected -9986 integer reference target')
        for flags in (0, 0x81000000, 0x2000000, 0x83000000):
            result = emulate([native.chunk(0x41ca48, 0x41ca54)], target,
                [datum('flags', context + 0x50ac, flags, kind='I', writable=False)], registers={'ecx': context}, stops=(0x41ca54,), budget=32)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            result.update(flags=flags, timeout=result['registers']['eax'])
            references.append(result)
        return {'schema': 'th128-original-phase-control/1', 'cases': cases, 'timeoutReferences': references,'inactiveSpellEnd':inactive,
                'code': native.code, 'constants': native.constants, 'apiReturnsAssigned': False,
                'sourceExeSha256': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    report = probe(parser.parse_args().exe)
    (ROOT / 'local/research/enemy-phase-control-fixtures.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print('original413/415 timer stores and four -9986 timeout references recorded')
