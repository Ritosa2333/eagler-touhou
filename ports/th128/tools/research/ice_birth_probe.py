import argparse
import hashlib
import json
import math
from pathlib import Path
import struct

from bullet_probe import NativeBullet, ROOT, address, datum, EXE_SHA256
from freeze_probe import emulate


MANAGER, POSITION = 0x300000, 0x390000
POOL_OFFSET, SLOT_COUNT, OBJECT_BYTES = 0x10c, 4000, 0x68
CURSOR_OFFSET, GLOBAL_HEAD, BUCKET_HEADS = 0x65a0c, 0x65a10, 0x65a1c
OLD_BUCKET, OLD_GLOBAL = 0x700000, 0x710000
MAX_INSTRUCTIONS, MAX_CASES = 100000, 128


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('ice-birth')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be owned private local/research/ice-birth*.json')
    return destination


def integer(value):
    if type(value) is not int or not -2**31 <= value < 2**31:
        raise ValueError('exact signed int32 input required')
    return value


def floating(value):
    if type(value) not in (int, float) or abs(value) > 2**24 or not math.isfinite(value):
        raise ValueError('bounded finite float32 input required')
    return struct.unpack('<f', struct.pack('<f', value))[0]


def portable_position_supported(position):
    return all(math.isfinite(value) and abs(value)<=2**24 for value in position)


def inputs(*, cursor=0, occupied_run=0, position=(0, 359, .1), value=50,
           duration=100, mode=1, timer_flags=0, delay_flags=0, nonempty=False,
           animation_handles=(0, 0), stage='returned'):
    if (type(cursor) is not int or not 0 <= cursor < SLOT_COUNT
            or type(occupied_run) is not int or not 0 <= occupied_run <= SLOT_COUNT
            or type(timer_flags) is not int or not 0 <= timer_flags < 2**32
            or type(delay_flags) is not int or not 0 <= delay_flags < 2**32
            or type(nonempty) is not bool or stage not in ('returned', 'selection', 'before-lists')):
        raise ValueError('unsupported bounded pool or prefix input')
    if not isinstance(position, (tuple, list)) or len(position) != 3:
        raise ValueError('exact three-component position required')
    if (not isinstance(animation_handles, (tuple, list)) or len(animation_handles) != 2
            or any(type(handle) is not int or not 0 <= handle < 2**32 for handle in animation_handles)):
        raise ValueError('two explicit opaque animation handles required')
    position = [floating(component) for component in position]
    return {'cursor': cursor, 'occupiedRun': occupied_run, 'position': position,
            'positionBits': list(struct.unpack('<3I', struct.pack('<3f', *position))),
            'value': floating(value), 'duration': integer(duration), 'mode': integer(mode),
            'timerFlags': timer_flags, 'delayFlags': delay_flags, 'nonempty': nonempty,
            'animationHandles': list(animation_handles), 'stage': stage}


def semantic(payload):
    words = list(struct.unpack('<26I', payload))
    signed = lambda offset: struct.unpack_from('<i', payload, offset)[0]
    return {'occupied': signed(0xc), 'positionBits': words[4:7], 'valueBits': words[7],
            'lifetime': [signed(0x20), signed(0x24), words[10], words[11], words[12]],
            'delay': [signed(0x34), signed(0x38), words[15], words[16], words[17]],
            'state48': words[18], 'mode': signed(0x4c), 'animation50': words[20],
            'animation54': words[21], 'maskSelector': words[22]}


def bounded_emulate(chunks, entry, data, **options):
    if (len(chunks) > 16 or sum(len(code) for _, code in chunks) > 8192
            or len(data) > 32 or sum(len(region['bytes']) for region in data) > 524288):
        raise ValueError('ice builder explicit code/data budget exceeded')
    return emulate(chunks, entry, data, budget=MAX_INSTRUCTIONS, **options)


