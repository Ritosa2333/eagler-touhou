import importlib.util
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from research.ecl_globals_probe import sample_float_global
from research.rng_probe import step32


class EclGlobalTests(unittest.TestCase):
    @unittest.skipUnless(importlib.util.find_spec('unicorn'),'optional original-byte emulator unavailable')
    def test_entity_and_player_fields_are_read_from_their_native_offsets(self):
        executable=Path('E:/gee/妖精大战争/th128.exe')
        if not executable.is_file():self.skipTest('original executable unavailable')
        self.assertEqual(sample_float_global(executable,-9995,0,{0x1230:12.25})['value'],12.25)
        self.assertEqual(sample_float_global(executable,-9991,0,{},(-21.5,400.0))['value'],-21.5)
        self.assertEqual(sample_float_global(executable,-9959,0,{},difficulty=2)['value'],2.0)

    @unittest.skipUnless(importlib.util.find_spec('unicorn'),'optional original-byte emulator unavailable')
    def test_native_unit_random_rounds_and_consumes_two_rng_calls(self):
        executable=Path('E:/gee/妖精大战争/th128.exe')
        if not executable.is_file():self.skipTest('original executable unavailable')
        result=sample_float_global(executable,-9999,0)
        self.assertEqual(result['seed'],step32(0,0)[1])
        self.assertEqual(result['calls'],2)
        self.assertAlmostEqual(result['value'],0x30DDEFF1/4294967296,places=7)
        self.assertEqual(sample_float_global(executable,-9998,1)['bits'],'7197f8bf')


if __name__=='__main__':unittest.main()
