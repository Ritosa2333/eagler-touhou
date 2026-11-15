import argparse
from collections import Counter
import hashlib
import json
import math
from pathlib import Path
import struct
from unittest.mock import patch

import unicorn
from unicorn.x86_const import UC_X86_REG_MXCSR
from bullet_probe import NativeBullet, ROOT, EXE_SHA256, address, datum
from freeze_probe import emulate, STACK_POINTER


PLAYER, RECORD, ANM_MANAGER, ANIMATION = 0x300000, 0x390000, 0x800000, 0x710000
POOL_OFFSET, SLOT_BYTES, SLOT_COUNT = 0xfa8, 0x78, 256
MAX_INSTRUCTIONS, MAX_CASES = 40000, 160
SHT_SHA256 = 'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b'
SPANS = ((0x43f760, 0x43fbdb), (0x46ae80, 0x46af07), (0x46c1f0, 0x46c210),
         (0x48fdf0, 0x48ff11), (0x467420, 0x467479), (0x4728fd, 0x472907))


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('player-shot-tick') or destination.drive.lower() == 'e:'):
        raise ValueError('report must be owned local/research/player-shot-tick*.json')
    return destination


def floating(value):
    if type(value) not in (int, float) or not math.isfinite(value) or abs(value) > 2**20:
        raise ValueError('bounded finite float32 required')
    return struct.unpack('<f', struct.pack('<f', value))[0]


def triple(value):
    if not isinstance(value, (list, tuple)) or len(value) != 3:
        raise ValueError('exact XYZ required')
    return [floating(component) for component in value]


def exact_integer(value):
    if type(value) is not int or not -2**31 <= value < 2**31:
        raise ValueError('exact signed int32 required')
    return value


def actual_record():
    local = (ROOT / 'local').resolve()
    catalog = json.loads((local / 'catalog.json').read_bytes())
    manifest = (local / catalog['variants']['ja']['manifest']).resolve()
    source = (manifest.parent / 'raw/pl00a.sht').resolve()
    if not manifest.is_relative_to(local) or not source.is_relative_to(local):
        raise ValueError('actual Japanese shot resource must stay in local generation')
    payload = source.read_bytes()
    if hashlib.sha256(payload).hexdigest() != SHT_SHA256:
        raise ValueError('unexpected actual shot resource')
    group = struct.unpack_from('<I', payload, 0x268)[0]
    return {'sha256': SHT_SHA256, 'group': 0, 'groupOffset': group,
            'recordOffset': group, 'record': payload[group:group + 52].hex()}


def slot_input(*, position=(0, 350, .1), velocity=(0, -24, 0), speed=24,
               angle=-1.5707963705062866, age=0, time=0, active=1, handle=0,
               secondary=0, slot=0, tick_target=0):
    position, velocity = triple(position), triple(velocity)
    exact_integer(age)
    if (type(slot) is not int or not 0 <= slot < SLOT_COUNT
            or any(type(value) is not int or not 0 <= value < 2**32
                   for value in (active, handle, secondary, tick_target))):
        raise ValueError('bounded exact pool index and native identities required')
    payload = bytearray(SLOT_BYTES)
    struct.pack_into('<ii fII', payload, 0, -1, age, floating(time), 0x4b6f58, 1)
    struct.pack_into('<3f', payload, 0x14, *position)
    struct.pack_into('<3f', payload, 0x20, *velocity)
    struct.pack_into('<2f', payload, 0x2c, floating(speed), floating(angle))
    struct.pack_into('<3I', payload, 0x48, active, handle, secondary)
    struct.pack_into('<I', payload, 0x58, 0x31415926)
    struct.pack_into('<i', payload, 0x60, 13)
    struct.pack_into('<2f', payload, 0x68, 18, 48)
    struct.pack_into('<I', payload, 0x74, RECORD)
    return bytes(payload), slot, tick_target


