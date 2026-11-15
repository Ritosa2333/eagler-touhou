import argparse
import hashlib
import importlib.metadata
import json
import math
import struct
from pathlib import Path

from bullet_probe import EXE_SHA256, ROOT, RETURN_TRAP, STACK_POINTER, NativeBullet, address, datum, emulate


PLAYER, CONTROL, SCENE, TAIL, RECORD = 0x300000, 0x310000, 0x320000, 0x330000, 0x340100


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('player-death-') or destination.drive.lower() == 'e:'):
        raise ValueError('report must be private local/research/player-death-*.json')
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
        raise ValueError('explicit pointer presence required')
    return value


def position(values):
    if not isinstance(values, (list, tuple)) or len(values) != 3:
        raise ValueError('explicit XYZ required')
    for value in values:
        if type(value) not in (float, int) or abs(value) > 65536 or not math.isfinite(value):
            raise ValueError('finite position magnitude <=65536 required')
    return struct.pack('<3f', *values)


def timer_bytes(values):
    if not isinstance(values, (list, tuple)) or len(values) != 5:
        raise ValueError('five explicit timer fields required')
    raw = struct.pack('<iiIII', signed(values[0]), signed(values[1]),
                      unsigned(values[2]), unsigned(values[3]), unsigned(values[4]))
    fractional = struct.unpack_from('<f', raw, 8)[0]
    if not math.isfinite(fractional) or abs(fractional) > 65536:
        raise ValueError('bounded finite timer fraction required')
    return raw


def timer_state(payload):
    return list(struct.unpack('<iiIII', payload))


def after(result, name):
    return bytes.fromhex(result['memory'][name]['after'])


def words(result, kind, binding=0, operands=()):
    return [kind, int(result['stop']['address'], 16), int(result['stop']['target'], 16), binding,
            *[value & 0xffffffff for value in operands], *[0] * (6 - len(operands))]


