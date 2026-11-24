import importlib
import importlib.util
import inspect
import math
import os
import struct
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools' / 'research'))
ORIGINAL = os.environ.get('TH128_BULLET_EXE')


def bits(value):
    return struct.unpack('<I', struct.pack('<f', value))[0]


def emitter(extension_type=2, parameter=1):
    block = bytearray(0x380)
    struct.pack_into('<2i', block, 0, 8, 10)
    struct.pack_into('<3f', block, 8, -128, -32, 0)
    struct.pack_into('<2i', block, 0x38, parameter, -999999)
    struct.pack_into('<2f', block, 0x28, -999999, -999999)
    struct.pack_into('<2i', block, 0x48, extension_type, 1)
    struct.pack_into('<3i', block, 0x36c, 35, 22, 40)
    return bytes(block)


class BulletLifecycleSafetyTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_lifecycle_probe'),
                             'original-byte bullet lifecycle probe is not implemented')
        return importlib.import_module('bullet_lifecycle_probe')

    def test_report_stays_private_and_does_not_overwrite_another_probe(self):
        module = self.module()
        accepted = ROOT / 'local/research/bullet-lifecycle-fixtures.json'
        self.assertEqual(module.report_destination(accepted), accepted.resolve())
        for path in (ROOT / 'README.md', ROOT / 'local/research/bullet-fixtures.json',
                     ROOT / 'local/research/nested/bullet-lifecycle-x.json', Path('E:/gee/bullet-lifecycle.json')):
            with self.subTest(path=path), self.assertRaises(ValueError):
                module.report_destination(path)

    def test_vector_budget_rejects_arbitrarily_large_integers_without_overflow(self):
        module = self.module()
        for value in (10**400, -(10**400), math.nan, math.inf, True):
            with self.subTest(value=str(value)[:30]), self.assertRaises(ValueError):
                module.vector((0, value), 2)


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'), 'provide original executable and Unicorn')
class BulletLifecycleOriginalTests(BulletLifecycleSafetyTests):
    def native(self):
        return self.module().NativeLifecycle(ORIGINAL)

    def test_actual_manager_aim_prefix_matches_finite_cardinals_and_quadrants(self):
        native = self.native()
        for player in ((0, 400), (100, 0), (-100, 0), (0, -100), (30, 40), (-30, 40),
                       (-30, -40), (30, -40), (0, 0)):
            with self.subTest(player=player):
                result = native.aim_prefix((0, 0), player)
                self.assertEqual(result['stop'], {'reason': 'boundary', 'address': '0x0040a910'})
                expected = math.atan2(player[1], player[0]) if player != (0, 0) else math.pi / 2
                self.assertEqual(result['aimBits'], bits(expected))
                self.assertFalse(result['completedEmission'])
                self.assertIn('0x0043c3a0', result['instructionTrace'])

    def test_type_two_spawn_tail_backshifts_four_velocities_and_advances_extension(self):
        result = self.native().spawn_tail(emitter(), (-128, -32, .125), (.25, .75, -.125))
        self.assertEqual(result['stop'], {'reason': 'boundary', 'address': '0x004099df'})
        self.assertFalse(result['completedEmission'])
        self.assertTrue(result['requiresAnimationSetup'])
        state = result['state']
        self.assertEqual(state['positionBits'], [bits(-129), bits(-35), bits(.625)])
        self.assertEqual((state['motionState'], state['animationInterrupt'], state['extensionIndex']), (2, 8, 1))
        self.assertEqual(state['activeExtensions'], 0)
        self.assertEqual(state['emitterFlags'], 35)
        self.assertEqual(state['sizeBits'], [bits(2.4), bits(2.4), bits(16)])
        self.assertEqual(state['transformSound'], 40)

    def test_type_eight_tail_reasserts_its_visual_flag(self):
        native = self.native()
        self.assertIn('post_animation_flags', inspect.signature(native.spawn_tail).parameters,
                      'spawn-tail flag invariant needs native-byte regression inputs')
        for flags in (0, 3, 0xffffffff):
            with self.subTest(flags=flags):
                result = native.spawn_tail(emitter(), (0, 0, 0), (0, 0, 0), post_animation_flags=flags)
                self.assertEqual(result['stop']['reason'], 'boundary')
                self.assertEqual(result['state']['flags'], flags | 0x10)
                self.assertIn('0x00409889', result['instructionTrace'])

    def test_saved_movement_stores_cannot_be_collapsed_into_one_expression(self):
        native = self.native()
        smallest = struct.unpack('<f', struct.pack('<I', 1))[0]
        for state, position, speed, scale, expected in ((1, -.07, .1, .7, 0), (2, 0, smallest, 2.9, 2)):
            with self.subTest(state=state):
                result = native.motion_prefix((position, 0, 0), (speed, 0, 0), state, scale)
                self.assertEqual(result['stop']['reason'], 'boundary')
                self.assertEqual(result['positionBits'][0], expected)
                values = struct.unpack('<3f', struct.pack('<3f', position, speed, scale))
                collapsed = bits(values[0] + values[1] * values[2] * (.5 if state == 2 else 1))
                self.assertNotEqual(result['positionBits'][0], collapsed)

    def test_spawn_interrupt_truncates_and_wraps_as_a_sixteen_bit_word(self):
        native = self.native()
        for parameter in (-10, -7, -1, 0, 1, 32767, 65535, 65536):
            with self.subTest(parameter=parameter):
                result = native.spawn_tail(emitter(parameter=parameter), (0, 0, 0), (0, 0, 0))
                self.assertEqual(result['stop']['reason'], 'boundary')
                self.assertEqual(result['state']['animationInterrupt'], (parameter + 7) & 65535)

    def test_non_type_two_uses_interrupt_two_without_claiming_extension_execution(self):
        result = self.native().spawn_tail(emitter(0), (1, 2, 3), (.25, .5, .75))
        self.assertEqual(result['stop']['reason'], 'boundary')
        self.assertEqual(result['state']['positionBits'], [bits(1), bits(2), bits(3)])
        self.assertEqual(result['state']['animationInterrupt'], 2)
        self.assertEqual(result['state']['motionState'], 1)
        self.assertEqual(result['state']['extensionIndex'], 0)

    def test_unknown_animation_callback_stops_without_an_invented_return(self):
        result = self.native().spawn_tail(emitter(), (0, 0, 0), (0, 0, 0), callback=0x420000)
        self.assertEqual(result['stop']['reason'], 'unsupported_call')
        self.assertFalse(result['completedEmission'])

    def test_spawn_motion_prefix_is_half_speed_but_active_motion_is_full_speed(self):
        native = self.native()
        for state, scale in ((1, 1), (2, .5), (2, 1), (2, 2)):
            with self.subTest(state=state, scale=scale):
                result = native.motion_prefix((1, 2, 3), (.5, 1, -.25), state, scale)
                self.assertEqual(result['stop']['reason'], 'boundary')
                speed = scale * (.5 if state == 2 else 1)
                self.assertEqual(result['positionBits'], [bits(1 + .5 * speed), bits(2 + speed), bits(3 - .25 * speed)])
                self.assertFalse(result['completedTick'])

    def test_nonfinite_or_unbounded_inputs_are_rejected_before_emulation(self):
        native = self.native()
        for value in (math.nan, math.inf, -math.inf):
            with self.subTest(value=value), self.assertRaises(ValueError):
                native.aim_prefix((0, value), (0, 1))
        with self.assertRaises(ValueError):
            native.spawn_tail(bytes(0x381), (0, 0, 0), (0, 0, 0))
        with self.assertRaises(ValueError):
            native.motion_prefix((0, 0, 0), (0, 0, 0), 6, 1)


if __name__ == '__main__':
    unittest.main()
