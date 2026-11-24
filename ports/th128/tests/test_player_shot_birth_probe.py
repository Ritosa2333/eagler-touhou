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


class PlayerShotBirthProbeTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('player_shot_birth_probe'),
                             'bounded original-byte player-shot birth probe is missing')
        return importlib.import_module('player_shot_birth_probe')

    def test_missing_module_and_owned_output(self):
        module = self.module()
        destination = ROOT / 'local/research/player-shot-birth-fixtures.json'
        self.assertEqual(module.report_destination(destination), destination.resolve())
        for destination in (ROOT / 'README.md', ROOT / 'local/research/player-fixtures.json',
                            ROOT / 'local/research/nested/player-shot-birth.json', Path('E:/gee/player-shot-birth.json')):
            with self.subTest(destination=destination), self.assertRaises(ValueError):
                module.report_destination(destination)

    def test_required_context_and_output_budget_checks_precede_original_access(self):
        module = self.module()
        native = object.__new__(module.NativePlayerShotBirth)
        for area, difficulty in ((-1, 0), (float('inf'), 0), (0, 4), (0, True)):
            with self.subTest(area=area, difficulty=difficulty), self.assertRaises(ValueError):
                native.group(area, difficulty)
        with patch.object(module, 'probe', side_effect=AssertionError('probe preceded output validation')):
            with self.assertRaises(ValueError):
                module.main(['--exe', 'unused', '--output', str(ROOT / 'README.md')])
        with self.assertRaises(ValueError):
            module.execute([(0x400000, bytes(8193))], 0x400000, [])

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_recycled_birth_preserves_unwritten_fields_but_resets_the_real_timer(self):
        module = self.module()
        raw, decoded, _ = module.actual_resources()
        native = module.NativePlayerShotBirth(os.environ['TH128_BULLET_EXE'])
        try:
            previous = bytearray([0xa5] * module.OBJECT_BYTES)
            struct.pack_into('<I', previous, 0x48, 0)
            struct.pack_into('<I', previous, 0x10, 0x80000001)
            sample = native.birth(raw, decoded['groups'][0]['records'][0]['offset'], initial_slot=bytes(previous))
            after = bytes.fromhex(sample['slotAfter'])
            self.assertEqual(after[0x34:0x48], previous[0x34:0x48])
            self.assertEqual(struct.unpack_from('<iii', after), (-1, 0, 0))
            self.assertEqual(struct.unpack_from('<I', after, 0x10)[0], 0x80000001)
            self.assertTrue(sample['recycledInput'])
        finally:
            native.image.close()

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_actual_sht_birth_prefixes_and_group_selection(self):
        report = self.module().probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['nativeCpuTrace'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertTrue(report['executable']['unchanged'])
        self.assertEqual(report['resource']['groupCount'], 16)
        self.assertEqual(report['resource']['recordCount'], 114)
        self.assertEqual(report['layout']['slotCount'], 256)
        self.assertEqual(report['layout']['objectBytes'], 0x78)
        self.assertEqual(len(report['schedulerCases']), 14)
        for sample in report['schedulerCases']:
            if sample['firstRecordOffset'] is None:
                self.assertEqual(sample['stop']['reason'], 'returned')
            else:
                self.assertEqual(sample['stop']['target'], '0x0043ed60')
        for sample in report['birthCases']:
            self.assertIn(sample['stop']['reason'], ('unsupported_call', 'returned'))
            self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
        self.assertTrue(any(sample['stop']['target'] == '0x0046a700'
                            for sample in report['birthCases'] if sample['stop']['reason'] == 'unsupported_call'))
        self.assertTrue(any(sample['stop']['reason'] == 'returned' for sample in report['birthCases']))


if __name__ == '__main__':
    unittest.main()
