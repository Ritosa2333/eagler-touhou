import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class AnmUpdateGateProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original111_flag_write_and_actual_control_gate_and_irq_bypass(self):
        self.assertIsNotNone(importlib.util.find_spec('anm_update_gate_probe'),'bounded ANM gate evidence is missing')
        report=importlib.import_module('anm_update_gate_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['handlerCases']),18)
        self.assertEqual(len(report['gateCases']),60)
        for case in report['handlerCases']:
            self.assertEqual(case['stop'],{'reason':'boundary','address':'0x00459e1a'})
            self.assertEqual(case['after'],(case['previous']&~0xc0)|((case['value']<<6)&0xc0))
        for case in report['gateCases']:
            paused=case['irq']==0 and case['flags2']&0xc0==0x40 and case['present'] and bool(case['controlFlags']&2)
            self.assertEqual(case['paused'],paused)
            self.assertEqual(case['stop']['reason'],'boundary')

if __name__=='__main__':unittest.main()