class NativeIceBirth(NativeBullet):
    def birth(self, **changes):
        values = inputs(**changes)
        count = min(values['occupiedRun'] + 1, SLOT_COUNT)
        selected = (values['cursor'] + count - 1) % SLOT_COUNT
        selected_offset = selected * OBJECT_BYTES
        pool = bytearray(SLOT_COUNT * OBJECT_BYTES)
        for index in range(values['occupiedRun']):
            struct.pack_into('<i', pool, ((values['cursor'] + index) % SLOT_COUNT) * OBJECT_BYTES + 0xc, 1)
        struct.pack_into('<I', pool, selected_offset + 0x30, values['timerFlags'])
        struct.pack_into('<I', pool, selected_offset + 0x44, values['delayFlags'])
        struct.pack_into('<I', pool, selected_offset + 0x2c, 0xabcdef01)
        struct.pack_into('<I', pool, selected_offset + 0x40, 0xabcdef02)
        struct.pack_into('<3I', pool, selected_offset + 0x50, *values['animationHandles'], 17)
        before = bytes(pool[selected_offset:selected_offset + OBJECT_BYTES])
        heads = bytearray(12 + 80 * 12)
        data = [datum('cursor', MANAGER + CURSOR_OFFSET, values['cursor'], kind='I'),
                datum('heads', MANAGER + GLOBAL_HEAD, bytes(heads)),
                datum('position', POSITION, struct.pack('<3f', *values['position']), writable=False),
                datum('managerPointer', 0x4b8938, MANAGER, kind='I', writable=False),
                datum('cpuFlag', 0x4dcd68, 1, writable=False)]
        for offset in range(0, len(pool), 65520):
            data.append(datum(f'pool-{offset}', MANAGER + POOL_OFFSET + offset, bytes(pool[offset:offset + 65520])))
        if values['nonempty']:
            data += [datum('oldBucket', OLD_BUCKET, bytes(12)), datum('oldGlobal', OLD_GLOBAL, bytes(12))]
        for start, size in ((0x4a6828, 4), (0x4a65d8, 8), (0x4a6638, 8), (0x4a6630, 8)):
            data.append(self.constant(start, size))
        spans = [(0x410590, 0x41065e), (0x410480, 0x4104f2),
                 (0x410660, 0x4106de), (0x48fac0, 0x48fadc)]
        chunks = [self.chunk(start, end) for start, end in spans]
        arguments = struct.pack('<Ifii', POSITION, values['value'], values['duration'], values['mode'])
        bucket_measurement = None
        observed_bucket = None
        if values['stage'] != 'selection':
            bucket_measurement = bounded_emulate(chunks, 0x410590, data,
                                                 registers={'ecx': MANAGER}, stops=[0x41069c],
                                                 arguments=arguments)
            if bucket_measurement['stop'] != {'reason': 'boundary', 'address': '0x0041069c'}:
                raise ValueError('native bucket measurement did not stop before first bucket read')
            base_delta = bucket_measurement['registers']['eax'] - MANAGER
            base_delta = (base_delta + 2**31) % 2**32 - 2**31
            if base_delta % 12:
                raise ValueError('executed native bucket address is not a sentinel stride')
            observed_bucket = base_delta // 12
            bucket_measurement.pop('memory')
        in_table = observed_bucket is not None and 0 <= observed_bucket < 80
        if values['nonempty'] and in_table:
            struct.pack_into('<I', heads, 4, OLD_GLOBAL)
            struct.pack_into('<I', heads, 12 + observed_bucket * 12 + 4, OLD_BUCKET)
            for region in data:
                if region['name'] == 'heads':
                    region['bytes'] = bytes(heads)
        stops = {'returned': [], 'selection': [0x4105cd], 'before-lists': [0x4104e8]}[values['stage']]
        if values['stage'] != 'selection' and not in_table:
            stops = [0x41069c]
        result = bounded_emulate(chunks, 0x410590, data, registers={'ecx': MANAGER},
                                 stops=stops, arguments=arguments)
        stop = result['stop']
        expected_reason = 'returned' if values['stage'] == 'returned' and in_table else 'boundary'
        if stop['reason'] != expected_reason:
            raise ValueError(f'original ice builder did not reach its declared boundary: {stop}')
        if result['instructionsExecuted'] >= MAX_INSTRUCTIONS:
            raise ValueError('ice builder instruction budget exhausted')
        observed_pointer = result['registers']['eax' if stop['reason'] == 'returned' else 'esi']
        if observed_pointer != MANAGER + POOL_OFFSET + selected_offset:
            raise ValueError('observed original selected object disagrees with declared pool state')
        final_pool = b''.join(bytes.fromhex(result['memory'][f'pool-{offset}']['after'])
                              for offset in range(0, len(pool), 65520))
        after = final_pool[selected_offset:selected_offset + OBJECT_BYTES]
        final_heads = bytes.fromhex(result['memory']['heads']['after'])
        if stop['reason'] == 'returned':
            sentinel = struct.unpack_from('<I', after, 8)[0]
            if sentinel != MANAGER + BUCKET_HEADS + observed_bucket * 12:
                raise ValueError('returned executed node sentinel disagrees with native address measurement')
        result.update(input=values, selection={'slot': (observed_pointer - MANAGER - POOL_OFFSET) // OBJECT_BYTES,
                      'nextCursor': struct.unpack('<I', bytes.fromhex(result['memory']['cursor']['after']))[0],
                      'examined': result['instructionTrace'].count('0x004105a0'),
                      'overwritesOccupied': values['occupiedRun'] == SLOT_COUNT},
                      before=semantic(before), next=semantic(after), objectBefore=before.hex(), objectAfter=after.hex(),
                      completedBuilder=stop['reason'] == 'returned',
                      observedBucket=observed_bucket, bucketMeasurement=bucket_measurement,
                      portableAccepted=in_table and portable_position_supported(values['position']),
                      poolBeforeSha256=hashlib.sha256(pool).hexdigest(), poolAfterSha256=hashlib.sha256(final_pool).hexdigest(),
                      listState={'globalFirst': struct.unpack_from('<I', final_heads, 4)[0],
                                 'bucketFirst': struct.unpack_from('<I', final_heads, 12 + observed_bucket * 12 + 4)[0] if in_table else None},
                      declaredCode=[f'{start:#x}:{end:#x}' for start, end in spans])
        if values['nonempty'] and in_table:
            result['listState']['oldBucketPrevious'] = struct.unpack_from('<I', bytes.fromhex(result['memory']['oldBucket']['after']), 8)[0]
            result['listState']['oldGlobalPrevious'] = struct.unpack_from('<I', bytes.fromhex(result['memory']['oldGlobal']['after']), 8)[0]
        result['declaredData'] = [{'name': name, 'address': region['address'], 'bytes': region['bytes'],
                                  'writable': region['writable'],
                                  'beforeSha256': hashlib.sha256(bytes.fromhex(region['before'])).hexdigest(),
                                  'afterSha256': hashlib.sha256(bytes.fromhex(region['after'])).hexdigest()}
                                 for name, region in result['memory'].items()]
        result.pop('memory')
        return result

    def required_call_prefixes(self):
        effect_data = [datum('activationSlot', MANAGER + 0x10, bytes(0x54)),
                       datum('position', POSITION, struct.pack('<3f', 0, 359, .1), writable=False),
                       datum('managerPointer', 0x4b8938, MANAGER, kind='I', writable=False),
                       datum('iceResource', MANAGER + 0x65ddc, 0x720000, kind='I', writable=False)]
        effect = bounded_emulate([self.chunk(0x41024b, 0x41027b)], 0x41024b, effect_data,
                                 registers={'ebx': POSITION, 'edi': MANAGER + 0x14,
                                            'esi': MANAGER + 0x10, 'ebp': 0x108200})
        sound = bounded_emulate([self.chunk(0x410290, 0x4102a0)], 0x410290,
                                [datum('position', POSITION, struct.pack('<3f', 0, 359, .1), writable=False)],
                                registers={'ebx': POSITION})
        if effect['stop'].get('target') != '0x00407ec0' or sound['stop'].get('target') != '0x004584a0':
            raise ValueError('required native activation calls were not preserved')
        effect['script'] = struct.unpack_from('<I', bytes.fromhex(effect['stackHead']), 4)[0]
        for result in (effect, sound):
            result['completedActivation'] = False
            result['independentSlice'] = True
            result.pop('memory')
        return [effect, sound]

    def activation_arguments(self, native_frozen_power, position=(0, 359, .1)):
        integer(native_frozen_power)
        if not 0 <= native_frozen_power <= 10000:
            raise ValueError('ordinary saved native frozen power required')
        if not isinstance(position, (tuple, list)) or len(position) != 3:
            raise ValueError('activation XYZ required')
        position = [floating(value) for value in position]
        data = [datum('savedPower', MANAGER + 0x6b200, native_frozen_power, writable=False),
                datum('position', POSITION, struct.pack('<3f', *position), writable=False),
                datum('managerPointer', 0x4b8938, MANAGER, kind='I', writable=False), self.constant(0x4a6b18, 4)]
        result = bounded_emulate([self.chunk(0x4102a0, 0x4102ce)], 0x4102a0, data,
                                 registers={'edi': POSITION})
        if result['stop'] != {'reason': 'unsupported_call', 'address': '0x004102c9', 'target': '0x00410590'}:
            raise ValueError('activation builder call was not preserved as a real handoff')
        arguments = struct.unpack('<4I', bytes.fromhex(result['stackHead'])[:16])
        result.update(nativeFrozenPower=native_frozen_power,
                      positionBits=list(struct.unpack('<3I', struct.pack('<3f', *position))),
                      builderArguments={'positionPointer': arguments[0], 'valueBits': arguments[1],
                                        'duration': arguments[2], 'mode': arguments[3]}, completedActivation=False)
        result.pop('memory')
        return result


