import importlib
import importlib.util
import os
from pathlib import Path
import struct
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class Geometry526ProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_grid_initialization_and_real_strip_callback_return_without_allocator_stubs(self):
        self.assertIsNotNone(importlib.util.find_spec('geometry526_probe'),'actual526 geometry grid evidence is missing')
        report=importlib.import_module('geometry526_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['gridCases']),4)
        self.assertEqual(len(report['creationCases']),4)
        self.assertFalse(report['allocationExecuted'])
        for case in report['gridCases']:
            self.assertEqual(case['stop']['reason'],'returned')
            columns,rows=case['input']['columns'],case['input']['rows']
            vertices=bytes.fromhex(case['verticesAfter']);strips=bytes.fromhex(case['stripsAfter'])
            self.assertEqual(len(vertices),columns*rows*28)
            self.assertEqual(len(strips),(columns-1)*rows*56)
            for column in range(columns-1):
                for row in range(rows):
                    start=(column*rows+row)*56
                    self.assertEqual(strips[start:start+28],vertices[(column*rows+row)*28:(column*rows+row+1)*28])
                    self.assertEqual(strips[start+28:start+56],vertices[((column+1)*rows+row)*28:((column+1)*rows+row+1)*28])
            self.assertTrue(all(struct.unpack_from('<I',vertices,index*28+16)[0]==0xffffffff for index in range(columns*rows)))
        for case in report['creationCases']:
            if case['input']['previousObject']:
                self.assertEqual(case['stop']['target'],'0x00403970')
            else:
                if case['input']['radius']>0:self.assertEqual(case['stop']['target'],'0x0047296c')
                else:self.assertEqual(case['stop'],{'reason':'boundary','address':'0x0041c52c'})
                self.assertEqual(case['after']['radius'],case['input']['radius'])
                self.assertEqual(case['after']['scale'],16)
                self.assertEqual(case['after']['color'],case['input']['color'])
                self.assertEqual(case['after']['angles'],[0,0])

if __name__=='__main__':unittest.main()
