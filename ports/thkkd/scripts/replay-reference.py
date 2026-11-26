"""从固定原程序采样读取对照流，按逻辑帧和字段定位差异。"""
from pathlib import PurePosixPath
import hashlib
import json
import struct

RECORD_SIZES = {'player': 240, 'enemies': 248, 'secondary': 192, 'bullets': 240,
                'items': 64, 'effects': 112, 'options': 560,
                'player_common_shots': 24, 'player_weapon_shots': 56}
PLAYER_FIELDS = {0: 'state', 0x10: 'score', 0x14: 'lives', 0x18: 'bombs',
                 0x24: 'continues', 0x90: 'x.low', 0x94: 'x.high', 0x98: 'y.low', 0x9c: 'y.high'}


def first_difference(expected, actual, frame, regions, stream_offset=0):
    if expected == actual:
        return None
    offset = next((i for i, (left, right) in enumerate(zip(expected, actual)) if left != right),
                  min(len(expected), len(actual)))
    field, address, local = 'frame', None, offset
    start = 4
    for region in regions:
        if start <= offset < start + region['bytes']:
            local = offset - start
            label = region['region']
            stride = RECORD_SIZES.get(label)
            index, member = divmod(local, stride) if stride else (None, local)
            word = member // 4 * 4
            field = f'{label}[{index}].word[0x{word:x}]' if stride and label != 'player' else f'{label}.word[0x{word:x}]'
            if label == 'player' and word in PLAYER_FIELDS:
                field = 'player.' + PLAYER_FIELDS[word]
            if region.get('address') is not None:
                address = hex(int(region['address'], 16) + local)
            break
        start += region['bytes']
    return {'frame': frame, 'field': field, 'address': address, 'byteOffset': offset,
            'streamOffset': stream_offset + offset, 'expectedByte': expected[offset] if offset < len(expected) else None,
            'actualByte': actual[offset] if offset < len(actual) else None,
            'expectedBytes': len(expected), 'actualBytes': len(actual)}


class NativeReference:
    def __init__(self, package, case, input_bytes):
        self.package = package
        self.case = case
        self.capture_bytes = package.read(case['capture'])
        self.capture = json.loads(self.capture_bytes)
        self.end, count = struct.unpack_from('<II', input_bytes)
        self.frames = struct.unpack_from(f'<{count}I', input_bytes, 8)
        if not self.frames or self.frames[0] != 0 or self.frames[-1] != self.end:
            raise ValueError('固定输入缺少起止采样帧')
        if any(a >= b for a, b in zip(self.frames, self.frames[1:])):
            raise ValueError('固定输入采样帧顺序错误')
        by_tick = {}
        for sample in self.capture['snapshots']:
            by_tick.setdefault(sample['tick'], []).append(sample)
        self.samples = []
        for frame in self.frames:
            matches = by_tick.get(frame, [])
            if not matches:
                raise ValueError(f'原程序采样缺少逻辑帧 {frame}')
            self.samples.extend(matches[-2:] if frame == self.end and case['mode'].endswith('-eof-pair') else matches[-1:])
        self.tail = self._tail(input_bytes)
        self.stream = None
        self.pack_name = None
        self.regions = 0

    def _tail(self, input_bytes):
        if self.case['mode'].endswith('-eof-pair'):
            final = self.capture['final_state']
            return struct.pack('<5I', 0x52464f45, self.end, self.capture['loaded']['input_bytes'],
                               final['replay_mode'], final['eof_pause'])
        if self.case['mode'].endswith('-return'):
            marker = struct.pack('<3I', 0x31545245, 1, 1)
            at = input_bytes.rfind(marker)
            if at < 0:
                raise ValueError('固定输入缺少 EX 返回尾部')
            count = struct.unpack_from('<I', input_bytes, at + 12)[0]
            if not 0 < count <= 0x100000 or at + 16 + count + 48 > len(input_bytes):
                raise ValueError('EX 返回尾部长度不匹配')
            buffer = bytearray(input_bytes[at + 16:at + 16 + count])
            values = struct.unpack_from('<12I', input_bytes, at + 16 + count)
            if values[0] != self.end:
                raise ValueError('EX 尾部逻辑帧与输入不匹配')
            if values[5] == 0:
                buffer[values[4]] = values[6] & 255
            result = (values[0], values[1], values[2], values[7], values[9], values[10], values[11], values[8], count)
            return struct.pack('<10I', 0x31524f45, *result) + buffer
        return b''

    def read_region(self, record):
        name = record.get('pack_path', record.get('path'))
        if not name or PurePosixPath(name).is_absolute() or '..' in PurePosixPath(name).parts:
            raise ValueError('采样引用路径无效')
        if name != self.pack_name:
            if self.stream:
                self.stream.close()
            self.stream = self.package.open(name)
            self.pack_name = name
        self.stream.seek(record.get('offset', 0))
        data = self.stream.read(record['bytes'])
        if len(data) != record['bytes'] or hashlib.sha256(data).hexdigest() != record['sha256']:
            raise ValueError('原采样区域校验失败：' + record['region'])
        self.regions += 1
        return data

    def chunks(self):
        try:
            for sample in self.samples:
                data = struct.pack('<I', sample['tick']) + b''.join(self.read_region(region) for region in sample['regions'])
                yield sample['tick'], sample['regions'], data
            if self.tail:
                names = ['tick', 'inputCount', 'mode', 'flags'] if self.case['mode'].endswith('-eof-pair') else [
                    'tick', 'gameFrame', 'entryECX', 'returnECX', 'cursor', 'mode', 'buttons', 'checksum', 'inputCount']
                regions = [{'region': 'replay-tail.' + name, 'bytes': 4} for name in names]
                if len(self.tail) > 4 + 4 * len(names):
                    regions.append({'region': 'replay-tail.inputs', 'bytes': len(self.tail) - 4 - 4 * len(names)})
                yield self.end, regions, self.tail
        finally:
            if self.stream:
                self.stream.close()
                self.stream = None

    def identity(self):
        fixture = self.capture['input_fixture']
        replay = self.package.read(fixture['fixture_path'])
        if hashlib.sha256(replay).hexdigest() != fixture['sha256']:
            raise ValueError('原录像文件校验失败')
        return {'exe': self.capture['exe_sha256'], 'replay': fixture['sha256'],
                'capture': hashlib.sha256(self.capture_bytes).hexdigest(),
                'resources': {name: value for name, value in self.capture['baseline']['input_sha256'].items() if name.endswith('.dxa')},
                'logicalSteps': self.end, 'snapshots': len(self.samples)}
