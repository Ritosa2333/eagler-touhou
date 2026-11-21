import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class PerfectHudProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_notification_requires_front47_and_actual_charge_helper_toggles_child_draw_bits(self):
        self.assertIsNotNone(importlib.util.find_spec('perfect_hud_probe'),'original Perfect Freeze HUD evidence is missing')
        report=importlib.import_module('perfect_hud_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(report['notification']['stop']['target'],'0x004670b0')
        self.assertEqual(report['notification']['argumentsAtFactory'][1:],[47,23,0])
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['chargeCases']))
        self.assertEqual([case['enabled'] for case in report['chargeCases']],[[False,False,False],[True,False,False],[True,True,False],[True,True,True]])
        self.assertEqual(report['flag85']['flagsAfter'],0x80000006)

if __name__=='__main__':unittest.main()
