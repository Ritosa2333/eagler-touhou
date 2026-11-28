import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))


class ShotBoundsEdgeProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'pinned original EXE opt-in')
    def test_original_rotated_corners_do_not_reject_exact_near_zero_crossings(self):
        report=importlib.import_module('shot_bounds_edge_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned']);self.assertEqual(len(report['cases']),18)
        self.assertTrue(all(sample['stop']['reason']=='returned' for sample in report['cases']))


if __name__=='__main__':unittest.main()
