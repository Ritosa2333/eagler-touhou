import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))


class ScreenShakeProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'pinned original EXE opt-in')
    def test_original_timer_and_shared_rng_are_preserved_until_exact_completion(self):
        report=importlib.import_module('screen_shake_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual([sample['randomCalls'] for sample in report['cases']],[4,4,4,4,0])
        self.assertEqual([sample['returned'] for sample in report['cases']],[1,1,1,1,7])


if __name__=='__main__':unittest.main()