def animation_source():
    local = (ROOT / 'local').resolve()
    catalog = json.loads((local / 'catalog.json').read_bytes())
    manifest = (local / catalog['variants']['ja']['manifest']).resolve()
    source = (manifest.parent / 'raw/ice.anm').resolve()
    if not manifest.is_relative_to(local) or not source.is_relative_to(local) or not 0 < source.stat().st_size <= 128 * 1024 * 1024:
        raise ValueError('unexpected actual Japanese ice ANM resource')
    return {'resource': 'ja/raw/ice.anm', 'sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
            'nativeResourceSlot': 36, 'activationLayer': 23, 'activationScript': 33}


def probe(executable):
    native = NativeIceBirth(executable)
    cases = []
    specifications = [('first', {}), ('first-with-existing-heads', {'nonempty': True}),
                      ('retained-animation-handles', {'animation_handles': (0x12345678, 0x23456789)}),
                      ('skip-one', {'occupied_run': 1}), ('skip-seventeen', {'occupied_run': 17}),
                      ('last-slot', {'cursor': 3999}), ('wrap', {'cursor': 3999, 'occupied_run': 2}),
                      ('delay-mode-zero', {'mode': 0}), ('delay-mode-negative', {'mode': -1}),
                      ('both-timers-initialized', {'timer_flags': 1, 'delay_flags': 1}),
                      ('timer-high-flags', {'timer_flags': 0x80000080, 'delay_flags': 0x80000001}),
                      ('duration-zero', {'duration': 0}), ('duration-negative', {'duration': -1}),
                      ('duration-int-min', {'duration': -2**31}), ('duration-int-max', {'duration': 2**31 - 1}),
                      ('negative-zero-value', {'value': -0.0}), ('tiny-value', {'value': 2**-149}),
                      ('left-edge', {'position': (-192, 0, 0)}), ('right-neighbor', {'position': (191.99998474121094, 447.9999694824219, 0)}),
                      ('negative-y', {'position': (-128, -32, .1)}), ('tiny-motion-position', {'position': (2**-149, 2**-22, -0.0)}),
                      ('before-list-publication', {'stage': 'before-lists'}),
                      ('selection-wrap', {'cursor': 3999, 'occupied_run': 1, 'stage': 'selection'}),
                      ('full-pool-selection', {'cursor': 19, 'occupied_run': 4000, 'stage': 'selection'})]
    specifications += [('review-negative-tiny-valid-bucket', {'position': (-2**-46, 359, .1)}),
                       ('review-negative-tiny-invalid-bucket', {'position': (-2**-46, -1792, .1)}),
                       ('tiny-negative-neighbor', {'position': (-2**-45, 359, .1)}),
                       ('tiny-positive-neighbor', {'position': (2**-46, 359, .1)}),
                       ('guard-minimum-negative', {'position': (-2**-16, 359, .1)}),
                       ('boundary-exact-48', {'position': (48, 359, .1)}),
                       ('boundary-before-48', {'position': (struct.unpack('<f', struct.pack('<I', 0x423fffff))[0], 359, .1)}),
                       ('boundary-after-48', {'position': (struct.unpack('<f', struct.pack('<I', 0x42400001))[0], 359, .1)}),
                       ('vertical-exact-minus-1792', {'position': (0, -1792, .1)}),
                       ('vertical-before-minus-1792', {'position': (0, struct.unpack('<f', struct.pack('<I', 0xc4dfffff))[0], .1)}),
                       ('vertical-after-minus-1792', {'position': (0, struct.unpack('<f', struct.pack('<I', 0xc4e00001))[0], .1)})]
    specifications += [(f'extended-precision-{power}-{sign}',{'position':(sign*2**-power,359,.1)}) for power in (46,47,55,57,149) for sign in (-1,1)]
    specifications += [(f'grid-column-before-{index}',{'position':(struct.unpack('<f',struct.pack('<I',struct.unpack('<I',struct.pack('<f',48.*index-192.))[0]-1))[0],403.1999816894531,.1)}) for index in range(5,9)]
    boundary=struct.unpack('<I',struct.pack('<f',-1537*2**-57))[0]
    specifications += [(f'extended-tie-{offset}',{'position':(struct.unpack('<f',struct.pack('<I',boundary+offset))[0],359,.1)}) for offset in (-1,0,1)]
    try:
        if len(specifications) > MAX_CASES:
            raise ValueError('ice builder case budget exceeded')
        for name, changes in specifications:
            sample = native.birth(**changes)
            sample['id'] = name
            cases.append(sample)
        activation = [native.activation_arguments(power, position) for power, position in
                      ((0, (-128.00001525878906, 359.0000305175781, .1)),
                       (2999, (-0.0, -32.000003814697266, .10000000894069672)),
                       (3000, (0, 359, .1)), (9999, (48, 200, -0.0)),
                       (10000, (128.00001525878906, 32.000003814697266, 2**-149)))]
        required_calls = native.required_call_prefixes()
        native.constant(0x4a2200, 8)
        masks = []
        for selector, pointer, height, words in ((1, 0x4b1ef8, 21, 1), (2, 0x4b1f50, 25, 1),
                                                 (3, 0x4b1fb8, 33, 1), (4, 0x4b2040, 49, 2),
                                                 (5, 0x4b21c8, 65, 2)):
            constant = native.constant(pointer, height * words * 4)
            masks.append({'selector': selector, 'pointer': address(pointer), 'height': height,
                          'wordsPerRow': words, 'sha256': hashlib.sha256(constant['bytes']).hexdigest(),
                          'evidence': 'static caller arguments; mask helper is not executed here'})
        for start, end in ((0x40e825, 0x40e83f), (0x410208, 0x4102e1), (0x407ec0, 0x407ef5),
                           (0x407f60, 0x407fa4), (0x466d60, 0x466ddc),
                           (0x40ecf2, 0x40ed2f), (0x40ed81, 0x40eeb0), (0x40ee9f, 0x40eed3),
                           (0x40fc60, 0x40fcf3), (0x410960, 0x410a00)):
            native.chunk(start, end)
        digest = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema': 'th128-original-byte-ice-birth-fixtures/1',
                'nativeCpuTrace': False, 'windowsGameProcessLaunched': False,
                'executable': {'sha256': EXE_SHA256, 'sha256After': digest, 'unchanged': digest == EXE_SHA256},
                'animation': animation_source(), 'layout': {'slotCount': SLOT_COUNT, 'objectBytes': OBJECT_BYTES,
                'poolOffset': POOL_OFFSET, 'cursorOffset': CURSOR_OFFSET, 'occupancyOffset': 12,
                'globalHeadOffset': GLOBAL_HEAD, 'bucketHeadsOffset': BUCKET_HEADS},
                'code': native.code, 'constants': native.constants, 'cases': cases, 'caseCount': len(cases),
                'productionDomain': {'nonzeroXMinimum': 2**-16, 'nonzeroQuotientIntegerGuard': 2**-20,
                                     'exactRatioBoundariesAllowed': True,
                                     'nativeBucketObservation': 'executed EAX at0x41069c; returned object+8 sentinel cross-check'},
                'supersedesFixtureSha256': 'fa93c81dec2ebf896c4a1ef8878ab29c2041ce86180aace9465cbddd15ec3e75',
                'activationCases': activation, 'requiredCallPrefixes': required_calls,
                'maskPointerObligations': masks,
                'effectRegistration': {'helper': '0x00466d60', 'listHeadOffset': 0x8b9704,
                                       'listTailOffset': 0x8b9708, 'insertion': 'primary-tail',
                                       'evidence': 'static bytes; not an executed ANM or allocator callback'},
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                'engineSha256': {name: hashlib.sha256((Path(__file__).parent / name).read_bytes()).hexdigest()
                                 for name in ('freeze_probe.py', 'bullet_probe.py')},
                'limitations': ['Original-byte emulation with explicit synthetic memory, not a native Windows game process.',
                'No allocator or Windows API return is invented; ANM/sound calls remain mandatory host work.',
                'Returned builder evidence includes actual initializer and both intrusive list insertions.',
                'Full-pool scan is selection-only; production birth rejects occupied-record reuse.',
                'No ice propagation, mask rasterization, damage, player-hit handling or full-frame equivalence is established.']}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded original-byte TH128 ice object builder')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/ice-birth-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    if not report['executable']['unchanged']:
        raise ValueError('read-only executable digest changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(f'{report["caseCount"]} bounded original-byte builders/prefixes; no allocator or game process')


if __name__ == '__main__':
    main()
