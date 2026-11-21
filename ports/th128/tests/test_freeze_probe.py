import importlib
import importlib.util
import os
import struct
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools' / 'research'))
ORIGINAL = os.environ.get('TH128_FREEZE_EXE')


@unittest.skipUnless(importlib.util.find_spec('unicorn'), 'optional Unicorn is not installed')
class FreezeSandboxTests(unittest.TestCase):
    def setUp(self):
        self.assertIsNotNone(importlib.util.find_spec('freeze_probe'), 'freeze sandbox support is missing')
        self.probe = importlib.import_module('freeze_probe')

    def run_code(self, code, **arguments):
        return self.probe.emulate([(0x410000, bytes.fromhex(code))], 0x410000, **arguments)

    def test_synthetic_return_preserves_callee_cleanup(self):
        result = self.run_code('b801000000c20400', arguments=struct.pack('<i', 17))
        self.assertEqual(result['stop']['reason'], 'returned')
        self.assertEqual(result['registers']['eax'], 1)
        self.assertEqual(result['stackDelta'], 8)

    def test_unknown_call_stops_before_call_instruction(self):
        result = self.run_code('e8fbff0f00c3')
        self.assertEqual(result['stop']['reason'], 'unsupported_call')
        self.assertEqual(result['stop']['address'], '0x00410000')
        self.assertEqual(result['stop']['target'], '0x00510000')
        self.assertEqual(result['stackDelta'], 0)
        self.assertEqual(result['instructionsExecuted'], 0)

    def test_only_explicit_original_callee_chunk_is_callable(self):
        result = self.probe.emulate(
            [(0x410000, bytes.fromhex('e8fb000000c3')),
             (0x410100, bytes.fromhex('b82a000000c3'))], 0x410000)
        self.assertEqual(result['stop']['reason'], 'returned')
        self.assertEqual(result['registers']['eax'], 42)

    def test_page_padding_is_not_executable(self):
        result = self.run_code('eb20')
        self.assertEqual(result['stop']['reason'], 'outside_code')

    def test_unmapped_reads_are_diagnostics_not_zero_filled_state(self):
        result = self.run_code('a100003000c3')
        self.assertEqual(result['stop']['reason'], 'fault')

    def test_mapped_page_padding_is_not_implicitly_valid_data(self):
        result = self.run_code('a104003000c3',
                               data=[self.probe.datum('power', 0x300000, 123)])
        self.assertEqual(result['stop']['reason'], 'outside_data')

    def test_instruction_limit_cannot_be_silently_accepted(self):
        result = self.run_code('ebfe', budget=9)
        self.assertEqual(result['stop']['reason'], 'budget_exhausted')
        self.assertEqual(result['instructionsExecuted'], 9)

    def test_data_snapshots_capture_original_instruction_writes(self):
        result = self.run_code('ff0500003000c3',
                               data=[self.probe.datum('power', 0x300000, 2999)])
        self.assertEqual(result['stop']['reason'], 'returned')
        self.assertEqual(result['memory']['power']['beforeI32'], 2999)
        self.assertEqual(result['memory']['power']['afterI32'], 3000)

    def test_readonly_constants_cannot_be_written(self):
        result = self.run_code('ff0500003000c3',
                               data=[self.probe.datum('constant', 0x300000, 10, writable=False)])
        self.assertEqual(result['stop']['reason'], 'readonly_data')

    def test_boundary_stop_does_not_execute_next_instruction(self):
        result = self.run_code('b80100000040c3', stops=[0x410005])
        self.assertEqual(result['stop']['reason'], 'boundary')
        self.assertEqual(result['registers']['eax'], 1)

    def test_report_destination_is_limited_to_freeze_research_evidence(self):
        for destination in (ROOT / 'README.md', ROOT / 'local' / 'research' / 'replay.json',
                            Path('E:/gee/freeze-test.json'),
                            ROOT / 'local' / 'research' / 'freeze-escape' / 'freeze-test.json'):
            with self.subTest(destination=destination), self.assertRaises(ValueError):
                self.probe.report_destination(destination)
        destination = ROOT / 'local' / 'research' / 'freeze-fixtures.json'
        self.assertEqual(self.probe.report_destination(destination), destination.resolve())

    def test_unpinned_executable_is_rejected_before_pe_parsing(self):
        with patch.object(Path, 'read_bytes', return_value=b'not the original executable'):
            with self.assertRaisesRegex(ValueError, 'SHA-256'):
                self.probe.probe(Path('not-a-game.exe'))

    def test_research_directory_alias_cannot_escape_deliverable(self):
        research = ROOT / 'local' / 'research'
        outside = ROOT.parent / 'outside-deliverable'
        original_resolve = Path.resolve

        def resolve_alias(path, *arguments, **keywords):
            if path == research:
                return outside
            if path.parent == research:
                return outside / path.name
            return original_resolve(path, *arguments, **keywords)

        with patch.object(Path, 'resolve', resolve_alias), self.assertRaises(ValueError):
            self.probe.report_destination(research / 'freeze-alias.json')


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'),
                     'set TH128_FREEZE_EXE and provide optional Unicorn for original-byte checks')
class OriginalFreezeRulesTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.probe = importlib.import_module('freeze_probe')
        cls.report = cls.probe.probe(Path(ORIGINAL))
        cls.cases = {record['id']: record for record in cls.report['cases']}

    def case(self, name):
        return self.cases[name]['result']

    def test_power_add_thresholds_clamp_and_absent_lower_clamp(self):
        for name, value, reason in [('power-add-2999-1', 3000, 'unsupported_call'),
                                    ('power-add-9999-1', 10000, 'unsupported_call'),
                                    ('power-add-10000-50', 10000, 'returned'),
                                    ('power-add-0--1', -1, 'returned')]:
            with self.subTest(name=name):
                result = self.case(name)
                self.assertEqual(result['memory']['power']['afterI32'], value)
                self.assertEqual(result['stop']['reason'], reason)

    def test_release_gate_inclusive_45_and_3000(self):
        for timer, power, busy, passes in [(44, 3000, 0, False), (45, 2999, 0, False),
                                           (45, 3000, 0, True), (45, 10000, 1, False)]:
            result = self.case(f'release-{timer}-{power}-{busy}')
            self.assertEqual(result['stop']['reason'], 'unsupported_call' if passes else 'boundary')
            if passes:
                self.assertEqual(result['stop']['target'], '0x00403280')
                self.assertEqual(struct.unpack('<i', bytes.fromhex(result['stackHead'])[:4])[0], -40)

    def test_held_charge_rate_boundaries(self):
        for timer, rate in [(20, 5), (45, 5), (46, 10), (80, 10), (81, 12),
                            (110, 12), (111, 13), (140, 13), (141, 15)]:
            with self.subTest(timer=timer):
                result = self.case(f'held-{timer}')
                self.assertEqual(result['stop']['target'], '0x0041f410')
                self.assertEqual(result['registers']['eax'], rate)
        self.assertEqual(self.case('held-19')['stop']['reason'], 'boundary')

    def test_no_free_slot_still_saves_and_discards_charge(self):
        result = self.case('activation-10000-1-1')
        self.assertEqual(result['stop']['reason'], 'returned')
        self.assertEqual(result['memory']['power']['afterI32'], 0)
        self.assertEqual(result['memory']['savedPower']['afterI32'], 10000)

    def test_activation_allocates_first_free_slot_then_stops_at_real_effect_call(self):
        for slots, selected in [((0, 0), 0), ((1, 0), 1)]:
            result = self.case(f'activation-3000-{slots[0]}-{slots[1]}')
            state = bytes.fromhex(result['memory']['slots']['after'])
            self.assertEqual(struct.unpack_from('<i', state, selected * 0x54)[0], 1)
            self.assertEqual(struct.unpack_from('<3f', state, selected * 0x54 + 4), (12.5, 200.0, 0.0))
            self.assertEqual(result['stop']['target'], '0x00407ec0')
            self.assertEqual(result['memory']['power']['afterI32'], 3000)

    def test_duration_boundary_and_integer_truncation(self):
        for power, timer, deadline, expired in [(3000, 11, 12, False), (3000, 12, 12, True),
                                               (10000, 99, 100, False), (10000, 100, 100, True),
                                               (3039, 12, 12, True), (3040, 12, 13, False)]:
            result = self.case(f'duration-{power}-{timer}')
            self.assertEqual(result['registers']['ecx'], deadline)
            self.assertEqual(result['stop']['address'], '0x00410409' if expired else '0x00410454')

    def test_perfect_freeze_clamps_and_blocks_at_each_whole_stock_crossing(self):
        for name, value, reason in [('perfect-add-0--1', 0, 'returned'),
                                    ('perfect-add-9999-1', 10000, 'unsupported_call'),
                                    ('perfect-add-29999-1', 30000, 'unsupported_call'),
                                    ('perfect-add-30000-1', 30000, 'returned'),
                                    ('perfect-add-30000--10000', 20000, 'returned')]:
            result = self.case(name)
            self.assertEqual(result['memory']['perfect']['afterI32'], value)
            self.assertEqual(result['stop']['reason'], reason)

    def test_area_percent_and_perfect_gain_use_original_float_instructions(self):
        for count, percentage in [(0, 0.0), (86016, 50.0), (172032, 100.0)]:
            result = self.case(f'area-{count}')
            self.assertEqual(result['memory']['lastArea']['afterF32'], percentage)
            self.assertEqual(result['memory']['area']['afterF32'], 125.0 + percentage)
        for feature in (0, 1):
            for area, gain in [('0', 0), ('10', 500), ('20', 750), ('30', 1000), ('31', 2000)]:
                result = self.case(f'area-gain-{area}-cpu{feature}')
                self.assertEqual(result['stop']['target'], '0x00427300')
                self.assertEqual(struct.unpack('<i', bytes.fromhex(result['stackHead'])[:4])[0], gain)

    def test_original_timer_setter_connects_release_to_negative_cooldown(self):
        for first, second in ((0, 0), (1, 1)):
            result = self.case(f'release-call-45-3000-{first}-{second}')
            timer = bytes.fromhex(result['memory']['timerState']['after'])
            self.assertEqual(struct.unpack_from('<iif', timer), (-41, -40, -40.0))
            self.assertEqual(result['memory']['savedPower']['afterI32'], 3000)
            self.assertEqual(result['memory']['power']['afterI32'], 3000 if first == 0 else 0)

    def test_activation_builder_arguments_use_truncated_percent_and_original_50_float(self):
        for power, percentage in ((0, 0), (2999, 29), (3000, 30), (10000, 100)):
            result = self.case(f'activation-arguments-{power}')
            values = struct.unpack('<Ifii', bytes.fromhex(result['stackHead'])[:16])
            self.assertEqual(values, (0x300014, 50.0, percentage, 1))
            self.assertEqual(result['stop']['target'], '0x00410590')

    def test_replenishment_switch_uses_any_nonzero_active_value(self):
        for active, amount in ((0, 100), (1, 10), (-1, 10)):
            self.assertEqual(self.case(f'impact-0-{active}')['memory']['power']['afterI32'], amount)
        for name, value in (('replenish_300-0', 300), ('replenish_100-0', 100)):
            self.assertEqual(self.case(name)['memory']['power']['afterI32'], value)

    def test_baseline_continuation_calls_original_power_adder_after_release(self):
        for power, after in ((0, 5), (10000, 10000)):
            result = self.case(f'baseline-{power}')
            self.assertEqual(result['memory']['power']['afterI32'], after)
            self.assertEqual(result['stop']['address'], '0x0043f469')
        result = self.case('baseline-2999')
        self.assertEqual(result['memory']['power']['afterI32'], 3004)
        self.assertEqual(result['stop']['target'], '0x00458410')

    def test_area_gain_discontinuity_and_special_power_bypass(self):
        for feature in (0, 1):
            for area, gain in (('29.999998092651367', 999), ('30.000001907348633', 2000)):
                result = self.case(f'area-gain-{area}-cpu{feature}')
                self.assertEqual(struct.unpack('<i', bytes.fromhex(result['stackHead'])[:4])[0], gain)
        self.assertEqual(self.case('area-gain-bypass-10001')['stop']['address'], '0x0040f861')

    def test_motivation_negative_latch_and_upper_clamp(self):
        result = self.case('motivation-0--1-0')
        self.assertEqual(result['memory']['motivation']['afterI32'], -1)
        self.assertEqual(result['memory']['negativeLatch']['afterI32'], 1)
        self.assertEqual(self.case('motivation-100000-1-0')['memory']['motivation']['afterI32'], 100000)
        self.assertEqual(self.case('motivation-0-10000-1')['memory']['motivation']['afterI32'], 0)
        result = self.case('motivation-99999-2-0')
        self.assertEqual(result['memory']['motivation']['afterI32'], 100001)
        self.assertEqual(result['stop']['target'], '0x00458410')


if __name__ == '__main__':
    unittest.main()
