import argparse
import hashlib
import importlib.metadata
import json
import math
import struct
from pathlib import Path

from bullet_probe import EXE_SHA256, ROOT, STACK_POINTER, NativeBullet, address, datum, emulate


PLAYER, PROFILE, CONTROL, BULLET = 0x300000, 0x380000, 0x390000, 0x3a0000
POSITION, SIZE = 0x3b0000, 0x3c0000
RECTANGLE, CIRCLE = 0x43c410, 0x43c570
ENTRIES = {'half-step': 0x40878d, 'full-step': 0x408a2a}


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('bullet-collision-')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be deliverable local/research/bullet-collision-*.json')
    return destination


def vector(values, length):
    if (type(length) is not int or not 1 <= length <= 4
            or not isinstance(values, (tuple, list)) or len(values) != length):
        raise ValueError('bounded float32 vector required')
    for value in values:
        if (type(value) not in (int, float) or abs(value) > 65536 or not math.isfinite(value)
                or 0 < abs(value) < 2**-16):
            raise ValueError('finite zero or magnitude [2^-16, 65536] required')
    return struct.pack('<' + 'f' * length, *values)


def integer(value, signed=True):
    if type(value) is not int or not (-(2**31) if signed else 0) <= value < (2**31 if signed else 2**32):
        raise ValueError('bounded 32-bit integer required')
    return value


