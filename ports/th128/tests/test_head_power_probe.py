import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))
class HeadPowerProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original read-only EXE opt-in')
    def test_original_head_percent_position_color_and_scale_prefix(self):
        self.assertIsNotNone(importlib.util.find_spec('head_power_probe'),'native head percent evidence missing')
        report=importlib.import_module('head_power_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertFalse(report['formatCallExecuted'])
        self.assertEqual(len(report['cases']),18)
        for sample in report['cases']:
            self.assertEqual(sample['stop'],{'reason':'boundary','address':'0x0043f58d'})
            self.assertEqual(sample['after']['percent'],sample['input']['power']//100)
            self.assertEqual(sample['after']['x'],320+sample['input']['x'])
            self.assertEqual(sample['after']['y'],sample['input']['y']-8)
            self.assertEqual(sample['after']['style'],7)
