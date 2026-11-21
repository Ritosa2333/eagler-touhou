import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))


class BossType10ProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'pinned original EXE opt-in')
    def test_actual_type10_constructor_tails_and_profile_are_not_synthetic(self):
        report=importlib.import_module('boss_type10_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['cases']),4)
        for sample in report['cases']:
            self.assertEqual(sample['stop'],{'reason':'boundary','address':'0x004099df'})
            self.assertEqual(sample['state']['motionState'],2)


if __name__=='__main__':unittest.main()
