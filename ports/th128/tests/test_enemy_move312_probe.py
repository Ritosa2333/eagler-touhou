import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class EnemyMove312ProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_original312_returns_with_actual_random_calls_and_original_mode4_interpolation_steps(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_move312_probe'), 'original312 randomized movement evidence is missing')
        report = importlib.import_module('enemy_move312_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'], report['executable']['sha256After'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertGreaterEqual(len(report['initializationCases']), 12)
        for sample in report['initializationCases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0041c52c'})
            self.assertEqual(sample['after']['mode'], 4)
            self.assertEqual(sample['after']['duration'], sample['input']['duration'])
            self.assertEqual(sample['after']['modeFlags'] & 3, 0)
            self.assertIn(sample['after']['randomCalls'] - sample['input']['randomCalls'], (2, 3))
        for sample in report['stepCases']:
            self.assertEqual(sample['stop']['reason'], 'returned')
            self.assertEqual(sample['stackDelta'], 4)


if __name__ == '__main__':
    unittest.main()