def execute(chunks, entry, data, **options):
    if (len(chunks) > 16 or sum(len(code) for _, code in chunks) > 8192
            or len(data) > 32 or sum(len(span['bytes']) for span in data) > 131072):
        raise ValueError('explicit shot tick code/data budget exceeded')
    factory = unicorn.Uc

    def configured_machine(*arguments, **keywords):
        machine = factory(*arguments, **keywords)
        machine.reg_write(UC_X86_REG_MXCSR, 0x1f80)
        return machine

    with patch.object(unicorn, 'Uc', configured_machine):
        result = emulate(chunks, entry, data, budget=MAX_INSTRUCTIONS, **options)
    if result['stop']['reason'] not in ('returned', 'boundary', 'unsupported_call'):
        raise ValueError(f'original-byte execution failed its boundary: {result["stop"]}')
    if result['instructionsExecuted'] >= MAX_INSTRUCTIONS:
        raise ValueError('tick instruction budget exhausted')
    result['cpuEnvironment'] = {'x87Control': 0x037f, 'mxcsr': 0x1f80,
                                'floorFeatureFlag': 1}
    return result


def compact(result):
    trace = result.pop('instructionTrace')
    result['traceSummary'] = {'length': len(trace), 'sha256': hashlib.sha256('\n'.join(trace).encode('ascii')).hexdigest(),
                              'head': trace[:24], 'tail': trace[-16:], 'visits': dict(sorted(Counter(trace).items()))}
    result['declaredData'] = [{'name': name, 'address': span['address'], 'bytes': span['bytes'],
                               'writable': span['writable'],
                               'beforeSha256': hashlib.sha256(bytes.fromhex(span['before'])).hexdigest(),
                               'afterSha256': hashlib.sha256(bytes.fromhex(span['after'])).hexdigest()}
                              for name, span in result['memory'].items()]
    result.pop('memory')
    return result


