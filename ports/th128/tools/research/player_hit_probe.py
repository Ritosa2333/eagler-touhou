import argparse
import hashlib
import importlib.metadata
import json
import math
import struct
from pathlib import Path

from bullet_probe import EXE_SHA256, ROOT, RETURN_TRAP, STACK_POINTER, NativeBullet, address, datum, emulate


PLAYER, POSITION, PROFILE, MANAGER, BANK, CONTROL, TAIL = (
    0x300000, 0x310000, 0x320000, 0x330000, 0x340000, 0x350000, 0x360000)


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('player-hit-') or destination.drive.lower() == 'e:'):
        raise ValueError('report must be private local/research/player-hit-*.json')
    return destination


def signed(value):
    if type(value) is not int or not -2147483648 <= value <= 2147483647:
        raise ValueError('explicit signed int32 required')
    return value


def unsigned(value):
    if type(value) is not int or not 0 <= value < 2**32:
        raise ValueError('explicit unsigned int32 required')
    return value


def present(value):
    if type(value) is not bool:
        raise ValueError('explicit pointer-presence boolean required')
    return value


def position(values):
    if not isinstance(values, (tuple, list)) or len(values) != 3:
        raise ValueError('explicit XYZ required')
    for value in values:
        if type(value) not in (int, float) or abs(value) > 65536 or not math.isfinite(value):
            raise ValueError('finite position magnitude <= 65536 required')
    return struct.pack('<3f', *values)


def timer_bytes(values):
    if not isinstance(values, (tuple, list)) or len(values) != 5:
        raise ValueError('five explicit timer fields required')
    payload = struct.pack('<iiIII', signed(values[0]), signed(values[1]),
                          unsigned(values[2]), unsigned(values[3]), unsigned(values[4]))
    fractional = struct.unpack_from('<f', payload, 8)[0]
    if not math.isfinite(fractional) or abs(fractional) > 65536:
        raise ValueError('bounded finite timer float required')
    return payload


def timer_state(payload):
    return list(struct.unpack('<iiIII', payload))


def request_words(kind, result, argument, player_offset=0, bank_source=0):
    return [kind, int(result['stop']['address'], 16), int(result['stop']['target'], 16),
            argument & 0xffffffff, player_offset, bank_source]