class NativeDeath(NativeBullet):
    def finish(self, result, kind, inputs, **details):
        result.update(kind=kind, input=inputs, completedNativeFunction=False, **details)
        return result

    def commit_call(self, stage):
        if type(stage) is not int or stage not in (0, 1):
            raise ValueError('explicit first or second commit call required')
        start = 0x43cdd0 if stage == 0 else 0x43cde4
        result = emulate([self.chunk(start, 0x43cdee)], start, registers={'eax': PLAYER}, budget=32)
        argument = struct.unpack_from('<i', bytes.fromhex(result['stackHead']))[0]
        return self.finish(result, 'commit-call', {'stage': stage}, argument=argument,
                           requestWords=words(result, stage, operands=(argument,)),
                           requiresEarlierCallsResolved=stage != 0)

    def motivation(self, value, flag):
        signed(value)
        signed(flag)
        if flag == 0 and value < -2147473648:
            raise ValueError('death resource subtraction would signed-overflow')
        result = emulate([self.chunk(0x427260, 0x4272f8)], 0x427260,
                         [datum('resource', 0x4b4d64, value), datum('resourceFlag', 0x4b4d68, flag)],
                         arguments=struct.pack('<i', -10000), budget=128)
        return self.finish(result, 'motivation', {'value4b4d64': value, 'flag4b4d68': flag},
                           valueAfter=struct.unpack('<i', after(result, 'resource'))[0],
                           flagAfter=struct.unpack('<i', after(result, 'resourceFlag'))[0],
                           completedIndependentHelper=result['stop']['reason'] == 'returned')

    def charge(self, value):
        signed(value)
        if value > 2147481147:
            raise ValueError('perfect charge addition would signed-overflow')
        result = emulate([self.chunk(0x427300, 0x427396)], 0x427300,
                         [datum('perfectCharge', 0x4b4d70, value)],
                         arguments=struct.pack('<i', 2500), budget=128)
        required = result['stop']['reason'] == 'unsupported_call'
        return self.finish(result, 'charge', {'perfectCharge4b4d70': value},
                           valueAfter=struct.unpack('<i', after(result, 'perfectCharge'))[0],
                           soundRequired=required, soundId=result['registers']['edx'] if required else None,
                           requestWords=words(result, 2, operands=(result['registers']['edx'],)) if required else None,
                           completedIndependentHelper=not required)

    def charge_suffix(self, stage):
        if stage not in (0, 1):
            raise ValueError('explicit charge notification or UI suffix required')
        start, end, kind = (0x42735a, 0x42736e, 3) if stage == 0 else (0x42736e, 0x42737a, 4)
        result = emulate([self.chunk(start, end)], start,
                         [datum('controlPointer', 0x4b8950, CONTROL, kind='I', writable=False)], budget=32)
        operands = (result['registers']['eax'], *struct.unpack_from('<2I', bytes.fromhex(result['stackHead'])))
        return self.finish(result, 'charge-suffix', {'stage': stage},
                           requestWords=words(result, kind, 1, operands if stage == 0 else ()),
                           requiresEarlierCallsResolved=True)



    def after_awards(self, selector, value00):
        result = emulate([self.chunk(0x43cdee, 0x43ce17)], 0x43cdee,
                         [datum('selector', 0x4b4cfc, signed(selector)),
                          datum('value00', 0x4b4d00, signed(value00)),
                          datum('power', 0x4b4d6c, -123),
                          datum('controlPointer', 0x4b8950, CONTROL, kind='I', writable=False)],
                         stops=[0x43ce17], budget=32)
        required = result['stop']['reason'] == 'unsupported_call'
        return self.finish(result, 'after-awards', {'selector4b4cfc': selector, 'value4b4d00': value00},
                           powerAfter=struct.unpack('<i', after(result, 'power'))[0],
                           requestWords=words(result, 5, 1, (selector, value00)) if required else None,
                           requiresEarlierCallsResolved=True, continuation=0x43ce17)

    def state_two_setup(self, state_timer, protection_timer):
        data = [datum('state', PLAYER + 0xf78, 4), datum('stateTimer', PLAYER + 0xf80, timer_bytes(state_timer)),
                datum('protectionTimer', PLAYER + 0x438d8, timer_bytes(protection_timer)),
                datum('playerBank', PLAYER + 0x10, CONTROL, kind='I'), self.constant(0x4a6bc8, 4)]
        result = emulate([self.chunk(0x43ce17, 0x43cecb)], 0x43ce17, data,
                         registers={'esi': PLAYER, 'edi': 0}, budget=128)
        return self.finish(result, 'state-two', {'stateTimer': list(state_timer),
                           'protectionTimer': list(protection_timer)},
                           stateAfter=struct.unpack('<i', after(result, 'state'))[0],
                           stateTimerAfter=timer_state(after(result, 'stateTimer')),
                           protectionTimerAfter=timer_state(after(result, 'protectionTimer')),
                           requestWords=words(result, 6, 2, (result['registers']['ebx'], 0x14)),
                           requiresEarlierCallsResolved=True, continuation=0x43cecb)

    def cleanup_record(self, active, primary, secondary, *, after_primary=False):
        data = [datum('active', RECORD - 0xb0, unsigned(active), kind='I'),
                datum('handles', RECORD, struct.pack('<2I', unsigned(primary), unsigned(secondary)))]
        start = 0x43cf8c if after_primary else 0x43cee0
        result = emulate([self.chunk(0x43cee0, 0x43d02c)], start, data,
                         registers={'ebx': RECORD}, stops=[0x43cef4, 0x43cf97, 0x43d02c], budget=32)
        boundary = int(result['stop']['address'], 16)
        required = result['registers']['ecx'] if boundary in (0x43cef4, 0x43cf97) else 0
        return self.finish(result, 'cleanup-record', {'active': active, 'primary': primary,
                           'secondary': secondary, 'afterPrimary': after_primary},
                           activeAfter=struct.unpack('<I', after(result, 'active'))[0],
                           requiredHandle=required, phase=1 if boundary == 0x43cf97 else 0,
                           lookupInstruction=boundary if required else 0,
                           continuation=0x43cf8c if boundary == 0x43cef4 else 0x43d02c,
                           requiresEarlierCallsResolved=True)

    def respawn_gate(self, elapsed, resource, scene_present, scene_mode, coordinates):
        signed(elapsed)
        signed(resource)
        present(scene_present)
        signed(scene_mode)
        if elapsed >= 30 and resource < 0 and not scene_present:
            raise ValueError('negative-resource scene pointer is unknown')
        raw_position = position(coordinates)
        data = [datum('state', PLAYER + 0xf78, 2), datum('elapsed', PLAYER + 0xf84, elapsed),
                datum('position', PLAYER + 0xecc, raw_position),
                datum('resource', 0x4b4d64, resource, writable=False),
                datum('scenePointer', 0x4b8a84, SCENE if scene_present else 0, kind='I', writable=False),
                datum('clock', 0x4b6f58, .75, kind='f'),
                self.constant(0x4a6760, 4), self.constant(0x4a6618, 4)]
        if scene_present:
            data.append(datum('sceneMode', SCENE + 0x10, scene_mode, writable=False))
        result = emulate([self.chunk(0x43b862, 0x43b9ee)], 0x43b862, data,
                         registers={'esi': PLAYER}, stops=[0x43b9ee], budget=96)
        target = result['stop'].get('target')
        outcome = {None: 0, '0x00438030': 1, '0x00437050': 2, '0x0043df90': 3}.get(target)
        request = (words(result, 10, 3, struct.unpack_from('<5I', bytes.fromhex(result['stackHead']), 4))
                   if outcome == 3 else words(result, 8 if outcome == 1 else 9) if outcome else None)
        return self.finish(result, 'respawn-gate', {'stateF78': 2, 'elapsedF84': elapsed,
                           'resource4b4d64': resource, 'scenePresent': scene_present, 'sceneMode10': scene_mode,
                           'positionBits': list(struct.unpack('<3I', raw_position))},
                           outcome=outcome, stateAfter=struct.unpack('<i', after(result, 'state'))[0],
                           clockAfterBits=struct.unpack('<I', after(result, 'clock'))[0], requestWords=request,
                           continuation=int(result['stop']['address'], 16) + 5 if target else 0x43b9ee)

    def after_field(self, difficulty):
        result = emulate([self.chunk(0x43b969, 0x43b97c)], 0x43b969,
                         [datum('difficulty', 0x4b4d04, signed(difficulty), writable=False)],
                         stops=[0x43b97c], budget=32)
        required = result['stop']['reason'] == 'unsupported_call'
        return self.finish(result, 'after-field', {'difficulty4b4d04': difficulty},
                           requestWords=words(result, 11, 4) if required else None,
                           continuation=0x43b97c, requiresEarlierCallsResolved=True)

    def respawn_position(self, coordinates, protection_timer, state_timer):
        raw_position = position(coordinates)
        data = [datum('position', PLAYER + 0xecc, raw_position),
                datum('savedPosition', PLAYER + 0xf58, bytes(12)),
                datum('rawPosition', PLAYER + 0xed8, struct.pack('<2i', 123, -456)),
                datum('protectionTimer', PLAYER + 0x438d8, timer_bytes(protection_timer)),
                datum('stateTimer', PLAYER + 0xf80, timer_bytes(state_timer)), self.constant(0x4a67a0, 4)]
        result = emulate([self.chunk(0x43b97c, 0x43b9ee), self.chunk(0x403280, 0x4032bd)],
                         0x43b97c, data, registers={'esi': PLAYER, 'edi': PLAYER + 0xecc, 'ebx': 0},
                         stops=[0x43b9ee], budget=128)
        return self.finish(result, 'respawn-position', {'positionBits': list(struct.unpack('<3I', raw_position)),
                           'protectionTimer': list(protection_timer), 'stateTimer': list(state_timer)},
                           positionAfterBits=list(struct.unpack('<3I', after(result, 'position'))),
                           savedPositionBits=list(struct.unpack('<3I', after(result, 'savedPosition'))),
                           rawAfter=list(struct.unpack('<2i', after(result, 'rawPosition'))),
                           protectionTimerAfter=timer_state(after(result, 'protectionTimer')),
                           stateTimerAfter=timer_state(after(result, 'stateTimer')),
                           requiresEarlierCallsResolved=True)

    def tail(self, flags, counter, value80, control_present, mode, scalar30):
        signed(scalar30)
        if scalar30 < -2147482624:
            raise ValueError('scalar30 subtraction would signed-overflow')
        if flags & 1 and counter < 60 and not present(control_present):
            raise ValueError('native branch dereferences unknown control')
        block = bytearray(0x84)
        struct.pack_into('<i', block, 0x28, signed(counter))
        struct.pack_into('<Ii', block, 0x7c, unsigned(flags), signed(value80))
        data = [datum('tailManager', TAIL, bytes(block)),
                datum('tailManagerPointer', 0x4b8934, TAIL, kind='I', writable=False),
                datum('controlPointer', 0x4b892c, CONTROL if present(control_present) else 0, kind='I', writable=False),
                datum('scalar30', 0x4b4d30, scalar30), datum('field438f8', PLAYER + 0x438f8, -91)]
        if control_present:
            data.append(datum('mode', CONTROL + 0x3c, signed(mode), writable=False))
        else:
            signed(mode)
        result = emulate([self.chunk(0x43d03e, 0x43d0b1)], 0x43d03e, data,
                         registers={'esi': PLAYER, 'ebp': STACK_POINTER + 12},
                         stack={0: struct.pack('<5I', 0, 0, 0, 0, RETURN_TRAP)}, budget=96)
        return self.finish(result, 'tail', {'flags7c': flags, 'counter28': counter, 'value80': value80,
                           'controlPresent': control_present, 'mode3c': mode, 'scalar4b4d30': scalar30},
                           flagsAfter=struct.unpack_from('<I', after(result, 'tailManager'), 0x7c)[0],
                           value80After=struct.unpack_from('<i', after(result, 'tailManager'), 0x80)[0],
                           scalar30After=struct.unpack('<i', after(result, 'scalar30'))[0],
                           field438f8After=struct.unpack('<i', after(result, 'field438f8'))[0],
                           completedIndependentTail=result['stop']['reason'] == 'returned',
                           requiresEarlierCallsResolved=True)


