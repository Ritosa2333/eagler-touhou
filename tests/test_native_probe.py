import importlib.util
import sys
import unittest
import tempfile
from unittest.mock import patch
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))

from probe_native_circle import emulate_pure_function, float32, main


class NativeProbeTests(unittest.TestCase):
    def test_report_cannot_overwrite_original(self):
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary) / 'game.exe'
            source.write_bytes(b'original')
            with patch('probe_native_circle.probe', return_value={'cases': []}):
                self.assertEqual(main(['--exe', str(source), '--report', str(source)]), 1)
            self.assertEqual(source.read_bytes(), b'original')

    def test_float32_inputs_are_explicit_and_finite(self):
        self.assertEqual(float32(1), 1)
        self.assertNotEqual(float32(0.1), 0.1)
        with self.assertRaises(ValueError):
            float32(float('nan'))

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'optional Unicorn is not installed')
    def test_stack_return_and_callee_cleanup_with_synthetic_code(self):
        code = bytes.fromhex('b801000000c21800')
        self.assertEqual(emulate_pure_function(code, 0x410C60, [0.0] * 6), 1)

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'optional Unicorn is not installed')
    def test_unmapped_calls_cannot_leave_the_pure_function_sandbox(self):
        with self.assertRaises(RuntimeError):
            emulate_pure_function(bytes.fromhex('e800000001c3'), 0x410C60, [0.0] * 6)


if __name__ == '__main__':
    unittest.main()
