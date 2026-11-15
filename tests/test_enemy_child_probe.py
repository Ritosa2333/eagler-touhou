import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyChildProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original88_requests_tail_registration_parent_links_and_saved_successor(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_child_probe'),'original enemy child/scheduler evidence is missing')
        report=importlib.import_module('enemy_child_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['requestCases']),3)
        for case in report['requestCases']:
            self.assertEqual(case['stop'],{'reason':'boundary','address':'0x00467220'})
            self.assertEqual(case['request']['registrationFlags'],0)
            self.assertEqual(case['request']['parent'],0x390000)
        self.assertEqual(len(report['linkCases']),2)
        for case in report['linkCases']:
            self.assertEqual(case['stop']['reason'],'returned')
            self.assertEqual(case['after']['head'],0x390004)
            self.assertEqual(case['after']['tail'],0x3a0004)
            self.assertEqual(case['after']['parentOfChild'],0x390000)
            self.assertEqual(case['after']['firstChild'],0x3a0010)
        self.assertEqual(report['successorPrefix']['cachedNext'],0x3a0004)
        self.assertEqual(report['cachedNullContinuation']['stop']['address'],'0x00466bac')
        self.assertEqual(report['bindResetFlags2']&2,0,'pre-reset display bit does not survive real bind/reset')

if __name__=='__main__':unittest.main()