def build_report(native):
    cases = []

    def record(identifier, result):
        if result['stop']['reason'] not in ('returned', 'boundary', 'unsupported_call'):
            raise ValueError(f"unexpected original stop in {identifier}: {result['stop']}")
        if result['instructionsExecuted'] >= result['instructionBudget']:
            raise ValueError('instruction budget exhausted')
        result['id'] = identifier
        cases.append(result)

    for stage in (0, 1):
        record(f'commit-call-{stage}', native.commit_call(stage))
        record(f'charge-suffix-{stage}', native.charge_suffix(stage))
    for flag in (-2147483648, -1, 0, 1, 2147483647):
        for value in (-2147483648, -2147473648, -10000, -1, 0, 9999, 10000, 10001, 100000,
                      110000, 200000, 2147483647):
            if flag == 0 and value < -2147473648:
                continue
            record(f'motivation-{value}-{flag}', native.motivation(value, flag))
    for value in (-2147483648, -20001, -20000, -19999, -12501, -12500, -12499, -10001,
                  -10000, -9999, -2501, -2500, -2499, -1, 0, 1, 7499, 7500, 9999, 10000,
                  17499, 17500, 19999, 20000, 27499, 27500, 29999, 30000, 30001, 50000,
                  2147481146, 2147481147):
        record(f'charge-{value}', native.charge(value))
    for selector in (-2147483648, -1, 0, 1, 8, 2147483647):
        for value00 in (-2147483648, 0, 2147483647):
            record(f'after-awards-{selector}-{value00}', native.after_awards(selector, value00))
    for flags in (0, 1, 2, 3, 0xfffffffe, 0xffffffff):
        record(f'state-two-{flags}', native.state_two_setup(
            [17, 9, 0x80000000, 0x12345678, flags], [-17, -9, 0xc1100000, 0x87654321, flags ^ 1]))
    for active in (0, 91, 0xffffffff):
        for primary in (0, 17, 0xffffffff):
            for secondary in (0, 22, 0xffffffff):
                for after_primary in (False, True):
                    record(f'cleanup-{active}-{primary}-{secondary}-{after_primary}',
                           native.cleanup_record(active, primary, secondary, after_primary=after_primary))
    for flags in (0, 1, 0x47, 0xff, 0xffffffff):
        for counter in (-2147483648, 59, 60, 2147483647):
            for control_present, mode in ((True, 0), (True, -1), (False, 0)):
                if flags & 1 and counter < 60 and not control_present:
                    continue
                for scalar in (-2147482624, -1024, -1, 0, 1024, 2048, 2147483647):
                    record(f'tail-{flags}-{counter}-{control_present}-{mode}-{scalar}',
                           native.tail(flags, counter, -17, control_present, mode, scalar))
    positions = ((0, 400, 0), (-133.25, 7.5, .1), (-0.0, -0.0, -0.0),
                 (65536, -65536, 65536), (2**-149, -2**-149, 2**-149))
    for elapsed in (-2147483648, 29, 30, 2147483647):
        for resource in (-2147483648, -1, 0, 2147483647):
            for scene_present, scene_mode in ((True, 0), (True, 1), (False, 0)):
                if elapsed >= 30 and resource < 0 and not scene_present:
                    continue
                record(f'respawn-gate-{elapsed}-{resource}-{scene_present}-{scene_mode}',
                       native.respawn_gate(elapsed, resource, scene_present, scene_mode,
                                           positions[len(cases) % len(positions)]))
    for difficulty in (-2147483648, -1, 0, 1, 2, 2147483647):
        record(f'after-field-{difficulty}', native.after_field(difficulty))
    for index, coordinates in enumerate(positions):
        for flags in (0, 1, 2, 0xffffffff):
            record(f'respawn-position-{index}-{flags}', native.respawn_position(coordinates,
                [17, 9, 0x80000000, 0x12345678, flags], [-17, -9, 0xc1100000, 0x87654321, flags ^ 1]))
    native.constant(0x43c1bc, 20)
    after_hash = hashlib.sha256(native.executable.read_bytes()).hexdigest()
    if after_hash != EXE_SHA256:
        raise ValueError('original executable changed during read-only probing')
    return {'schema': 'th128-original-byte-player-death-fixtures/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': after_hash, 'unchanged': True},
            'nativeCpuTrace': False, 'windowsGameProcessLaunched': False, 'fakeCallReturns': False,
            'completedNativeFunction': False, 'emulator': {'unicorn': importlib.metadata.version('unicorn'),
                                                          'capstone': importlib.metadata.version('capstone')},
            'code': native.code, 'constants': native.constants, 'caseCount': len(cases), 'cases': cases}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Read-only bounded TH128 death stages, not a game process')
    parser.add_argument('executable', type=Path)
    parser.add_argument('--report', type=Path, required=True)
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.report)
    report = build_report(NativeDeath(arguments.executable))
    destination.write_text(json.dumps(report, indent=2, ensure_ascii=True) + '\n', encoding='utf-8')
    print(json.dumps({'report': str(destination), 'caseCount': report['caseCount'],
                      'sha256': hashlib.sha256(destination.read_bytes()).hexdigest(),
                      'originalUnchanged': report['executable']['unchanged']}))


if __name__ == '__main__':
    main()