class NativePlayerShotTick(NativeBullet):
    def constants_data(self):
        return [self.constant(pointer, size) for pointer, size in
                ((0x4a6590, 8), (0x4a6ca0, 80), (0x4a6588, 8), (0x4a6698, 8),
                 (0x4a6694, 4), (0x4a66a0, 4), (0x4a6770, 4), (0x4a6760, 4), (0x4a6768, 8))]

    def tick(self, *, stage='movement', **changes):
        if stage not in ('movement', 'returned', 'callback', 'motion'):
            raise ValueError('known tick evidence stage required')
        before, selected, target = slot_input(**changes)
        pool = bytearray(SLOT_COUNT * SLOT_BYTES)
        pool[selected * SLOT_BYTES:(selected + 1) * SLOT_BYTES] = before
        record = bytearray(bytes.fromhex(actual_record()['record']))
        struct.pack_into('<I', record, 0x28, target)
        heads = bytes(12)
        data = [datum('pool', PLAYER + POOL_OFFSET, bytes(pool)),
                datum('record', RECORD, bytes(record), writable=False),
                datum('cookie', 0x4b01f4, 0x12345678, kind='I', writable=False),
                datum('clock', 0x4b6f58, struct.pack('<f', 1), writable=False),
                datum('anmManager', 0x4d2e50, ANM_MANAGER, kind='I', writable=False),
                datum('heads', ANM_MANAGER + 0x8b9704, heads, writable=False),
                datum('floorFeature', 0x4dbc20, 1, writable=False),
                datum('actualPlayer', 0x4b8a80, PLAYER, kind='I', writable=False), *self.constants_data()]
        spans = [span for span in SPANS if stage != 'motion' or span[0] != 0x46c1f0]
        result = execute([self.chunk(*span) for span in spans], 0x43f760, data,
                         arguments=struct.pack('<I', PLAYER), stops=[0x43f823] if stage == 'movement' else [])
        after = bytes.fromhex(result['memory']['pool']['after'])[selected * SLOT_BYTES:(selected + 1) * SLOT_BYTES]
        result.update(stage=stage, selectedSlot=selected, callbackTarget=target, before=before.hex(), next=after.hex(),
                      completedPass=result['stop']['reason'] == 'returned')
        return compact(result)

    def integrate(self, **changes):
        before, _, _ = slot_input(**changes)
        data = [datum('slot', PLAYER + POOL_OFFSET, before), datum('floorFeature', 0x4dbc20, 1, writable=False),
                *self.constants_data()]
        result = execute([self.chunk(0x46ae80, 0x46af07), self.chunk(0x48fdf0, 0x48ff11)],
                         0x46ae80, data, registers={'eax': PLAYER + POOL_OFFSET + 0x14})
        result.update(before=before.hex(), next=result['memory']['slot']['after'], completedFunction=True)
        return compact(result)

    def age(self, **changes):
        before, _, _ = slot_input(**changes)
        location = PLAYER + POOL_OFFSET
        data = [datum('slot', location, before), datum('clock', 0x4b6f58, struct.pack('<f', 1), writable=False),
                *self.constants_data()]
        result = execute([self.chunk(0x43fa5e, 0x43faa4)], 0x43fa5e, data,
                         registers={'ebp': STACK_POINTER + 0x100, 'edi': location + 0x20},
                         stack={0xbc: struct.pack('<I', location)}, stops=[0x43fbae])
        result.update(before=before.hex(), next=result['memory']['slot']['after'], completedTick=False,
                      independentSurvivorTail=True)
        return compact(result)

    def bounds(self, corners, *, age=10):
        if not isinstance(corners, (list, tuple)) or len(corners) != 4 or any(len(point) != 2 for point in corners):
            raise ValueError('exact four XY bounds required')
        corners = [[floating(value) for value in point] for point in corners]
        before, _, _ = slot_input(age=age)
        stack = {offset: struct.pack('<3f', *point, 0) for offset, point in zip((0xcc, 0xd8, 0xe4, 0xf0), corners)}
        result = execute([self.chunk(0x43f936, 0x43fa42), self.chunk(0x43faa4, 0x43fb60)],
                         0x43f936, [datum('slot', PLAYER + POOL_OFFSET, before),
                                    datum('record', RECORD, bytes(52), writable=False), *self.constants_data()],
                         registers={'ebp': STACK_POINTER + 0x100, 'edi': PLAYER + POOL_OFFSET + 0x20},
                         stack=stack, stops=[0x43f9e6, 0x43fb60])
        result.update(cornerBits=[list(struct.unpack('<2I', struct.pack('<2f', *point))) for point in corners],
                      age=age, survivor=result['stop']['address'] == '0x0043f9e6', completedTick=False)
        return compact(result)

    def axis_bounds(self, *, position=(192, 216, 0), sprite=(32, 64), scale=(.1, .8), anchors=(0, 0)):
        position = triple(position)
        if len(sprite) != 2 or len(scale) != 2 or len(anchors) != 2 or any(value not in (0, 1, 2) for value in anchors):
            raise ValueError('bounded axis-aligned actual animation state required')
        animation = bytearray(0x4c0)
        struct.pack_into('<4f', animation, 0x40, *[floating(value) for value in scale], 1, 1)
        struct.pack_into('<2f', animation, 0x60, *[floating(value) for value in sprite])
        struct.pack_into('<3f', animation, 0x464, *position)
        flags = anchors[0] << 20 | anchors[1] << 22
        struct.pack_into('<I', animation, 0x4b0, flags)
        data = [datum('animation', ANIMATION, bytes(animation), writable=False),
                datum('output', RECORD, bytes(48)), datum('floorFeature', 0x4dbc20, 1, writable=False),
                *self.constants_data(), self.constant(0x4a65a8, 8)]
        result = execute([self.chunk(0x45f110, 0x45f5c9), self.chunk(0x48fdf0, 0x48ff11)], 0x45f110, data,
                         registers={'esi': ANIMATION, 'edi': RECORD + 36, 'ebx': RECORD + 24},
                         arguments=struct.pack('<2I', RECORD, RECORD + 12))
        output = bytes.fromhex(result['memory']['output']['after'])
        result.update(positionBits=list(struct.unpack('<3I', struct.pack('<3f', *position))),
                      spriteBits=list(struct.unpack('<2I', struct.pack('<2f', *sprite))),
                      scaleBits=list(struct.unpack('<2I', struct.pack('<2f', *scale))), flags=flags,
                      cornerBits=[list(struct.unpack_from('<2I', output, offset)) for offset in (0, 12, 24, 36)],
                      completedFunction=True)
        return compact(result)

    def sync(self, *, position=(-128.25, 401.5, .1)):
        before, _, _ = slot_input(position=position)
        result = execute([self.chunk(0x43f9e6, 0x43fa5e)], 0x43f9e6,
                         [datum('slot', PLAYER + POOL_OFFSET, before),
                          datum('animation', ANIMATION, bytes(0x4c0)),
                          self.constant(0x4a65f8, 8), self.constant(0x4a65d8, 8), self.constant(0x4a6610, 8)],
                         registers={'ebx': PLAYER + POOL_OFFSET + 0x14,
                                    'edi': PLAYER + POOL_OFFSET + 0x20, 'esi': ANIMATION},
                         stops=[0x43fa1c])
        animation = bytes.fromhex(result['memory']['animation']['after'])
        result.update(before=before.hex(), animationPositionBits=list(struct.unpack_from('<3I', animation, 0x464)),
                      completedTick=False, independentSyncPrefix=True)
        return compact(result)

    def rotation_basis(self, rotation):
        rotation = floating(rotation)
        if abs(rotation) > 3.141593:
            raise ValueError('rotation must already be normalized')
        result = execute([self.chunk(0x46c1f0, 0x46c210)], 0x46c1f0,
                         [datum('output', RECORD, bytes(8))], registers={'ecx': RECORD},
                         arguments=struct.pack('<2f', rotation, 1))
        result.update(cosineSineBits=list(struct.unpack('<2I', bytes.fromhex(result['memory']['output']['after']))))
        return compact(result)

    def rotated_bounds(self, *, position=(310, 408, .1), sprite=(32, 64), scale=(1, .8),
                       anchors=(0, 0), rotation=-1.5707963705062866):
        position, rotation = triple(position), floating(rotation)
        if (len(sprite) != 2 or len(scale) != 2 or len(anchors) != 2
                or any(value not in (0, 1, 2) for value in anchors) or abs(rotation) > 3.141593):
            raise ValueError('bounded no-parent mode1 animation state required')
        animation = bytearray(0x4c0)
        struct.pack_into('<f', animation, 0x2c, rotation)
        struct.pack_into('<4f', animation, 0x40, *[floating(value) for value in scale], 1, 1)
        struct.pack_into('<2f', animation, 0x60, *[floating(value) for value in sprite])
        struct.pack_into('<3f', animation, 0x464, *position)
        flags = 1 << 24 | anchors[0] << 20 | anchors[1] << 22
        struct.pack_into('<I', animation, 0x4b0, flags)
        result = execute([self.chunk(0x45fad0, 0x45fd3f)], 0x45fad0,
                         [datum('animation', ANIMATION, bytes(animation), writable=False),
                          datum('output', RECORD, bytes(48)), self.constant(0x4a65a8, 8)],
                         registers={'eax': ANIMATION, 'esi': RECORD + 36, 'edi': RECORD + 24, 'ebx': RECORD + 12},
                         arguments=struct.pack('<I', RECORD))
        output = bytes.fromhex(result['memory']['output']['after'])
        result.update(positionBits=list(struct.unpack('<3I', struct.pack('<3f', *position))),
                      spriteBits=list(struct.unpack('<2I', struct.pack('<2f', *sprite))),
                      scaleBits=list(struct.unpack('<2I', struct.pack('<2f', *scale))), flags=flags,
                      rotationBits=struct.unpack('<I', struct.pack('<f', rotation))[0],
                      basisEvidence=self.rotation_basis(rotation),
                      cornerBits=[list(struct.unpack_from('<2I', output, offset)) for offset in (0, 12, 24, 36)],
                      completedFunction=True)
        return compact(result)


