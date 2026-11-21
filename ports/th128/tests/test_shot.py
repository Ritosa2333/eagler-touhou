import hashlib
import json
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from th128_assets.formats import FormatError
from th128_assets.shot import decode_shot


def fixture(records=1):
    start = 0x270
    raw = bytearray(start + records * 52 + 4)
    struct.pack_into('<HH', raw, 0, 3, 1)
    struct.pack_into('<4f', raw, 0x10, 4.5, 2.0, 3.181980609893799, 1.4142135381698608)
    struct.pack_into('<2I', raw, 0x268, start, 0xaabbccdd)
    for index in range(records):
        struct.pack_into('<bb', raw, start + index * 52, 3, -1)
    raw[-4:] = bytes.fromhex('ffffffff')
    return raw


class ShotTests(unittest.TestCase):
    def test_aliased_terminator_tails_share_immutable_storage(self):
        count=256
        start=0x268+count*8
        raw=bytearray(start+131072)
        struct.pack_into('<HH',raw,0,3,count)
        for index in range(count):struct.pack_into('<II',raw,0x268+index*8,start,index)
        raw[start]=255
        decoded=decode_shot(raw)
        first=decoded['groups'][0]['tailRaw']
        self.assertTrue(all(group['tailRaw'] is first for group in decoded['groups']))
        self.assertIsInstance(first,bytes)

    def test_verified_layout_preserves_all_unknown_bytes(self):
        raw = fixture()
        raw[4:16] = bytes(range(12))
        raw[0x28:0x268] = bytes(range(256)) * 2 + bytes(range(64))
        raw[0x272:0x294] = bytes(range(34))
        struct.pack_into('<4I', raw, 0x294, 0, 5, 0xffffffff, 0x12345678)
        result = decode_shot(raw)
        group = result['groups'][0]
        record = group['records'][0]
        self.assertEqual(result['raw'], bytes(raw))
        self.assertEqual(result['headerRaw'], bytes(raw[:0x268]))
        self.assertEqual(result['tagWord'], 3)
        self.assertEqual(group['unknownWord'], 0xaabbccdd)
        self.assertEqual((record['period'], record['phase']), (3, -1))
        self.assertEqual(record['payloadRaw'], bytes(range(34)))
        self.assertEqual(record['callbackIds'], [0, 5, 0xffffffff, 0x12345678])
        self.assertEqual(record['raw'], bytes(raw[0x270:0x2a4]))
        self.assertEqual(group['tailRaw'], bytes.fromhex('ffffffff'))
        self.assertEqual(result['speedFloats'], [4.5, 2.0, 3.181980609893799, 1.4142135381698608])

    def test_bytes_are_copied_and_memoryviews_are_supported(self):
        raw = fixture()
        result = decode_shot(memoryview(raw))
        before = result['raw']
        raw[-1] = 0
        self.assertEqual(result['raw'], before)

    def test_native_negative_first_byte_terminates_without_full_record(self):
        raw = fixture(0)[:-3]
        raw[-1] = 0x80
        self.assertEqual(decode_shot(raw)['groups'][0]['tailRaw'], b'\x80')

    def test_truncated_headers_and_tables_fail(self):
        raw = fixture()
        for size in (0, 1, 3, 0x267, 0x26f):
            with self.subTest(size=size), self.assertRaises(FormatError):
                decode_shot(raw[:size])

    def test_offsets_into_header_table_or_outside_file_fail(self):
        for offset in (0, 0x268, 0x26f, 0xffffffff, len(fixture())):
            raw = fixture()
            struct.pack_into('<I', raw, 0x268, offset)
            with self.subTest(offset=offset), self.assertRaises(FormatError):
                decode_shot(raw)

    def test_no_terminator_truncated_records_and_zero_period_fail(self):
        for raw in (fixture()[:-4], fixture()[:-7]):
            with self.assertRaises(FormatError):
                decode_shot(raw)
        raw = fixture()
        raw[0x270] = 0
        with self.assertRaisesRegex(FormatError, 'period'):
            decode_shot(raw)

    def test_next_group_bounds_the_record_scan(self):
        raw = bytearray(0x300)
        struct.pack_into('<HH', raw, 0, 3, 2)
        struct.pack_into('<4I', raw, 0x268, 0x278, 0, 0x280, 0)
        raw[0x278] = 3
        raw[0x280] = 0xff
        with self.assertRaises(FormatError):
            decode_shot(raw)

    def test_aliases_and_nonmonotonic_offsets_preserve_table_order(self):
        raw = bytearray(0x284)
        struct.pack_into('<HH', raw, 0, 3, 3)
        struct.pack_into('<6I', raw, 0x268, 0x283, 9, 0x280, 8, 0x283, 7)
        raw[0x280:0x284] = bytes.fromhex('ff0102fe')
        result = decode_shot(raw)
        self.assertEqual([group['offset'] for group in result['groups']], [0x283, 0x280, 0x283])
        self.assertEqual([group['tailRaw'] for group in result['groups']], [b'\xfe', b'\xff\x01\x02', b'\xfe'])

    def test_budgets_are_bounded_before_record_allocation(self):
        for changes in ({'max_size': 50}, {'max_groups': 0}, {'max_records': 0},
                        {'max_size': True}, {'max_groups': 257}, {'max_records': 65537}):
            with self.subTest(changes=changes), self.assertRaises(FormatError):
                decode_shot(fixture(), **changes)
        with self.assertRaises(FormatError):
            decode_shot(fixture(2), max_records=1)
        with self.assertRaises(FormatError):
            decode_shot(6736)

    def test_only_actual_tag_three_layout_is_claimed(self):
        raw = fixture()
        struct.pack_into('<H', raw, 0, 4)
        with self.assertRaises(FormatError):
            decode_shot(raw)

    def test_actual_catalog_resource_is_decoded_without_formation_substitution(self):
        catalog_path = ROOT / 'local/catalog.json'
        if not catalog_path.is_file():
            self.skipTest('local asset catalog unavailable')
        catalog = json.loads(catalog_path.read_text(encoding='utf-8'))
        manifest = ROOT / 'local' / catalog['variants']['ja']['manifest']
        source = manifest.parent / 'raw/pl00a.sht'
        if not source.is_file():
            self.skipTest('actual Japanese shot asset unavailable')
        raw = source.read_bytes()
        self.assertEqual(hashlib.sha256(raw).hexdigest(),
                         'aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b')
        result = decode_shot(raw)
        self.assertEqual(len(raw), 6736)
        self.assertEqual(result['groupCount'], 16)
        self.assertEqual([len(group['records']) for group in result['groups']],
                         [2, 3, 4, 4, 6, 6, 6, 6, 6, 8, 8, 8, 10, 10, 10, 17])
        self.assertEqual(sum(len(group['records']) for group in result['groups']), 114)
        self.assertEqual(result['raw'], raw)
        self.assertEqual(result['groups'][0]['records'][0]['callbackIds'], [0, 0, 0, 0])


if __name__ == '__main__':
    unittest.main()
