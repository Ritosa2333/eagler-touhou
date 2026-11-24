"""Bounded native TH128 replay codec; provenance: docs/research/replay.md."""

import operator
import struct
from collections.abc import Sequence

from .formats import FormatError, integer, require, span


MAX_COMPRESSED_SIZE = 8 * 1024 * 1024
MAX_UNCOMPRESSED_SIZE = 32 * 1024 * 1024
MAX_FILE_SIZE = 16 * 1024 * 1024
MAX_USER_DATA_SIZE = 4 * 1024 * 1024
MAX_FRAMES = 1000000
MAX_STAGES = 3

ROUTES = ('A1', 'A2', 'B1', 'B2', 'C1', 'C2', 'Extra')
RANKS = ('Easy', 'Normal', 'Hard', 'Lunatic', 'Extra')
STAGES = (None, 'A1-1', 'A1-2', 'A1-3', 'A2-2', 'A2-3',
          'B1-1', 'B1-2', 'B1-3', 'B2-2', 'B2-3',
          'C1-1', 'C1-2', 'C1-3', 'C2-2', 'C2-3', 'Extra',
          'A1 All', 'A2 All', 'B1 All', 'B2 All', 'C1 All', 'C2 All', 'Ex All')


def _raw(data, label, *, size=None, cap=MAX_FILE_SIZE):
    require(isinstance(data, (bytes, bytearray)), f'{label} must be bytes')
    require(len(data) <= cap, f'{label} exceeds size budget')
    require(size is None or len(data) == size, f'invalid {label} length')
    return bytes(data)


def _budget(value, ceiling, label):
    require(type(value) is int and 0 <= value <= ceiling, f'invalid {label} budget')
    return value


