import importlib.util
from pathlib import Path
import struct
import unittest

spec = importlib.util.spec_from_file_location('replay_reference', Path(__file__).resolve().parents[1] / 'scripts/replay-reference.py')
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class DifferenceTests(unittest.TestCase):
    def setUp(self):
        self.regions = [{'region': 'player', 'address': '0xa8cd68', 'bytes': 240},
                        {'region': 'bullets', 'address': '0xa8e660', 'bytes': 480}]
        self.expected = struct.pack('<I', 71) + bytes(720)

    def test_equal_frame(self):
        self.assertIsNone(module.first_difference(self.expected, self.expected, 71, self.regions))

    def test_first_frame_and_player_field(self):
        actual = bytearray(self.expected)
        actual[4 + 0x14] = 3
        actual[4 + 240 + 50] = 9
        result = module.first_difference(self.expected, actual, 71, self.regions, 1024)
        self.assertEqual(result['frame'], 71)
        self.assertEqual(result['field'], 'player.lives')
        self.assertEqual(result['address'], '0xa8cd7c')
        self.assertEqual(result['streamOffset'], 1048)
        self.assertEqual(result['actualByte'], 3)

    def test_object_slot_and_word(self):
        actual = bytearray(self.expected)
        actual[4 + 240 + 240 + 12] = 1
        result = module.first_difference(self.expected, actual, 71, self.regions)
        self.assertEqual(result['field'], 'bullets[1].word[0xc]')

    def test_frame_alignment_and_truncation(self):
        actual = struct.pack('<I', 72) + self.expected[4:]
        self.assertEqual(module.first_difference(self.expected, actual, 71, self.regions)['field'], 'frame')
        result = module.first_difference(self.expected, self.expected[:-1], 71, self.regions)
        self.assertIsNone(result['actualByte'])
        self.assertEqual(result['expectedByte'], 0)

    def test_secondary_object_stride(self):
        expected = struct.pack('<I', 9) + bytes(384)
        actual = bytearray(expected)
        actual[4 + 192 + 8] = 1
        result = module.first_difference(expected, actual, 9, [{'region': 'secondary', 'address': '0xa8ce60', 'bytes': 384}])
        self.assertEqual(result['field'], 'secondary[1].word[0x8]')

    def test_return_tail_allows_following_drawing_records(self):
        reference = object.__new__(module.NativeReference)
        reference.case = {'mode': '--world-ex-return'}
        reference.end = 12
        values = (12, 511, 257, 0, 3, 0, 32, 288, 288, 4, 0, 32)
        source = struct.pack('<4I', 0x31545245, 1, 1, 8) + bytes(8) + struct.pack('<12I', *values) + b'drawing records'
        result = reference._tail(source)
        self.assertEqual(struct.unpack('<10I', result[:40]), (0x31524f45, 12, 511, 257, 288, 4, 0, 32, 288, 8))
        self.assertEqual(result[40:], bytes([0, 0, 0, 32, 0, 0, 0, 0]))


if __name__ == '__main__':
    unittest.main()
