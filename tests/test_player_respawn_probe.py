import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class PlayerRespawnProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_entry_motion_and_empty_laser_callbacks_keep_boundaries(self):
        self.assertIsNotNone(importlib.util.find_spec('player_respawn_probe'),'original entry-motion probe is missing')
        report=importlib.import_module('player_respawn_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['controlCases']))
        cases={case['elapsed']:case for case in report['entryCases']}
        self.assertEqual(cases[0]['rawVertical'],61440)
        self.assertEqual(cases[29]['rawVertical'],56491)
        self.assertEqual(cases[30]['rawVertical'],56320)
        self.assertEqual(cases[60]['rawVertical'],51200)
        self.assertEqual(cases[0]['stop']['address'],'0x0043b9ee')
        self.assertEqual(cases[30]['stop']['address'],'0x0043b703')
        self.assertEqual(report['controlCases'][1]['pendingInterrupts'],[2,2,10,3,3,3,3,3])
        for case in report['entryCases']:
            self.assertEqual(case['memory']['rapidShotTimer']['before'],case['memory']['rapidShotTimer']['after'])
            self.assertNotEqual(case['memory']['movementDeltas']['before'],case['memory']['movementDeltas']['after'])
            self.assertEqual(case['memory']['movementDeltas']['after'],'0000000000000000')
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_cancel_record_changes_active_motion_and_age_only_after_required_effects(self):
        module=importlib.import_module('player_respawn_probe')
        native=module.NativeRespawn(os.environ['TH128_BULLET_EXE'])
        try:
            self.assertTrue(hasattr(native,'cancel_record'),'original cancellation record/timer evidence is missing')
            active=native.cancel_record(2)
            self.assertEqual(active['motionAfter'],3)
            self.assertEqual(active['ageAfter'],0)
            self.assertEqual(active['lifeClockAfter'],91)
            inactive=native.cancel_record(3)
            self.assertEqual(inactive['ageAfter'],123)
        finally:native.image.close()

if __name__=='__main__':unittest.main()
