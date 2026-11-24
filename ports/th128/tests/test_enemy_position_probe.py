import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))
class EnemyPositionProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_actual301_initialization_and_complete_mode4_interpolator_returns(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_position_probe'),'native301 position interpolation evidence is missing')
        report=importlib.import_module('enemy_position_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['initializationCases']),6)
        self.assertEqual(len(report['stepCases']),8)
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['stepCases']))
        self.assertTrue(all(case['stop']['address']=='0x0041c52c' for case in report['initializationCases']))
if __name__=='__main__':unittest.main()
