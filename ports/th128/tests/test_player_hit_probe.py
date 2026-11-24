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


class PlayerHitProbeSafetyTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('player_hit_probe'),
                             'bounded original-byte player hit probe is not implemented')
        return importlib.import_module('player_hit_probe')

    def test_report_path_only_allows_owned_private_evidence(self):
        module = self.module()
        accepted = ROOT / 'local/research/player-hit-fixtures.json'
        self.assertEqual(module.report_destination(accepted), accepted.resolve())
        for path in (ROOT / 'README.md', ROOT / 'local/research/player-native-fixtures.json',
                     ROOT / 'local/research/nested/player-hit-x.json', Path('E:/gee/player-hit-fixtures.json')):
            with self.subTest(path=path), self.assertRaises(ValueError):
                module.report_destination(path)

    def test_explicit_input_domains_are_bounded(self):
        module = self.module()
        for value in (10**400, -(10**400), math.nan, math.inf, True, 65537):
            with self.subTest(value=str(value)[:25]), self.assertRaises(ValueError):
                module.position((value, 0, 0))
        for timer in ([0] * 4, [0, 0, 0x7f800000, 0, 0], [0, 0, 0x7fc00000, 0, 0],
                      [2147483648, 0, 0, 0, 0], [0, -2147483649, 0, 0, 0]):
            with self.subTest(timer=timer), self.assertRaises(ValueError):
                module.timer_bytes(timer)


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'), 'provide pinned EXE and local Unicorn')
class PlayerHitOriginalTests(PlayerHitProbeSafetyTests):
    def native(self):
        return self.module().NativePlayerHit(ORIGINAL)

    def test_original_entry_sets_state_four_before_first_unknown_allocation(self):
        result = self.native().entry(1, (-133.25, 7.5, .1))
        self.assertEqual(result['stateAfter'], 4)
        self.assertEqual(result['stop'], {'reason': 'unsupported_call', 'address': '0x0043d11b',
                                          'target': '0x004670b0'})
        expected = struct.unpack('<3I', struct.pack('<3f', 186.75, 23.5, .1))
        self.assertEqual(result['deferredPositionBits'], list(expected))
        self.assertEqual(result['requestWords'][3], 83)
        self.assertFalse(result['completedNativeFunction'])

    def test_confirmed_overlap_gates_have_complete_suppressed_or_counter_returns(self):
        native = self.native()
        for state, flags, counter, present, control, expected in (
                (2, 0, 0, False, 0, 0), (3, 0, 0, False, 0, 0), (4, 0, 0, False, 0, 0),
                (1, 2, 0, False, 0, 0), (1, 0, 0, True, -1, 0),
                (1, 0, 1, False, 0, 1), (1, 0, 0, False, 0, None),
                (1, 0, -1, False, 0, None)):
            with self.subTest(state=state, flags=flags, counter=counter, control=control):
                result = native.gate(state, flags, counter, present, control)
                self.assertEqual(result['helperReturn'], expected)
                self.assertEqual(result['stop']['reason'], 'returned' if expected is not None else 'unsupported_call')

    def test_timer_suffix_sound_gate_stops_before_counter_reset(self):
        native = self.native()
        for flags in (0, 1, 2, 0xffffffff):
            timer = [17, 9, 0x41100000, 0x12345678, flags]
            with self.subTest(flags=flags):
                result = native.timer_reset(timer, True, 0)
                self.assertEqual(result['stop']['address'], '0x0043d2cc')
                self.assertEqual(result['timerAfter'], [-1, 0, 0,
                    0x12345678 if flags & 1 else 0x4b6f58, flags | 1])
                self.assertEqual(result['requestWords'][3], 2)
        for present, flags in ((False, 0), (True, 0x200), (True, 0xffffffff)):
            result = native.timer_reset(TIMER, present, flags)
            self.assertEqual(result['stop'], {'reason': 'boundary', 'address': '0x0043d2d1'})
            self.assertIsNone(result['requestWords'])

    def test_counter_reset_is_six_not_the_eight_tick_state_four_window(self):
        for flags in (0, 1, 2, 0xffffffff):
            result = self.native().counter_reset([17, 9, 0x41100000, 0x12345678, flags])
            self.assertEqual(result['timerAfter'], [5, 6, 0x40c00000,
                0x12345678 if flags & 1 else 0x4b6f58, flags | 1])
            self.assertEqual(result['stop']['target'], '0x004593b0')
            self.assertEqual(result['requestWords'][3:5], [0, 0x14])

    def test_state_four_window_runs_real_timer_setter_then_stops_at_system_calls(self):
        native = self.native()
        for elapsed, held, present, mode, charge, keys, target in (
                (8, 1, True, 0, 10000, 2, '0x0043cdd0'),
                (7, 1, True, 0, 10000, 2, '0x00458590'),
                (7, 0, True, 0, 10000, 2, '0x00407810'),
                (7, 0, True, 0, 9999, 2, None), (7, 0, True, 1, 10000, 2, None),
                (7, 0, False, 0, 10000, 2, None), (7, 0, True, 0, 10000, 0, None)):
            with self.subTest(elapsed=elapsed, held=held, mode=mode, charge=charge):
                result = native.window(elapsed, held, present, mode, charge, keys)
                self.assertEqual(result['stop'].get('target'), target)
                self.assertEqual(result['stop']['reason'], 'unsupported_call' if target else 'boundary')
                if target in ('0x00458590', '0x00407810'):
                    self.assertIn('0x00403280', result['instructionTrace'])
                if target == '0x00407810':
                    self.assertEqual(result['stateTimerAfter'][:3], [59, 60, 0x42700000])

    def test_independent_tail_returns_use_only_explicit_manager_snapshots(self):
        native = self.native()
        for flags, count, value80, present, mode, expected_flags, expected80 in (
                (0, 0, 7, False, 0, 0, 7), (0xff, 60, 7, False, 0, 0xb9, 0),
                (1, 59, -1, True, 0, 1, -1), (1, -1, -7, True, 1, 0x41, -7)):
            result = native.tail(flags, count, value80, present, mode)
            self.assertEqual(result['stop']['reason'], 'returned')
            self.assertEqual((result['flagsAfter'], result['value80After']), (expected_flags, expected80))
            self.assertFalse(result['completedNativeFunction'])

    def test_normal_init_stops_at_first_detach_without_assigning_unknown_hit_counter(self):
        native = self.native()
        self.assertTrue(callable(getattr(native, 'normal_init', None)), 'normal-init prefix is not implemented')
        result = native.normal_init(TIMER, TIMER, TIMER, 0xffffffff)
        self.assertEqual(result['stateAfter'], 1)
        self.assertEqual(result['flagsAfter'], 0xfffffff8)
        self.assertEqual(result['stateTimerAfter'][:3], [-1, 0, 0])
        self.assertEqual(result['rechargeTimerAfter'][:3], [-2, -1, 0xbf800000])
        self.assertEqual(result['focusTimerAfter'][:3], [-1, 0, 0])
        self.assertEqual(result['stop']['target'], '0x004679b0')
        self.assertNotIn('hitCounter', result['memory'])

    def test_constructor_counter_write_is_conditional_on_an_explicit_initialized_timer(self):
        native = self.native()
        self.assertTrue(callable(getattr(native, 'constructor_counter', None)),
                        'bounded constructor counter proof is not implemented')
        result = native.constructor_counter([17, 9, 0x41100000, 0x4b6f58, 1])
        self.assertEqual(result['timerAfter'], [119, 120, 0x42f00000, 0x4b6f58, 1])
        self.assertEqual(result['stop'], {'reason': 'boundary', 'address': '0x0043aafb'})

    def test_counter_decrement_uses_actual_inactive_shot_loop_and_real_conversion_helper(self):
        native = self.native()
        self.assertTrue(callable(getattr(native, 'counter_step', None)), 'ordinary counter prefix is not implemented')
        for current, fractional, scale, expected in ((120, 120, 1, 119), (6, 6, 1, 5),
                                                    (1, 1, 1, 0), (0, 0, 1, 0), (6, 6, .5, 5)):
            raw = struct.unpack('<I', struct.pack('<f', fractional))[0]
            result = native.counter_step([current - 1, current, raw, 0x4b6f58, 1], scale)
            self.assertEqual(result['timerAfter'][1], expected)
            self.assertEqual(result['stop']['reason'], 'boundary')
            if current > 0:
                self.assertIn('0x0048fac0', result['instructionTrace'])
            self.assertFalse(result['completedNativeFunction'])

    def test_fixture_builder_preserves_actual_stop_provenance_and_golden_regeneration(self):
        module = self.module()
        self.assertTrue(callable(getattr(module, 'build_report', None)),
                        'original-byte player hit fixture builder is not implemented')
        report = module.build_report(self.native())
        self.assertEqual(report['schema'], 'th128-original-byte-player-hit-fixtures/1')
        self.assertFalse(report['fakeCallReturns'])
        self.assertFalse(report['completedNativeFunction'])
        self.assertGreaterEqual(report['caseCount'], 180)
        for sample in report['cases']:
            with self.subTest(sample=sample['id']):
                self.assertFalse(sample['completedNativeFunction'])
                self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
                self.assertIn(sample['stop']['reason'], ('returned', 'boundary', 'unsupported_call'))
                if sample['stop']['reason'] == 'unsupported_call':
                    self.assertNotIn(sample['stop']['address'], sample['instructionTrace'])
        golden_path = ROOT / 'local/research/player-hit-fixtures.json'
        self.assertTrue(golden_path.is_file(), 'private original-byte golden fixture is required')
        golden = json.loads(golden_path.read_text(encoding='utf-8'))
        for field in ('code', 'constants', 'cases'):
            self.assertEqual(report[field], golden[field], field)


if __name__ == '__main__':
    unittest.main()
