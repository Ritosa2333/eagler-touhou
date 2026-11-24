import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools/research'))
class DamageFieldProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_declared_sse_x87_environment_is_recorded_without_patching_instructions(self):
        probe=importlib.import_module('freeze_probe')
        result=probe.emulate([(0x400000,b'\xc3')],0x400000,cpu_state={'mxcsr':0x1f80,'fpcw':0x37f})
        self.assertEqual(result['inputCpuState'],{'mxcsr':0x1f80,'fpcw':0x37f})
        self.assertEqual(result['stop']['reason'],'returned')
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_field_birth_returns_and_tails_stop(self):
        report=importlib.import_module('damage_field_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'],report['executable']['sha256After'])
        self.assertTrue(all(case['stop']['reason']=='returned' for case in report['birthCases']))
        self.assertTrue(all(case['stop']['reason']=='boundary' for case in report['tailCases']))
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_original_circle_queries_return_with_native_interval_tag_limit_and_score(self):
        report=importlib.import_module('damage_field_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertIn('queryCases',report,'original circular-source query evidence is missing')
        cases={case['name']:case for case in report['queryCases']}
        self.assertTrue(all(case['stop']['reason']=='returned' for case in cases.values()))
        self.assertEqual(cases['center']['damage'],150)
        self.assertEqual(cases['tangent']['damage'],150)
        self.assertEqual(cases['outside']['damage'],0)
        self.assertEqual(cases['divisible-clock-change']['damage'],0)
        self.assertEqual(cases['equal-clock']['damage'],150)
        self.assertEqual(cases['tagged']['outFlagAfter'],2)
        self.assertEqual(cases['limit']['sourceDamageAfter'],0)
        self.assertEqual(cases['center']['scoreAfter'],2)
        self.assertEqual(cases['score-cap']['scoreAfter'],999999999)
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_pool_birth_wrap_reuse_and_full_pool_preserve_native_slots(self):
        native=importlib.import_module('damage_field_probe').NativeDamageField(os.environ['TH128_BULLET_EXE'])
        try:
            wrapped=native.pool_birth('wrap',1999,[1999,0,1])
            self.assertEqual((wrapped['selectedSlot'],wrapped['cursorAfter']),(2,3))
            full=native.pool_birth('full',17,[],True)
            self.assertEqual((full['selectedSlot'],full['cursorAfter']),(17,18))
            self.assertEqual(full['selectedBefore'],full['selectedAfter'])
        finally:native.image.close()
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'original exe opt-in')
    def test_stationary_field_update_executes_original_motion_and_quantization(self):
        native=importlib.import_module('damage_field_probe').NativeDamageField(os.environ['TH128_BULLET_EXE'])
        try:
            self.assertTrue(hasattr(native,'motion'),'complete bounded stationary motion update is missing')
            result=native.motion((-.001,400.123,.1))
            self.assertEqual(result['stop']['reason'],'boundary')
            self.assertEqual(result['positionAfter'],[-0.009999999776482582,400.1199951171875,0.10000000149011612])
        finally:native.image.close()
if __name__=='__main__':unittest.main()
