import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class DifficultyConditionProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_integer_and_float_native_difficulty_conditions_preserve_equal_rank_predicate(self):
        self.assertIsNotNone(importlib.util.find_spec('difficulty_condition_probe'),'original typed difficulty conditions are missing')
        report=importlib.import_module('difficulty_condition_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        for case in report['integerCases']:
            self.assertEqual(case['result'],int(case['difficulty']==case['identifier']+9953))
        for case in report['floatCases']:
            self.assertEqual(case['result'],float(case['difficulty']==case['identifier']+9953))

if __name__=='__main__':unittest.main()
