import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyDropProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_406_preserves_primary_and_spread_407_writes_selected_word_408_writes_spread(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_drop_probe'),'original 406/407/408 evidence is missing')
        report=importlib.import_module('enemy_drop_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='boundary' for case in report['cases']))
        cleared=report['cases'][0]
        self.assertEqual(cleared['afterWords'][:11],[1]+[0]*10)
        selected=report['cases'][1]
        self.assertEqual(selected['afterWords'][2],2)
        self.assertEqual(selected['afterWords'][1],selected['beforeWords'][1])

if __name__=='__main__':unittest.main()
