import importlib
import importlib.util
import os
from pathlib import Path
import sys
import struct
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyVariableProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_manager_words_are_shared_integer_lvalues_with_real_zeroing(self):
        report=importlib.import_module('enemy_variable_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertIn('managerCases',report,'actual MBoss manager-variable evidence is missing')
        self.assertEqual(report['managerInitialization']['stop']['reason'],'returned')
        initialized=bytes.fromhex(report['managerInitialization']['memory']['manager']['after'])
        self.assertEqual(struct.unpack_from('<3i',initialized,0x10),(0,0,0))
        self.assertEqual(struct.unpack_from('<I',initialized)[0],2)
        self.assertEqual(len(report['managerCases']),15)
        for case in report['managerCases']:
            self.assertEqual(case['lvalue']['stop']['reason'],'returned')
            self.assertEqual(case['nativeOffset'],0x10+4*(case['identifier']+9949))
            self.assertEqual(case['reader']['stop']['reason'],'returned')
            self.assertEqual(case['reader']['registers']['eax'],case['input']&0xffffffff)
            self.assertEqual(case['floatReaderOpcode'],'fild-int32')
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_float_lvalue_resolves_into_inherited_words_not_global_memory(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_variable_probe'),'original enemy float lvalue proof is missing')
        report=importlib.import_module('enemy_variable_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual([case['nativeOffset'] for case in report['cases']],[0x1414,0x1418])
        self.assertTrue(all(case['stop']['reason']=='returned' and case['readOffset']==case['nativeOffset'] for case in report['cases']))
        self.assertIn('integerCases',report,'original integer inherited-lvalue evidence is missing')
        self.assertEqual([case['nativeOffset'] for case in report['integerCases']],[0x1404,0x1408,0x140c,0x1410])

if __name__=='__main__':unittest.main()
