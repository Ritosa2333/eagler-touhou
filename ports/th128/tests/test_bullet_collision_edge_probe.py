import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))


class CollisionEdgeProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'pinned original EXE opt-in')
    def test_original_circle_helper_accepts_finite_small_and_subnormal_positions(self):
        report=importlib.import_module('bullet_collision_edge_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['cases']),7)
        self.assertTrue(all(sample['geometry'] in (0,1,2) for sample in report['cases']))


if __name__=='__main__':unittest.main()
