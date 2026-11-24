import argparse
from collections import Counter
import hashlib
import json
import math
from pathlib import Path
import random
import struct

from bullet_probe import NativeBullet, ROOT, EXE_SHA256, address, datum, emulate as short_emulate
from freeze_probe import emulate as complete_emulate


MANAGER, OBJECT, SOURCE, BULLET = 0x300000, 0x390000, 0x700000, 0x710000
MASK_OFFSET, MASK_BYTES = 0x65de4, 21504
MAX_SOURCE_WORDS, MAX_INSTRUCTIONS, MAX_CASES = 2048, 200000, 128
PROFILES = {1: (0x4b1ef8, 21, 1), 2: (0x4b1f50, 25, 1),
            3: (0x4b1fb8, 33, 1), 4: (0x4b2040, 49, 2), 5: (0x4b21c8, 65, 2)}


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed
            or destination.suffix != '.json' or not destination.name.startswith('ice-mask')
            or destination.drive.lower() == 'e:'):
        raise ValueError('report must be private local/research/ice-mask*.json')
    return destination


def coordinate(value):
    if type(value) not in (int, float) or abs(value) > 2**24 or not math.isfinite(value):
        raise ValueError('coordinate must be bounded finite float32')
    return struct.unpack('<f', struct.pack('<f', value))[0]