def _crypt(data, block_size, base_mask, mask_increment, *, encrypt=False):
    data = _raw(data, 'cipher data', cap=MAX_COMPRESSED_SIZE)
    require(type(block_size) is int and 4 <= block_size <= 0x800, 'invalid cipher block size')
    require(type(base_mask) is int and 0 <= base_mask <= 255, 'invalid cipher mask')
    require(type(mask_increment) is int and 0 <= mask_increment <= 255, 'invalid cipher increment')
    tail = len(data) % block_size
    count = len(data) - (tail if tail < block_size // 4 else 0) - (len(data) & 1)
    count = max(0, count)
    result = bytearray(data)
    cursor, mask = 0, base_mask
    while cursor < count:
        size = min(block_size, count - cursor)
        base = cursor
        for starting_index in (base + size - 1, base + size - 2):
            for target in range(starting_index, base - 1, -2):
                if encrypt:
                    result[cursor] = data[target] ^ mask
                else:
                    result[target] = data[cursor] ^ mask
                mask = (mask + mask_increment) & 255
                cursor += 1
    return bytes(result)


class _BitReader:
    def __init__(self, data):
        self.data = data
        self.position = 0

    @property
    def remaining(self):
        return len(self.data) * 8 - self.position

    def read(self, width):
        require(width <= self.remaining, 'truncated LZSS token')
        start = self.position // 8
        end = (self.position + width + 7) // 8
        shift = end * 8 - self.position - width
        value = (int.from_bytes(self.data[start:end], 'big') >> shift) & ((1 << width) - 1)
        self.position += width
        return value


def _decompress_lzss(data, expected_size, *, max_size=MAX_UNCOMPRESSED_SIZE):
    data = _raw(data, 'compressed data', cap=MAX_COMPRESSED_SIZE)
    _budget(max_size, MAX_UNCOMPRESSED_SIZE, 'uncompressed size')
    require(type(expected_size) is int and 0 <= expected_size <= max_size,
            'uncompressed size exceeds budget')
    require(bool(data) and expected_size <= len(data) * 8, 'impossible LZSS expansion size')
    reader = _BitReader(data)
    output = bytearray()
    while len(output) < expected_size:
        if reader.read(1):
            output.append(reader.read(8))
        else:
            index = reader.read(13)
            require(index != 0, 'LZSS terminated before declared size')
            length = reader.read(4) + 3
            require(length <= expected_size - len(output), 'LZSS expansion exceeds declared size')
            distance = ((len(output) - index) % 8192) + 1
            require(distance <= len(output), 'LZSS reference to uninitialized history')
            for _ in range(length):
                output.append(output[-distance])
    require(0 < reader.remaining <= 21, 'missing or excessive LZSS terminator data')
    require(reader.read(reader.remaining) == 0, 'nonzero LZSS terminator or padding')
    return bytes(output)


def _compress_literals(data, *, max_size=MAX_COMPRESSED_SIZE):
    data = _raw(data, 'uncompressed data', cap=MAX_UNCOMPRESSED_SIZE)
    _budget(max_size, MAX_COMPRESSED_SIZE, 'compressed size')
    require((len(data) * 9 + 9) // 8 <= max_size, 'literal encoding exceeds compressed size budget')
    result = bytearray()
    accumulator, available = 0, 0
    for value in data:
        accumulator = (accumulator << 9) | 0x100 | value
        available += 9
        while available >= 8:
            available -= 8
            result.append((accumulator >> available) & 255)
        accumulator &= (1 << available) - 1
    accumulator <<= 2
    available += 2
    while available >= 8:
        available -= 8
        result.append((accumulator >> available) & 255)
    if available:
        result.append((accumulator << (8 - available)) & 255)
    return bytes(result)


class ReplayInputs(Sequence):
    """Lazy (holding, pressed, released) uint16 tuples; no key bits are discarded."""

    def __init__(self, data):
        self.raw = _raw(data, 'input records', cap=MAX_FRAMES * 6)
        require(len(self.raw) % 6 == 0, 'partial replay input record')

    def __len__(self):
        return len(self.raw) // 6

    def __getitem__(self, index):
        if isinstance(index, slice):
            return [self[position] for position in range(*index.indices(len(self)))]
        position = operator.index(index)
        if position < 0:
            position += len(self)
        if not 0 <= position < len(self):
            raise IndexError('replay input index out of range')
        return struct.unpack_from('<3H', self.raw, position * 6)

    def __iter__(self):
        return struct.iter_unpack('<3H', self.raw)


def _label(labels, value):
    return labels[value] if 0 <= value < len(labels) else None


def _parse_payload(payload, max_frames):
    raw_metadata = span(payload, 0, 112)
    count = integer(raw_metadata, 88)
    require(1 <= count <= MAX_STAGES, 'unsupported replay stage count')
    route, rank = integer(raw_metadata, 92), integer(raw_metadata, 100)
    metadata = {
        'raw': raw_metadata, 'nameRaw': raw_metadata[:12],
        'name': raw_metadata[:8].split(b'\0', 1)[0].decode('cp932', errors='replace'),
        'timestamp': integer(raw_metadata, 12, '<Q'),
        'scoreRaw': integer(raw_metadata, 20), 'score': integer(raw_metadata, 20) * 10,
        'gameConfigRaw': raw_metadata[24:84], 'slowdown': integer(raw_metadata, 84, '<f'),
        'stageCount': count, 'route': route, 'routeName': _label(ROUTES, route),
        'rank': rank, 'rankName': _label(RANKS, rank), 'lastStage': integer(raw_metadata, 104),
    }
    stages, cursor, frames = [], 112, 0
    for _ in range(count):
        raw_header = span(payload, cursor, 144)
        num_frames, size = integer(raw_header, 4), integer(raw_header, 8)
        require(num_frames <= max_frames - frames, 'replay frame budget exceeded')
        require(num_frames * 6 <= size, 'stage payload is shorter than its frame records')
        frames += num_frames
        body = span(payload, cursor + 144, size)
        inputs = body[:num_frames * 6]
        stage_id = integer(raw_header, 0, '<H')
        position = struct.unpack_from('<ii', raw_header, 24)
        stages.append({
            'rawHeader': raw_header, 'stage': stage_id, 'stageName': _label(STAGES, stage_id),
            'seed': integer(raw_header, 2, '<H'), 'numFrames': num_frames, 'size': size,
            'scoreRaw': integer(raw_header, 12), 'score': integer(raw_header, 12) * 10,
            'shotPowerRaw': integer(raw_header, 16), 'shotPower': integer(raw_header, 16) + 1,
            'maxPivRaw': integer(raw_header, 20), 'maxPiv': integer(raw_header, 20) / 100,
            'positionRaw': position, 'position': tuple(value / 128 for value in position),
            'continues': integer(raw_header, 32), 'graze': integer(raw_header, 40),
            'motivationRaw': integer(raw_header, 128), 'motivation': integer(raw_header, 128) / 100,
            'perfectFreezeRaw': integer(raw_header, 132),
            'perfectFreeze': integer(raw_header, 132) / 100,
            'freezeArea': integer(raw_header, 136, '<f'),
            'inputRaw': inputs, 'inputs': ReplayInputs(inputs), 'fpsRaw': body[num_frames * 6:],
        })
        cursor += 144 + size
    return metadata, stages, payload[cursor:]


def decode_replay(data, *, max_compressed_size=MAX_COMPRESSED_SIZE,
                  max_uncompressed_size=MAX_UNCOMPRESSED_SIZE, max_frames=MAX_FRAMES):
    """Decode version-2 128r bytes into bounded metadata and lossless raw records."""
    _budget(max_compressed_size, MAX_COMPRESSED_SIZE, 'compressed size')
    _budget(max_uncompressed_size, MAX_UNCOMPRESSED_SIZE, 'uncompressed size')
    _budget(max_frames, MAX_FRAMES, 'frame count')
    data = _raw(data, 'replay file')
    header = span(data, 0, 36)
    magic, version, _, user_offset, game_version, _, _, compressed_size, size = struct.unpack('<4s8I', header)
    require(magic == b'128r', 'invalid TH128 replay magic')
    require(version == 2, f'unsupported TH128 replay version {version}')
    require(0 < compressed_size <= max_compressed_size, 'compressed size exceeds budget')
    require(112 <= size <= max_uncompressed_size, 'uncompressed size exceeds budget')
    encoded = span(data, 36, compressed_size)
    end = 36 + compressed_size
    user_blocks, gap = [], b''
    if user_offset:
        require(end <= user_offset <= len(data) - 24, 'invalid or overlapping USER offset')
        gap, cursor, user_size = data[end:user_offset], user_offset, 0
        for _ in range(2):
            user_header = span(data, cursor, 12)
            require(user_header[:4] == b'USER', 'invalid USER block magic')
            block_size = integer(user_header, 4)
            require(12 <= block_size <= MAX_USER_DATA_SIZE - user_size, 'invalid USER block size')
            block = span(data, cursor, block_size)
            user_blocks.append({'headerRaw': user_header, 'kind': integer(user_header, 8),
                                'data': block[12:], 'raw': block,
                                'text': block[12:].decode('cp932', errors='replace')})
            user_size += block_size
            cursor += block_size
        file_tail = data[cursor:]
    else:
        file_tail = data[end:]
    decrypted = _crypt(_crypt(encoded, 0x800, 0x5E, 0xE7), 0x80, 0x7D, 0x36)
    payload = _decompress_lzss(decrypted, size, max_size=max_uncompressed_size)
    metadata, stages, payload_tail = _parse_payload(payload, max_frames)
    return {
        'format': 'th128-replay/1',
        'header': {'raw': header, 'replayVersion': version, 'gameVersion': game_version,
                   'compressedSize': compressed_size, 'uncompressedSize': size,
                   'userDataOffset': user_offset},
        'metadata': metadata, 'stages': stages, 'payloadTailRaw': payload_tail,
        'encodedPayload': encoded, 'rawPayload': payload, 'dataGapRaw': gap,
        'userBlocks': user_blocks, 'fileTailRaw': file_tail,
    }


def encode_replay(decoded, *, max_compressed_size=MAX_COMPRESSED_SIZE,
                  max_uncompressed_size=MAX_UNCOMPRESSED_SIZE, max_frames=MAX_FRAMES):
    """Encode authoritative raw components; parsed scalar views are not edit fields."""
    _budget(max_compressed_size, MAX_COMPRESSED_SIZE, 'compressed size')
    _budget(max_uncompressed_size, MAX_UNCOMPRESSED_SIZE, 'uncompressed size')
    _budget(max_frames, MAX_FRAMES, 'frame count')
    require(isinstance(decoded, dict), 'decoded replay must be a mapping')
    header_fields, metadata_fields = decoded.get('header'), decoded.get('metadata')
    require(isinstance(header_fields, dict) and isinstance(metadata_fields, dict), 'missing raw replay headers')
    header = bytearray(_raw(header_fields.get('raw'), 'replay header', size=36))
    metadata = bytearray(_raw(metadata_fields.get('raw'), 'replay metadata', size=112))
    stages = decoded.get('stages')
    require(isinstance(stages, (list, tuple)) and 1 <= len(stages) <= MAX_STAGES,
            'unsupported replay stage count')
    struct.pack_into('<I', metadata, 88, len(stages))
    parts, total_size, frames = [bytes(metadata)], 112, 0
    for stage in stages:
        require(isinstance(stage, dict), 'invalid stage mapping')
        stage_header = bytearray(_raw(stage.get('rawHeader'), 'stage header', size=144))
        inputs = _raw(stage.get('inputRaw'), 'input records', cap=MAX_FRAMES * 6)
        fps = _raw(stage.get('fpsRaw'), 'FPS data', cap=max_uncompressed_size)
        require(len(inputs) % 6 == 0, 'partial replay input record')
        num_frames, size = len(inputs) // 6, len(inputs) + len(fps)
        require(num_frames <= max_frames - frames, 'replay frame budget exceeded')
        frames += num_frames
        total_size += 144 + size
        require(total_size <= max_uncompressed_size, 'uncompressed size exceeds budget')
        struct.pack_into('<II', stage_header, 4, num_frames, size)
        parts.extend((bytes(stage_header), inputs, fps))
    tail = _raw(decoded.get('payloadTailRaw'), 'payload tail', cap=max_uncompressed_size)
    require(len(tail) <= max_uncompressed_size - total_size, 'uncompressed size exceeds budget')
    parts.append(tail)
    payload = b''.join(parts)
    encoded = _raw(decoded.get('encodedPayload', b''), 'cached ciphertext', cap=MAX_COMPRESSED_SIZE)
    if encoded:
        try:
            cached = _decompress_lzss(_crypt(_crypt(encoded, 0x800, 0x5E, 0xE7),
                                           0x80, 0x7D, 0x36), len(payload),
                                      max_size=max_uncompressed_size)
        except FormatError:
            encoded = b''
        else:
            if cached != payload:
                encoded = b''
    if not encoded:
        compressed = _compress_literals(payload, max_size=max_compressed_size)
        encoded = _crypt(_crypt(compressed, 0x80, 0x7D, 0x36, encrypt=True),
                         0x800, 0x5E, 0xE7, encrypt=True)
    require(len(encoded) <= max_compressed_size, 'compressed size exceeds budget')
    gap = _raw(decoded.get('dataGapRaw'), 'container gap')
    file_tail = _raw(decoded.get('fileTailRaw'), 'file tail')
    user_blocks = decoded.get('userBlocks')
    require(isinstance(user_blocks, (list, tuple)) and len(user_blocks) in (0, 2),
            'expected zero or two USER blocks')
    require(bool(user_blocks) or not gap, 'container gap needs a USER section')
    user_parts, user_size = [], 0
    for block in user_blocks:
        require(isinstance(block, dict), 'invalid USER block mapping')
        user_header = bytearray(_raw(block.get('headerRaw'), 'USER header', size=12))
        require(user_header[:4] == b'USER', 'invalid USER block magic')
        content = _raw(block.get('data'), 'USER data', cap=MAX_USER_DATA_SIZE)
        user_size += 12 + len(content)
        require(user_size <= MAX_USER_DATA_SIZE, 'USER data exceeds size budget')
        struct.pack_into('<I', user_header, 4, len(content) + 12)
        user_parts.extend((bytes(user_header), content))
    user_offset = 36 + len(encoded) + len(gap) if user_blocks else 0
    require(36 + len(encoded) + len(gap) + user_size + len(file_tail) <= MAX_FILE_SIZE,
            'replay file exceeds size budget')
    struct.pack_into('<I', header, 12, user_offset)
    struct.pack_into('<II', header, 28, len(encoded), len(payload))
    output = b''.join((bytes(header), encoded, gap, *user_parts, file_tail))
    decode_replay(output, max_compressed_size=max_compressed_size,
                  max_uncompressed_size=max_uncompressed_size, max_frames=max_frames)
    return output
