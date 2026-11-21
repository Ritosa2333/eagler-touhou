import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))


class IceMaskProbeTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('ice_mask_probe'),
                             'bounded original-byte ice mask probe is not implemented')
        return importlib.import_module('ice_mask_probe')

    def test_owned_output_and_missing_module(self):
        module = self.module()
        destination = ROOT / 'local/research/ice-mask-fixtures.json'
        self.assertEqual(module.report_destination(destination), destination.resolve())
        for destination in (ROOT / 'README.md', ROOT / 'local/research/ice-birth-fixtures.json',
                            ROOT / 'local/research/nested/ice-mask-fixtures.json', Path('E:/gee/ice-mask.json')):
            with self.subTest(destination=destination), self.assertRaises(ValueError):
                module.report_destination(destination)

    def test_invalid_inputs_before_original_access(self):
        module = self.module()
        native = object.__new__(module.NativeIceMask)
        for changes in ({'height': 449}, {'words_per_row': 13}, {'height': -1},
                        {'position': (float('nan'), 0)}, {'position': (2**25, 0)},
                        {'position': (0,)}, {'height': 448, 'words_per_row': 12},
                        {'operation': 'unknown'}, {'height': True}, {'bitmap': bytes(3)},
                        {'mask': bytes(21503)}, {'mask': bytes(21505)}):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                native.raster(**changes)

    def test_finite_tiny_float32_positions_are_preserved_for_native_execution(self):
        module = self.module()
        for position in ((2**-32, 0), (-2**-32, 0), (0, 2**-32), (0, -2**-32),
                         (2**-149, -2**-149), (2**-80, 0)):
            with self.subTest(position=position):
                values, _, _ = module.frame_input(position=position)
                self.assertEqual(values['position'], list(position))

    def test_output_and_execution_preflight_reject_before_any_work(self):
        module = self.module()
        with patch.object(module, 'probe', side_effect=AssertionError('probe ran before output preflight')):
            with self.assertRaises(ValueError):
                module.main(['--exe', 'unused', '--output', str(ROOT / 'README.md')])
        with self.assertRaises(ValueError):
            module.bounded_execute([(0x400000, bytes(8193))], 0x400000, [])
        with self.assertRaises(ValueError):
            module.bounded_execute([], 0x400000, [{'bytes': bytes(131073)}])

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_complete_counter_raster_and_explicit_prefix_boundaries(self):
        report = self.module().probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['nativeCpuTrace'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertTrue(report['executable']['unchanged'])
        self.assertEqual(report['layout']['maskBytes'], 21504)
        self.assertEqual(report['layout']['bits'], 172032)
        counters = {sample['id']: sample for sample in report['counterCases']}
        self.assertEqual(counters['all-zero']['count'], 0)
        self.assertEqual(counters['all-one']['count'], 172032)
        self.assertEqual(counters['bit31']['count'], 1)
        aligned = next(sample for sample in report['rasterCases'] if sample['id'] == 'or-bit31-aligned')
        self.assertTrue('placement' in aligned, 'placement must be observed at the native pre-loop boundary')
        self.assertEqual(aligned['placement'], {'startRow': 0, 'startWord': 0, 'signedRemainder': 0})
        for sample in report['counterCases'] + report['rasterCases']:
            self.assertEqual(sample['stop']['reason'], 'returned')
            self.assertTrue(sample['completedFunction'])
            self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
        for sample in report['attachmentPrefixes']:
            self.assertEqual(sample['stop']['reason'], 'unsupported_call')
            self.assertEqual(sample['stop']['target'], '0x00407ec0')
            self.assertFalse(sample['completedCaller'])
        for sample in report['removalPrefixes']:
            self.assertEqual(sample['stop']['reason'], 'boundary')
            self.assertFalse(sample['completedCaller'])


if __name__ == '__main__':
    unittest.main()
