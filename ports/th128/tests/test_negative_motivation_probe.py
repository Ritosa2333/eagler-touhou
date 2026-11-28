import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))


class NegativeMotivationProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'pinned original EXE opt-in')
    def test_original_shot_impact_handles_signed_post_death_motivation_without_lower_clamping(self):
        report=importlib.import_module('negative_motivation_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual([sample['valueAfter'] for sample in report['cases']],[-8528,-9998,-9997,-9999,0,-2147483646,-10001])


if __name__=='__main__':unittest.main()
