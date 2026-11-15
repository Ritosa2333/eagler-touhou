import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))
class IceContactTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'read-only original EXE opt-in')
    def test_complete_native_ice_contact_returns_real_selected_node(self):
        self.assertIsNotNone(importlib.util.find_spec('ice_contact_probe'),'original ice contact evidence missing')
        report=importlib.import_module('ice_contact_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertGreaterEqual(len(report['cases']),30)
        for sample in report['cases']:
            self.assertEqual(sample['stop']['reason'],'returned')
            self.assertEqual(sample['stackDelta'],8)
            self.assertEqual(sample['memory']['object']['before'],sample['memory']['object']['after'])
