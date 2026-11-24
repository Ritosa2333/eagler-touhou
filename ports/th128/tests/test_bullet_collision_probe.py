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


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


class CollisionProbeSafetyTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_collision_probe'),
                             'bounded original-byte collision probe is not implemented')
        return importlib.import_module('bullet_collision_probe')

    def test_report_destination_cannot_overwrite_other_evidence_or_write_original_drive(self):
        module = self.module()
        accepted = ROOT / 'local/research/bullet-collision-fixtures.json'
        self.assertEqual(module.report_destination(accepted), accepted.resolve())
        for path in (ROOT / 'README.md', ROOT / 'local/research/bullet-fixtures.json',
                     ROOT / 'local/research/nested/bullet-collision-x.json',
                     Path('E:/gee/bullet-collision-fixtures.json')):
            with self.subTest(path=path), self.assertRaises(ValueError):
                module.report_destination(path)

    def test_float_domain_is_explicit_and_rejects_nonfinite_tiny_and_unbounded_values(self):
        module = self.module()
        self.assertEqual(module.vector((0, -65536), 2), struct.pack('<2f', 0, -65536))
        for value in (10**400, -(10**400), math.nan, math.inf, True, 65537, 2**-17):
            with self.subTest(value=str(value)[:30]), self.assertRaises(ValueError):
                module.vector((0, value), 2)


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'),
                     'provide TH128_BULLET_EXE and Unicorn for read-only original-byte probes')
