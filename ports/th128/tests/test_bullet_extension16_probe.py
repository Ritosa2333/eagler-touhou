import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))

class BulletExtensionSixteenProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_real_type16_initialization_returns_with_silent_context_and_sound_context_stops(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_extension16_probe'),'original type16 lifecycle evidence is missing')
        report=importlib.import_module('bullet_extension16_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['initializationCases']))
        self.assertEqual(report['initializationCases'][0]['stateAfter']['active'],16)
        self.assertEqual(report['initializationCases'][0]['stateAfter']['index'],2)
        self.assertEqual(report['initializationSound']['stop']['reason'],'returned')
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['tickCases']))
        self.assertEqual(report['updateSound']['stop']['target'],'0x00458410')
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_real_update_executes_extension_then_motion_without_restarting_life_clock(self):
        module=importlib.import_module('bullet_extension16_probe');native=module.NativeExtension16(os.environ['TH128_BULLET_EXE'])
        try:
            self.assertTrue(hasattr(native,'integrated'),'original complete pre-ANM extension/motion path is missing')
            for half in (False,True):
                case=native.integrated(half=half)
                self.assertEqual(case['stop'],{'reason':'boundary','address':'0x00408d09'})
                self.assertEqual(case['lifeClockAfter'],1)
                self.assertIn('0x0040b200',case['instructionTrace'])
                self.assertEqual(case['stateAfter']['timer'][1],1)
        finally:native.image.close()

if __name__=='__main__':unittest.main()
