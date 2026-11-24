import importlib
import os
from pathlib import Path
import struct
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class BossEclProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_original436_writes_real_float_locals_for_all_five_native_ranks(self):
        report = importlib.import_module('boss_ecl_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['cases']), 5)
        for rank, sample in enumerate(report['cases']):
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0041c52c'})
            self.assertEqual(sample['valueBits'], struct.unpack('<I', struct.pack('<f', 1 << min(rank, 3)))[0])


if __name__ == '__main__':
    unittest.main()
