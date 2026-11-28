import importlib
import importlib.util
import json
import math
import os
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))
ORIGINAL = os.environ.get('TH128_BULLET_EXE')


class GrazeProbeSafetyTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('graze_probe'),
                             'bounded original-byte graze probe is not implemented')
        return importlib.import_module('graze_probe')

    def test_report_path_cannot_overwrite_other_evidence_or_original_drive(self):
        module = self.module()
        accepted = ROOT / 'local/research/graze-fixtures.json'
        self.assertEqual(module.report_destination(accepted), accepted.resolve())
        for path in (ROOT / 'README.md', ROOT / 'local/research/bullet-collision-fixtures.json',
                     ROOT / 'local/research/nested/graze-x.json', Path('E:/gee/graze-fixtures.json')):
            with self.subTest(path=path), self.assertRaises(ValueError):
                module.report_destination(path)

    def test_signed_overflow_domain_and_float_budgets_are_enforced(self):
        module = self.module()
        for value in (10**400, -(10**400), math.nan, math.inf, True, 65537):
            with self.subTest(value=str(value)[:30]), self.assertRaises(ValueError):
                module.position((value, 0, 0))
        for count, power in ((0, 2147483548), (0, 2147483647), (2147483648, 0),
                             (-2147483649, 0), (0, -2147483649), (True, 0)):
            with self.subTest(count=count, power=power), self.assertRaises(ValueError):
                module.scalar_inputs(count, power)
        self.assertEqual(module.scalar_inputs(2147483647, 2147483547), (2147483647, 2147483547))
        self.assertEqual(module.scalar_inputs(-2147483648, -2147483648), (-2147483648, -2147483648))


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'),
                     'provide TH128_BULLET_EXE and local Unicorn')
class GrazeOriginalTests(GrazeProbeSafetyTests):
    def native(self):
        return self.module().NativeGraze(ORIGINAL)

    def test_scalar_thresholds_stop_at_real_first_sound_or_effect_call(self):
        native = self.native()
        for power, updated, target, sound in ((2899, 2999, '0x00407ec0', None),
                (2900, 3000, '0x00458410', 73), (2999, 3099, '0x00458410', 73),
                (3000, 3100, '0x00407ec0', None), (9899, 9999, '0x00407ec0', None),
                (9900, 10000, '0x00458410', 74), (9999, 10000, '0x00458410', 74),
                (10000, 10000, '0x00407ec0', None), (10001, 10000, '0x00407ec0', None)):
            with self.subTest(power=power):
                result = native.prefix(5, power, (-133.25, 7.5, .1))
                self.assertEqual(result['updatedGlobals'], {'grazeCount': 6, 'powerRaw': updated})
                self.assertEqual(result['stop']['reason'], 'unsupported_call')
                self.assertEqual(result['stop']['target'], target)
                self.assertEqual(result['firstThresholdSound'], sound)
                self.assertFalse(result['completedNativeFunction'])
                self.assertNotIn(result['stop']['address'], result['instructionTrace'])

    def test_count_cap_does_not_clamp_above_cap_and_signed_values_are_preserved(self):
        native = self.native()
        for count, expected in ((-2147483648, -2147483647), (-1, 0), (99999998, 99999999),
                                 (99999999, 99999999), (100000000, 100000000),
                                 (2147483647, 2147483647)):
            with self.subTest(count=count):
                result = native.prefix(count, -2147483648, (0, 0, 0))
                self.assertEqual(result['updatedGlobals'], {'grazeCount': expected, 'powerRaw': -2147483548})

    def test_independent_effect_and_sound_prefixes_preserve_position_bits_and_call_abi(self):
        native = self.native()
        effect = native.effect_prefix((-133.25, 7.5, .1), bank=0x12345678)
        self.assertEqual(effect['stop'], {'reason': 'unsupported_call', 'address': '0x0043e119',
                                          'target': '0x00407ec0'})
        self.assertEqual(effect['request']['scriptId'], 175)
        self.assertEqual(effect['request']['layer'], 23)
        self.assertEqual(effect['request']['bankValue'], 0x12345678)
        self.assertEqual(effect['request']['bankSource'], 'bullet-manager+8a7f9c')
        sound = native.positional_sound_prefix((-0.0, 7.5, .1))
        self.assertEqual(sound['stop'], {'reason': 'unsupported_call', 'address': '0x0043e129',
                                         'target': '0x004584a0'})
        self.assertEqual(sound['request']['soundId'], 44)
        self.assertEqual(sound['request']['positionXBits'], 0x80000000)
        self.assertFalse(sound['completedNativeFunction'])

    def test_already_grazed_helper_two_reaches_real_continuation_with_no_effect_call(self):
        native = self.native()
        for site, continuation in (('half-step', '0x0040888b'), ('full-step', '0x00408c93')):
            already = native.caller_prefix(site, 2, 0x17)
            self.assertEqual(already['stop'], {'reason': 'boundary', 'address': continuation})
            self.assertEqual(already['flagsAfter'], 0x17)
            self.assertFalse(already['grazeCallRequired'])
            required = native.caller_prefix(site, 2, 0x13)
            self.assertEqual(required['stop']['reason'], 'unsupported_call')
            self.assertEqual(required['stop']['target'], '0x0043e090')
            self.assertEqual(required['flagsAfter'], 0x13)
            self.assertEqual(required['pendingFlagsSetAfterSuccess'], 4)

    def test_fixture_builder_keeps_independent_provenance_and_unknown_calls_unexecuted(self):
        module = self.module()
        self.assertTrue(callable(getattr(module, 'build_report', None)),
                        'original-byte graze fixture builder is not implemented')
        report = module.build_report(self.native())
        self.assertEqual(report['schema'], 'th128-original-byte-graze-fixtures/1')
        self.assertFalse(report['fakeCallReturns'])
        self.assertFalse(report['completedNativeFunction'])
        self.assertGreaterEqual(len(report['planCases']), 150)
        for group in ('planCases', 'effectCases', 'positionalSoundCases', 'callerCases', 'postCallFlagCases'):
            for sample in report[group]:
                with self.subTest(sample=sample['id']):
                    self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
                    self.assertFalse(sample['completedNativeFunction'])
                    self.assertIn(sample['stop']['reason'], ('boundary', 'unsupported_call'))
                    if sample['stop']['reason'] == 'unsupported_call':
                        self.assertNotIn(sample['stop']['address'], sample['instructionTrace'])
        golden_path = ROOT / 'local/research/graze-fixtures.json'
        self.assertTrue(golden_path.is_file(), 'saved original-byte golden fixture is required')
        golden = json.loads(golden_path.read_text(encoding='utf-8'))
        for group in ('code', 'planCases', 'effectCases', 'positionalSoundCases',
                      'callerCases', 'postCallFlagCases'):
            self.assertEqual(report[group], golden[group], group)


if __name__ == '__main__':
    unittest.main()
