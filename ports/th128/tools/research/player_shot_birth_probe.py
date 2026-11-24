import argparse
import hashlib
import json
import math
from pathlib import Path
import struct
import sys

from bullet_probe import NativeBullet, ROOT, EXE_SHA256, address, datum
from freeze_probe import emulate


PLAYER, SHOT, RESOURCE = 0x300000, 0x700000, 0x390000
POOL_OFFSET, SLOT_COUNT, OBJECT_BYTES = 0xfa8, 256, 0x78
SHT_SHA256 = 'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b'
TABLES = (0x4b1cd8, 0x4b1a14, 0x4d2e2c, 0x4b1cf8)


def report_destination(path):
    destination = Path(path).resolve()
    allowed = ROOT / 'local/research'
    if (allowed.resolve() != allowed or destination.parent != allowed or destination.suffix != '.json'
            or not destination.name.startswith('player-shot-birth') or destination.drive.lower() == 'e:'):
        raise ValueError('report must be private local/research/player-shot-birth*.json')
    return destination


def floating(value):
    if type(value) not in (int, float) or abs(value) > 2**20 or not math.isfinite(value):
        raise ValueError('bounded finite float32 value required')
    return struct.unpack('<f', struct.pack('<f', value))[0]


def position_input(position):
    if not isinstance(position, (tuple, list)) or len(position) != 3:
        raise ValueError('exact XYZ required')
    return [floating(value) for value in position]


def actual_resources():
    local = (ROOT / 'local').resolve()
    catalog = json.loads((local / 'catalog.json').read_bytes())
    manifest = (local / catalog['variants']['ja']['manifest']).resolve()
    source = (manifest.parent / 'raw/pl00a.sht').resolve()
    animation = (manifest.parent / 'raw/pl00.anm').resolve()
    if (not manifest.is_relative_to(local) or not source.is_relative_to(local)
            or not animation.is_relative_to(local) or not 0 < source.stat().st_size <= 4 * 1024 * 1024):
        raise ValueError('actual Japanese SHT/ANM must remain inside the current local generation')
    raw = source.read_bytes()
    if hashlib.sha256(raw).hexdigest() != SHT_SHA256:
        raise ValueError('first-shot evidence requires the pinned actual Japanese pl00a.sht')
    sys.path.insert(0, str(ROOT / 'tools'))
    from th128_assets.shot import decode_shot
    return raw, decode_shot(raw), {'resource': 'ja/raw/pl00a.sht', 'sha256': SHT_SHA256,
                                 'animation': 'ja/raw/pl00.anm',
                                 'animationSha256': hashlib.sha256(animation.read_bytes()).hexdigest()}


def execute(chunks, entry, data, **options):
    if (len(chunks) > 16 or sum(len(code) for _, code in chunks) > 8192 or len(data) > 32
            or sum(len(region['bytes']) for region in data) > 131072):
        raise ValueError('player-shot explicit code/data budget exceeded')
    return emulate(chunks, entry, data, budget=20000, **options)


def compact(result):
    result['declaredData'] = [{'name': name, 'address': item['address'], 'bytes': item['bytes'],
                              'writable': item['writable'],
                              'beforeSha256': hashlib.sha256(bytes.fromhex(item['before'])).hexdigest(),
                              'afterSha256': hashlib.sha256(bytes.fromhex(item['after'])).hexdigest()}
                             for name, item in result['memory'].items()]
    result.pop('memory')
    return result


def semantic(block):
    signed = lambda offset: struct.unpack_from('<i', block, offset)[0]
    words = list(struct.unpack('<30I', block))
    return {'timer': [signed(0), signed(4), words[2], words[3], words[4]],
            'positionBits': words[5:8], 'velocityBits': words[8:11],
            'speedBits': words[11], 'angleBits': words[12], 'flags44': words[17],
            'occupied': signed(0x48), 'damage': signed(0x60), 'sizeBits': words[26:28],
            'sourceRecordPointer': words[29]}


