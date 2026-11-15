import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemySpawn261ProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_261_prepares_absolute_position_and_mode1_before_real_constructor(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_spawn261_probe'),'original261 request evidence is missing')
        report=importlib.import_module('enemy_spawn261_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(report['result']['stop']['target'],'0x00414ff0')
        self.assertEqual(report['position'],[10.,20.,0.])
        self.assertEqual(report['mode'],1)
        self.assertEqual(report['inheritedWords'],list(range(12)))

if __name__=='__main__':unittest.main()
