import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class PhaseContinuationProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_original_phase_tail_queries_resets_and_resolves_real_sorted_entry(self):
        report = importlib.import_module('enemy_phase_continuation_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['cases']), 3)
        for sample in report['cases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x004167ae'})
            state = sample['after']
            self.assertEqual(state['life'], 6300)
            self.assertEqual(state['phaseThreshold'], -1)
            self.assertEqual(state['timeBits'], 0)
            self.assertEqual(state['scriptPointer'], state['lookupResult'])
            self.assertEqual(state['asyncHead'], 0)
            self.assertEqual(state['stackDepths'], [0] * 8)
            self.assertEqual(state['preservedEntityVariable'], 0x12345678)


if __name__ == '__main__':
    unittest.main()
