import importlib.util
import struct
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from research.player_probe import NativePlayer, ProbeError, Region, emulate, load_actual_shot, report_destination


class SandboxTests(unittest.TestCase):
    def test_leaf_returns_and_preserves_declared_data(self):
        result = emulate([(0x401000, bytes.fromhex('b82a000000c3'))], 0x401000,
                         [Region('value', 0x300000, b'KEEP', False)])
        self.assertEqual(result['registers']['eax'], 42)
        self.assertEqual(result['memory']['value'], b'KEEP')
        self.assertEqual(result['stop'], 0x200000)

    def test_missing_direct_and_indirect_callouts_are_rejected(self):
        for code in ('e8fb0f0000c3', 'ffd0c3'):
            with self.subTest(code=code), self.assertRaisesRegex(ProbeError, 'call'):
                emulate([(0x401000, bytes.fromhex(code))], 0x401000)

    def test_padding_on_a_mapped_page_is_not_declared_memory(self):
        code = bytes.fromhex('a104003000c3')
        with self.assertRaisesRegex(ProbeError, 'memory'):
            emulate([(0x401000, code)], 0x401000,
                    [Region('value', 0x300000, b'KEEP', False)])

    def test_readonly_write_is_rejected(self):
        with self.assertRaisesRegex(ProbeError, 'read.only'):
            emulate([(0x401000, bytes.fromhex('a300003000c3'))], 0x401000,
                    [Region('value', 0x300000, b'KEEP', False)])

    def test_instruction_budget_is_enforced(self):
        with self.assertRaisesRegex(ProbeError, 'budget'):
            emulate([(0x401000, bytes.fromhex('ebfe'))], 0x401000, budget=10)

    def test_jump_into_undeclared_code_is_rejected(self):
        with self.assertRaisesRegex(ProbeError, 'code'):
            emulate([(0x401000, bytes.fromhex('eb08'))], 0x401000)

    def test_reaching_return_trap_without_return_is_not_a_native_return(self):
        with self.assertRaisesRegex(ProbeError, 'return'):
            emulate([(0x401000, bytes.fromhex('e9fbefdfff'))], 0x401000)

    def test_mapping_count_is_bounded_even_for_tiny_regions(self):
        data = [Region(str(index), 0x300000 + index * 4096, b'X') for index in range(257)]
        with self.assertRaisesRegex(ValueError, 'budget'):
            emulate([(0x401000, b'\xc3')], 0x401000, data)

    def test_report_scope_and_invalid_configuration(self):
        self.assertEqual(report_destination(ROOT / 'local/research/player-test.json').name,
                         'player-test.json')
        for destination in (ROOT / 'docs/player-test.json', ROOT / 'local/research/freeze-test.json',
                            Path('E:/player-test.json')):
            with self.assertRaises(ValueError):
                report_destination(destination)
        for budget in (0, -1, 200001, True):
            with self.assertRaises(ValueError):
                emulate([(0x401000, b'\xc3')], 0x401000, budget=budget)


@unittest.skipUnless(importlib.util.find_spec('unicorn') and importlib.util.find_spec('pefile'),
                     'optional native-byte libraries unavailable')
class OriginalPlayerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        executable = Path('E:/gee/妖精大战争/th128.exe')
        if not executable.is_file():
            raise unittest.SkipTest('original Japanese executable unavailable')
        cls.native = NativePlayer(executable)

    def test_direction_priority_including_conflicting_buttons(self):
        expected = [0, 1, 2, 2, 3, 5, 7, 5, 4, 6, 8, 6, 3, 5, 7, 5]
        for nibble, direction in enumerate(expected):
            self.assertEqual(self.native.direction_focus(nibble << 4)['direction'], direction)
            self.assertEqual(self.native.direction_focus((nibble << 4) | 0xfe0f)['direction'], direction)

    def test_focus_requires_all_native_gates_and_hold_threshold_is_twelve(self):
        self.assertFalse(self.native.direction_focus(0, hold_count=11)['focused'])
        self.assertTrue(self.native.direction_focus(0, hold_count=12)['focused'])
        self.assertTrue(self.native.direction_focus(8)['focused'])
        for changes in ({'manager_present': False}, {'manager_active': False}, {'counter_f98': 3}):
            result = self.native.direction_focus(8, hold_count=50, **changes)
            self.assertFalse(result['focused'])
            self.assertEqual(result['cooldown_438d4'], 30)

    def test_original_constructor_converts_actual_shot_speed_floats(self):
        header = struct.pack('<4f', 4.5, 2.0, 3.181980609893799, 1.4142135381698608)
        self.assertEqual(self.native.speeds(header), [576, 256, 407, 181])

    def test_wrong_executable_is_not_treated_as_original_code(self):
        with self.assertRaisesRegex(ValueError, 'SHA'):
            NativePlayer(Path(__file__))

    def test_constants_distinguish_declared_pe_zero_fill_from_unknown_memory(self):
        self.assertEqual(self.native.constant('bss', 0x4d2e2c, 4).data, bytes(4))
        with self.assertRaises(ValueError):
            self.native.constant('not_a_section', 0x100000, 4)

    def test_native_loader_relocates_only_verified_pointer_fields(self):
        source, raw = load_actual_shot()
        result = self.native.relocate_shot(raw)
        self.assertEqual(result['groupCount'], 16)
        self.assertEqual(result['recordCount'], 114)
        self.assertTrue(result['unknownBytesUnchanged'])
        self.assertTrue(source.name == 'pl00a.sht')
        self.assertEqual(self.native.speeds(raw[0x10:0x20]), [576, 256, 407, 181])

    def test_native_velocity_uses_truncated_diagonal_components(self):
        for focused, cardinal, diagonal in ((False, 576, 407), (True, 256, 181)):
            self.assertEqual(self.native.velocity(1, focused), [0, -cardinal])
            self.assertEqual(self.native.velocity(8, focused), [diagonal, diagonal])

    def test_native_bounds_float_mirror_and_signed_wrap(self):
        for location, delta, expected in (((23552, 55296), (1, 1), (23552, 55296)),
                                          ((-23552, 4096), (-1, -1), (-23552, 4096)),
                                          ((0, 51200), (407, -407), (407, 50793)),
                                          ((2147483647, 2147483647), (1, 1), (-23552, 4096))):
            result = self.native.position(*location, *delta)
            self.assertEqual(result['raw'], list(expected))
            self.assertEqual(result['pixels'], [value / 128 for value in expected])

    def test_shot_start_distinguishes_pressed_from_holding_and_rapid(self):
        self.assertEqual(self.native.shot_start(1, 0), 'inactive')
        self.assertEqual(self.native.shot_start(1, 1), 'start')
        self.assertEqual(self.native.shot_start(512, 0), 'start')
        self.assertEqual(self.native.shot_start(513, 0), 'inactive')
        self.assertEqual(self.native.shot_start(513, 1), 'start')
        self.assertEqual(self.native.shot_start(512, 0, current=0), 'active')

    def test_shot_cycle_thresholds_use_original_timer_helpers(self):
        self.assertEqual(self.native.shot_cycle(0, 0, 0)['counts'], [0, 1])
        self.assertEqual(self.native.shot_cycle(1, 1, 5)['counts'], [5, 6])
        self.assertEqual(self.native.shot_cycle(1, 1, 6)['counts'], [-1, 0])
        self.assertEqual(self.native.shot_cycle(1, 0, 14)['counts'], [-2, -1])
        self.assertEqual(self.native.shot_cycle(512, 0, 14)['counts'], [-1, 0])
        self.assertEqual(self.native.shot_cycle(513, 0, 14)['counts'], [-2, -1])

    def test_native_record_scheduler_uses_signed_remainder(self):
        self.assertTrue(self.native.record_due(3, 0, 6))
        self.assertFalse(self.native.record_due(3, 0, 7))
        self.assertTrue(self.native.record_due(3, -1, -4))
        with self.assertRaises(ValueError):
            self.native.record_due(0, 0, 0)


if __name__ == '__main__':
    unittest.main()
