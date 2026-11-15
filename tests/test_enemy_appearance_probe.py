import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyAppearanceProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_culling_stores_transposed_scaled_sprite_extents(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_appearance_probe'),'original rectangular appearance evidence is missing')
        report=importlib.import_module('enemy_appearance_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['cases'][0]['bounds'],[32.,48.])
        self.assertEqual(report['cases'][1]['bounds'],[16.,96.])
        self.assertTrue(all(case['stop']['reason']=='boundary' for case in report['cases']))
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])

if __name__=='__main__':unittest.main()