class NativePlayerShotBirth(NativeBullet):
    def scheduler(self, raw, frozen_area, difficulty, shot_count):
        frozen_area = floating(frozen_area)
        if (frozen_area < 0 or type(difficulty) is not int or not 0 <= difficulty <= 3
                or type(shot_count) is not int or not 0 <= shot_count <= 14):
            raise ValueError('bounded actual scheduler input required')
        relocated = bytearray(raw)
        count = struct.unpack_from('<H', relocated, 2)[0]
        for index in range(count):
            location = 0x268 + index * 8
            struct.pack_into('<I', relocated, location, SHOT + struct.unpack_from('<I', relocated, location)[0])
        data = [datum('sht', SHOT, bytes(relocated), writable=False),
                datum('playerSht', PLAYER + 0xf7c, SHOT, kind='I', writable=False),
                datum('area', 0x4b4d60, struct.pack('<f', frozen_area), writable=False),
                datum('difficulty', 0x4b4d0c, difficulty, writable=False), self.constant(0x4a28c0, 120)]
        result = execute([self.chunk(0x43f160, 0x43f1ac), self.chunk(0x4271c0, 0x427257)], 0x43f160,
                         data, registers={'edi': PLAYER, 'ebx': shot_count})
        if result['stop']['reason'] == 'returned':
            first = None
        elif result['stop'].get('target') == '0x0043ed60':
            first = result['registers']['esi'] - SHOT
        else:
            raise ValueError('actual scheduler crossed an unsupported boundary')
        result.update(frozenAreaBits=struct.unpack('<I', struct.pack('<f', frozen_area))[0], difficulty=difficulty,
                      shotCount=shot_count, firstRecordOffset=first, completedEmitter=False)
        return compact(result)

    def callback_table_first(self, pointer):
        relative = pointer - 0x400000
        for section in self.image.sections:
            if (section.VirtualAddress + section.SizeOfRawData <= relative
                    and relative + 4 <= section.VirtualAddress + section.Misc_VirtualSize):
                payload = bytes(4)
                self.constants[address(pointer)] = {'bytes': payload.hex(), 'sha256': hashlib.sha256(payload).hexdigest(),
                                                    'origin': 'declared PE virtual-section zero-fill, not a runtime callback result'}
                return
        self.constant(pointer, 4)

    def group(self, frozen_area, difficulty):
        frozen_area = floating(frozen_area)
        if type(difficulty) is not int or not 0 <= difficulty <= 3 or frozen_area < 0:
            raise ValueError('finite nonnegative cumulative area and difficulty0..3 required')
        data = [datum('area', 0x4b4d60, struct.pack('<f', frozen_area), writable=False),
                datum('difficulty', 0x4b4d0c, difficulty, writable=False), self.constant(0x4a28c0, 120)]
        result = execute([self.chunk(0x4271c0, 0x427257)], 0x4271c0, data, registers={'esi': 0x4b4cc0})
        if result['stop']['reason'] != 'returned' or not 0 <= result['registers']['eax'] <= 15:
            raise ValueError('actual group helper did not return a supported index')
        result.update(frozenAreaBits=struct.unpack('<I', struct.pack('<f', frozen_area))[0], difficulty=difficulty,
                      group=result['registers']['eax'], completedFunction=True)
        return compact(result)

    def birth(self, raw, record_offset, *, position=(0, 359, .1), occupied_run=0, timer_flags=0,
              motion=False, shot_count=0, initial_slot=None):
        position = position_input(position)
        if (type(record_offset) is not int or not 0 <= record_offset <= len(raw) - 52
                or type(occupied_run) is not int or not 0 <= occupied_run <= SLOT_COUNT
                or type(timer_flags) is not int or not 0 <= timer_flags < 2**32
                or type(motion) is not bool or type(shot_count) is not int or not 0 <= shot_count <= 14):
            raise ValueError('bounded declared SHT record/pool/counter required')
        record = raw[record_offset:record_offset + 52]
        if struct.unpack_from('<b', record, 28)[0] != 0 or record[29] != 0:
            raise ValueError('only actual owner0/kind0 first-shot body is supported')
        if initial_slot is not None and (type(initial_slot) is not bytes or len(initial_slot) != OBJECT_BYTES
                or struct.unpack_from('<I', initial_slot, 0x48)[0] != 0):
            raise ValueError('exact inactive previous slot bytes are required')
        pool = bytearray(SLOT_COUNT * OBJECT_BYTES)
        for index in range(occupied_run):
            struct.pack_into('<I', pool, index * OBJECT_BYTES + 0x48, 1)
        if occupied_run < SLOT_COUNT:
            if initial_slot is not None:
                pool[occupied_run * OBJECT_BYTES:(occupied_run + 1) * OBJECT_BYTES] = initial_slot
                timer_flags = struct.unpack_from('<I', initial_slot, 0x10)[0]
            else:
                struct.pack_into('<I', pool, occupied_run * OBJECT_BYTES + 0x10, timer_flags)
                struct.pack_into('<I', pool, occupied_run * OBJECT_BYTES + 0xc, 0xabcdef01)
        data = [datum('pool', PLAYER + POOL_OFFSET, bytes(pool)),
                datum('position', PLAYER + 0xecc, struct.pack('<3f', *position), writable=False),
                datum('record', SHOT + record_offset, record, writable=False),
                datum('playerPointer', 0x4b8a80, PLAYER, kind='I', writable=False),
                datum('playerResource', PLAYER + 0x10, RESOURCE, kind='I', writable=False),
                datum('allocationCounter', RESOURCE + 0x130, 0),
                datum('clock', 0x4b6f58, struct.pack('<f', 1), writable=False),
                datum('debugFlags', 0x4d33f8, 0, writable=False)]
        constants = ((0x4a6ab0, 8), (0x4a6580, 8), (0x4a6578, 8), (0x4a65b8, 8)) if motion else ((0x4a6ab0, 8),)
        for pointer, size in constants:
            data.append(self.constant(pointer, size))
        spans = [(0x43ed60, 0x43f15f)]
        if motion:
            spans += [(0x46a700, 0x46a78e), (0x46ae00, 0x46ae71), (0x46c1f0, 0x46c210)]
        result = execute([self.chunk(first, last) for first, last in spans], 0x43ed60, data,
                         registers={'ecx': PLAYER + 0xecc},
                         arguments=struct.pack('<3I', PLAYER, SHOT + record_offset, shot_count))
        expected_target = '0x00468010' if motion else '0x0046a700'
        if occupied_run == SLOT_COUNT:
            if result['stop']['reason'] != 'returned' or result['registers']['eax'] != 0:
                raise ValueError('native full shot pool did not return without constructing a shot')
        elif result['stop'].get('target') != expected_target:
            raise ValueError(f'actual first-shot prefix crossed its declared unknown-call boundary: {result["stop"]}')
        block = bytes.fromhex(result['memory']['pool']['after'])
        selected = occupied_run if occupied_run < SLOT_COUNT else None
        if selected is not None:
            image = block[selected * OBJECT_BYTES:(selected + 1) * OBJECT_BYTES]
            before = bytes(pool[selected * OBJECT_BYTES:(selected + 1) * OBJECT_BYTES])
            result.update(slot=selected, next=semantic(image), slotBefore=before.hex(), slotAfter=image.hex())
        result.update(recordOffset=record_offset, rawRecord=record.hex(), positionBits=list(struct.unpack('<3I', struct.pack('<3f', *position))),
                      occupiedRun=occupied_run, timerFlags=timer_flags, shotCount=shot_count, clockBits=0x3f800000,
                      motionExecuted=motion, recycledInput=initial_slot is not None,
                      completedBirth=occupied_run == SLOT_COUNT, examined=min(occupied_run + 1, SLOT_COUNT),
                      requiredRuntimeCallbacks='all four file IDs require genuine live table resolution; no relocated zero assumed',
                      declaredCode=[f'{first:#x}:{last:#x}' for first, last in spans])
        return compact(result)


