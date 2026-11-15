import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))


class IceBirthProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_review_tiny_negative_bucket_is_observed_not_predicted(self):
        module = self.module()
        native = module.NativeIceBirth(os.environ['TH128_BULLET_EXE'])
        try:
            sample = native.birth(position=(-2**-46, 359, .1))
            self.assertEqual(sample.get('observedBucket'), 38,
                             'native bucket must come from executed sentinel arithmetic, not duplicated double math')
            self.assertEqual(sample['listState']['bucketFirst'], sample['registers']['eax'])
        finally:
            native.image.close()

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_review_negative_native_bucket_stops_before_bucket_access(self):
        module = self.module()
        native = module.NativeIceBirth(os.environ['TH128_BULLET_EXE'])
        try:
            sample = native.birth(position=(-2**-46, -1792, .1))
            self.assertFalse(sample['completedBuilder'], 'out-of-table sentinel must not be presented as a supported returned birth')
            self.assertEqual(sample.get('observedBucket'), -10)
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0041069c'})
        finally:
            native.image.close()

    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('ice_birth_probe'),
                             'bounded original-byte ice builder probe is not implemented')
        return importlib.import_module('ice_birth_probe')

    def test_module_and_owned_destination(self):
        module = self.module()
        destination = ROOT / 'local/research/ice-birth-fixtures.json'
        self.assertEqual(module.report_destination(destination), destination.resolve())
        for destination in (ROOT / 'README.md', ROOT / 'local/research/freeze-fixtures.json',
                            ROOT / 'local/research/nested/ice-birth-fixtures.json',
                            Path('E:/gee/ice-birth.json')):
            with self.subTest(destination=destination), self.assertRaises(ValueError):
                module.report_destination(destination)

    def test_invalid_input_precedes_original_access(self):
        module = self.module()
        native = object.__new__(module.NativeIceBirth)
        for changes in ({'cursor': -1}, {'cursor': 4000}, {'position': (0, 0)},
                        {'position': (float('nan'), 0, 0)}, {'duration': 2**31},
                        {'occupied_run': 4001}, {'mode': True}):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                native.birth(**changes)

    def test_output_preflight_and_state_budgets(self):
        module = self.module()
        with patch.object(module, 'probe', side_effect=AssertionError('probe ran before destination validation')):
            with self.assertRaises(ValueError):
                module.main(['--exe', 'unused', '--output', str(ROOT / 'README.md')])
        with self.assertRaises(ValueError):
            module.bounded_emulate([(0x400000, bytes(8193))], 0x400000, [])
        with self.assertRaises(ValueError):
            module.bounded_emulate([], 0x400000,
                                   [{'bytes': bytes(524289)}])

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_actual_builder_and_required_activation_calls(self):
        report = self.module().probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['nativeCpuTrace'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertTrue(report['executable']['unchanged'])
        self.assertGreaterEqual(report['caseCount'], 20)
        self.assertTrue(any(sample['stop']['reason'] == 'returned' for sample in report['cases']))
        self.assertTrue(any(sample['stop']['reason'] == 'boundary' for sample in report['cases']))
        self.assertEqual(report['layout']['slotCount'], 4000)
        self.assertEqual(report['layout']['objectBytes'], 0x68)
        first = next(sample for sample in report['cases'] if sample['id'] == 'first')
        self.assertEqual(first['next']['animation50'], 0)
        self.assertEqual(first['next']['animation54'], 0)
        self.assertEqual(first['next']['maskSelector'], 0)
        self.assertEqual(first['next']['lifetime'][:2], [99, 100])
        for sample in report['cases']:
            self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
            self.assertEqual(sample['completedBuilder'], sample['stop']['reason'] == 'returned')
            self.assertTrue(all(span['bytes'] <= 65536 for span in sample['declaredData']))
            self.assertLessEqual(sum(span['bytes'] for span in sample['declaredData']), 524288)

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_required_effects_stop_at_real_calls_without_allocator_stubs(self):
        report = self.module().probe(os.environ['TH128_BULLET_EXE'])
        self.assertTrue('requiredCallPrefixes' in report, 'required activation calls need executed original-byte prefixes')
        effect, sound = report['requiredCallPrefixes']
        self.assertEqual(effect['stop']['target'], '0x00407ec0')
        self.assertEqual(effect['registers']['eax'], 23)
        self.assertEqual(effect['script'], 33)
        self.assertEqual(sound['stop']['target'], '0x004584a0')
        self.assertEqual(sound['registers']['eax'], 47)
        self.assertEqual(report['animation'].get('nativeResourceSlot'), 36,
                         'factory layer23 must not be mistaken for the ice ANM resource slot')
        self.assertEqual(report['animation'].get('activationLayer'), 23)
        self.assertEqual(report['effectRegistration']['helper'], '0x00466d60')
        self.assertEqual(report['effectRegistration']['listHeadOffset'], 0x8b9704)
        self.assertEqual(report['effectRegistration']['insertion'], 'primary-tail')


if __name__ == '__main__':
    unittest.main()
