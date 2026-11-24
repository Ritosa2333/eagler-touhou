import argparse
import hashlib
import importlib.metadata
import json
import math
import struct
from pathlib import Path

from bullet_probe import EXE_SHA256, ROOT, STACK_POINTER, NativeBullet, address, datum, emulate


POSITION, BULLET, MANAGER, BANK = 0x300000, 0x310000, 0x320000, 0x330000
COUNT, POWER = 0x4b4d40, 0x4b4d6c
SITES = {'half-step': (0x408879, 0x40888b, 0x408883, 0x408888),
         'full-step': (0x408b55, 0x408c93, 0x408b67, 0x408b6c)}


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('graze-') or destination.drive.lower() == 'e:'):
        raise ValueError('report must be private local/research/graze-*.json')
    return destination


def signed(value):
    if type(value) is not int or not -2147483648 <= value <= 2147483647:
        raise ValueError('explicit signed int32 required')
    return value


def unsigned(value):
    if type(value) is not int or not 0 <= value < 2**32:
        raise ValueError('explicit unsigned int32 required')
    return value


def scalar_inputs(count, power):
    signed(count)
    signed(power)
    if power > 2147483547:
        raise ValueError('power addition would overflow signed int32')
    return count, power


def position(values):
    if not isinstance(values, (tuple, list)) or len(values) != 3:
        raise ValueError('explicit bullet XYZ required')
    for value in values:
        if type(value) not in (int, float) or abs(value) > 65536 or not math.isfinite(value):
            raise ValueError('finite position magnitude <= 65536 required')
    return struct.pack('<3f', *values)


