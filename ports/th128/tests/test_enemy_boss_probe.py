import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyBossProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_boss_binding_and_life_stores_complete_without_invented_returns(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_boss_probe'),'actual boss registration/life evidence is missing')
        report=importlib.import_module('enemy_boss_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['bindingCases']),30)
        self.assertEqual(len(report['lifeCases']),24)
        for case in report['bindingCases']+report['lifeCases']:
            self.assertEqual(case['stop'],{'reason':'boundary','address':'0x0041c52c'})
        for case in report['bindingCases']:
            self.assertEqual(case['after']['managerFlags'],case['input']['managerFlags']&0xfffffffe)
            self.assertEqual(bool(case['after']['flags']&0x1000000),case['input']['slot']>=0)
        for case in report['lifeCases']:
            hp=case['input']['life']&0xffffffff
            self.assertEqual(case['after']['lifeWords'],[hp,hp,hp,(hp*7)&0xffffffff,0x55555555,0x66666666])

if __name__=='__main__':unittest.main()
