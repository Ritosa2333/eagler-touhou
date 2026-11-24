import importlib
import importlib.util
import os
from pathlib import Path
import sys
import struct
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))


class PlayerShotTickProbeTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('player_shot_tick_probe'),
                             'bounded original-byte player-shot tick probe is missing')
        return importlib.import_module('player_shot_tick_probe')

    def test_probe_module_and_owned_destination(self):
        module = self.module()
        destination = ROOT / 'local/research/player-shot-tick-fixtures.json'
        self.assertEqual(module.report_destination(destination), destination.resolve())
        for target in (ROOT / 'README.md', ROOT / 'local/research/player-shot-birth-fixtures.json',
                       ROOT / 'local/research/nested/player-shot-tick.json', Path('E:/gee/player-shot-tick.json')):
            with self.subTest(target=target), self.assertRaises(ValueError):
                module.report_destination(target)

    def test_invalid_inputs_precede_original_access(self):
        module = self.module()
        native = object.__new__(module.NativePlayerShotTick)
        for changes in ({'position': (0, 0)}, {'position': (float('nan'), 0, 0)},
                        {'age': True}, {'age': 2**31}, {'slot': 256}, {'active': -1},
                        {'speed': float('inf')}, {'tick_target': 2**32}):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                native.tick(**changes)

    def test_output_and_execution_budgets_preflight(self):
        module = self.module()
        with patch.object(module, 'probe', side_effect=AssertionError('probe ran before destination validation')):
            with self.assertRaises(ValueError):
                module.main(['--exe', 'unused', '--output', str(ROOT / 'README.md')])
        with self.assertRaises(ValueError):
            module.execute([(0x400000, bytes(8193))], 0x400000, [])
        with self.assertRaises(ValueError):
            module.execute([], 0x400000, [{'bytes': bytes(131073)}])

    def test_mode1_requires_a_real_returning_native_helper(self):
        self.assertTrue(hasattr(self.module().NativePlayerShotTick, 'rotated_bounds'),
                        'mode1 requires executed 0x45fad0 corners, not mode0/render-quad substitution')

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'readonly original EXE is opt-in')
    def test_mode1_really_executes_native_sincos_and_returns_all_corners(self):
        native = self.module().NativePlayerShotTick(os.environ['TH128_BULLET_EXE'])
        try:
            sample = native.rotated_bounds(rotation=0, position=(310, 408, .1), sprite=(32, 64), scale=(1, 1))
            self.assertEqual(sample['stop']['reason'], 'returned')
            self.assertEqual(sample['stackDelta'], 8)
            self.assertEqual(sample['basisEvidence']['stop']['reason'], 'returned')
            self.assertEqual(sample['basisEvidence']['cosineSineBits'], [0x3f800000, 0])
            expected = ((294, 376), (326, 376), (294, 440), (326, 440))
            self.assertEqual(sample['cornerBits'], [list(struct.unpack('<2I', struct.pack('<2f', *point))) for point in expected])
        finally:
            native.image.close()

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'readonly original EXE is opt-in')
    def test_native_returns_prefixes_and_actual_resource(self):
        report = self.module().probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['resource']['sha256'], self.module().SHT_SHA256)
        self.assertTrue(report['executable']['unchanged'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        for sample in report['returnedPasses']:
            self.assertEqual(sample['stop']['reason'], 'returned')
            self.assertEqual(sample['stackDelta'], 8)
        callback, motion = report['requiredPrefixes']
        self.assertEqual(callback['stop']['address'], '0x0043f7ae')
        self.assertEqual(motion['stop']['target'], '0x0046c1f0')
        self.assertTrue(all(sample['stop']['reason'] == 'returned' for sample in report['integratorCases']))

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'readonly original EXE is opt-in')
    def test_negative_quantization_and_strict_native_bounds(self):
        native = self.module().NativePlayerShotTick(os.environ['TH128_BULLET_EXE'])
        try:
            self.assertFalse(native.bounds(((128, 16), (512, 16), (128, 464), (512, 464)))['survivor'])
            self.assertTrue(native.bounds(((129, 17), (0, 0), (0, 0), (0, 0)))['survivor'])
            self.assertTrue(native.bounds(((0, 0),) * 4, age=9)['survivor'])
        finally:
            native.image.close()

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'readonly original EXE is opt-in')
    def test_sync_prefix_observes_both_native_horizontal_additions(self):
        native = self.module().NativePlayerShotTick(os.environ['TH128_BULLET_EXE'])
        try:
            sample = native.sync(position=(-128.25, 401.5, .1))
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0043fa1c'})
            self.assertEqual(sample['animationPositionBits'],
                             list(struct.unpack('<3I', struct.pack('<3f', 191.75, 417.5, .1))))
            self.assertTrue(sample['independentSyncPrefix'])
            self.assertFalse(sample['completedTick'])
        finally:
            native.image.close()


if __name__ == '__main__':
    unittest.main()
