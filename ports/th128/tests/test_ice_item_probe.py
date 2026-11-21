import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class IceItemProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'read-only original EXE opt-in')
    def test_original_kind9_uses_independent_cursor_count_delayed_state_and_no_ordinary_item_allocation(self):
        report=importlib.import_module('ice_item_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned']);self.assertEqual(len(report['cases']),7)
        for sample in report['cases']:
            self.assertEqual(sample['stop']['reason'],'returned');self.assertEqual(sample['stackDelta'],16)
            original=sample['input'];births=original['count']+1;cursor=original['cursor']
            self.assertEqual(sample['birthsAfter'],births);self.assertEqual(sample['cursorAfter'],(cursor+1)%100)
            self.assertEqual(sample['state'],original['occupied'] or 5)
            if not original['occupied']:
                delay=cursor%32+16 if births>=1024 else cursor%16+8 if births>=512 else cursor%8+4 if births>=256 else cursor%4
                self.assertEqual(sample['delay'],delay)

if __name__=='__main__':unittest.main()