def probe(executable):
    native = NativePlayerShotTick(executable)
    try:
        movement = [native.tick(), native.tick(position=(-10, 350, 0)), native.tick(slot=255),
                    native.tick(age=11, time=11, handle=0x1234), native.tick(position=(-.125, .125, -0.0))]
        returned = [native.tick(stage='returned', **changes) for changes in
                    ({}, {'slot': 255, 'handle': 123}, {'secondary': 456}, {'active': 0})]
        prefixes = [native.tick(stage='callback', tick_target=0x720000), native.tick(stage='motion')]
        integrate = [native.integrate(**changes) for changes in
                     ({}, {'position': (-.125, .125, -0.0), 'velocity': (-.001, -.001, 0)},
                      {'position': (0, 0, 0), 'velocity': (0, 0, 0)},
                      {'position': (1, -1, 3), 'velocity': (2**-24, -2**-24, 0)},
                      {'position': (191.99998474121094, 447.9999694824219, .1), 'velocity': (.01, -.01, 0)})]
        ages = [native.age(age=value, time=value if abs(value) < 2**20 else 100) for value in
                (0, 9, 10, -1, 2**31 - 1, -2**31)]
        bounds = [native.bounds(corners, age=age) for corners, age in
                  ((((200, 200), (200, 220), (220, 200), (220, 220)), 10),
                   (((0, 0), (0, 0), (0, 0), (0, 0)), 9),
                   (((128, 16), (128, 464), (512, 16), (512, 464)), 10),
                   (((0, 0), (0, 0), (200, 200), (0, 0)), 10),
                   (((0, 0), (0, 0), (0, 0), (200, 200)), 10),
                   (((0, 0), (0, 0), (0, 0), (0, 0)), 10))]
        axis = [native.axis_bounds(anchors=(horizontal, vertical)) for horizontal in range(3) for vertical in range(3)]
        axis += [native.axis_bounds(position=(-.125, -.5, .1), scale=(-.1, .8)),
                 native.axis_bounds(position=(384, 448, 0), sprite=(18, 48), scale=(1, 1))]
        sync = [native.sync(position=position) for position in
                ((-128.25, 401.5, .1), (-320, -16, -0.0), (191.99998474121094, 447.9999694824219, 2**-149))]
        rotated = [native.rotated_bounds(anchors=(horizontal, vertical), rotation=.5)
                   for horizontal in range(3) for vertical in range(3)]
        rotated += [native.rotated_bounds(rotation=rotation) for rotation in
                    (0, -1.5707963705062866, 1.5707963705062866, -2.1, 2.1)]
        rotated += [native.rotated_bounds(position=(-.125, -.5, .1), scale=(-.1, .8), rotation=-.7),
                    native.rotated_bounds(position=(384, 448, 0), sprite=(18, 48), scale=(1, 1)),
                    native.rotated_bounds(position=(0, 0, 0), sprite=(0, 0), scale=(1, 1), rotation=0)]
        for start, end in ((0x43bff9, 0x43c005), (0x43c1a5, 0x43c1ab), (0x43fbdc, 0x43fbec),
                           (0x43fbf0, 0x43fd90), (0x45fad0, 0x45fd3f), (0x45f610, 0x45fa0c)):
            native.chunk(start, end)
        count = sum(map(len, (movement, returned, prefixes, integrate, ages, bounds, axis, sync, rotated)))
        if count > MAX_CASES:
            raise ValueError('tick fixture case budget exceeded')
        digest = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema': 'th128-original-byte-player-shot-tick-fixtures/1', 'nativeCpuTrace': False,
                'windowsGameProcessLaunched': False, 'executable': {'sha256': EXE_SHA256, 'sha256After': digest,
                'unchanged': digest == EXE_SHA256}, 'resource': actual_record(),
                'layout': {'poolOffset': POOL_OFFSET, 'slots': SLOT_COUNT, 'stride': SLOT_BYTES},
                'movementCases': movement, 'returnedPasses': returned, 'requiredPrefixes': prefixes,
                'integratorCases': integrate, 'ageCases': ages, 'boundsCases': bounds, 'axisBoundsCases': axis,
                'syncCases': sync, 'rotatedBoundsCases': rotated,
                'caseCount': count, 'code': native.code, 'constants': native.constants,
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                'engineSha256': {name: hashlib.sha256((Path(__file__).parent / name).read_bytes()).hexdigest()
                                 for name in ('freeze_probe.py', 'bullet_probe.py')},
                'limits': {'instructions': MAX_INSTRUCTIONS, 'cases': MAX_CASES, 'codeBytes': 8192,
                           'dataBytes': 131072, 'codeSpans': 16, 'dataSpans': 32},
                'limitations': ['Unit clock and ordinary kind0/non-angular position integrator only.',
                                'Callback zero is explicitly supplied synthetic runtime binding, not inferred from file IDs.',
                                'Complete no-match passes execute original lookup and security-cookie return.',
                                'Bounds and age tails are independent slices, not fabricated effect or hit returns.',
                                'Native floor runs original SSE bytes with explicitly initialized MXCSR, not a return stub.',
                                'Damage, hit, graze, animation execution and full shot-engine completion are not implemented.']}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded original-byte ordinary player-shot tick')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/player-shot-tick-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    if not report['executable']['unchanged']:
        raise ValueError('original executable hash changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(f'{report["caseCount"]} bounded player-shot tick original-byte cases; no game process or return stubs')


if __name__ == '__main__':
    main()
