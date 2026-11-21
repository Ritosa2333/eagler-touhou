import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class EnemyClampProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_actual404_stores_and_complete_combine_quantize_clamp_rebuild_return_without_api_stubs(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_clamp_probe'), 'original404 full position helper evidence is missing')
        report = importlib.import_module('enemy_clamp_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'], report['executable']['sha256After'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertGreaterEqual(len(report['combineCases']), 12)
        self.assertEqual(len(report['registrationCases']), 3)
        for sample in report['combineCases']:
            self.assertEqual(sample['stop']['reason'], 'returned')
            self.assertEqual(sample['stackDelta'], 4)
            self.assertEqual(sample['after']['flags'], sample['input']['flags'])
            self.assertEqual(sample['after']['area'], sample['input']['area'])
        for sample in report['registrationCases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0041c52c'})
            self.assertTrue(sample['after']['flags'] & 0x40000)
            self.assertEqual(sample['after']['area'], sample['input']['area'])


if __name__ == '__main__':
    unittest.main()
