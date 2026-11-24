import argparse
import hashlib
import json
from pathlib import Path
import struct
from bullet_probe import NativeBullet, ROOT, EXE_SHA256, CONTEXT, TASK, STATE, COMMAND, HANDLER_STOP, datum, emulate as pair_emulate
from enemy_probe import emulate

PLAYER, RNG, TRACK, OUTPUT = 0x380000, 0x4d2ae0, 0x390000, 0x3a0000


def probe(executable):
    native = NativeBullet(executable)
    try:
        index = native.read(0x41c7c0 + 312 - 256, 1)[0]
        if struct.unpack('<I', native.read(0x41c554 + index * 4, 4))[0] != 0x419f9e:
            raise ValueError('unexpected original312 dispatch')
        chunks = [native.chunk(start, end) for start, end in ((0x419f9e, 0x41a224),
            (0x41e0c0, 0x41e0d0), (0x41e110, 0x41e11e), (0x46fed0, 0x46ff66), (0x46ff70, 0x470026),
            (0x410d00, 0x410d42), (0x46a3f0, 0x46a459), (0x46a460, 0x46a4f5), (0x46a700, 0x46a790),
            (0x41ed90, 0x41eda3), (0x41edb0, 0x41edc3), (0x41ede0, 0x41ee14), (HANDLER_STOP, HANDLER_STOP + 2))]
        initials = []
        vectors = [(x, y, player_x, seed, speed) for x, y, player_x in ((0, 144, -30), (0, 144, 30),
            (-130, 144, 0), (130, 144, 0), (0, 120, 0), (0, 170, 0)) for seed, speed in ((0, 2), (34772, 4), (65535, 2))]
        vectors.extend((x,y,player_x,seed,2) for x,y,player_x in ((-72,128,0),(-72.001,128,0),
            (72,160,0),(72.001,160,0),(0,128,0),(0,127.999,0),(0,160,0),(0,160.001,0)) for seed in (0,34772))
        vectors.extend((0,144,player_x,seed,2) for player_x in (-30,0,30) for seed in range(0,65536,1024))
        for x, y, player_x, seed, speed in vectors:
            context = bytearray(0x4000)
            struct.pack_into('<I', context, 0x3f7c, TASK)
            struct.pack_into('<3f', context, 0x34, x, y, 0)
            struct.pack_into('<2fI', context, 0x80, 1, .5, 0)
            struct.pack_into('<I', context, 0x98, 0xffffffff)
            struct.pack_into('<4f', context, 0x3e28, 0, 144, 288, 64)
            state = bytearray(0x1020)
            struct.pack_into('<I', state, 4, COMMAND)
            command = struct.pack('<iHHHBBIiif', 0, 312, 28, 0, 255, 3, 0, 40, 4, speed)
            player = bytearray(0xed0)
            struct.pack_into('<f', player, 0xecc, player_x)
            random = struct.pack('<HHI', seed, 0, 0)
            result = emulate(chunks, 0x419f9e,
                [datum('context', CONTEXT, bytes(context)), datum('state', STATE, bytes(state)),
                 datum('task', TASK, struct.pack('<2I', 0, STATE), writable=False),
                 datum('command', COMMAND, command, writable=False), datum('player', PLAYER, bytes(player), writable=False),
                 datum('playerGlobal', 0x4b8a80, struct.pack('<I', PLAYER), writable=False),
                 datum('random', RNG, random), datum('systemFlags', 0x4d33f8, struct.pack('<I', 0), writable=False),
                 datum('zeroPair', 0x4d2e60, bytes(8), writable=False), native.constant(0x4a6500, 0x800)],
                registers={'eax': 312, 'ebx': CONTEXT}, stops=(HANDLER_STOP,), budget=2048)
            if result['stop'] != {'reason': 'boundary', 'address': '0x0041c52c'}:
                raise ValueError(result['stop'])
            after = bytes.fromhex(result['memory']['context']['after'])
            random_after = bytes.fromhex(result['memory']['random']['after'])
            result.update(input={'position': [x, y, 0], 'playerX': player_x, 'seed': seed, 'randomCalls': 0,
                'area': [0, 144, 288, 64], 'duration': 40, 'mode': 4, 'speed': speed, 'oldAngle': .5, 'oldSpeed': 1},
                after={'angle': struct.unpack_from('<f', after, 0x338)[0], 'targetSpeed': struct.unpack_from('<f', after, 0x344)[0],
                    'trackWords': list(struct.unpack_from('<15I', after, 0x338)), 'duration': struct.unpack_from('<i', after, 0x36c)[0],
                    'mode': struct.unpack_from('<i', after, 0x370)[0], 'modeFlags': struct.unpack_from('<I', after, 0x98)[0],
                    'seed': struct.unpack_from('<H', random_after)[0], 'randomCalls': struct.unpack_from('<I', random_after, 4)[0]})
            initials.append(result)
        steps = []
        chunks = [native.chunk(start, end) for start, end in ((0x41eb00, 0x41ed5f), (0x402ac0, 0x402b18),
            (0x46b190, 0x46b1c2), (0x4161f6, 0x416234), (0x46a700, 0x46a790))]
        for elapsed in (0, 1, 19, 38, 39):
            for angle, speed in ((-2.5, 2), (2.5, 4), (-3.415066719055176, 4), (3.415066719055176, 4)):
                context = bytearray(0x4000)
                struct.pack_into('<I', context, 0x98, 0)
                struct.pack_into('<4f', context, 0x338, angle, speed, angle, 0)
                struct.pack_into('<iifIIii', context, 0x358, elapsed - 1, elapsed, float(elapsed), 0x4b6f58, 1, 40, 4)
                result = pair_emulate(chunks, 0x4161f6,
                    [datum('context', CONTEXT, bytes(context)), datum('output', OUTPUT, bytes(8)),
                     datum('unitClock', 0x4b6f58, struct.pack('<f', 1), writable=False),
                     native.constant(0x4a6500, 0x800), native.constant(0x46b428, 64)],
                    registers={'esi': CONTEXT}, stops=(0x416234,), budget=512)
                if result['stop'] != {'reason': 'boundary', 'address': '0x00416234'}:
                    raise ValueError(result['stop'])
                track = bytes.fromhex(result['memory']['context']['after'])[0x338:0x374]
                helper = pair_emulate([chunks[0], chunks[1], chunks[2]], 0x41eb00,
                    [datum('track', TRACK, bytes.fromhex(result['memory']['context']['before'])[0x338:0x374]),
                     datum('output', OUTPUT, bytes(8)), datum('unitClock', 0x4b6f58, struct.pack('<f', 1), writable=False),
                     native.constant(0x4a6500, 0x800), native.constant(0x46b428, 64)],
                    registers={'edi': TRACK, 'ebx': OUTPUT}, budget=512)
                after = bytes.fromhex(result['memory']['context']['after'])
                if helper['stop']['reason'] != 'returned':
                    raise ValueError(helper['stop'])
                helper.update(input={'duration': 40, 'mode': 4, 'elapsed': elapsed, 'angle': angle, 'speed': speed},
                    caller=result, after={'angleBits': struct.unpack_from('<I', after, 0x84)[0],
                        'speedBits': struct.unpack_from('<I', after, 0x80)[0], 'trackWords': list(struct.unpack('<15I', track))})
                steps.append(helper)
        return {'schema': 'th128-original-enemy312-motion/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': hashlib.sha256(native.executable.read_bytes()).hexdigest()},
            'initializationCases': initials, 'stepCases': steps, 'code': native.code, 'constants': native.constants,
            'apiReturnsAssigned': False, 'windowsGameProcessLaunched': False, 'nativeCpuTrace': False,
            'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
            'scope': 'complete original312 randomized direction/pair setup with real RNG and explicit area/player state; original mode4 pair helper returns and caller normalization prefix; not whole enemy or motion frame trace'}
    finally:
        native.image.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--exe', required=True)
    arguments = parser.parse_args()
    report = probe(arguments.exe)
    (ROOT / 'local/research/enemy-move312-fixtures.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(f"{len(report['initializationCases'])} original312 RNG/direction pair setups and{len(report['stepCases'])} actual mode4 pair helper/caller samples")
