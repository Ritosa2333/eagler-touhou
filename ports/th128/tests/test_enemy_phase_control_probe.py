import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class PhaseControlProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_real_timer_controls_and_timeout_branch_reference(self):
        report = importlib.import_module('enemy_phase_control_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual([sample['timer'][:3] for sample in report['cases']], [[-1, 0, 0.], [59, 60, 60.]])
        self.assertEqual([sample['timeout'] for sample in report['timeoutReferences']], [0, 0, 1, 1])
        for sample in report['inactiveSpellEnd']:
            self.assertEqual(sample['stop']['reason'],'returned')
            self.assertEqual(sample['memory']['spell']['before'],sample['memory']['spell']['after'])


if __name__ == '__main__':
    unittest.main()
