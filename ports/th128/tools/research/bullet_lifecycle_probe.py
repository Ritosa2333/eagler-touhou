import argparse
import hashlib
import json
import math
import struct
from pathlib import Path

from bullet_probe import NativeBullet, ROOT, STACK_POINTER, address, datum, emulate


EMITTER, BULLET, PLAYER = 0x370000, 0x390000, 0x3b0000


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local' / 'research'
    if (allowed.resolve() != allowed or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('bullet-lifecycle-')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be deliverable local/research/bullet-lifecycle-*.json')
    return destination


def vector(values, length):
    if not isinstance(values, (tuple, list)) or len(values) != length:
        raise ValueError('invalid bounded float32 vector')
    if any(type(value) not in (int, float) or abs(value) > 1e20 or not math.isfinite(value) for value in values):
        raise ValueError('finite bounded float32 values required')
    return struct.pack('<' + 'f' * length, *values)


def bullet_state(block):
    return {
        'flags': struct.unpack_from('<I', block, 0)[0],
        'positionBits': list(struct.unpack_from('<3I', block, 0x9d8)),
        'velocityBits': list(struct.unpack_from('<3I', block, 0x9e4)),
        'sizeBits': list(struct.unpack_from('<3I', block, 0x9f8)),
        'animationInterrupt': struct.unpack_from('<H', block, 0x400)[0],
        'motionState': struct.unpack_from('<H', block, 0xa2a)[0],
        'activeExtensions': struct.unpack_from('<I', block, 0xa20)[0],
        'emitterFlags': struct.unpack_from('<I', block, 0xa24)[0],
        'transformSound': struct.unpack_from('<i', block, 0xa38)[0],
        'extensionIndex': struct.unpack_from('<i', block, 0xa3c)[0],
        'profileValue': struct.unpack_from('<i', block, 0xa40)[0],
        'extensionBytes': block[0xa44:0xd5c].hex(),
    }


class NativeLifecycle(NativeBullet):
    def aim_prefix(self, position, player_position):
        block, player = bytearray(0x380), bytearray(0xed4)
        block[8:16] = vector(position, 2)
        player[0xecc:0xed4] = vector(player_position, 2)
        spans = ((0x40a8f0, 0x40a910), (0x43c3a0, 0x43c40c), (0x49007a, 0x490084),
                 (0x491380, 0x4913be), (0x491443, 0x4914bb), (0x491667, 0x4916f3),
                 (0x49289c, 0x4928bc), (0x4928ec, 0x492928), (0x4917c3, 0x4917ca))
        data = [datum('emitter', EMITTER, bytes(block)), datum('player', PLAYER, bytes(player), writable=False),
                datum('managerPointer', 0x4b8930, 0x380000, kind='I', writable=False),
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                datum('crtDispatch', 0x4b7720, 0, writable=False),
                datum('crtExceptions', 0x4dbc10, 0, writable=False),
                self.constant(0x4a66f0, 4), self.constant(0x4a6d3a, 10),
                self.constant(0x4a6d4c, 16), self.constant(0x4a9e80, 80)]
        result = emulate([self.chunk(start, end) for start, end in spans], 0x40a8f0, data,
                         registers={'esi': EMITTER}, stops=[0x40a910])
        result.update(completedEmission=False, entryKind='actual-manager-aim-prefix',
                      input={'position': list(struct.unpack('<2f', block[8:16])),
                             'playerPosition': list(struct.unpack('<2f', player[0xecc:0xed4])),
                             'crtDispatch': 0, 'crtExceptions': 0})
        if result['stop']['reason'] == 'boundary' and result['stackHead']:
            stack = bytes.fromhex(result['stackHead'])
            result['aimBits'] = struct.unpack_from('<I', stack, 8)[0]
        result['memory'] = {name: record for name, record in result['memory'].items()
                            if name not in ('emitter', 'player')}
        return result

    def spawn_tail(self, emitter, position, velocity, *, callback=0, post_animation_flags=0x13):
        if not isinstance(emitter, bytes) or len(emitter) != 0x380:
            raise ValueError('exact emitter span required')
        bullet_type, color = struct.unpack_from('<2i', emitter)
        extension_index = struct.unpack_from('<i', emitter, 0x378)[0]
        if (bullet_type != 8 or not 0 <= color <= 15 or extension_index != 0
                or type(callback) is not int or not 0 <= callback < 2**32
                or type(post_animation_flags) is not int or not 0 <= post_animation_flags < 2**32):
            raise ValueError('only bounded type8/color0..15/extension0 spawn tails are supported')
        block = bytearray(0x11b8)
        struct.pack_into('<I', block, 0, post_animation_flags)
        struct.pack_into('<I', block, 0x4d0, callback)
        struct.pack_into('<h', block, 0x11b0, bullet_type)
        struct.pack_into('<H', block, 0xa2a, 1)
        block[0x9d8:0x9e4], block[0x9e4:0x9f0] = vector(position, 3), vector(velocity, 3)
        profile_address = 0x4b24d0 + bullet_type * 0x120
        chunks = [self.chunk(0x4097d0, 0x4099df)]
        data = [datum('emitter', EMITTER, emitter, writable=False), datum('bullet', BULLET, bytes(block)),
                self.constant(profile_address, 0x120), self.constant(0x409a5c, 40), self.constant(0x4a6680, 8)]
        result = emulate(chunks, 0x4097d0, data, registers={'ebx': BULLET, 'edi': EMITTER,
                         'ebp': STACK_POINTER + 0x100}, stops=[0x4099df],
                         stack={0x10c: struct.pack('<I', EMITTER)})
        result.update(completedEmission=False, requiresAnimationSetup=True, completedExtensionDispatch=False,
                      entryKind='original-post-animation-spawn-tail',
                      input={'emitter': emitter.hex(), 'positionBits': list(struct.unpack('<3I', block[0x9d8:0x9e4])),
                             'velocityBits': list(struct.unpack('<3I', block[0x9e4:0x9f0])),
                             'animationCallback': callback, 'postAnimationFlags': post_animation_flags})
        if result['stop']['reason'] == 'boundary':
            result['state'] = bullet_state(bytes.fromhex(result['memory']['bullet']['after']))
        result['memory'] = {name: record for name, record in result['memory'].items() if name != 'bullet'}
        return result

    def motion_prefix(self, position, velocity, state, time_scale):
        scale = vector((time_scale,), 1)
        if type(state) is not int or state not in (1, 2):
            raise ValueError('only normal or spawn-state movement prefixes are supported')
        block = bytearray(0x11b8)
        block[0x9d8:0x9e4], block[0x9e4:0x9f0] = vector(position, 3), vector(velocity, 3)
        struct.pack_into('<H', block, 0xa2a, state)
        start, end = (0x408727, 0x40878d) if state == 2 else (0x4089e2, 0x408a2a)
        data = [datum('bullet', BULLET, bytes(block)), datum('timeScale', 0x4b6f58, scale, writable=False)]
        if state == 2:
            data.append(self.constant(0x4a65a8, 8))
        result = emulate([self.chunk(start, end)], start, data,
                         registers={'ebx': BULLET, 'ebp': STACK_POINTER + 0x100}, stops=[end])
        result.update(completedTick=False, entryKind='original-pre-collision-motion-prefix',
                      input={'positionBits': list(struct.unpack('<3I', block[0x9d8:0x9e4])),
                             'velocityBits': list(struct.unpack('<3I', block[0x9e4:0x9f0])),
                             'motionState': state, 'timeScaleBits': struct.unpack('<I', scale)[0]})
        if result['stop']['reason'] == 'boundary':
            final = bytes.fromhex(result['memory']['bullet']['after'])
            result['positionBits'] = list(struct.unpack_from('<3I', final, 0x9d8))
        result['memory'] = {name: record for name, record in result['memory'].items() if name != 'bullet'}
        return result


def probe(executable):
    native = NativeLifecycle(executable)
    cases = []
    for index, (position, player) in enumerate((((0, 0), (0, 400)), ((0, 0), (100, 0)),
            ((0, 0), (-100, 0)), ((0, 0), (0, -100)), ((0, 0), (30, 40)), ((0, 0), (-30, 40)),
            ((0, 0), (-30, -40)), ((0, 0), (30, -40)), ((0, 0), (0, 0)),
            ((-128, -32), (0, 400)), ((1.0000001192092896, 2), (1, 2.000000238418579)),
            ((10000000, -10000000), (-.375, .625)))):
        result = native.aim_prefix(position, player)
        result['id'] = f'aim-{index}'
        cases.append(result)
    emitter = bytearray(0x380)
    struct.pack_into('<2i', emitter, 0, 8, 10)
    struct.pack_into('<2f', emitter, 0x28, -999999, -999999)
    struct.pack_into('<2i', emitter, 0x3c, -999999, 0)
    struct.pack_into('<2i', emitter, 0x48, 2, 1)
    struct.pack_into('<3i', emitter, 0x36c, 35, 22, 40)
    for parameter in (-10, -7, -1, 0, 1, 32767, 65535, 65536):
        struct.pack_into('<i', emitter, 0x38, parameter)
        result = native.spawn_tail(bytes(emitter), (-128, -32, .125), (.25, .75, -.125))
        result['id'] = f'spawn-type2-{parameter}'
        cases.append(result)
    struct.pack_into('<i', emitter, 0x48, 0)
    result = native.spawn_tail(bytes(emitter), (1, 2, 3), (.25, .5, .75))
    result['id'] = 'spawn-none'
    cases.append(result)
    struct.pack_into('<2f', emitter, 0x28, .625, -8.25)
    struct.pack_into('<2i', emitter, 0x38, 1, -123456)
    struct.pack_into('<2i', emitter, 0x48, 2, 42)
    for flags in (0, 3):
        result = native.spawn_tail(bytes(emitter), (1, 2, 3), (.1, .3, -.2), post_animation_flags=flags)
        result['id'] = f'spawn-flags-{flags}'
        cases.append(result)
    for state in (1, 2):
        for index, (position, velocity, scale) in enumerate((((1, 2, 3), (.5, 1, -.25), 1),
                ((-129, -35, .625), (.25, .75, -.125), .5), ((0, -1, .5), (.125, -3, .625), 2),
                ((1, 2, 3), (.1, .3, -.2), .7), ((-10000, .01, 3), (.0001, -1.25, .5), 0))):
            result = native.motion_prefix(position, velocity, state, scale)
            result['id'] = f'motion-{state}-{index}'
            cases.append(result)
    smallest = struct.unpack('<f', struct.pack('<I', 1))[0]
    for identifier, state, position, velocity, scale in (
            ('cancel', 1, (-.07, 0, 0), (.1, 0, 0), .7),
            ('subnormal', 2, (0, 0, 0), (smallest, 0, 0), 2.9)):
        result = native.motion_prefix(position, velocity, state, scale)
        result['id'] = f'motion-{state}-store-{identifier}'
        cases.append(result)
    if any(case['stop']['reason'] != 'boundary' or case['instructionsExecuted'] >= case['instructionBudget'] for case in cases):
        raise ValueError('original-byte lifecycle case did not reach its declared boundary')
    after = hashlib.sha256(native.executable.read_bytes()).hexdigest()
    report = {'schema': 'th128-original-byte-bullet-lifecycle-fixtures/1',
              'nativeCpuTrace': False, 'windowsGameProcessLaunched': False,
              'executable': {'sha256': hashlib.sha256(native.binary).hexdigest(), 'sha256After': after,
                             'unchanged': native.binary == native.executable.read_bytes()},
              'code': native.code, 'constants': native.constants, 'cases': cases, 'caseCount': len(cases),
              'limitations': ['Original-byte emulation, not native Windows CPU execution.',
                              'CRT aim cases use explicit zero dispatch and exception flags.',
                              'Spawn tails start after the two original animation setup calls.',
                              'No animation callback, ETEX dispatcher, completed emission or completed tick is inferred.',
                              'Motion spans stop before collision, animation gates, timers, bounds and manager scheduling.']}
    native.image.close()
    return report


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded original-byte TH128 bullet lifecycle prefixes')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/bullet-lifecycle-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    if not report['executable']['unchanged']:
        raise ValueError('read-only executable digest changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(f'{report["caseCount"]} bounded original-byte cases; no complete emission or tick')


if __name__ == '__main__':
    main()
