import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class RadiusCancellationProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_real_radius_selection_cancel_interrupt_half_step_and_empty_laser_manager(self):
        report = importlib.import_module('bullet_radius_cancel_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual([sample['motionState'] for sample in report['cases']], [4, 4, 1, 4])
        self.assertEqual([sample['pending'] for sample in report['cases']], [1, 1, 0, 1])
        self.assertEqual(report['cases'][0]['lifeTimer'][:3], [-1, 0, 0.])
        self.assertEqual(report['emptyLaserReturn']['stop']['reason'], 'returned')
        self.assertEqual(report['emptyLaserReturn']['registers']['eax'], 0)


if __name__ == '__main__':
    unittest.main()
