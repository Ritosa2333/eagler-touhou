import importlib
import os
from pathlib import Path
import sys
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools/research'))
class ItemProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_actual_item_birth_and_motion_prefixes(self):
        report=importlib.import_module('item_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop'].get('target')=='0x00402ea0' for case in report['birthCases']))
        self.assertTrue(all(case['stop']['reason']=='boundary' for case in report['tickCases']))
if __name__=='__main__':unittest.main()
