import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EmitterExtensionProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_537_writes_all_ten_fields_in_selected_44_byte_record_without_clearing_callback(self):
        self.assertIsNotNone(importlib.util.find_spec('emitter_extension_probe'),'original537 extension record evidence is missing')
        report=importlib.import_module('emitter_extension_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='boundary' and case['adjacentUnchanged'] for case in report['cases']))
        self.assertEqual(report['cases'][0]['recordAfter'][10],0xa5a5a5a5)
        self.assertEqual(report['cases'][0]['recordAfter'][8:10],[16,0])

if __name__=='__main__':unittest.main()
