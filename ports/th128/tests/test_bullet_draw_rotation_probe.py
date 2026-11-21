import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class BulletDrawRotationProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_rotation_flag_adds_native_half_pi_before_draw(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_draw_rotation_probe'),'original type20 draw rotation evidence is missing')
        report=importlib.import_module('bullet_draw_rotation_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['cases'][0]['rotationAfter'],1.5707963705062866)
        self.assertEqual(report['cases'][1]['rotationAfter'],3.1415927410125732)
        self.assertEqual(report['cases'][-1]['rotationAfter'],.25)
        self.assertTrue(all(case['stop']['reason']=='boundary' for case in report['cases']))
        self.assertEqual(report['flagCases'][0]['flagsAfter'],0x40000000)
        self.assertEqual(report['flagCases'][1]['flagsAfter'],0xbfffffff)

if __name__=='__main__':unittest.main()
