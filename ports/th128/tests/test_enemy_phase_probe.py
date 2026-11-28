import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class EnemyPhaseProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original414_records_and_complete_health_time_query_are_not_fabricated(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_phase_probe'),'original414/phase query evidence is missing')
        report=importlib.import_module('enemy_phase_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertEqual(len(report['registrationCases']),6)
        for case in report['registrationCases']:
            self.assertEqual(case['stop'],{'reason':'boundary','address':'0x0041c52c'})
        self.assertGreaterEqual(len(report['queryCases']),20)
        for case in report['queryCases']:
            self.assertEqual(case['stop']['reason'],'returned')
            active=[index for index,record in enumerate(case['before']['records']) if record[0]<0x80000000]
            self.assertEqual(active,{'single':[0],'inactive':[],'late':[7],'priority':[0,1],'timeless':[0]}[case['input']['variant']],
                'synthetic input must not overwrite a neighboring native phase record')
        actual=report['registrationCases'][:2]
        self.assertEqual([case['after']['records'][0][:2] for case in actual],[[6300,1800],[8200,1800]])
        self.assertTrue(all(case['after']['records'][0][2:]==[0x360020,0x360020] for case in actual))
        self.assertFalse(report['windowsGameProcessLaunched'])

if __name__=='__main__':unittest.main()
