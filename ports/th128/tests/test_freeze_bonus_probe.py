import importlib
import json
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class FreezeBonusProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'read-only original EXE opt-in')
    def test_native_score_bonus_prefix_retains_real_float_to_integer_helper_and_cap(self):
        report=importlib.import_module('freeze_bonus_probe').probe(os.environ['TH128_BULLET_EXE'])
        fixture=json.loads((Path(__file__).resolve().parents[1]/'local/research/freeze-bonus-fixtures.json').read_text(encoding='utf8'))
        self.assertFalse(report['apiReturnsAssigned']);self.assertEqual(len(report['cases']),24)
        for actual,expected in zip(report['cases'],fixture['cases']):
            self.assertEqual(actual['stop'],{'reason':'boundary','address':'0x0040f9c3'})
            self.assertEqual((actual['percent'],actual['bonus'],actual['score']),(expected['percent'],expected['bonus'],expected['score']))

if __name__=='__main__':unittest.main()
