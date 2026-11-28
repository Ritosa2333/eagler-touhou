import hashlib
import json
import struct
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))

from th128_assets.formats import FormatError
from th128_assets.replay import (
    MAX_COMPRESSED_SIZE, MAX_FILE_SIZE, MAX_FRAMES, MAX_UNCOMPRESSED_SIZE,
    MAX_USER_DATA_SIZE, ReplayInputs,
    _compress_literals, _crypt, _decompress_lzss, decode_replay, encode_replay,
)


def bitstream(bits):
    padded = bits + '0' * (-len(bits) % 8)
    return int(padded, 2).to_bytes(len(padded) // 8, 'big') if padded else b''


def literals(data):
    return bitstream(''.join('1' + format(value, '08b') for value in data) + '00')


def fixture_encrypt(data, block_size, mask, increment):
    result = bytearray(data)
    remaining = len(data) % block_size
    count = max(0, len(data) - (remaining if remaining < block_size // 4 else 0)
                - (len(data) & 1))
    cursor = 0
    while cursor < count:
        size = min(block_size, count - cursor)
        order = list(range(size - 1, -1, -2)) + list(range(size - 2, -1, -2))
        for source in order:
            result[cursor] = data[cursor - (cursor % block_size) + source] ^ mask
            mask = (mask + increment) & 255
            cursor += 1
    return bytes(result)


def fixture_payload():
    metadata = bytearray((index * 37 + 9) & 255 for index in range(112))
    struct.pack_into('<12sQI', metadata, 0, b'Tester\0\x81\xffTAIL', 1281268800, 1234)
    struct.pack_into('<fIIIIII', metadata, 84, 1.25, 1, 5, 0x12345678, 2, 15, 0x89ABCDEF)
    stage = bytearray((index * 13 + 7) & 255 for index in range(144))
    struct.pack_into('<HHIIIIIiiII', stage, 0, 15, 0x1234, 2, 14, 10, 1,
                     1000000, -128, 256, 1, 0xCAFEBABE)
    struct.pack_into('<I', stage, 40, 99)
    struct.pack_into('<IIf', stage, 128, 5000, 10000, 12.5)
    inputs = struct.pack('<6H', 0xFFFF, 0xFFFF, 0xFFFF, 0x0201, 0x0200, 0)
    return bytes(metadata + stage) + inputs + b'\x3c\x3b' + b'unknown-payload'


def replay_fixture(payload=None, *, compressed=None, user=True, gap=b'gap', tail=b'opaque-tail'):
    payload = fixture_payload() if payload is None else payload
    compressed = literals(payload) if compressed is None else compressed
    encrypted = fixture_encrypt(fixture_encrypt(compressed, 0x80, 0x7D, 0x36),
                                0x800, 0x5E, 0xE7)
    blocks = b'USER' + struct.pack('<Q', 15) + b'\x82\xa0\0'
    blocks += b'USER' + struct.pack('<II', 14, 1) + b'\xff\0'
    offset = 36 + len(encrypted) + len(gap) if user else 0
    header = struct.pack('<4s8I', b'128r', 2, 0xDEADBEEF, offset, 0x100,
                         0x12345678, 0x89ABCDEF, len(encrypted), len(payload))
    return header + encrypted + (gap + blocks if user else b'') + tail


class CipherTests(unittest.TestCase):
    def test_known_interleaving_and_mask_vector(self):
        self.assertEqual(_crypt(bytes(range(16)), 8, 0x10, 1), b'\x10' * 16)
        self.assertEqual(_crypt(b'\x10' * 16, 8, 0x10, 1, encrypt=True), bytes(range(16)))

    def test_odd_ignored_tail_keeps_reference_mask_order(self):
        self.assertEqual(_crypt(bytes(range(17)), 8, 0x10, 1), b'\x10' * 15 + b'\x0f\x10')
        self.assertEqual(_crypt(bytes(range(19)), 8, 0x10, 1), b'\x10' * 16 + b'00\x12')

    def test_short_blocks_do_not_underflow_or_modify_ignored_data(self):
        for size in (0, 1, 2, 3, 31, 127):
            data = bytes(range(size))
            self.assertEqual(_crypt(data, 0x800, 0x5E, 0xE7), data)

    def test_both_cipher_passes_at_boundary_lengths(self):
        for size in (32, 33, 127, 128, 129, 159, 160, 511, 512, 513,
                     2047, 2048, 2049, 6191, 6228):
            with self.subTest(size=size):
                data = bytes((index * 83 + 11) & 255 for index in range(size))
                encrypted = _crypt(_crypt(data, 0x80, 0x7D, 0x36, encrypt=True),
                                   0x800, 0x5E, 0xE7, encrypt=True)
                self.assertEqual(encrypted, fixture_encrypt(
                    fixture_encrypt(data, 0x80, 0x7D, 0x36), 0x800, 0x5E, 0xE7))
                self.assertEqual(_crypt(_crypt(encrypted, 0x800, 0x5E, 0xE7),
                                        0x80, 0x7D, 0x36), data)


class LzssTests(unittest.TestCase):
    def test_known_literal_and_overlapping_reference(self):
        self.assertEqual(_decompress_lzss(bytes.fromhex('a080'), 1), b'A')
        stream = bitstream('101000001' + '0' + format(1, '013b') + '1111' + '00')
        self.assertEqual(_decompress_lzss(stream, 19), b'A' * 19)

    def test_window_wrap_uses_initialized_absolute_ring_slots(self):
        prefix = bytes(index & 255 for index in range(8193))
        for index, suffix in ((1, b'\0\0\0'), (8191, b'\xfe\xff\0')):
            stream = bitstream(''.join('1' + format(value, '08b') for value in prefix)
                               + '0' + format(index, '013b') + '0000' + '00')
            self.assertEqual(_decompress_lzss(stream, 8196), prefix + suffix)

    def test_rejects_uninitialized_history(self):
        for prefix, index in (('', 1), ('101000001', 2), ('101000001', 8191)):
            stream = bitstream(prefix + '0' + format(index, '013b') + '0000' + '00')
            with self.subTest(index=index), self.assertRaises(FormatError):
                _decompress_lzss(stream, 4)

    def test_rejects_expansion_past_declared_size(self):
        stream = bitstream('101000001' + '0' + format(1, '013b') + '1111' + '00')
        with self.assertRaises(FormatError):
            _decompress_lzss(stream, 2)

    def test_rejects_truncated_tokens_and_early_termination(self):
        for stream, expected in ((b'', 1), (b'\x80', 1), (b'\0', 1),
                                 (b'\xA0\x80', 2), (b'\xA0\x80\x04', 4)):
            with self.subTest(stream=stream), self.assertRaises(FormatError):
                _decompress_lzss(stream, expected)

    def test_accepts_full_and_truncated_zero_terminators(self):
        self.assertEqual(_decompress_lzss(b'\0', 0), b'')
        self.assertEqual(_decompress_lzss(bytes.fromhex('a08000'), 1), b'A')

    def test_rejects_nonzero_or_excess_trailing_compressed_data(self):
        for stream in (bytes.fromhex('a08001'), bytes.fromhex('a080000000')):
            with self.assertRaises(FormatError):
                _decompress_lzss(stream, 1)

    def test_rejects_sizes_before_expansion(self):
        for expected in (-1, MAX_UNCOMPRESSED_SIZE + 1, 1000):
            with self.assertRaises(FormatError):
                _decompress_lzss(b'\0', expected)

    def test_literal_encoder_has_independent_known_bytes(self):
        self.assertEqual(_compress_literals(b'A'), bytes.fromhex('a080'))
        self.assertEqual(_compress_literals(b''), b'\0')
        for data in (b'', b'A', bytes(range(256))):
            self.assertEqual(_compress_literals(data), literals(data))
            self.assertEqual(_decompress_lzss(_compress_literals(data), len(data)), data)

    def test_literal_encoder_rejects_output_budget_before_emitting(self):
        with self.assertRaises(FormatError):
            _compress_literals(b'abc', max_size=1)

    def test_lazy_input_storage_rejects_partial_or_over_budget_records(self):
        for data in (b'bad', bytes(MAX_FRAMES * 6 + 1)):
            with self.assertRaises(FormatError):
                ReplayInputs(data)


class ReplayTests(unittest.TestCase):
    def test_metadata_and_raw_unknown_fields(self):
        decoded = decode_replay(replay_fixture())
        self.assertEqual(decoded['metadata']['name'], 'Tester')
        self.assertEqual(decoded['metadata']['timestamp'], 1281268800)
        self.assertEqual(decoded['metadata']['score'], 12340)
        self.assertEqual(decoded['metadata']['routeName'], 'C2')
        self.assertEqual(decoded['metadata']['rankName'], 'Hard')
        self.assertEqual(decoded['metadata']['raw'], fixture_payload()[:112])
        self.assertEqual(decoded['rawPayload'], fixture_payload())
        self.assertEqual(decoded['payloadTailRaw'], b'unknown-payload')
        self.assertEqual(decoded['dataGapRaw'], b'gap')
        self.assertEqual(decoded['fileTailRaw'], b'opaque-tail')
        self.assertEqual(decoded['userBlocks'][0]['data'], b'\x82\xa0\0')
        self.assertEqual(decoded['userBlocks'][0]['text'], '\u3042\0')

    def test_stage_and_lazy_input_records(self):
        stage = decode_replay(replay_fixture())['stages'][0]
        self.assertEqual(stage['stage'], 15)
        self.assertEqual(stage['stageName'], 'C2-3')
        self.assertEqual(stage['seed'], 0x1234)
        self.assertEqual(stage['numFrames'], 2)
        self.assertEqual(stage['positionRaw'], (-128, 256))
        self.assertEqual(stage['graze'], 99)
        self.assertEqual(stage['motivation'], 50)
        self.assertEqual(stage['perfectFreeze'], 100)
        self.assertEqual(stage['fpsRaw'], b'\x3c\x3b')
        self.assertIsInstance(stage['inputs'], ReplayInputs)
        self.assertEqual(list(stage['inputs']), [(65535, 65535, 65535), (513, 512, 0)])
        self.assertEqual(stage['inputs'][-1], (513, 512, 0))
        self.assertEqual(stage['inputs'][:1], [(65535, 65535, 65535)])
        with self.assertRaises(IndexError):
            stage['inputs'][2]

    def test_user_length_and_kind_are_separate_uint32_fields(self):
        original = replay_fixture()
        decoded = decode_replay(original)
        self.assertEqual([block['kind'] for block in decoded['userBlocks']], [0, 1])
        self.assertEqual(decoded['userBlocks'][1]['headerRaw'], b'USER\x0e\0\0\0\x01\0\0\0')
        raw_header = bytearray(decoded['userBlocks'][1]['headerRaw'])
        struct.pack_into('<I', raw_header, 8, 0xDEADBEEF)
        decoded['userBlocks'][1]['headerRaw'] = bytes(raw_header)
        rebuilt = decode_replay(encode_replay(decoded))
        self.assertEqual(rebuilt['userBlocks'][1]['kind'], 0xDEADBEEF)

    def test_byte_exact_roundtrip_preserves_user_and_unknown_data(self):
        for user in (True, False):
            original = replay_fixture(user=user)
            self.assertEqual(encode_replay(decode_replay(original)), original)

    def test_raw_edits_rebuild_lengths_without_normalizing_unknown_bytes(self):
        decoded = decode_replay(replay_fixture())
        raw = bytearray(decoded['metadata']['raw'])
        struct.pack_into('<I', raw, 20, 9000)
        decoded['metadata']['raw'] = bytes(raw)
        decoded['stages'][0]['inputRaw'] += struct.pack('<3H', 0x8000, 1, 2)
        decoded['userBlocks'][0]['data'] = b'edited\0'
        encoded = encode_replay(decoded)
        again = decode_replay(encoded)
        self.assertEqual(again['metadata']['score'], 90000)
        self.assertEqual(again['stages'][0]['numFrames'], 3)
        self.assertEqual(again['stages'][0]['inputs'][2], (0x8000, 1, 2))
        self.assertEqual(again['metadata']['raw'][24:84], decoded['metadata']['raw'][24:84])
        self.assertEqual(again['stages'][0]['rawHeader'][44:128],
                         decoded['stages'][0]['rawHeader'][44:128])
        self.assertEqual(again['userBlocks'][0]['data'], b'edited\0')
        self.assertEqual(again['fileTailRaw'], b'opaque-tail')
        self.assertEqual(again['header']['raw'][8:12], bytes.fromhex('efbeadde'))

    def test_cached_payload_is_verified_not_blindly_trusted(self):
        decoded = decode_replay(replay_fixture())
        decoded['encodedPayload'] = b'bad'
        encoded = encode_replay(decoded)
        self.assertEqual(decode_replay(encoded)['rawPayload'], fixture_payload())

    def test_unknown_route_rank_and_nan_fields_remain_raw(self):
        payload = bytearray(fixture_payload())
        struct.pack_into('<I', payload, 84, 0x7FC00001)
        struct.pack_into('<I', payload, 92, 0xFFFFFFFF)
        struct.pack_into('<I', payload, 100, 0xFFFFFFFF)
        original = replay_fixture(bytes(payload))
        decoded = decode_replay(original)
        self.assertIsNone(decoded['metadata']['routeName'])
        self.assertIsNone(decoded['metadata']['rankName'])
        self.assertEqual(encode_replay(decoded), original)

    def test_rejects_headers_lengths_and_overlapping_user_offset(self):
        original = replay_fixture()
        variants = [b'', original[:35], b'BAD!' + original[4:]]
        for offset, value in ((4, 3), (12, 35), (12, len(original)),
                              (28, 0), (28, len(original)),
                              (28, MAX_COMPRESSED_SIZE + 1),
                              (32, 111), (32, MAX_UNCOMPRESSED_SIZE + 1)):
            damaged = bytearray(original)
            struct.pack_into('<I', damaged, offset, value)
            variants.append(bytes(damaged))
        for binary in variants:
            with self.subTest(header=binary[:36].hex()), self.assertRaises(FormatError):
                decode_replay(binary)

    def test_rejects_all_truncated_file_prefixes(self):
        original = replay_fixture(tail=b'')
        for length in range(len(original)):
            with self.subTest(length=length), self.assertRaises(FormatError):
                decode_replay(original[:length])

    def test_rejects_stage_counts_sizes_frames_and_truncations(self):
        for offset, value in ((88, 0), (88, 4), (112 + 4, 0xFFFFFFFF),
                              (112 + 8, 11), (112 + 8, 0xFFFFFFFF)):
            damaged = bytearray(fixture_payload())
            struct.pack_into('<I', damaged, offset, value)
            with self.subTest(offset=offset, value=value), self.assertRaises(FormatError):
                decode_replay(replay_fixture(bytes(damaged)))
        for size in (111, 112, 255, 269):
            with self.subTest(size=size), self.assertRaises(FormatError):
                decode_replay(replay_fixture(fixture_payload()[:size]))

    def test_rejects_invalid_user_magic_sizes_and_overflow(self):
        original = replay_fixture()
        start = struct.unpack_from('<I', original, 12)[0]
        variants = []
        for size in (0, 11, len(original), 2 ** 64 - 1):
            damaged = bytearray(original)
            struct.pack_into('<Q', damaged, start + 4, size)
            variants.append(bytes(damaged))
        damaged = bytearray(original)
        damaged[start:start + 4] = b'NOPE'
        variants.append(bytes(damaged))
        for binary in variants:
            with self.assertRaises(FormatError):
                decode_replay(binary)

    def test_custom_budgets_can_only_tighten_hard_limits(self):
        for options in ({'max_compressed_size': 1}, {'max_uncompressed_size': 111},
                        {'max_frames': 1}, {'max_compressed_size': MAX_COMPRESSED_SIZE + 1},
                        {'max_uncompressed_size': MAX_UNCOMPRESSED_SIZE + 1},
                        {'max_frames': -1}):
            with self.subTest(options=options), self.assertRaises(FormatError):
                decode_replay(replay_fixture(), **options)
            with self.subTest(encode_options=options), self.assertRaises(FormatError):
                encode_replay(decode_replay(replay_fixture()), **options)

    def test_encoder_rejects_invalid_raw_components(self):
        for key, value in (('inputRaw', b'bad'), ('rawHeader', b''), ('fpsRaw', 'bad')):
            decoded = decode_replay(replay_fixture())
            decoded['stages'][0][key] = value
            with self.subTest(key=key), self.assertRaises(FormatError):
                encode_replay(decoded)

    def test_hard_file_and_user_caps_apply_before_copy_or_decompression(self):
        with self.assertRaises(FormatError):
            decode_replay(bytes(MAX_FILE_SIZE + 1))
        original = replay_fixture()
        start = struct.unpack_from('<I', original, 12)[0]
        damaged = bytearray(original)
        struct.pack_into('<I', damaged, start + 4, MAX_USER_DATA_SIZE + 1)
        with self.assertRaises(FormatError):
            decode_replay(bytes(damaged))

    def test_three_stage_frame_budget_is_aggregate(self):
        single = fixture_payload()
        metadata = bytearray(single[:112])
        struct.pack_into('<I', metadata, 88, 3)
        payload = bytes(metadata) + single[112:270] * 3 + b'opaque'
        original = replay_fixture(payload)
        decoded = decode_replay(original, max_frames=6)
        self.assertEqual([stage['numFrames'] for stage in decoded['stages']], [2, 2, 2])
        self.assertEqual(decoded['payloadTailRaw'], b'opaque')
        self.assertEqual(encode_replay(decoded, max_frames=6), original)
        with self.assertRaises(FormatError):
            decode_replay(original, max_frames=5)
        with self.assertRaises(FormatError):
            encode_replay(decoded, max_frames=5)


class ActualDemoTests(unittest.TestCase):
    def test_all_prepared_japanese_demos(self):
        root = Path(__file__).resolve().parents[1]
        catalog_path = root / 'local' / 'catalog.json'
        if not catalog_path.is_file():
            self.skipTest('private prepared catalog is not present')
        catalog = json.loads(catalog_path.read_text(encoding='utf-8'))
        raw = (catalog_path.parent / catalog['variants']['ja']['manifest']).parent / 'raw'
        expected = [
            ('33024e1b08c2a0f27c0b84f4a49d609e4b931d15c0359f88406cd5b257973c9e',
             40595, 0, 1, 1, 6686, 223, 114806, 1280787710,
             'e16ef4bb2685beb2fe861680d7701ce089f4ec478cf578cbc44bccd0eddc1fa8'),
            ('26adf86f10326d3672bfabf4a034f1a2a1fa606416cd3f29529c7550c3a3bcc6',
             40813, 2, 1, 6, 6722, 225, 86121, 1280787845,
             'd61c00fea637049b73ce422f45f7ba0609163acef83af5833305fccd1df7e6ba'),
            ('53d688e575961f793fba91621045d4dd520719b05fe0feadfb4905c4822866b3',
             37024, 4, 2, 11, 6094, 204, 87724, 1280788087,
             '9c0a2d840985a975439c5aaa30bfce204f16f31f6a17fa4168cf9ff50ed35318'),
        ]
        for index, (digest, size, route, rank, stage_id, frames, fps, score, timestamp,
                    payload_digest) in enumerate(expected):
            with self.subTest(demo=index):
                original = (raw / f'demo{index}.rpy').read_bytes()
                self.assertEqual(hashlib.sha256(original).hexdigest(), digest)
                decoded = decode_replay(original)
                self.assertEqual(len(decoded['rawPayload']), size)
                self.assertEqual(hashlib.sha256(decoded['rawPayload']).hexdigest(), payload_digest)
                self.assertEqual(decoded['metadata']['stageCount'], 1)
                self.assertEqual(decoded['metadata']['name'], 'ZUN     ')
                self.assertEqual(decoded['metadata']['route'], route)
                self.assertEqual(decoded['metadata']['rank'], rank)
                self.assertEqual(decoded['metadata']['scoreRaw'], score)
                self.assertEqual(decoded['metadata']['timestamp'], timestamp)
                self.assertEqual(decoded['stages'][0]['stage'], stage_id)
                self.assertEqual(len(decoded['stages'][0]['inputs']), frames)
                self.assertEqual(len(decoded['stages'][0]['fpsRaw']), fps)
                self.assertEqual([block['kind'] for block in decoded['userBlocks']], [0, 1])
                self.assertEqual(encode_replay(decoded), original)
                decoded['encodedPayload'] = b''
                rebuilt = decode_replay(encode_replay(decoded))
                self.assertEqual(rebuilt['rawPayload'], decoded['rawPayload'])
                self.assertEqual([stage['inputRaw'] for stage in rebuilt['stages']],
                                 [stage['inputRaw'] for stage in decoded['stages']])


if __name__ == '__main__':
    unittest.main()