class CollisionOriginalTests(CollisionProbeSafetyTests):
    def native(self):
        return self.module().NativeCollision(ORIGINAL)

    def test_rectangle_uses_full_dimensions_and_inclusive_touch(self):
        native = self.native()
        for center, expected in (((3, 0), 1), ((3.000000238418579, 0), 2),
                                 ((26, 0), 2), ((26.000001907348633, 0), 0)):
            with self.subTest(center=center):
                result = native.rectangle(center, (2, 2), bounds=(-2, -2, 2, 2), counter=1)
                self.assertEqual(result['stop']['reason'], 'returned')
                self.assertEqual(result['helperReturn'], expected)
                self.assertEqual(result['hitBoundsBits'][0], bits(center[0] - 1))
        self.assertEqual(native.rectangle((0, 0), (2.4, 4), counter=1)['hitBoundsBits'],
                         [bits(-1.2), bits(-2), bits(1.2), bits(2)])

    def test_circle_is_strict_squared_sum_not_summed_radii_and_graze_is_strict(self):
        native = self.native()
        for center, expected in (((4, 0), 1), ((5, 0), 2), ((6, 0), 2),
                                 ((43, 3), 2), ((43, 4), 0)):
            with self.subTest(center=center):
                result = native.circle(center, 4, player_radius=3, counter=1)
                self.assertEqual(result['stop']['reason'], 'returned')
                self.assertEqual(result['helperReturn'], expected)
                self.assertEqual(result['distanceSquaredBits'], bits(center[0]**2 + center[1]**2))
        result = native.circle((100, 100), 101, player_radius=3)
        self.assertEqual(result['dividedGrazeSizeBits'], bits(101 / 2.5))
        self.assertEqual(result['grazeSizeBits'], bits(101 / 2.5))

    def test_hit_gates_do_not_suppress_geometry_miss_graze(self):
        native = self.native()
        for method, size in ((native.rectangle, (2, 2)), (native.circle, 4)):
            for gates in ({'state': 2}, {'state': 3}, {'state': 4}, {'flags': 2},
                          {'control_present': True, 'control_counter': -1}):
                with self.subTest(method=method.__name__, gates=gates):
                    hit = method((0, 0), size, **gates)
                    graze = method((10, 0), size, **gates)
                    self.assertEqual(hit['helperReturn'], 0)
                    self.assertEqual(graze['helperReturn'], 2)

    def test_retained_x87_significand_cannot_be_replaced_with_binary64_thresholds(self):
        native = self.native()
        for profile_bits, expected in ((0x37800000, 2), (0x37800001, 1)):
            profile_radius = struct.unpack('<f', struct.pack('<I', profile_bits))[0]
            with self.subTest(profile_bits=profile_bits):
                result = native.circle((65536, 0), 65536, player_radius=profile_radius, counter=1)
                self.assertEqual(result['distanceSquaredBits'], 0x4f800000)
                self.assertEqual(result['helperReturn'], expected)

    def test_rect_bounds_and_circle_position_are_independent_original_fields(self):
        native = self.native()
        rect = native.rectangle((0, 0), (2, 2), player_position=(100, 100), counter=1)
        circle = native.circle((0, 0), 2.4, player_position=(100, 100), counter=1)
        self.assertEqual(rect['helperReturn'], 1)
        self.assertEqual(circle['helperReturn'], 0)

    def test_geometry_only_original_bytes_need_no_gate_or_control_snapshot(self):
        native = self.native()
        self.assertTrue(callable(getattr(native, 'circle_geometry', None)),
                        'original-byte geometry-only circle entry is not implemented')
        for center, expected in (((100, 100), 'miss'), ((0, 0), 'potential-hit'),
                                 ((10, 0), 'potential-graze')):
            with self.subTest(center=center):
                result = native.circle_geometry(center, 2.4, (0, 0), 2)
                self.assertEqual(result['geometry'], expected)
                self.assertFalse(result['hitGatesRead'])
                self.assertNotIn('0x0043c608', result['instructionTrace'])
                self.assertEqual(set(result['memory']), {'center', 'playerXY', 'profilePointer',
                    'profileRadius', 'playerPointer', '0x004a6910', '0x004a6a00'})
                if expected == 'potential-hit':
                    self.assertEqual(result['stop'], {'reason': 'boundary', 'address': '0x0043c608'})
                    self.assertIsNone(result['helperReturn'])
                else:
                    self.assertEqual(result['stop']['reason'], 'returned')

    def test_damage_is_an_explicit_original_call_stop_not_a_fabricated_return(self):
        native = self.native()
        for method, size, location in ((native.rectangle, (2, 2), '0x0043c4e5'),
                                       (native.circle, 2.4, '0x0043c648')):
            with self.subTest(method=method.__name__):
                result = method((0, 0), size, counter=0)
                self.assertEqual(result['stop'], {'reason': 'unsupported_call', 'address': location,
                                                  'target': '0x0043d0c0'})
                self.assertIsNone(result['helperReturn'])
                self.assertEqual(result['decision'], 'damage-call-required')
                self.assertNotIn(location, result['instructionTrace'])

    def test_actual_update_prefix_selects_type8_circle_and_preserves_unknown_graze_call(self):
        native = self.native()
        result = native.update_prefix('half-step', (10, 0), (2.4, 2.4), bullet_flags=0x13,
                                      age=8, counter=1)
        self.assertEqual(result['stop'], {'reason': 'unsupported_call', 'address': '0x00408883',
                                          'target': '0x0043e090'})
        self.assertEqual(result['route'], 'circle')
        self.assertIn('0x0040880f', result['instructionTrace'])
        self.assertIn('0x0043c570', result['instructionTrace'])
        self.assertEqual(result['effectiveSizeBits'], [bits(2.4), bits(2.4)])
        self.assertEqual(result['flagsAfter'], 0x13)
        self.assertFalse(result['completedTick'])

    def test_update_gates_age_only_at_half_step_and_does_not_set_graze_flag_before_call(self):
        native = self.native()
        half = native.update_prefix('half-step', (10, 0), (2.4, 2.4), age=7)
        full = native.update_prefix('full-step', (10, 0), (2.4, 2.4), age=7)
        already = native.update_prefix('full-step', (10, 0), (2.4, 2.4), bullet_flags=0x17)
        self.assertEqual(half['intent'], 'no-helper')
        self.assertEqual(full['intent'], 'graze-call-required')
        self.assertEqual(already['intent'], 'helper-returned')
        self.assertEqual(already['helperReturn'], 2)
        self.assertEqual(already['flagsAfter'], 0x17)

    def test_fixtures_are_byte_backed_and_never_complete_unported_calls(self):
        module, native = self.module(), self.native()
        self.assertTrue(callable(getattr(module, 'build_report', None)),
                        'bounded original-byte collision fixture builder is not implemented')
        report = module.build_report(native)
        self.assertEqual(report['schema'], 'th128-original-byte-bullet-collision-fixtures/1')
        self.assertGreaterEqual(report['caseCount'], 120)
        self.assertEqual(len({sample['id'] for sample in report['cases']}), report['caseCount'])
        self.assertTrue(report['executable']['unchanged'])
        self.assertFalse(report['fakeCallReturns'])
        golden = json.loads((ROOT / 'local/research/bullet-collision-fixtures.json').read_text(encoding='utf-8'))
        self.assertEqual(report['code'], golden['code'])
        self.assertEqual(report['constants'], golden['constants'])
        for observed, saved in zip(report['cases'], golden['cases'], strict=True):
            self.assertEqual(observed, saved, observed['id'])
        self.assertEqual(report['geometryCaseCount'], golden['geometryCaseCount'])
        for observed, saved in zip(report['geometryCases'], golden['geometryCases'], strict=True):
            self.assertEqual(observed, saved, observed['id'])
        for sample in report['cases']:
            with self.subTest(sample=sample['id']):
                self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
                self.assertFalse(sample['completedTick'])
                self.assertIn(sample['stop']['reason'], ('returned', 'boundary', 'unsupported_call'))
                if sample['stop']['reason'] == 'unsupported_call':
                    self.assertNotIn(sample['stop']['address'], sample['instructionTrace'])
                    self.assertIn(sample['stop']['target'], ('0x0043d0c0', '0x0043e090', '0x0040d2a0'))


if __name__ == '__main__':
    unittest.main()
