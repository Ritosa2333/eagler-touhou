import importlib
import importlib.util
import os
from pathlib import Path
import struct
import sys
import unittest


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))


class ShotEnemyCollisionProbeTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('shot_enemy_collision_probe'),
                             'original-byte shot/enemy collision probe is missing')
        return importlib.import_module('shot_enemy_collision_probe')

    def test_probe_is_available_and_output_is_owned(self):
        module = self.module()
        for path in (ROOT / 'README.md', ROOT / 'local/research/player-hit-fixtures.json',
                     Path('E:/shot-enemy-collision.json')):
            with self.assertRaises(ValueError):
                module.report_destination(path)
        self.assertEqual(module.report_destination(ROOT / 'local/research/shot-enemy-collision-fixtures.json'),
                         ROOT / 'local/research/shot-enemy-collision-fixtures.json')

    def test_exact_bit_inputs_and_probe_budgets(self):
        module = self.module()
        for bits in (0, 0x80000000, 1, 0x807fffff, 0x00800000, 0x7f7fffff,
                     0x7f800000, 0xff800000, 0x7fc00001, 0x7f800001):
            self.assertEqual(module.bit_payload(bits), struct.pack('<I', bits))
        for value in (-1, 2**32, True, 0.0):
            with self.assertRaises(ValueError):
                module.bit_payload(value)
        with self.assertRaises(ValueError):
            module.execute([(0x400000, bytes(8193))], 0x400000, [])
        with self.assertRaises(ValueError):
            module.execute([], 0x400000, [{'bytes': bytes(131073)}])

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'readonly pinned EXE is opt-in')
    def test_original_misses_positive_prefixes_and_complete_query_abi(self):
        report = self.module().probe(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(report['executable']['unchanged'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertEqual(report['abi']['outFlagBytes'], 4)
        self.assertTrue(report['completeMissReturns'])
        self.assertTrue(all(case['stop']['reason'] == 'returned' for case in report['completeMissReturns']))
        self.assertTrue(any(case['stop']['address'] == '0x0043ec8a' for case in report['contactCases']))
        self.assertTrue(any(case['stop']['address'] == '0x0043ec10' for case in report['contactCases']))


if __name__ == '__main__':
    unittest.main()
