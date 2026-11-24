import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))


class MessageControlProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'pinned original EXE opt-in')
    def test_original419_gate_and11_waits_are_not_fixed_delay_stubs(self):
        report=importlib.import_module('message_control_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual([sample['held'] for sample in report['gateCases']],[False,True,False,False])
        self.assertEqual([sample['nextRemaining'] for sample in report['waitCases']],[599,4,0,4,4])
        self.assertEqual([sample['stop']['address'] for sample in report['waitCases']],['0x00424966','0x00424966','0x0042443b','0x0042443b','0x0042443b'])


if __name__=='__main__':unittest.main()
