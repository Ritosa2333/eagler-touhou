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
ORIGINAL = os.environ.get('TH128_ENEMY_EXE')


class EnemyProbeContractTests(unittest.TestCase):
    def setUp(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_probe'), 'bounded enemy probe is missing')
        self.probe = importlib.import_module('enemy_probe')

    def test_report_destination_cannot_escape_enemy_evidence(self):
        for destination in (ROOT / 'README.md', ROOT / 'local/research/freeze-enemy.json',
                            Path('E:/gee/enemy-test.json'), ROOT / 'local/research/enemy-dir/enemy-x.json'):
            with self.subTest(destination=destination), self.assertRaises(ValueError):
                self.probe.report_destination(destination)
        destination = ROOT / 'local/research/enemy-fixtures.json'
        self.assertEqual(self.probe.report_destination(destination), destination.resolve())

    def test_wrong_executable_is_rejected_before_pe_parsing(self):
        with patch.object(Path, 'read_bytes', return_value=b'not TH128'):
            with self.assertRaisesRegex(ValueError, 'SHA-256'):
                self.probe.probe(Path('not-a-game.exe'))

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'optional Unicorn is not installed')
    def test_unknown_call_has_no_invented_return(self):
        result = self.probe.emulate([(0x410000, bytes.fromhex('e8fbff0f00c3'))], 0x410000)
        self.assertEqual(result['stop']['reason'], 'unsupported_call')
        self.assertEqual(result['stop']['target'], '0x00510000')
        self.assertEqual(result['registers']['esp'], self.probe.STACK_POINTER)

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'optional Unicorn is not installed')
    def test_original_instruction_writes_are_captured(self):
        result = self.probe.emulate([(0x410000, bytes.fromhex('ff0500003000c3'))], 0x410000,
                                    data=[self.probe.datum('flags', 0x300000, 31)])
        self.assertEqual(result['stop']['reason'], 'returned')
        self.assertEqual(result['memory']['flags']['afterI32'], 32)


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'), 'set TH128_ENEMY_EXE and install Unicorn')
class OriginalEnemyTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.report = importlib.import_module('enemy_probe').probe(Path(ORIGINAL))
        cls.cases = {case['id']: case for case in cls.report['cases']}

    def test_dispatch_is_recovered_from_the_pinned_executable(self):
        expected = {256: 0x41807d, 257: 0x418366, 258: 0x4188f9, 260: 0x418720,
                    262: 0x418da3, 304: 0x419467, 305: 0x41954a,
                    400: 0x41a31f, 401: 0x41a345, 402: 0x41a36b, 403: 0x41a3a4}
        self.assertEqual({int(key): int(value['handler'], 16) for key, value in self.report['dispatch'].items()}, expected)

    def test_first_actual_spawn_preserves_all_call_arguments(self):
        case = self.cases['actual-main-spawn-257']
        self.assertEqual(case['stop']['reason'], 'unsupported_call')
        self.assertEqual(case['stop']['target'], '0x00414ff0')
        self.assertEqual(case['spawnArguments']['name'], 'GirlA1b')
        self.assertEqual(case['spawnArguments']['position'], [-128.0, -32.0, 0.0])
        self.assertEqual(case['spawnArguments']['life'], 20)
        self.assertEqual(case['spawnArguments']['score'], 1000)
        self.assertEqual(case['spawnArguments']['drop'], 2)
        self.assertEqual(case['spawnArguments']['mode'], 0)

    def test_flags_include_the_original_animation_visibility_call(self):
        case = self.cases['set-root-32-visible-animation']
        self.assertEqual(case['stop']['reason'], 'boundary')
        self.assertEqual(case['memory']['flags']['afterI32'], 32)
        self.assertEqual(case['memory']['animationFlags']['beforeI32'], 2)
        self.assertEqual(case['memory']['animationFlags']['afterI32'], 0)

    def test_first_polar_interpolation_is_an_original_byte_boundary_not_a_stub(self):
        case = self.cases['polar-interpolate-first']
        self.assertEqual(case['stop']['reason'], 'boundary')
        self.assertEqual(case['memory']['interpolationDuration']['afterI32'], 100)
        self.assertEqual(case['memory']['interpolationMode']['afterI32'], 0)
        self.assertEqual(case['memory']['interpolationTargetAngle']['afterF32'], struct.unpack('<f', struct.pack('<f', 0.3926991))[0])
        self.assertEqual(case['memory']['interpolationTargetSpeed']['afterF32'], 1.0)

    def test_first_actual_304_consumes_the_typed_expression_stack(self):
        case = self.cases['polar-first-stack-argument']
        self.assertEqual(case['stop']['reason'], 'boundary')
        self.assertEqual(case['memory']['contextStackOffset']['beforeI32'], 8)
        self.assertEqual(case['memory']['contextStackOffset']['afterI32'], 0)
        self.assertEqual(case['memory']['angle']['afterF32'], 1.75)
        self.assertEqual(case['memory']['speed']['afterF32'], 3.0)

    def test_linear_first_step_uses_one_over_duration_not_zero(self):
        case = self.cases['linear-step-1-of-100']
        self.assertEqual(case['stop']['reason'], 'returned')
        self.assertEqual(case['memory']['timerCurrent']['afterI32'], 1)
        self.assertEqual(case['memory']['timerFloat']['afterF32'], 1.0)
        self.assertLess(case['memory']['outputSpeed']['afterF32'], 3.0)

    def test_native_cartesian_integrator_quantizes_hundredths_downwards(self):
        case = self.cases['cartesian-negative-floor']
        self.assertEqual(case['stop']['reason'], 'returned')
        self.assertEqual(case['memory']['positionX']['afterF32'], struct.unpack('<f', struct.pack('<f', -0.01))[0])
        self.assertEqual(case['memory']['positionY']['afterF32'], 0.0)
        self.assertEqual(self.report['motionQuantizationFactor'], 100.0)

    def test_every_case_stops_at_its_declared_evidence_boundary(self):
        self.assertEqual(self.report['caseCount'], len(self.report['cases']))
        for case in self.report['cases']:
            if case['operation'] in ('spawn', 'animation-request'):
                expected = 'unsupported_call'
            elif case['operation'] in ('linear-step', 'cartesian'):
                expected = 'returned'
            else:
                expected = 'boundary'
            with self.subTest(case=case['id']):
                self.assertEqual(case['stop']['reason'], expected)

    def test_scheduling_is_static_local_order_not_a_fabricated_frame_trace(self):
        order = self.report['localCallOrder']
        self.assertFalse(order['executed'])
        self.assertFalse(order['fullFrameScheduleProven'])
        targets = {item['call']: item['target'] for item in order['directCalls']}
        self.assertEqual(targets['0x00415180'], '0x00416160')
        self.assertEqual(targets['0x00415927'], '0x00416160')
        self.assertEqual(targets['0x004164e1'], '0x0046fe20')
        self.assertEqual(targets['0x0046fe52'], '0x0046e100')
        self.assertEqual(order['hostVtableFirstSlot'], '0x004a2704')
        self.assertEqual(order['hostVtableTarget'], '0x00417fe0')


if __name__ == '__main__':
    unittest.main()
