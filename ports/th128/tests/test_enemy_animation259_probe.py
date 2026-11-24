import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyAnimation259ProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_actual_nonprimary_animation259_request_stops_before_native_allocation(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_animation259_probe'),'original259 request evidence is missing')
        report=importlib.import_module('enemy_animation259_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['cases']),4)
        for case in report['cases']:
            self.assertEqual(case['stop'],{'reason':'boundary','address':'0x0041f2a0'})
            self.assertEqual(case['request']['bank'],0)
            self.assertEqual(case['request']['layer'],7)
            self.assertIn((case['request']['slot'],case['request']['script']),((1,99),(2,189)))
        self.assertFalse(report['allocationExecuted'])

if __name__=='__main__':unittest.main()
