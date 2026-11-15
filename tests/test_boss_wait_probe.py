import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class BossWaitProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_actual420_queries_only_three_boss_owners_and_core_holds_pc_and_clock(self):
        self.assertIsNotNone(importlib.util.find_spec('boss_wait_probe'), 'original420 boss wait evidence is missing')
        report = importlib.import_module('boss_wait_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'], report['executable']['sha256After'])
        self.assertGreaterEqual(len(report['handlerCases']), 7)
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertFalse(report['hostCallExecutedInCoreTail'])
        for sample in report['handlerCases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0041c52e'})
            self.assertEqual(sample['result'], -1 if any(sample['owners'][:3]) else 0)
            self.assertEqual(sample['memory']['manager']['before'], sample['memory']['manager']['after'])
        for sample in report['coreCases']:
            if sample['result'] == -1:
                self.assertEqual(sample['after']['pc'], sample['before']['pc'])
                self.assertEqual(sample['after']['time'], sample['before']['time'])
            else:
                self.assertEqual(sample['after']['pc'], sample['before']['pc'] + 16)
                self.assertEqual(sample['after']['time'], sample['before']['time'] + 1)


if __name__ == '__main__':
    unittest.main()
