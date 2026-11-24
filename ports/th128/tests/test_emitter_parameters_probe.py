import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EmitterParametersProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_direct_speed_and_count_handlers_keep_original_stores_and_word_truncation(self):
        self.assertIsNotNone(importlib.util.find_spec('emitter_parameters_probe'),'original505/506 evidence is missing')
        report=importlib.import_module('emitter_parameters_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='boundary' and case['adjacentUnchanged'] for case in report['cases']))
        counts=[case for case in report['cases'] if case['opcode']==506]
        self.assertEqual(counts[0]['counts'],[11,1])
        self.assertEqual(counts[1]['counts'],[-1,-32768])

if __name__=='__main__':unittest.main()
