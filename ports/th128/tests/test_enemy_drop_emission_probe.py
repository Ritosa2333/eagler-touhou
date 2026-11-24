import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyDropEmissionProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_nonempty_drop_helper_stops_before_actual_item_and_independent_tail_preserves_rng(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_drop_emission_probe'),'original drop emission evidence is missing')
        report=importlib.import_module('enemy_drop_emission_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop'].get('target')=='0x004293d0' for case in report['entryCases']))
        self.assertEqual(report['entryCases'][0]['itemKind'],2)
        self.assertEqual(report['entryCases'][0]['callsAfter'],4)
        self.assertTrue(all(case['stop']['reason']=='boundary' for case in report['angleCases']))
        self.assertNotEqual(report['angleCases'][0]['angleAfterBits'],0,'sample must capture the actual post-normalization store')
        self.assertNotEqual(report['geometryCases'][0]['positionBits'],[0,0,0],'scatter sample must capture declared locals, not stack-head padding')
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['emptyCases']))
        self.assertEqual(report['emptyCases'][0]['callsAfter'],2)
        self.assertFalse(report['completedNonemptyHelper'])

if __name__=='__main__':unittest.main()