def pattern(name, size):
    if name == 'zero':
        return bytes(size)
    if name == 'one':
        return b'\xff' * size
    if name == 'alternating':
        return (b'\xaa\x55\xaa\x55' * ((size + 3) // 4))[:size]
    if name == 'random':
        return random.Random(4106960).randbytes(size)
    output = bytearray(size)
    if size and name in ('bit0', 'bit31', 'last-bit', 'row-end'):
        if name == 'row-end':
            for offset in range(44, size - 3, 48):
                output[offset + 3] = 128
        else:
            output[0 if name == 'bit0' else 3 if name == 'bit31' else size - 1] = 1 if name == 'bit0' else 128
        return bytes(output)
    if size == 0 and name in ('bit0', 'bit31', 'last-bit', 'row-end'):
        return bytes()
    raise ValueError('unknown bounded byte pattern')


def frame_input(*, height=1, words_per_row=1, position=(0, 200), operation='or',
                bitmap=None, mask=None, bitmap_pattern='one', mask_pattern='zero'):
    if (type(height) is not int or not 0 <= height <= 448
            or type(words_per_row) is not int or not 0 <= words_per_row <= 12
            or height * words_per_row > MAX_SOURCE_WORDS or operation not in ('or', 'and-not')):
        raise ValueError('bounded source dimensions and known operation required')
    if not isinstance(position, (list, tuple)) or len(position) != 2:
        raise ValueError('exact XY coordinate required')
    position = [coordinate(value) for value in position]
    bitmap = pattern(bitmap_pattern, height * words_per_row * 4) if bitmap is None else bitmap
    mask = pattern(mask_pattern, MASK_BYTES) if mask is None else mask
    if type(bitmap) is not bytes or len(bitmap) != height * words_per_row * 4:
        raise ValueError('source byte span must match its exact declared dimensions')
    if type(mask) is not bytes or len(mask) != MASK_BYTES:
        raise ValueError('destination mask must contain exactly21504 bytes')
    return {'height': height, 'wordsPerRow': words_per_row, 'position': position,
            'positionBits': list(struct.unpack('<2I', struct.pack('<2f', *position))),
            'operation': operation}, bitmap, mask


def compact(result):
    trace = result.pop('instructionTrace')
    result['traceSummary'] = {'length': len(trace), 'sha256': hashlib.sha256('\n'.join(trace).encode('ascii')).hexdigest(),
                              'head': trace[:24], 'tail': trace[-16:], 'visits': dict(sorted(Counter(trace).items()))}
    result['declaredData'] = [{'name': name, 'address': span['address'], 'bytes': len(bytes.fromhex(span['before'])),
                              'writable': span['writable'],
                              'beforeSha256': hashlib.sha256(bytes.fromhex(span['before'])).hexdigest(),
                              'afterSha256': hashlib.sha256(bytes.fromhex(span['after'])).hexdigest()}
                             for name, span in result['memory'].items()]
    result.pop('memory')
    return result


def bounded_execute(chunks, entry, data, *, short=False, **options):
    if (len(chunks) > 16 or sum(len(code) for _, code in chunks) > 8192
            or len(data) > 32 or sum(len(span['bytes']) for span in data) > 131072):
        raise ValueError('mask probe explicit code/data budget exceeded')
    engine = short_emulate if short else complete_emulate
    return engine(chunks, entry, data, budget=20000 if short else MAX_INSTRUCTIONS, **options)


class NativeIceMask(NativeBullet):
    def constants_data(self):
        return [self.constant(pointer, 8) for pointer in (0x4a6628, 0x4a65d8, 0x4a6610, 0x4a6620, 0x4a65a8)]

    def raster(self, **changes):
        values, bitmap, mask = frame_input(**changes)
        entry, end = (0x4106e0, 0x410814) if values['operation'] == 'or' else (0x410820, 0x410958)
        data = [datum('mask', MANAGER + MASK_OFFSET, mask),
                datum('position', OBJECT, struct.pack('<2f', *values['position']), writable=False),
                datum('cpuFlag', 0x4dcd68, 1, writable=False), *self.constants_data()]
        if bitmap:
            data.append(datum('bitmap', SOURCE, bitmap, writable=False))
        spans = [(entry, end), (0x48fac0, 0x48fadc)]
        chunks = [self.chunk(start, stop) for start, stop in spans]
        arguments = struct.pack('<4I', MANAGER, SOURCE, values['height'], values['wordsPerRow'])
        measurement = bounded_execute(chunks, entry, data, registers={'eax': OBJECT},
                                      arguments=arguments, stops=[0x41074a if values['operation'] == 'or' else 0x41088a])
        if measurement['stop']['reason'] != 'boundary':
            raise ValueError('original placement measurement crossed its pre-loop boundary')
        signed = lambda value: (value + 2**31) % 2**32 - 2**31
        placement = {'startRow': signed(measurement['registers']['eax']),
                     'startWord': struct.unpack_from('<i', bytes.fromhex(measurement['stackHead']), 20)[0],
                     'signedRemainder': signed(measurement['registers']['esi'])}
        compact(measurement)
        result = bounded_execute(chunks, entry, data,
                                 registers={'eax': OBJECT},
                                 arguments=arguments)
        if result['stop']['reason'] != 'returned' or result['stackDelta'] != 20:
            raise ValueError(f'original mask helper did not complete: {result["stop"]}')
        after = bytes.fromhex(result['memory']['mask']['after'])
        writes = ('0x004107ae', '0x004107e9') if values['operation'] == 'or' else ('0x004108f0', '0x0041092d')
        result.update(input=values, bitmap=bitmap.hex(), maskBefore=mask.hex(), maskAfter=after.hex(),
                      placement=placement, placementMeasurement=measurement,
                      writes=sum(result['instructionTrace'].count(site) for site in writes),
                      changedBits=sum((before ^ current).bit_count() for before, current in zip(mask, after)),
                      completedFunction=True, declaredCode=[f'{start:#x}:{stop:#x}' for start, stop in spans])
        return compact(result)

    def count(self, mask):
        if type(mask) is not bytes or len(mask) != MASK_BYTES:
            raise ValueError('exact complete native frozen-area mask required')
        result = bounded_execute([self.chunk(0x410960, 0x410a5c)], 0x410960,
                                 [datum('mask', MANAGER + MASK_OFFSET, mask, writable=False)],
                                 registers={'eax': MANAGER})
        if result['stop']['reason'] != 'returned' or result['stackDelta'] != 4:
            raise ValueError(f'complete native bit counter did not return: {result["stop"]}')
        result.update(mask=mask.hex(), count=result['registers']['eax'], completedFunction=True,
                      declaredCode=['0x410960:0x410a5c'])
        return compact(result)

    def attachment(self, selector):
        if type(selector) is not int or selector not in PROFILES:
            raise ValueError('known selector1..5 required for an attachment prefix')
        pointer, height, words = PROFILES[selector]
        source = self.constant(pointer, height * words * 4)
        object_bytes = bytearray(0x68)
        struct.pack_into('<3f', object_bytes, 0x10, 0, 200, .1)
        data = [datum('object', OBJECT, bytes(object_bytes), writable=False),
                datum('mask', MANAGER + MASK_OFFSET, bytes(MASK_BYTES)), source,
                datum('resource', MANAGER + 0x65ddc, 0x720000, kind='I', writable=False),
                datum('cpuFlag', 0x4dcd68, 1, writable=False), *self.constants_data()]
        start, end = {1: (0x40ed81, 0x40edb9), 2: (0x40edd1, 0x40ee0b),
                      3: (0x40ee1e, 0x40ee55), 4: (0x40ee55, 0x40eed3), 5: (0x40ee55, 0x40eed3)}[selector]
        if selector >= 4:
            boundary = self.constant(0x4a6b90, 4)
            value = struct.unpack('<f', boundary['bytes'])[0]
            data += [boundary, datum('bulletScalar', BULLET + 0xa00, struct.pack('<f', value if selector == 4 else value + 1000), writable=False)]
        spans = [(start, end), (0x4106e0, 0x410814), (0x48fac0, 0x48fadc)]
        result = bounded_execute([self.chunk(first, last) for first, last in spans], start, data,
                                 registers={'ebx': MANAGER, 'edi': OBJECT + 0x10, 'esi': OBJECT, 'ecx': BULLET})
        if result['stop'].get('target') != '0x00407ec0':
            raise ValueError(f'attachment must stop before the real effect call: {result["stop"]}')
        result.update(selector=selector, bitmap=source['bytes'].hex(), height=height, wordsPerRow=words,
                      maskAfter=result['memory']['mask']['after'], script=struct.unpack_from('<I', bytes.fromhex(result['stackHead']), 4)[0],
                      layer=result['registers']['eax'], objectSelector=struct.unpack_from('<I', bytes.fromhex(result['memory']['object']['after']), 0x58)[0],
                      completedCaller=False, declaredCode=[f'{first:#x}:{last:#x}' for first, last in spans])
        return compact(result)

    def removal(self, selector):
        if type(selector) is not int or not 0 <= selector <= 6:
            raise ValueError('bounded declared removal selector required')
        object_bytes = bytearray(0x68)
        struct.pack_into('<3f', object_bytes, 0x10, 0, 200, .1)
        struct.pack_into('<I', object_bytes, 0x58, selector)
        data = [datum('object', OBJECT, bytes(object_bytes), writable=False),
                datum('mask', MANAGER + MASK_OFFSET, pattern('one', MASK_BYTES)),
                datum('cpuFlag', 0x4dcd68, 1, writable=False), self.constant(0x40fc48, 20), *self.constants_data()]
        bitmap = bytes()
        if selector in PROFILES:
            pointer, height, words = PROFILES[selector]
            source = self.constant(pointer, height * words * 4)
            bitmap = source['bytes']
            data.append(source)
        spans = [(0x40f1b0, 0x40f1fe), (0x410820, 0x410958), (0x48fac0, 0x48fadc)]
        result = bounded_execute([self.chunk(first, last) for first, last in spans], 0x40f1b0, data,
                                 short=True, registers={'edi': OBJECT, 'ebx': MANAGER}, stops=[0x40f1fe])
        if result['stop']['reason'] != 'boundary':
            raise ValueError(f'removal prefix crossed its actual continuation: {result["stop"]}')
        result.update(selector=selector, bitmap=bitmap.hex(), maskAfter=result['memory']['mask']['after'],
                      objectSelector=struct.unpack_from('<I', bytes.fromhex(result['memory']['object']['after']), 0x58)[0],
                      completedCaller=False, declaredCode=[f'{first:#x}:{last:#x}' for first, last in spans])
        return compact(result)


def probe(executable):
    native = NativeIceMask(executable)
    counters, rasters = [], []
    specifications = [('zero-bitmap', {'bitmap_pattern': 'zero'}), ('one-bitmap', {}),
                      ('bit31-aligned', {'bitmap_pattern': 'bit31', 'position': (-176, .5)}),
                      ('bit0-spill', {'bitmap_pattern': 'bit0', 'position': (-175, .5)}),
                      ('left-clipped-spill', {'position': (-209, .5)}), ('left-aligned-offscreen', {'position': (-208, .5)}),
                      ('negative-signed-shift', {'words_per_row': 12, 'position': (-200, .5)}),
                      ('right-last-word', {'position': (191, .5)}), ('right-offscreen', {'position': (208, .5)}),
                      ('last-row', {'position': (-176, 448)}), ('below-last-row', {'position': (-176, 449)}),
                      ('negative-fraction-truncates-zero', {'position': (-176, -.25)}),
                      ('above-first-row', {'position': (-176, -.5)}),
                      ('several-rows', {'height': 3, 'words_per_row': 2, 'bitmap_pattern': 'random', 'position': (0, -1)}),
                      ('all-rows', {'height': 448, 'position': (-176, 224)}),
                      ('maximum-source-budget', {'height': 256, 'words_per_row': 8, 'bitmap_pattern': 'random', 'position': (0, 224)}),
                      ('zero-height', {'height': 0}), ('zero-width', {'words_per_row': 0}),
                      ('small-supported-coordinate', {'position': (-2**-15, 2**-15)}),
                      ('minimum-coordinate', {'position': (-2**-16, 2**-16)})]
    for exponent in (-32, -46, -80, -149):
        for sign in (-1, 1):
            specifications.append((f'tiny-x-{exponent}-{sign}',
                                   {'height': 49, 'words_per_row': 2, 'position': (sign * 2**exponent, 200)}))
    for exponent in (-46, -149):
        for sign in (-1, 1):
            specifications.append((f'tiny-y-{exponent}-{sign}',
                                   {'height': 2, 'position': (0, sign * 2**exponent)}))
    for name, midpoint, axis, sign in (('word-halfway', 2049 * 2**-57, 0, -1),
                                       ('left-halfway', 2049 * 2**-56, 0, -1),
                                       ('row-positive-halfway', 2049 * 2**-65, 1, 1),
                                       ('row-negative-halfway', 2049 * 2**-64, 1, -1)):
        midpoint_bits = struct.unpack('<I', struct.pack('<f', midpoint))[0]
        for offset in (-1, 0, 1):
            value = sign * struct.unpack('<f', struct.pack('<I', midpoint_bits + offset))[0]
            specifications.append((f'{name}-{offset}',
                                   {'height': 49 if axis == 0 else 2, 'words_per_row': 2 if axis == 0 else 1,
                                    'position': (value, 200) if axis == 0 else (0, value)}))
    try:
        for name in ('zero', 'one', 'bit0', 'bit31', 'last-bit', 'row-end', 'alternating', 'random'):
            sample = native.count(pattern(name, MASK_BYTES))
            sample['id'] = 'all-' + name if name in ('zero', 'one') else name
            counters.append(sample)
        for operation in ('or', 'and-not'):
            for name, changes in specifications:
                sample = native.raster(operation=operation, mask_pattern='zero' if operation == 'or' else 'one', **changes)
                sample['id'] = operation + '-' + name
                rasters.append(sample)
        attachments = [native.attachment(selector) for selector in PROFILES]
        removals = [native.removal(selector) for selector in range(7)]
        if len(counters) + len(rasters) + len(attachments) + len(removals) > MAX_CASES:
            raise ValueError('mask report case budget exceeded')
        for start, end in ((0x40ed81, 0x40eed3), (0x407f60, 0x407fa4), (0x466d60, 0x466ddc), (0x40e825, 0x40e83f)):
            native.chunk(start, end)
        digest = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        return {'schema': 'th128-original-byte-ice-mask-fixtures/1', 'nativeCpuTrace': False,
                'windowsGameProcessLaunched': False, 'executable': {'sha256': EXE_SHA256, 'sha256After': digest, 'unchanged': digest == EXE_SHA256},
                'layout': {'width': 384, 'height': 448, 'wordsPerRow': 12, 'maskBytes': MASK_BYTES, 'bits': 172032,
                           'managerOffset': MASK_OFFSET, 'endExclusive': 0x6b1e4, 'counterGroups': 0x700},
                'profiles': [{'selector': selector, 'pointer': address(pointer), 'height': height,
                              'wordsPerRow': words, 'bytes': height * words * 4,
                              'sha256': native.constants[address(pointer)]['sha256']}
                             for selector, (pointer, height, words) in PROFILES.items()],
                'code': native.code, 'constants': native.constants, 'counterCases': counters,
                'rasterCases': rasters, 'attachmentPrefixes': attachments, 'removalPrefixes': removals,
                'caseCount': len(counters) + len(rasters) + len(attachments) + len(removals),
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                'engineSha256': {name: hashlib.sha256((Path(__file__).parent / name).read_bytes()).hexdigest()
                                 for name in ('bullet_probe.py', 'freeze_probe.py')},
                'limits': {'coordinateMinNonzero': 2**-149, 'coordinateMax': 2**24,
                           'sourceWords': MAX_SOURCE_WORDS, 'maxInstructions': MAX_INSTRUCTIONS,
                           'codeBytes': 8192, 'dataBytes': 131072, 'cases': MAX_CASES},
                'limitations': ['Bounded original-byte emulation, not native Windows CPU or a Windows game process.',
                                'Counter and OR/AND-NOT helpers genuinely return; caller slices explicitly remain incomplete.',
                                'No effect/allocator/API return, selector store or animation handle is invented.',
                                'Immutable actual bitmap tables are copied from the pinned EXE into explicit readonly spans.',
                                'No area reward, propagation scheduler, 80-bucket coupling or full-game frame is ported here.']}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description='Bounded original-byte TH128 ice mask raster and complete bit counter')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/ice-mask-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    if not report['executable']['unchanged']:
        raise ValueError('read-only executable digest changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(f'{report["caseCount"]} bounded native mask/counter cases and explicit caller prefixes; no game process or stubs')


if __name__ == '__main__':
    main()