def bits(payload):
    return list(struct.unpack('<' + 'I' * (len(payload) // 4), payload))


def serial_request(request):
    kind = request['kind']
    return [{'threshold-sound': 0, 'effect': 1, 'positional-sound': 2}[kind],
            int(request['callSite'], 16), int(request['callTarget'], 16), request.get('soundId', 0),
            request.get('scriptId', 0), request.get('layer', 0), 0,
            *(request['positionBits'] if kind == 'effect' else
              [request.get('positionXBits', 0), 0, 0])]


class NativeGraze(NativeBullet):
    def position_data(self, bullet_position):
        payload = position(bullet_position)
        return datum('position', POSITION, payload, writable=False), bits(payload)

    def bank_data(self, bank):
        return [datum('managerPointer', 0x4b8930, MANAGER, kind='I', writable=False),
                datum('managerAnmBank', MANAGER + 0x8a7f9c, unsigned(bank), kind='I', writable=False)]

    def effect_request(self, result, position_bits):
        head = bytes.fromhex(result['stackHead'])
        if len(head) < 12 or struct.unpack_from('<I', head, 8)[0] != POSITION:
            raise ValueError('original effect ABI did not retain the bullet position pointer')
        return {'kind': 'effect', 'callSite': '0x0043e119', 'callTarget': '0x00407ec0',
                'scriptId': struct.unpack_from('<i', head, 4)[0], 'layer': result['registers']['eax'],
                'outputPointer': address(struct.unpack_from('<I', head)[0]),
                'bankSource': 'bullet-manager+8a7f9c', 'bankValue': result['registers']['ecx'],
                'positionPointer': address(POSITION), 'positionBits': position_bits}

    def prefix(self, count, power, bullet_position, *, bank=BANK):
        scalar_inputs(count, power)
        point, position_bits = self.position_data(bullet_position)
        data = [datum('grazeCount', COUNT, count), datum('powerRaw', POWER, power), point,
                *self.bank_data(bank)]
        result = emulate([self.chunk(0x43e090, 0x43e133)], 0x43e090, data,
                         registers={'edi': POSITION}, budget=128)
        memory = result['memory']
        updated = {name: struct.unpack('<i', bytes.fromhex(memory[name]['after']))[0]
                   for name in ('grazeCount', 'powerRaw')}
        sound = result['registers']['edx'] if result['stop'].get('target') == '0x00458410' else None
        request = ({'kind': 'threshold-sound', 'callSite': result['stop']['address'],
                    'callTarget': result['stop']['target'], 'soundId': sound} if sound is not None
                   else self.effect_request(result, position_bits))
        result.update(kind='scalar-prefix', input={'grazeCount': count, 'powerRaw': power,
                                                   'positionBits': position_bits, 'bankValue': bank},
                      updatedGlobals=updated, firstThresholdSound=sound, firstRequest=request,
                      firstRequestWords=serial_request(request), completedNativeFunction=False,
                      scalarWritesComplete=True)
        return result

    def effect_prefix(self, bullet_position, *, bank=BANK):
        point, position_bits = self.position_data(bullet_position)
        result = emulate([self.chunk(0x43e0fe, 0x43e11e)], 0x43e0fe,
                         [point, *self.bank_data(bank)], registers={'edi': POSITION, 'ebp': STACK_POINTER + 64},
                         budget=64)
        request = self.effect_request(result, position_bits)
        result.update(kind='effect-prefix', request=request, requestWords=serial_request(request),
                      input={'positionBits': position_bits, 'bankValue': bank},
                      requiresEarlierCallsResolved=True, completedNativeFunction=False)
        return result

    def positional_sound_prefix(self, bullet_position):
        point, position_bits = self.position_data(bullet_position)
        result = emulate([self.chunk(0x43e11e, 0x43e12e)], 0x43e11e, [point],
                         registers={'edi': POSITION}, budget=32)
        request = {'kind': 'positional-sound', 'callSite': '0x0043e129', 'callTarget': '0x004584a0',
                   'soundId': result['registers']['eax'],
                   'positionXBits': bits(bytes.fromhex(result['stackHead'])[:4])[0]}
        result.update(kind='positional-sound-prefix', request=request, requestWords=serial_request(request),
                      input={'positionBits': position_bits}, requiresEarlierCallsResolved=True,
                      completedNativeFunction=False)
        return result

    def caller_prefix(self, site, helper_return, bullet_flags):
        if site not in SITES or type(helper_return) is not int or helper_return not in (0, 2):
            raise ValueError('explicit non-hit helper return 0/2 and known caller site required')
        unsigned(bullet_flags)
        start, continuation, call_site, after_call = SITES[site]
        end = continuation if site == 'half-step' else 0x408b74
        result = emulate([self.chunk(start, end)], start,
                         [datum('bulletFlags', BULLET, bullet_flags, kind='I')],
                         registers={'ebx': BULLET, 'eax': helper_return}, stops=[continuation], budget=32)
        flags_after = struct.unpack('<I', bytes.fromhex(result['memory']['bulletFlags']['after']))[0]
        required = result['stop'].get('target') == '0x0043e090'
        result.update(kind='caller-prefix', input={'site': site, 'helperReturn': helper_return,
                                                   'bulletFlags': bullet_flags},
                      flagsAfter=flags_after, grazeCallRequired=required,
                      pendingFlagsSetAfterSuccess=4 if required else 0,
                      callSite=address(call_site) if required else None,
                      callTarget='0x0043e090' if required else None,
                      continuation=address(continuation), completedNativeFunction=False)
        return result

    def post_call_flags(self, site, bullet_flags):
        if site not in SITES:
            raise ValueError('known update site required')
        unsigned(bullet_flags)
        _, continuation, _, start = SITES[site]
        end = continuation if site == 'half-step' else 0x408b74
        result = emulate([self.chunk(start, end)], start,
                         [datum('bulletFlags', BULLET, bullet_flags, kind='I')],
                         registers={'ebx': BULLET}, stops=[continuation], budget=16)
        result.update(kind='post-call-flags', input={'site': site, 'bulletFlags': bullet_flags},
                      flagsAfter=struct.unpack('<I', bytes.fromhex(result['memory']['bulletFlags']['after']))[0],
                      requiresEarlierCallsResolved=True, completedNativeFunction=False)
        return result


def build_report(native):
    positions = ((0, 0, 0), (-133.25, 7.5, .1), (-0.0, -0.0, -0.0),
                 (65536, -65536, 65536), (2**-149, -2**-149, 0), (.5, -.125, .25))
    effects, sounds, plans, callers, post_calls = [], [], [], [], []

    def record(group, identifier, result):
        if result['stop']['reason'] not in ('unsupported_call', 'boundary'):
            raise ValueError(f"unexpected original stop: {identifier}: {result['stop']}")
        if result['instructionsExecuted'] >= result['instructionBudget']:
            raise ValueError('original instruction budget exhausted')
        result['id'] = identifier
        group.append(result)
        return result

    for index, coordinates in enumerate(positions):
        record(effects, f'effect-position-{index}', native.effect_prefix(coordinates))
        record(sounds, f'positional-sound-{index}', native.positional_sound_prefix(coordinates))
    for bank in (0, 1, 0x12345678, 0xffffffff):
        record(effects, f'effect-bank-{bank}', native.effect_prefix(positions[1], bank=bank))
    counts = (-2147483648, -1, 0, 1, 99999998, 99999999, 100000000, 2147483647)
    powers = (-2147483648, -101, -100, -1, 0, 1, 2899, 2900, 2999, 3000, 3001,
              9899, 9900, 9999, 10000, 10001, 2147483546, 2147483547)
    for count in counts:
        for power in powers:
            coordinates = positions[len(plans) % len(positions)]
            sample = native.prefix(count, power, coordinates)
            effect = effects[len(plans) % len(positions)]
            sound = sounds[len(plans) % len(positions)]
            sample['plannedCallWords'] = ([sample['firstRequestWords']] if sample['firstThresholdSound'] is not None
                                           else []) + [effect['requestWords'], sound['requestWords']]
            sample['requestProofs'] = ([{'kind': 'threshold-sound', 'source': 'own-prefix-first-stop'}]
                                       if sample['firstThresholdSound'] is not None else [])
            sample['requestProofs'] += [{'kind': 'effect', 'source': effect['id']},
                                        {'kind': 'positional-sound', 'source': sound['id']}]
            sample['requestSequenceKind'] = 'conditional-independent-suffix-proofs'
            record(plans, f'plan-count-{count}-power-{power}', sample)
    for index, coordinates in enumerate(positions):
        for power in (2900, 9900, 10000):
            sample = native.prefix(0, power, coordinates)
            sample['plannedCallWords'] = ([sample['firstRequestWords']] if sample['firstThresholdSound'] is not None
                                           else []) + [effects[index]['requestWords'], sounds[index]['requestWords']]
            sample['requestProofs'] = ([{'kind': 'threshold-sound', 'source': 'own-prefix-first-stop'}]
                                       if sample['firstThresholdSound'] is not None else [])
            sample['requestProofs'] += [{'kind': 'effect', 'source': effects[index]['id']},
                                        {'kind': 'positional-sound', 'source': sounds[index]['id']}]
            sample['requestSequenceKind'] = 'conditional-independent-suffix-proofs'
            record(plans, f'plan-position-{index}-power-{power}', sample)
    for site in SITES:
        for flags in (0, 1, 2, 3, 4, 7, 0x13, 0x17, 0xfffffffb, 0xffffffff):
            for helper_return in (0, 2):
                record(callers, f'caller-{site}-{helper_return}-{flags}',
                       native.caller_prefix(site, helper_return, flags))
            record(post_calls, f'post-call-{site}-{flags}', native.post_call_flags(site, flags))
    after_hash = hashlib.sha256(native.executable.read_bytes()).hexdigest()
    if after_hash != EXE_SHA256:
        raise ValueError('original executable changed during read-only probing')
    return {'schema': 'th128-original-byte-graze-fixtures/1',
            'executable': {'sha256': EXE_SHA256, 'sha256After': after_hash, 'unchanged': True},
            'windowsGameProcessLaunched': False, 'nativeCpuTrace': False, 'fakeCallReturns': False,
            'completedNativeFunction': False, 'emulator': {'unicorn': importlib.metadata.version('unicorn'),
                                                          'capstone': importlib.metadata.version('capstone')},
            'code': native.code, 'planCases': plans, 'effectCases': effects, 'positionalSoundCases': sounds,
            'callerCases': callers, 'postCallFlagCases': post_calls,
            'caseCount': sum(map(len, (plans, effects, sounds, callers, post_calls)))}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Read-only TH128 graze scalar and request proofs; no game process')
    parser.add_argument('executable', type=Path)
    parser.add_argument('--report', type=Path, required=True)
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.report)
    report = build_report(NativeGraze(arguments.executable))
    destination.write_text(json.dumps(report, indent=2, ensure_ascii=True) + '\n', encoding='utf-8')
    print(json.dumps({'report': str(destination), 'caseCount': report['caseCount'],
                      'sha256': hashlib.sha256(destination.read_bytes()).hexdigest(),
                      'originalUnchanged': report['executable']['unchanged']}))


if __name__ == '__main__':
    main()
