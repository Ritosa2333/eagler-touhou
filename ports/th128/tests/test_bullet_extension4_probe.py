import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class BulletExtensionFourProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_actual_type4_sentinel_initialization_and_acceleration_return_without_math_stubs(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_extension4_probe'),'original type4 proof is missing')
        report=importlib.import_module('bullet_extension4_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['initializationCases']))
        self.assertEqual(report['initializationCases'][0]['stateAfter']['active'],4)
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['tickCases']))
        self.assertTrue(any('0x004900cc' in case['instructionTrace'] for case in report['tickCases']))
        self.assertIn('integratedCases',report,'composed original type4/motion evidence is missing')
        self.assertTrue(all(case['stop']=={'reason':'boundary','address':'0x00408d09'} for case in report['integratedCases']))
        self.assertIn('initializationSound',report,'mandatory type4 sound prefix is missing')
        self.assertEqual(report['initializationSound']['stop']['target'],'0x00458410')
        self.assertEqual(report['initializationSound']['stateAfter']['index'],1)

if __name__=='__main__':unittest.main()
