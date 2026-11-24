import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EclPlayerAimProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_minus_9989_dispatch_calls_the_already_proven_native_aim_helper(self):
        self.assertIsNotNone(importlib.util.find_spec('ecl_player_aim_probe'),'original -9989 dispatch evidence is missing')
        report=importlib.import_module('ecl_player_aim_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['dispatchTarget'],0x41d3be)
        self.assertEqual(report['prefix']['stop']['target'],'0x0043c3a0')
        self.assertEqual(report['prefix']['registers']['ecx'],0x3011fc)
        self.assertFalse(report['completedGlobalQuery'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])

if __name__=='__main__':unittest.main()
