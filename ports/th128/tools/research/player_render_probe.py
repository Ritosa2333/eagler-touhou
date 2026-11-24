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
        player = 0x390000
        for power in (0, 2999, 3000, 9999, 10000):
            result = emulate([native.chunk(0x43f697, 0x43f75e)], 0x43f697,
                [datum('player', player, bytes(0x9b4)), datum('power', 0x4b4d6c, power, writable=False),
                 native.constant(0x4a6c10, 4), native.constant(0x4a670c, 4)], registers={'esi': player},
                stops=(0x43f6e3, 0x43f753), budget=128)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            final = bytes.fromhex(result['memory']['player']['after'])
            result.update(power=power, scaleBits=struct.unpack_from('<I', final, 0x53c)[0],
                          color=struct.unpack_from('<I', final, 0x8ec)[0], flags=struct.unpack_from('<I', final, 0x9ac)[0])
            cases.append(result)
        sprites = []
        for frame in (0, 7, 8, 15, 16, 23, 24, 27):
            result = emulate([native.chunk(0x43bc0a, 0x43bc2b)], 0x43bc0a,
                [datum('resource', player + 0x928, 0x710000, kind='I', writable=False),
                 datum('frame', player + 0x42c, frame, kind='h', writable=False)], registers={'esi': player}, stops=(0x43bc26,), budget=64)
            if result['stop']['reason'] != 'boundary':
                raise ValueError(result['stop'])
            result.update(frame=frame, outlineSprite=result['registers']['ecx'])
            sprites.append(result)
        import capstone
        decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        passes = []
        for name, start, end, callback, priority in (
            ('hostile-bullets', 0x408292, 0x4082af, 0x4091d0, 32),
            ('player', 0x43a5e0, 0x43a5f3, 0x43c270, 25),
            ('frozen-bullets', 0x40e8db, 0x40e8f9, 0x4101c0, 24)):
            chunk = native.chunk(start, end)
            instructions = list(decoder.disasm(chunk[1], start))
            if not any(item.mnemonic == 'mov' and item.op_str == f'ebx, {priority:#x}' for item in instructions):
                raise ValueError('unexpected native draw registration priority')
            if not any(item.mnemonic in ('mov', 'push') and hex(callback) in item.op_str for item in instructions):
                raise ValueError('unexpected native draw callback')
            passes.append({'name': name, 'priority': priority, 'callback': hex(callback)})
        return {'schema': 'th128-original-player-render/1', 'powerCases': cases, 'spriteCases': sprites, 'passes': passes,
                'code': native.code, 'constants': native.constants, 'apiReturnsAssigned': False,
                'sourceExeSha256': hashlib.sha256(native.executable.read_bytes()).hexdigest(),
                'scope': 'real power silhouette color/scale stores and sprite+79 request; static explicit manual draw registrations, not a native GPU frame',
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    report = probe(parser.parse_args().exe)
    (ROOT / 'local/research/player-render-fixtures.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf8')
    print('original power silhouette and manual hostile/player/ice draw priorities recorded')
