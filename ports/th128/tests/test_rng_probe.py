import importlib.util
import sys
import unittest
from pathlib import Path

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from research.rng_probe import step16, step32, sample_original


class RandomProbeTests(unittest.TestCase):
    def test_literal_vectors_observed_from_original_bytes(self):
        self.assertEqual(step16(0,0),(0xC374,0xC374,1))
        self.assertEqual(step32(0,0),(0x30DDEFF1,49095,2))
        self.assertEqual(step32(0x1234,0),(0x1EB187A1,7814,2))
        self.assertEqual(step16(0xFFFF,0),(0x11F0,0x11F0,1))

    def test_counter_wrap_is_explicit(self):
        self.assertEqual(step32(0,0xFFFFFFFF)[2],1)

    @unittest.skipUnless(importlib.util.find_spec('unicorn') and importlib.util.find_spec('pefile'), 'optional native probe libraries unavailable')
    def test_original_rng_retains_seed_and_counter_transition(self):
        original=Path('E:/gee/妖精大战争/th128.exe')
        if not original.is_file():
            self.skipTest('optional original executable unavailable')
        self.assertEqual(sample_original(original,16,0,0),step16(0,0))
        self.assertEqual(sample_original(original,32,0x1234,0xFFFFFFFE),step32(0x1234,0xFFFFFFFE))


if __name__=='__main__':
    unittest.main()
