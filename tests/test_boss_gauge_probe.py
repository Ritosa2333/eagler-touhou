import importlib
import importlib.util
import os
from pathlib import Path
import struct
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class BossGaugeProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_actual427_completes_original_normalization_and_real_hud_table_stores(self):
        self.assertIsNotNone(importlib.util.find_spec('boss_gauge_probe'),'original boss gauge evidence is missing')
        report=importlib.import_module('boss_gauge_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['cases']),18)
        self.assertEqual({case['input']['slot'] for case in report['cases']},{0,1,2,3})
        for case in report['cases']:
            self.assertEqual(case['stop'],{'reason':'boundary','address':'0x0041c52c'})
            expected=struct.unpack('<I',struct.pack('<f',case['input']['life']/case['input']['maximum']))[0]
            self.assertEqual(case['after']['ratioBits'],expected)
            self.assertEqual(case['after']['color'],case['input']['color']&0xffffffff)
            self.assertEqual(case['after']['offset'],0x5918+case['input']['boss']*0x54+case['input']['slot']*8)

if __name__=='__main__':unittest.main()
