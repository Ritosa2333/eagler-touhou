import importlib.util
import sys
import unittest
import tempfile
from unittest.mock import patch
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))

from inspect_native import find_string_addresses, scan_references, main, inspect
from th128_assets.formats import FormatError


class NativeInspectionTests(unittest.TestCase):
    @unittest.skipUnless(importlib.util.find_spec('pefile'), 'optional pefile is not installed')
    def test_oversized_executables_fail_before_an_unbounded_read(self):
        with tempfile.TemporaryDirectory() as temporary:
            source = Path(temporary) / 'large.exe'
            source.write_bytes(b'x' * (16 * 1024 * 1024 + 1))
            with patch.object(Path, 'read_bytes', side_effect=AssertionError('unbounded read')):
                with self.assertRaises(FormatError):
                    inspect(source)

    def test_report_cannot_overwrite_input_or_sibling_original_files(self):
        with tempfile.TemporaryDirectory() as temporary:
            original = Path(temporary) / 'original'
            original.mkdir()
            source = original / 'game.exe'
            source.write_bytes(b'original executable fixture')
            with patch('inspect_native.inspect', return_value={'references': {}}):
                self.assertEqual(main(['--exe', str(source), '--report', str(source)]), 1)
                self.assertEqual(main(['--exe', str(source), '--report', str(original / 'score.dat')]), 1)
            self.assertEqual(source.read_bytes(), b'original executable fixture')
            self.assertFalse((original / 'score.dat').exists())

    def test_only_complete_null_terminated_strings_are_matched(self):
        data = b'ice.anm\0ice.anmX\0title.anm\0'
        self.assertEqual(find_string_addresses(data, 0x500000, ['ice.anm']), {'ice.anm': [0x500000]})

    @unittest.skipUnless(importlib.util.find_spec('capstone'), 'optional capstone is not installed')
    def test_x86_absolute_string_and_memory_references(self):
        code = bytes.fromhex('6800104000a16c4d4b00c3')
        references = scan_references(code, 0x401000, {'asset': [0x401000], 'power': [0x4B4D6C]})
        self.assertEqual(references['asset'][0]['address'], '0x00401000')
        self.assertEqual(references['power'][0]['address'], '0x00401005')
        self.assertEqual(references['power'][0]['mnemonic'], 'mov')


if __name__ == '__main__':
    unittest.main()
