import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class ModeTwoEmissionProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_type18_aim2_geometry_stops_before_constructor_without_return_stubs(self):
        self.assertIsNotNone(importlib.util.find_spec('mode2_emission_probe'),'original aim2/type18 evidence is missing')
        report=importlib.import_module('mode2_emission_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(report['profile']['script'],72)
        self.assertEqual(report['profile']['hitboxSize'],8.5)
        self.assertEqual(report['geometryCases'][0]['geometry']['rawAngle'],1.)
        self.assertEqual(report['geometryCases'][0]['geometry']['speed'],1.5)
        self.assertTrue(all(case['stop']['reason']=='boundary' and not case['completedEmission'] for case in report['geometryCases']))

if __name__=='__main__':unittest.main()