def float_bits(payload):
    return list(struct.unpack('<' + 'I' * (len(payload) // 4), payload))


def snapshot(result):
    return {key: result[key] for key in ('stop', 'instructionBudget', 'instructionsExecuted',
                                        'instructionTrace', 'stackHead')}


class NativeCollision(NativeBullet):
    def state(self, center, size, *, player_position=(0, 0), bounds=(-2, -2, 2, 2),
              player_radius=2, state=0, flags=0, counter=0,
              control_present=False, control_counter=0):
        center_bytes, size_bytes = vector(center, 2), vector(size, 2)
        player_bytes, bounds_bytes = vector(player_position, 2), vector(bounds, 4)
        radius_bytes = vector((player_radius,), 1)
        if any(value < 0 for value in size) or player_radius < 0 or bounds[0] > bounds[2] or bounds[1] > bounds[3]:
            raise ValueError('nonnegative dimensions/radius and ordered bounds required')
        if type(control_present) is not bool:
            raise ValueError('explicit control presence required')
        geometry = bytearray(0xb4)
        geometry[:8] = player_bytes
        geometry[0x50:0x58], geometry[0x5c:0x64] = bounds_bytes[:8], bounds_bytes[8:]
        struct.pack_into('<iI', geometry, 0xac, integer(state), PROFILE)
        tail = bytearray(0x14)
        struct.pack_into('<i', tail, 0, integer(counter))
        struct.pack_into('<I', tail, 0x10, integer(flags, False))
        data = [datum('playerGeometry', PLAYER + 0xecc, bytes(geometry), writable=False),
                datum('playerTail', PLAYER + 0x438dc, bytes(tail), writable=False),
                datum('playerProfileRadius', PROFILE + 4, radius_bytes, writable=False),
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                datum('controlPointer', 0x4b8950, CONTROL if control_present else 0, kind='I', writable=False)]
        integer(control_counter)
        if control_present:
            data.append(datum('controlCounter', CONTROL + 0x5a1c, control_counter, writable=False))
        inputs = {'centerBits': float_bits(center_bytes), 'sizeBits': float_bits(size_bytes),
                  'playerPositionBits': float_bits(player_bytes), 'boundsBits': float_bits(bounds_bytes),
                  'playerRadiusBits': float_bits(radius_bytes)[0], 'stateF78': state,
                  'flags438ec': flags, 'counter438dc': counter, 'controlPresent': control_present,
                  'controlCounter5a1c': control_counter}
        return data, inputs, center_bytes, size_bytes

    def chunks(self, update=False):
        spans = [(RECTANGLE, 0x43c56e), (CIRCLE, 0x43c658)]
        if update:
            spans += [(0x40878d, 0x40888b), (0x408a2a, 0x408b74)]
        return [self.chunk(start, end) for start, end in spans]

    def constants_data(self):
        return [self.constant(start, size) for start, size in
                ((0x4a65a8, 8), (0x4a6ac0, 8), (0x4a6910, 8), (0x4a6a00, 4))]

    def circle_geometry(self, center, hitbox_size, player_position, profile_radius):
        center_bytes, player_bytes = vector(center, 2), vector(player_position, 2)
        size_bytes, radius_bytes = vector((hitbox_size,), 1), vector((profile_radius,), 1)
        if hitbox_size < 0 or profile_radius < 0:
            raise ValueError('nonnegative circle sizes required')
        data = [datum('center', POSITION, center_bytes, writable=False),
                datum('playerXY', PLAYER + 0xecc, player_bytes, writable=False),
                datum('profilePointer', PLAYER + 0xf7c, PROFILE, kind='I', writable=False),
                datum('profileRadius', PROFILE + 4, radius_bytes, writable=False),
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                self.constant(0x4a6910, 8), self.constant(0x4a6a00, 4)]
        result = emulate([self.chunk(CIRCLE, 0x43c658)], CIRCLE, data,
                         registers={'eax': POSITION}, arguments=size_bytes, stops=[0x43c608], budget=256)
        returned = result['stop']['reason'] == 'returned'
        helper_return = result['registers']['eax'] if returned else None
        if result['stop'] == {'reason': 'boundary', 'address': '0x0043c608'}:
            geometry = 'potential-hit'
        elif returned and helper_return in (0, 2):
            geometry = 'potential-graze' if helper_return == 2 else 'miss'
        else:
            raise ValueError(f"unexpected geometry-only original stop: {result['stop']}")
        result.update(kind='circle-geometry', geometry=geometry, helperReturn=helper_return,
                      hitGatesRead=False, completedHelper=returned, completedTick=False,
                      input={'centerBits': float_bits(center_bytes), 'hitboxSizeBits': float_bits(size_bytes)[0],
                             'playerPositionBits': float_bits(player_bytes),
                             'playerRadiusBits': float_bits(radius_bytes)[0]})
        return result

    def helper(self, route, center, size, **player):
        data, inputs, center_bytes, size_bytes = self.state(center, size, **player)
        data += [datum('center', POSITION, center_bytes, writable=False),
                 datum('fullSize', SIZE, size_bytes, writable=False)] + self.constants_data()
        entry = RECTANGLE if route == 'rectangle' else CIRCLE
        registers = {'eax': SIZE, 'edx': POSITION} if route == 'rectangle' else {'eax': POSITION}
        arguments = b'' if route == 'rectangle' else size_bytes[:4]
        chunks = self.chunks()

        def run(stops=()):
            return emulate(chunks, entry, data, registers=registers, arguments=arguments,
                           stops=stops, budget=512)

        result = run()
        result.update(kind=route, input=inputs, completedTick=False, measurements={})
        returned = result['stop']['reason'] == 'returned'
        result['helperReturn'] = result['registers']['eax'] if returned else None
        hit_gate = '0x0043c497' if route == 'rectangle' else '0x0043c608'
        result['decision'] = ({0: 'suppressed' if hit_gate in result['instructionTrace'] else 'miss',
                               1: 'hit-return', 2: 'graze-return'}.get(result['helperReturn']) if returned
                              else 'damage-call-required' if result['stop'].get('target') == '0x0043d0c0'
                              else 'unsupported')
        if route == 'rectangle':
            measured = run([0x43c447])
            result['measurements']['hitBounds'] = snapshot(measured)
            result['hitBoundsBits'] = float_bits(bytes.fromhex(measured['stackHead'])[:16])
            if '0x0043c4f3' in result['instructionTrace']:
                measured = run([0x43c517])
                result['measurements']['grazeBounds'] = snapshot(measured)
                result['grazeBoundsBits'] = float_bits(bytes.fromhex(measured['stackHead'])[:16])
        else:
            measured = run([0x43c59c])
            result['measurements']['distanceSquared'] = snapshot(measured)
            result['distanceSquaredBits'] = float_bits(bytes.fromhex(measured['stackHead'])[:4])[0]
            if '0x0043c5ba' in result['instructionTrace']:
                for name, location in (('dividedGrazeSize', 0x43c5c5), ('grazeSize', 0x43c5dc)):
                    measured = run([location])
                    result['measurements'][name] = snapshot(measured)
                    result[name + 'Bits'] = float_bits(bytes.fromhex(measured['stackHead'])[12:16])[0]
        return result



    def rectangle(self, center, full_size, **player):
        return self.helper('rectangle', center, full_size, **player)

    def circle(self, center, hitbox_size, **player):
        return self.helper('circle', center, (hitbox_size, hitbox_size), **player)

    def update_prefix(self, site, center, full_size, *, bullet_flags=0x13, age=8, scale=1, **player):
        if site not in ENTRIES:
            raise ValueError('explicit half-step/full-step collision entry required')
        data, inputs, center_bytes, size_bytes = self.state(center, full_size, **player)
        scale_bytes = vector((scale,), 1)
        if scale < 0:
            raise ValueError('nonnegative scale required')
        block = bytearray(0x118c)
        struct.pack_into('<I', block, 0, integer(bullet_flags, False))
        block[0x9d8:0x9e0], block[0x9f8:0xa00] = center_bytes, size_bytes
        block[0x1180:0x1184] = scale_bytes
        struct.pack_into('<i', block, 0x1188, integer(age))
        data += [datum('bullet', BULLET, bytes(block))] + self.constants_data()
        inputs.update(site=site, bulletFlags=bullet_flags, age1188=age, scale1180Bits=float_bits(scale_bytes)[0])
        chunks = self.chunks(update=True)

        def run(extra_stops=()):
            return emulate(chunks, ENTRIES[site], data,
                           registers={'ebx': BULLET, 'edi': BULLET + 0x9d8, 'ebp': STACK_POINTER + 32},
                           stops=(0x40888b, 0x408c93, *extra_stops), budget=512)

        result = run()
        trace, stop = result['instructionTrace'], result['stop']
        route = 'rectangle' if address(RECTANGLE) in trace else 'circle' if address(CIRCLE) in trace else 'none'
        target = stop.get('target')
        intent = ('no-helper' if route == 'none' else 'damage-call-required' if target == '0x0043d0c0'
                  else 'hit-effects-required' if target == '0x0040d2a0'
                  else 'graze-call-required' if target == '0x0043e090'
                  else 'helper-returned' if stop['reason'] == 'boundary' else 'unsupported')
        helper_return = (None if intent in ('no-helper', 'damage-call-required', 'unsupported')
                         else 1 if intent == 'hit-effects-required' else result['registers']['eax'])
        result.update(kind='update-prefix', input=inputs, route=route, intent=intent,
                      helperReturn=helper_return, completedTick=False)
        gate = '0x0043c497' if route == 'rectangle' else '0x0043c608'
        result['helperDecision'] = (None if route == 'none' else 'damage-call-required'
                                    if intent == 'damage-call-required' else
                                    {0: 'suppressed' if gate in trace else 'miss', 1: 'hit-return',
                                     2: 'graze-return'}.get(helper_return))
        after = bytes.fromhex(result['memory']['bullet']['after'])
        result.update(flagsAfter=struct.unpack_from('<I', after)[0],
                      motionStateAfter=struct.unpack_from('<H', after, 0xa2a)[0])
        if route != 'none':
            measured = run([RECTANGLE if route == 'rectangle' else CIRCLE])
            result['helperEntryMeasurement'] = snapshot(measured)
            head = bytes.fromhex(measured['stackHead'])
            if route == 'circle':
                result['effectiveSizeBits'] = float_bits(head[4:8]) * 2
            elif bullet_flags & 0x40:
                offset = measured['registers']['eax'] - measured['registers']['esp']
                result['effectiveSizeBits'] = float_bits(head[offset:offset + 8])
            else:
                result['effectiveSizeBits'] = float_bits(size_bytes)
        result['memory']['bullet'] = {key: value for key, value in result['memory']['bullet'].items()
                                      if key not in ('before', 'after')}
        return result


def build_report(native):
    import random

    cases = []

    def add(identifier, result):
        if result['stop']['reason'] not in ('returned', 'boundary', 'unsupported_call'):
            raise ValueError(f"unexpected original-byte stop in {identifier}: {result['stop']}")
        if result.get('decision') == 'unsupported' or result.get('intent') == 'unsupported':
            raise ValueError(f'unported non-intent path in {identifier}')
        if result['instructionsExecuted'] >= result['instructionBudget']:
            raise ValueError('instruction budget exhausted')
        result['id'] = identifier
        cases.append(result)

    def adjacent(value, delta):
        return struct.unpack('<f', struct.pack('<I', float_bits(struct.pack('<f', value))[0] + delta))[0]

    for index, (center, size) in enumerate((
            ((0, 0), (2.4, 4)), ((-0.0, -0.0), (0, 0)), ((-0.0, -0.0), (-0.0, -0.0)),
            ((3, 0), (2, 2)), ((adjacent(3, 1), 0), (2, 2)), ((-3, 0), (2, 2)),
            ((-adjacent(3, 1), 0), (2, 2)), ((0, 3), (2, 2)), ((0, -3), (2, 2)),
            ((26, 0), (2, 2)), ((adjacent(26, 1), 0), (2, 2)), ((0, -26), (2, 2)),
            ((-128.125, 32.0625), (2.4, 2.4)), ((65536, -65536), (2**-16, 2**-16)),
            ((0, 0), (65536, 65536)), ((2**-16, -2**-16), (2**-16, 2**-16)))):
        add(f'rect-bounds-{index}', native.rectangle(center, size, counter=1))
    for index, (center, hitbox_size, player_radius) in enumerate((
            ((0, 0), 0, 0), ((0, 0), -0.0, -0.0), ((4, 0), 4, 3), ((5, 0), 4, 3),
            ((adjacent(5, -1), 0), 4, 3), ((adjacent(5, 1), 0), 4, 3), ((6, 0), 4, 3),
            ((43, 3), 4, 3), ((43, 4), 4, 3), ((-43, -4), 4, 3),
            ((43, adjacent(4, -1)), 4, 3), ((43, adjacent(4, 1)), 4, 3),
            ((100, 40), 100, 0), ((100, 40), adjacent(100, -1), 0),
            ((100, 40), adjacent(100, 1), 0), ((100, 100), 101, 3), ((200, 100), 500, 4),
            ((2**-16, 0), 2**-16, 0), ((0, 0), 2**-16, 2**-16),
            ((65536, -65536), 65536, 65536), ((-128.125, 32.0625), 2.4, 2),
            ((math.sqrt(2.4**2 + 2**2), 0), 2.4, 2))):
        add(f'circle-threshold-{index}', native.circle(center, hitbox_size, player_radius=player_radius, counter=1))
    for raw in (0x37800000, 0x37800001):
        profile_radius = struct.unpack('<f', struct.pack('<I', raw))[0]
        add(f'circle-retained-x87-{raw:08x}', native.circle(
            (65536, 0), 65536, player_radius=profile_radius, counter=1))
    for index, (center, player_position) in enumerate((
            ((65536, 2**-16), (-2**-16, -65536)), ((-65536, 65536), (65536, -65536)))):
        add(f'circle-domain-extreme-{index}', native.circle(
            center, 2.4, player_position=player_position, counter=1))
    for index, (player_position, bounds) in enumerate((
            ((100, 100), (-2, -2, 2, 2)), ((0, 0), (100, 100, 104, 104)))):
        add(f'rect-precomputed-bounds-{index}', native.rectangle(
            (0, 0), (2.4, 4), player_position=player_position, bounds=bounds, counter=1))
        add(f'circle-actual-position-{index}', native.circle(
            (0, 0), 2.4, player_position=player_position, bounds=bounds, counter=1))
    gates = [dict(state=value) for value in (-2147483648, -1, 0, 1, 2, 3, 4, 5, 2147483647)]
    gates += [dict(flags=value) for value in (1, 2, 3, 0x200, 0xffffffff)]
    gates += [dict(control_present=present, control_counter=value)
              for present, value in ((False, 1), (True, 0), (True, 1), (True, -1))]
    for route, method, size in (('rect', native.rectangle, (2, 2)), ('circle', native.circle, 2.4)):
        for index, gate in enumerate(gates):
            for center in ((0, 0), (10, 0)):
                add(f'{route}-gate-{index}-{center[0]}', method(center, size, **gate))
        for counter in (-2147483648, -1, 0, 1, 2147483647):
            add(f'{route}-counter-{counter}', method((0, 0), size, counter=counter))
    for site in ENTRIES:
        for flags in (0, 1, 2, 6, 0x13, 0x17, 0x10000013):
            for center in ((0, 0), (10, 0), (100, 100)):
                add(f'prefix-{site}-{flags}-{center[0]}', native.update_prefix(
                    site, center, (2.4, 4), bullet_flags=flags, counter=1))
        for age in (-2147483648, 7, 8, 2147483647):
            add(f'prefix-{site}-age-{age}', native.update_prefix(site, (10, 0), (2.4, 4), age=age))
        for flags in (0x42, 0x52):
            for scale in (0, .5, 1, 2, 100, 2**-16):
                add(f'prefix-{site}-scale-{flags}-{scale}', native.update_prefix(
                    site, (10, 0), (2.4, 4), bullet_flags=flags, scale=scale, counter=1))
        for counter in (-1, 0):
            add(f'prefix-{site}-damage-{counter}', native.update_prefix(
                site, (0, 0), (2.4, 4), counter=counter))
    random_source = random.Random(0x43c570)
    for index in range(32):
        center = tuple(struct.unpack('<2f', struct.pack('<2f', *(random_source.uniform(-256, 512) for _ in range(2)))))
        player = tuple(random_source.randrange(-25600, 51200) / 128 for _ in range(2))
        bounds = (player[0] - 2, player[1] - 2, player[0] + 2, player[1] + 2)
        add(f'rect-finite-{index}', native.rectangle(center, (2.4, 4), player_position=player,
                                                    bounds=bounds, counter=1))
        add(f'circle-finite-{index}', native.circle(center, 2.4, player_position=player,
                                                   bounds=bounds, player_radius=2, counter=1))
    geometry_cases = []
    for source in cases:
        if source['kind'] != 'circle' and source.get('route') != 'circle':
            continue
        inputs = source['input']
        center = struct.unpack('<2f', struct.pack('<2I', *inputs['centerBits']))
        player_position = struct.unpack('<2f', struct.pack('<2I', *inputs['playerPositionBits']))
        hitbox_size = struct.unpack('<f', struct.pack('<I',
            source.get('effectiveSizeBits', inputs['sizeBits'])[0]))[0]
        profile_radius = struct.unpack('<f', struct.pack('<I', inputs['playerRadiusBits']))[0]
        observed = native.circle_geometry(center, hitbox_size, player_position, profile_radius)
        expected = ('potential-hit' if '0x0043c608' in source['instructionTrace'] else
                    'potential-graze' if '0x0043c5fd' in source['instructionTrace'] else 'miss')
        if observed['geometry'] != expected:
            raise ValueError(f"geometry-only classification differs from original case {source['id']}")
        observed.update(id='geometry-' + source['id'], sourceCaseId=source['id'])
        geometry_cases.append(observed)
    profile_block, emitter = bytearray(0x11b4), struct.pack('<2i', 8, 10)
    struct.pack_into('<H', profile_block, 0x11b0, 8)
    profile = emulate([native.chunk(0x4097e3, 0x40988c)], 0x4097e3,
                      [datum('bullet', BULLET, bytes(profile_block)),
                       datum('emitter', POSITION, emitter, writable=False),
                       native.constant(0x409a5c, 40), native.constant(0x4b2ee0, 4),
                       native.constant(0x4b2e80, 4)], registers={'ebx': BULLET, 'edi': POSITION},
                      stops=[0x40988c], budget=128)
    profile['flagsAfter'] = struct.unpack_from('<I', bytes.fromhex(profile['memory']['bullet']['after']))[0]
    profile['input'] = {'type': 8, 'color': 10, 'initialFlags': 0, 'bulletType11b0': 8}
    profile['completedEmission'] = False
    profile['hitboxSizeBytes'] = native.read(0x4b2ed4, 4).hex()
    if profile['stop'] != {'reason': 'boundary', 'address': '0x0040988c'}:
        raise ValueError('type8 flag proof did not reach the bounded original branch end')
    native.constant(0x4b2ed4, 8)
    profile['memory'].pop('bullet')
    after_hash = hashlib.sha256(native.executable.read_bytes()).hexdigest()
    if after_hash != EXE_SHA256:
        raise ValueError('original executable changed during read-only probing')
    return {'schema': 'th128-original-byte-bullet-collision-fixtures/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': after_hash, 'unchanged': True},
            'nativeCpuTrace': False, 'windowsGameProcessLaunched': False, 'fakeCallReturns': False,
            'completedNativeFrame': False, 'emulator': {'unicorn': importlib.metadata.version('unicorn'),
                                                       'capstone': importlib.metadata.version('capstone')},
            'floatDomain': {'minimumNonzeroMagnitude': 2**-16, 'maximumMagnitude': 65536,
                            'x87ControlWord': '0x037f'},
            'code': native.code, 'constants': native.constants, 'type8Evidence': profile,
            'caseCount': len(cases), 'cases': cases,
            'geometryCaseCount': len(geometry_cases), 'geometryCases': geometry_cases}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Read-only bounded TH128 collision decisions, not a game process')
    parser.add_argument('executable', type=Path)
    parser.add_argument('--report', type=Path, required=True)
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.report)
    report = build_report(NativeCollision(arguments.executable))
    destination.write_text(json.dumps(report, indent=2, ensure_ascii=True) + '\n', encoding='utf-8')
    print(json.dumps({'report': str(destination), 'caseCount': report['caseCount'],
                      'sha256': hashlib.sha256(destination.read_bytes()).hexdigest(),
                      'originalUnchanged': report['executable']['unchanged']}))


if __name__ == '__main__':
    main()
