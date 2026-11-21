import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class TypeTwentyBulletProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_type20_tail_uses_its_own_profile_and_cancellation_not_type18_alias(self):
        module=importlib.import_module('type18_bullet_probe')
        self.assertTrue(hasattr(module,'probe_type20'),'type20 original-byte evidence is missing')
        report=module.probe_type20(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(report['spawnCases'][0]['state']['cancellationScript'],381)
        self.assertEqual(report['spawnCases'][0]['state']['profileValue'],2)
        self.assertTrue(all(case['input']['type']==20 for case in report['tickCases']))
        self.assertTrue(all(case['nativeTypeBefore']==20 for case in report['tickCases']))
        self.assertEqual(report['profile']['hitboxSize'],7.)

if __name__=='__main__':unittest.main()
