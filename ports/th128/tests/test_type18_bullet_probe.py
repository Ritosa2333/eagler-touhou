import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class TypeEighteenBulletProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_type18_spawn_tail_and_update_prefixes_preserve_animation_obligations(self):
        self.assertIsNotNone(importlib.util.find_spec('type18_bullet_probe'),'type18 spawn/update evidence is missing')
        report=importlib.import_module('type18_bullet_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='boundary' for case in report['spawnCases']))
        self.assertEqual(report['spawnCases'][0]['state']['motionState'],2)
        self.assertEqual(report['spawnCases'][0]['state']['animationInterrupt'],8)
        self.assertEqual(report['spawnCases'][0]['state']['profileValue'],1)
        self.assertEqual(report['spawnCases'][0]['state']['cancellationScript'],375)
        self.assertTrue(any(case['continuation']=='NeedsCollision' for case in report['tickCases']))
        self.assertTrue(any(case['continuation']=='NeedsAnimation' for case in report['tickCases']))
        self.assertFalse(report['completedConstructor'])

if __name__=='__main__':unittest.main()
