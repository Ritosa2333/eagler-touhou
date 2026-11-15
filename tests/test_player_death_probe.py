import importlib
import importlib.util
import json
import math
import os
import struct
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))
ORIGINAL = os.environ.get('TH128_BULLET_EXE')
TIMER = [17, 9, 0x41100000, 0x12345678, 0]


class DeathProbeSafetyTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('player_death_probe'),
                             'bounded original-byte player death probe is not implemented')
        return importlib.import_module('player_death_probe')

    def test_private_report_destination_respects_task_ownership(self):
        module = self.module()
        accepted = ROOT / 'local/research/player-death-fixtures.json'
        self.assertEqual(module.report_destination(accepted), accepted.resolve())
        for path in (ROOT / 'README.md', ROOT / 'local/research/player-hit-fixtures.json',
                     ROOT / 'local/research/nested/player-death-x.json', Path('E:/gee/player-death.json')):
            with self.subTest(path=path), self.assertRaises(ValueError):
                module.report_destination(path)

    def test_float_and_timer_inputs_are_explicitly_bounded(self):
        module = self.module()
        for value in (10**400, -(10**400), math.nan, math.inf, True, 65537):
            with self.subTest(value=str(value)[:25]), self.assertRaises(ValueError):
                module.position((value, 0, 0))
        for values in ([0] * 4, [0, 0, 0x7f800000, 0, 0], [2147483648, 0, 0, 0, 0]):
            with self.subTest(values=values), self.assertRaises(ValueError):
                module.timer_bytes(values)


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'), 'provide pinned EXE and local Unicorn')
class DeathOriginalTests(DeathProbeSafetyTests):
    def native(self):
        return self.module().NativeDeath(ORIGINAL)

    def test_first_actual_calls_have_no_early_scalar_reset(self):
        native = self.native()
        for stage, site, target, delta in ((0, '0x0043cddf', '0x00427260', -10000),
                                           (1, '0x0043cde9', '0x00427300', 2500)):
            result = native.commit_call(stage)
            self.assertEqual(result['stop'], {'reason': 'unsupported_call', 'address': site, 'target': target})
            self.assertEqual(result['argument'], delta)
            self.assertFalse(result['completedNativeFunction'])

    def test_actual_motivation_helper_is_complete_only_in_nonoverflowing_death_domain(self):
        native = self.native()
        for value, flag, expected, expected_flag in ((10000, 0, 0, 0), (9999, 0, -1, 1),
                (-1, 0, -10001, 1), (200000, 0, 100000, 0), (123, 1, 123, 1),
                (-2147483648, -1, -2147483648, -1)):
            result = native.motivation(value, flag)
            self.assertEqual((result['valueAfter'], result['flagAfter']), (expected, expected_flag))
            self.assertEqual(result['stop']['reason'], 'returned')
            self.assertTrue(result['completedIndependentHelper'])
        with self.assertRaises(ValueError):
            native.motivation(-2147483648, 0)

    def test_charge_award_strict_crossings_include_negative_truncation_buckets(self):
        native = self.native()
        for value, expected, sound in ((7499, 9999, False), (7500, 10000, True),
                (9999, 12499, True), (10000, 12500, False), (29999, 30000, True),
                (30000, 30000, False), (-10000, -7500, True), (-12500, 0, False)):
            with self.subTest(value=value):
                result = native.charge(value)
                self.assertEqual(result['valueAfter'], expected)
                self.assertEqual(result['stop']['reason'], 'unsupported_call' if sound else 'returned')
                self.assertEqual(result['soundRequired'], sound)
                if sound:
                    self.assertEqual(result['soundId'], 72)
        with self.assertRaises(ValueError):
            native.charge(2147481148)

    def test_after_awards_and_state_two_timer_writes_stop_before_real_animation(self):
        native = self.native()
        for selector in (-1, 0, 2):
            result = native.after_awards(selector, 17)
            self.assertEqual(result['powerAfter'], 10000)
            self.assertEqual(result['stop']['reason'], 'boundary' if selector < 0 else 'unsupported_call')
        for flags in (0, 1, 2, 0xffffffff):
            timer = TIMER[:-1] + [flags]
            result = native.state_two_setup(timer, timer)
            self.assertEqual(result['stateAfter'], 2)
            clock = 0x12345678 if flags & 1 else 0x4b6f58
            self.assertEqual(result['stateTimerAfter'], [-1, 0, 0, clock, flags | 1])
            self.assertEqual(result['protectionTimerAfter'], [179, 180, 0x43340000, clock, flags | 1])
            self.assertEqual(result['stop']['target'], '0x004593b0')

    def test_cleanup_mutates_only_current_record_before_unknown_handle_resolution(self):
        native = self.native()
        for primary, secondary, phase, expected_handle in ((0, 0, 0, 0), (17, 22, 0, 17),
                                                           (0, 22, 1, 22)):
            result = native.cleanup_record(91, primary, secondary)
            self.assertEqual(result['activeAfter'], 0)
            self.assertEqual(result['requiredHandle'], expected_handle)
            self.assertEqual(result['phase'], phase)
            self.assertEqual(result['stop']['reason'], 'boundary')
            self.assertFalse(result['completedNativeFunction'])

    def test_state_two_does_not_respawn_until_thirty_and_negative_resource_requires_scene(self):
        native = self.native()
        waiting = native.respawn_gate(29, -1, False, 0, (-133.25, 7.5, .1))
        self.assertEqual(waiting['stop']['reason'], 'boundary')
        for resource, mode, target in ((-1, 1, '0x00437050'), (-1, 0, '0x00438030'),
                                       (0, 0, '0x0043df90')):
            result = native.respawn_gate(30, resource, True, mode, (-133.25, 7.5, .1))
            self.assertEqual(result['stop']['target'], target)
            self.assertEqual(result['stateAfter'], 0 if resource >= 0 else 2)

    def test_position_suffix_preserves_old_xyz_and_sets_raw480_with_real_timer_calls(self):
        result = self.native().respawn_position((-133.25, 7.5, .1), TIMER, TIMER)
        self.assertEqual(result['rawAfter'], [0, 61440])
        self.assertEqual(result['positionAfterBits'], list(struct.unpack('<3I', struct.pack('<3f', 0, 480, .1))))
        self.assertEqual(result['savedPositionBits'], list(struct.unpack('<3I', struct.pack('<3f', -133.25, 7.5, .1))))
        self.assertEqual(result['protectionTimerAfter'][:3], [279, 280, 0x438c0000])
        self.assertEqual(result['stateTimerAfter'][:3], [-1, 0, 0])
        self.assertIn('0x00403280', result['instructionTrace'])

    def test_charge_suffix_call_order_and_tail_clamp_have_independent_native_proofs(self):
        native = self.native()
        notify, ui = native.charge_suffix(0), native.charge_suffix(1)
        self.assertEqual(notify['stop']['target'], '0x00424b80')
        self.assertEqual(notify['requestWords'][4:7], [2, 2, 0])
        self.assertEqual(ui['stop']['target'], '0x00425820')
        for scalar, expected in ((-2147482624, -1024), (-1, -1024), (0, -1024),
                                  (1024, 0), (2048, 1024), (2147483647, 1024)):
            result = native.tail(0, 0, 17, False, 0, scalar)
            self.assertEqual(result['scalar30After'], expected)
            self.assertEqual(result['field438f8After'], 0)
            self.assertEqual(result['stop']['reason'], 'returned')

    def test_original_byte_fixture_builder_records_independent_suffixes_without_fake_calls(self):
        module = self.module()
        self.assertTrue(callable(getattr(module, 'build_report', None)),
                        'original-byte death fixture builder is not implemented')
        report = module.build_report(self.native())
        self.assertEqual(report['schema'], 'th128-original-byte-player-death-fixtures/1')
        self.assertFalse(report['fakeCallReturns'])
        self.assertFalse(report['completedNativeFunction'])
        self.assertGreaterEqual(report['caseCount'], 200)
        for sample in report['cases']:
            with self.subTest(sample=sample['id']):
                self.assertFalse(sample['completedNativeFunction'])
                self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
                self.assertIn(sample['stop']['reason'], ('returned', 'boundary', 'unsupported_call'))
                if sample['stop']['reason'] == 'unsupported_call':
                    self.assertNotIn(sample['stop']['address'], sample['instructionTrace'])
        golden_path = ROOT / 'local/research/player-death-fixtures.json'
        self.assertTrue(golden_path.is_file(), 'private original-byte golden fixture is required')
        golden = json.loads(golden_path.read_text(encoding='utf-8'))
        for field in ('code', 'constants', 'cases'):
            self.assertEqual(report[field], golden[field], field)


if __name__ == '__main__':
    unittest.main()