def probe(executable):
    raw, decoded, resource = actual_resources()
    native = NativePlayerShotBirth(executable)
    groups, births, motions, schedulers, recycled = [], [], [], [], []
    record_metadata = []
    try:
        for difficulty in (0, 1, 2, 3):
            for area in (0, 49.999996185302734, 50, 99.99999237060547, 100, 349.9999694824219, 350, 2000, 3000):
                groups.append(native.group(area, difficulty))
        for difficulty in (0, 1):
            for count in (0, 1, 2, 3, 5, 6, 14):
                schedulers.append(native.scheduler(raw, 0, difficulty, count))
        for group_index, group in enumerate(decoded['groups']):
            records = []
            for index, record in enumerate(group['records']):
                sample = native.birth(raw, record['offset'])
                sample.update(group=group_index, record=index, id=f'group{group_index}-record{index}')
                births.append(sample)
                motion = native.birth(raw, record['offset'], motion=True)
                motion.update(group=group_index, record=index, id=f'group{group_index}-record{index}-motion')
                motions.append(motion)
                records.append({'offset': record['offset'], 'bytes': record['raw'].hex(), 'callbackIds': record['callbackIds']})
            record_metadata.append({'offset': group['offset'], 'unknownWord': group['unknownWord'], 'records': records})
        first = decoded['groups'][0]['records'][0]['offset']
        for name, changes in (('skip-first', {'occupied_run': 1}), ('last-free', {'occupied_run': 255}),
                              ('full-pool', {'occupied_run': 256}), ('preserved-timer-flags', {'timer_flags': 0x80000001}),
                              ('nonzero-position', {'position': (-128.00001525878906, 200.00001525878906, -0.0)})):
            sample = native.birth(raw, first, **changes)
            sample.update(group=0, record=0, id=name)
            births.append(sample)
        for pointer in TABLES:
            native.callback_table_first(pointer)
        for flags in (0, 1, 0x80000001):
            previous = bytearray([0xa5] * OBJECT_BYTES)
            previous[0x44] &= 0xfe
            struct.pack_into('<I', previous, 0x48, 0)
            struct.pack_into('<I', previous, 0x10, flags)
            struct.pack_into('<I', previous, 0xc, 0x4b6f58)
            struct.pack_into('<if', previous, 4, 37, 37.5)
            for motion in (False, True):
                sample = native.birth(raw, first, initial_slot=bytes(previous), motion=motion)
                sample.update(group=0, record=0, id=f'recycled-{flags}-{motion}')
                recycled.append(sample)
        for start, end in ((0x43f160, 0x43f1ac), (0x43f1b0, 0x43f264),
                           (0x43f760, 0x43f7b0), (0x43c298, 0x43c322),
                           (0x43efeb, 0x43f045), (0x43f0e3, 0x43f15f), (0x466d60, 0x466ddc)):
            native.chunk(start, end)
        digest = hashlib.sha256(native.executable.read_bytes()).hexdigest()
        if len(groups) + len(births) + len(motions) + len(schedulers) > 512:
            raise ValueError('player-shot native case budget exceeded')
        resource.update(groupCount=decoded['groupCount'], recordCount=decoded['recordCount'], groups=record_metadata,
                        rawSha256=hashlib.sha256(raw).hexdigest(), bytes=len(raw))
        return {'schema': 'th128-original-byte-player-shot-birth-fixtures/1', 'nativeCpuTrace': False,
                'windowsGameProcessLaunched': False, 'executable': {'sha256': EXE_SHA256, 'sha256After': digest, 'unchanged': digest == EXE_SHA256},
                'resource': resource, 'layout': {'slotCount': SLOT_COUNT, 'objectBytes': OBJECT_BYTES, 'poolOffset': POOL_OFFSET,
                                                'occupancyOffset': 0x48, 'recordBytes': 52, 'recordTableOffset': 0x268},
                'code': native.code, 'constants': native.constants, 'groupCases': groups, 'birthCases': births, 'motionCases': motions,
                'schedulerCases': schedulers, 'recycledCases': recycled,
                'callbackTables': [{'address': address(pointer), 'state': 'PE initial bytes or declared BSS zero-fill, not live runtime',
                                    'firstWord': native.constants[address(pointer)]['bytes']}
                                   for pointer in TABLES],
                'generatorSha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                'engineSha256': {name: hashlib.sha256((Path(__file__).parent / name).read_bytes()).hexdigest()
                                 for name in ('bullet_probe.py', 'freeze_probe.py')},
                'limits': {'codeBytes': 8192, 'dataBytes': 131072, 'instructions': 20000, 'maxCases': 512},
                'limitations': ['Bounded original-byte emulation, not native Windows game execution.',
                                'All actual records are owner0/kind0; option origins, special kinds and runtime callbacks remain unsupported.',
                                'Motion cases execute actual normalizer/FSINCOS helpers; CPP motion results must be supplied by a genuine host, not invented.',
                                'ANM allocator/bind/registration and callback/sound continuations are never given fabricated returns.',
                                'Group selection uses cumulative frozen-area and native difficulty, not assumed power/focus group aliases.']}
    finally:
        native.image.close()


def main(argv=None):
    parser = argparse.ArgumentParser(description='Actual TH128 SHT first-shot birth prefixes and group selection')
    parser.add_argument('--exe', required=True)
    parser.add_argument('--output', default=str(ROOT / 'local/research/player-shot-birth-fixtures.json'))
    arguments = parser.parse_args(argv)
    destination = report_destination(arguments.output)
    report = probe(arguments.exe)
    if not report['executable']['unchanged']:
        raise ValueError('original read-only executable digest changed')
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8', newline='\n')
    print(f'{len(report["birthCases"])} birth prefixes/returns, {len(report["motionCases"])} actual motion-to-allocator prefixes; no game process')


if __name__ == '__main__':
    main()