class NativePlayerHit(NativeBullet):
    def entry(self, state, coordinates):
        coordinates = position(coordinates)
        result = emulate([self.chunk(0x43d0c0, 0x43d120)], 0x43d0c0,
                         [datum('playerState', PLAYER + 0xf78, signed(state)),
                          datum('playerPosition', PLAYER + 0xecc, coordinates),
                          datum('managerPointer', 0x4b8930, MANAGER, kind='I', writable=False),
                          datum('managerBank', MANAGER + 0x8a7f9c, BANK, kind='I', writable=False),
                          *[self.constant(location, 8) for location in (0x4a65f8, 0x4a65d8, 0x4a6610)]],
                         arguments=struct.pack('<I', PLAYER), budget=96)
        head = bytes.fromhex(result['stackHead'])
        result.update(kind='entry', input={'stateF78': state, 'positionBits': list(struct.unpack('<3I', coordinates))},
                      stateAfter=struct.unpack('<i', bytes.fromhex(result['memory']['playerState']['after']))[0],
                      deferredPositionBits=list(struct.unpack_from('<3I', head, 40)),
                      requestWords=request_words(0, result, struct.unpack_from('<i', head, 4)[0], bank_source=1),
                      completedNativeFunction=False)
        return result



    def normal_init(self, state_timer, focus_timer, recharge_timer, flags):
        data = [datum('stateTimer', PLAYER + 0xf80, timer_bytes(state_timer)),
                datum('focusTimer', PLAYER + 0xf94, timer_bytes(focus_timer)),
                datum('rechargeTimer', PLAYER + 0x438fc, timer_bytes(recharge_timer)),
                datum('playerFlags', PLAYER + 0x438ec, unsigned(flags), kind='I'),
                datum('playerState', PLAYER + 0xf78, 0),
                datum('movementFields', PLAYER + 0xf70, struct.pack('<2i', 123, -45)),
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                self.constant(0x4a6688, 4)]
        result = emulate([self.chunk(0x43ab20, 0x43ac23)], 0x43ab20, data, budget=128)
        for name in ('stateTimer', 'focusTimer', 'rechargeTimer'):
            result[name + 'After'] = timer_state(bytes.fromhex(result['memory'][name]['after']))
        result.update(kind='normal-init', input={'stateTimer': list(state_timer), 'focusTimer': list(focus_timer),
                                                 'rechargeTimer': list(recharge_timer), 'flags438ec': flags},
                      stateAfter=struct.unpack('<i', bytes.fromhex(result['memory']['playerState']['after']))[0],
                      flagsAfter=struct.unpack('<I', bytes.fromhex(result['memory']['playerFlags']['after']))[0],
                      requestWords=request_words(6, result, 0, result['registers']['esi'] - PLAYER),
                      completedNativeFunction=False)
        return result

    def constructor_counter(self, timer):
        payload = timer_bytes(timer)
        if not timer[4] & 1:
            raise ValueError('explicit initialized timer snapshot required at constructor suffix')
        result = emulate([self.chunk(0x43aadb, 0x43aafb)], 0x43aadb,
                         [datum('counter', PLAYER + 0x438d8, payload), self.constant(0x4a6774, 4)],
                         registers={'esi': PLAYER}, stops=[0x43aafb], budget=32)
        result.update(kind='constructor-counter', input={'timer': list(timer)},
                      timerAfter=timer_state(bytes.fromhex(result['memory']['counter']['after'])),
                      requiresInitializedTimer=True, completedNativeFunction=False)
        return result

    def counter_step(self, timer, scale):
        from enemy_probe import emulate as enemy_emulate, STACK_POINTER as enemy_stack_pointer

        if type(scale) not in (float, int) or not math.isfinite(scale) or not 0 <= scale <= 4:
            raise ValueError('explicit clock magnitude 0..4 required')
        if timer[1] > 0 and timer[3] != 0x4b6f58:
            raise ValueError('only resolved native global-clock binding is supported')
        block = bytearray(0x43a94)
        block[0x438d8:0x438ec] = timer_bytes(timer)
        data = [datum(f'playerPart{index}', PLAYER + start, bytes(block[start:start + 65536]))
                for index, start in enumerate(range(0, len(block), 65536))]
        scale_bytes = struct.pack('<f', scale)
        data += [datum('clock', 0x4b6f58, scale_bytes, writable=False),
                 datum('sse2Selector', 0x4dcd68, 1, writable=False),
                 self.constant(0x4a6698, 8), self.constant(0x4a6694, 4)]
        result = enemy_emulate([self.chunk(0x43b9ee, 0x43bb6f), self.chunk(0x48fac0, 0x48fadc)],
                               0x43b9ee, data, registers={'esi': PLAYER, 'ebp': enemy_stack_pointer + 256},
                               stops=[0x43bb6f, 0x43bbad], budget=15000)
        final = b''.join(bytes.fromhex(result['memory'][f'playerPart{index}']['after'])
                         for index in range((len(block) + 65535) // 65536))
        result.update(kind='counter-step', input={'timer': list(timer),
                      'clockBits': struct.unpack('<I', scale_bytes)[0]},
                      timerAfter=timer_state(final[0x438d8:0x438ec]),
                      syntheticInactiveShotRecords=2000, completedNativeFunction=False)
        for name, region in result['memory'].items():
            if name.startswith('playerPart'):
                for field in ('before', 'after'):
                    region[field + 'Sha256'] = hashlib.sha256(bytes.fromhex(region.pop(field))).hexdigest()
        return result

    def gate(self, state, flags, counter, control_present, control_counter):
        geometry = bytearray(0xb4)
        struct.pack_into('<iI', geometry, 0xac, signed(state), PROFILE)
        data = [datum('playerGeometry', PLAYER + 0xecc, bytes(geometry), writable=False),
                datum('playerCounter', PLAYER + 0x438dc, signed(counter), writable=False),
                datum('playerFlags', PLAYER + 0x438ec, unsigned(flags), kind='I', writable=False),
                datum('profileRadius', PROFILE + 4, 2.0, kind='f', writable=False),
                datum('position', POSITION, struct.pack('<2f', 0, 0), writable=False),
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                datum('controlPointer', 0x4b8950, CONTROL if present(control_present) else 0,
                      kind='I', writable=False)]
        signed(control_counter)
        if control_present:
            data.append(datum('controlCounter', CONTROL + 0x5a1c, control_counter, writable=False))
        result = emulate([self.chunk(0x43c570, 0x43c658)], 0x43c570, data,
                         registers={'eax': POSITION}, arguments=struct.pack('<f', 2), budget=128)
        returned = result['stop']['reason'] == 'returned'
        result.update(kind='gate', input={'stateF78': state, 'flags438ec': flags, 'counter438dc': counter,
                                          'controlPresent': control_present, 'controlCounter5a1c': control_counter},
                      helperReturn=result['registers']['eax'] if returned else None,
                      outcome=0 if returned and result['registers']['eax'] == 0 else 1 if returned else 2,
                      geometryAssumption='explicit-original-circle-overlap-zero-XY-size2-profile2',
                      completedNativeFunction=False)
        return result

    def timer_suffix(self, state_timer, counter_timer, control_present, control_flags, *, counter_stage):
        data = [datum('stateTimer', PLAYER + 0xf80, timer_bytes(state_timer)),
                datum('counterTimer', PLAYER + 0x438d8, timer_bytes(counter_timer)),
                datum('animationBank', PLAYER + 0x10, BANK, kind='I'),
                datum('soundControlPointer', 0x4b8954, CONTROL if present(control_present) else 0,
                      kind='I', writable=False), self.constant(0x4a6ab8, 4)]
        unsigned(control_flags)
        if control_present:
            data.append(datum('soundControlFlags', CONTROL + 0x60, control_flags, kind='I', writable=False))
        result = emulate([self.chunk(0x43d264, 0x43d331)], 0x43d264, data,
                         registers={'ebp': STACK_POINTER - 4}, arguments=struct.pack('<I', PLAYER),
                         stops=() if counter_stage else (0x43d2d1,), budget=128)
        timer_name = 'counterTimer' if counter_stage else 'stateTimer'
        words = (request_words(2, result, result['registers']['ebx'], 0x14, 2) if counter_stage
                 else request_words(1, result, result['registers']['edx'])
                 if result['stop']['reason'] == 'unsupported_call' else None)
        result.update(kind='counter-reset' if counter_stage else 'timer-reset',
                      input={'timer': list(counter_timer if counter_stage else state_timer),
                             'soundControlPresent': control_present, 'soundControlFlags60': control_flags},
                      timerAfter=timer_state(bytes.fromhex(result['memory'][timer_name]['after'])),
                      requestWords=words, requiresPriorHitEffectsResolved=True, completedNativeFunction=False)
        return result

    def timer_reset(self, timer, control_present, control_flags):
        return self.timer_suffix(timer, [0, 0, 0, 0, 0], control_present, control_flags, counter_stage=False)

    def counter_reset(self, timer):
        return self.timer_suffix([0, 0, 0, 0, 0], timer, False, 0, counter_stage=True)

    def window(self, elapsed, held, control_present, mode, charge, keys, *,
               state_timer=None, held_timer=None):
        signed(elapsed)
        signed(held)
        state_timer = list(state_timer) if state_timer is not None else [17, elapsed, 0x41100000, 0x12345678, 0]
        held_timer = list(held_timer) if held_timer is not None else [11, held, 0x40a00000, 0x87654321, 0]
        if state_timer[1] != elapsed or held_timer[1] != held:
            raise ValueError('explicit timer current fields must match branch inputs')
        data = [datum('stateTimer', PLAYER + 0xf80, timer_bytes(state_timer)),
                datum('heldTimer', PLAYER + 0x43910, timer_bytes(held_timer)),
                datum('windowControlPointer', 0x4b892c, CONTROL if present(control_present) else 0,
                      kind='I', writable=False),
                datum('perfectCharge', 0x4b4d70, signed(charge), writable=False),
                datum('keys', 0x4db210, unsigned(keys), kind='I', writable=False)]
        signed(mode)
        if control_present:
            data.append(datum('windowMode', CONTROL + 0x3c, mode, writable=False))
        result = emulate([self.chunk(0x43b843, 0x43b9ee), self.chunk(0x403280, 0x4032bd)],
                         0x43b843, data, registers={'esi': PLAYER}, stops=[0x43b9ee], budget=128)
        target = result['stop'].get('target')
        kind = {'0x0043cdd0': 4, '0x00458590': 3, '0x00407810': 5}.get(target)
        argument = result['registers']['eax'] if kind == 3 else 0
        result.update(kind='window', input={'stateF78': 4, 'stateTimer': state_timer, 'heldTimer': held_timer,
                                            'controlPresent': control_present, 'controlMode3c': mode,
                                            'perfectCharge4b4d70': charge, 'keys4db210': keys},
                      outcome={None: 0, '0x0043cdd0': 1, '0x00458590': 2, '0x00407810': 3}.get(target),
                      stateTimerAfter=timer_state(bytes.fromhex(result['memory']['stateTimer']['after'])),
                      heldTimerAfter=timer_state(bytes.fromhex(result['memory']['heldTimer']['after'])),
                      requestWords=request_words(kind, result, argument) if kind is not None else None,
                      completedNativeFunction=False)
        return result

    def tail(self, flags, count, value80, control_present, mode):
        block = bytearray(0x84)
        struct.pack_into('<i', block, 0x28, signed(count))
        struct.pack_into('<Ii', block, 0x7c, unsigned(flags), signed(value80))
        data = [datum('tailManager', TAIL, bytes(block)),
                datum('tailManagerPointer', 0x4b8934, TAIL, kind='I', writable=False),
                datum('tailControlPointer', 0x4b892c, CONTROL if present(control_present) else 0,
                      kind='I', writable=False)]
        signed(mode)
        if control_present:
            data.append(datum('tailMode', CONTROL + 0x3c, mode, writable=False))
        result = emulate([self.chunk(0x43d331, 0x43d373)], 0x43d331, data,
                         registers={'esi': 0, 'ebp': STACK_POINTER + 12},
                         stack={0: struct.pack('<6I', 0, 0, 0, 0, RETURN_TRAP, 0)}, budget=64)
        after = bytes.fromhex(result['memory']['tailManager']['after'])
        result.update(kind='tail', input={'flags7c': flags, 'counter28': count, 'value80': value80,
                                          'controlPresent': control_present, 'controlMode3c': mode},
                      flagsAfter=struct.unpack_from('<I', after, 0x7c)[0],
                      value80After=struct.unpack_from('<i', after, 0x80)[0],
                      requiresPriorPlayerAnimationResolved=True, completedNativeFunction=False,
                      completedIndependentTail=result['stop']['reason'] == 'returned')
        result['memory']['tailManager'].pop('before')
        result['memory']['tailManager'].pop('after')
        return result


def build_report(native):
    cases = []

    def record(identifier, result):
        if result['stop']['reason'] not in ('returned', 'boundary', 'unsupported_call'):
            raise ValueError(f"unexpected original stop in {identifier}: {result['stop']}")
        if result['instructionsExecuted'] >= result['instructionBudget']:
            raise ValueError('instruction budget exhausted')
        result['id'] = identifier
        cases.append(result)

    positions = ((0, 400, 0), (-133.25, 7.5, .1), (-320, -16, -0.0),
                 (65536, -65536, 65536), (-65536, 65536, -65536), (2**-149, -2**-149, 0),
                 (2**-16, 2**-20, .125), (-2**-16, -2**-20, -.125))
    for index, coordinates in enumerate(positions):
        for state in (-2147483648, 1, 4):
            record(f'entry-{index}-{state}', native.entry(state, coordinates))
    for state in (-2147483648, -1, 0, 1, 2, 3, 4, 2147483647):
        for counter in (-1, 0, 1):
            record(f'gate-state-{state}-counter-{counter}', native.gate(state, 0, counter, False, 0))
    for flags in (1, 2, 3, 0x200, 0xffffffff):
        for counter in (-2147483648, 0, 2147483647):
            record(f'gate-flags-{flags}-counter-{counter}', native.gate(1, flags, counter, False, 0))
    for control_present in (False, True):
        for control_counter in (-2147483648, 0, 2147483647):
            for counter in (-1, 0, 1):
                record(f'gate-control-{control_present}-{control_counter}-{counter}',
                       native.gate(1, 0, counter, control_present, control_counter))
    for flags in (0, 1, 2, 3, 0xfffffffe, 0xffffffff):
        for previous, current, fractional, clock in ((17, 9, 0x41100000, 0x12345678),
                                                    (-2147483648, 2147483647, 0x80000000, 0)):
            timer = [previous, current, fractional, clock, flags]
            for control_present, control_flags in ((False, 0), (True, 0), (True, 0x200), (True, 0xffffffff)):
                record(f'timer-{flags}-{current}-{control_present}-{control_flags}',
                       native.timer_reset(timer, control_present, control_flags))
            record(f'counter-{flags}-{current}', native.counter_reset(timer))
    controls = ((False, 0, 10000, 2), (True, 0, 10000, 2), (True, 1, 10000, 2),
                (True, 0, 9999, 2), (True, 0, 10000, 0), (True, -1, 2147483647, 0xffffffff))
    for elapsed in (-2147483648, -1, 0, 7, 8, 2147483647):
        for held in (-2147483648, -1, 0, 1, 2147483647):
            for index, (control_present, mode, charge, keys) in enumerate(controls):
                record(f'window-{elapsed}-{held}-{index}', native.window(
                    elapsed, held, control_present, mode, charge, keys,
                    state_timer=[17, elapsed, 0x41100000, 0x12345678, index & 1],
                    held_timer=[11, held, 0x40a00000, 0x87654321, (index >> 1) & 1]))
    for flags in (0, 1, 0x47, 0xff, 0xffffffff):
        for count in (-2147483648, -1, 0, 59, 60, 2147483647):
            for control_present, mode in ((True, 0), (True, -1), (False, 0)):
                if not control_present and flags & 1 and count < 60:
                    continue
                record(f'tail-{flags}-{count}-{control_present}-{mode}',
                       native.tail(flags, count, -17, control_present, mode))
    for flags in (0, 1, 2, 3, 0xfffffffe, 0xffffffff):
        record(f'normal-init-{flags}', native.normal_init(
            [17, 9, 0x41100000, 0x12345678, flags],
            [11, 7, 0x40e00000, 0x87654321, flags ^ 1],
            [-17, -9, 0xc1100000, 0x4b6f58, flags], flags))
    for flags in (1, 3, 0xffffffff):
        for clock in (0x4b6f58, 0x12345678):
            record(f'constructor-counter-{flags}-{clock}', native.constructor_counter(
                [17, 9, 0x41100000, clock, flags]))
    for index, (current, fractional, scale) in enumerate(((120, 120, 1), (6, 6, 1), (1, 1, 1),
            (0, 0, 1), (-1, -1, 1), (6, 6, .5), (6, 6, 0), (6, 6, .99), (6, 6, 1.01),
            (6, 6, 1.005), (6, 6, 2), (6, .25, .5), (6, -.25, .5), (1, 1, 4))):
        record(f'counter-step-{index}', native.counter_step(
            [current - 1, current, struct.unpack('<I', struct.pack('<f', fractional))[0], 0x4b6f58, 1], scale))
    native.constant(0x43c1bc, 20)
    after_hash = hashlib.sha256(native.executable.read_bytes()).hexdigest()
    if after_hash != EXE_SHA256:
        raise ValueError('original changed during read-only probing')
    return {'schema': 'th128-original-byte-player-hit-fixtures/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': after_hash, 'unchanged': True},
            'nativeCpuTrace': False, 'windowsGameProcessLaunched': False, 'fakeCallReturns': False,
            'completedNativeFunction': False, 'emulator': {'unicorn': importlib.metadata.version('unicorn'),
                                                          'capstone': importlib.metadata.version('capstone')},
            'code': native.code, 'constants': native.constants, 'caseCount': len(cases), 'cases': cases}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Read-only bounded TH128 player hit prefixes, not a game process')
    parser.add_argument('executable', type=Path)
    parser.add_argument('--report', type=Path, required=True)
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.report)
    report = build_report(NativePlayerHit(arguments.executable))
    destination.write_text(json.dumps(report, indent=2, ensure_ascii=True) + '\n', encoding='utf-8')
    print(json.dumps({'report': str(destination), 'caseCount': report['caseCount'],
                      'sha256': hashlib.sha256(destination.read_bytes()).hexdigest(),
                      'originalUnchanged': report['executable']['unchanged']}))


if __name__ == '__main__':
    main()
