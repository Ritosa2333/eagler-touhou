import hashlib
import importlib
import importlib.util
import os
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[2]


class BulletVisualProbeTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_visual_probe'),
                             'bounded actual-byte bullet visual probe is not implemented')
        return importlib.import_module('bullet_visual_probe')

    def test_destination_does_not_overwrite_shared_or_original_files(self):
        module = self.module()
        valid = ROOT / 'local/research/bullet-visual-fixtures.json'
        self.assertEqual(module.report_destination(valid), valid.resolve())
        for name in ('README.md', 'local/research/bullet-fixtures.json',
                     'local/research/bullet-lifecycle-fixtures.json',
                     'local/research/nested/bullet-visual-fixtures.json', 'E:/gee/bullet-visual.json'):
            with self.subTest(name=name), self.assertRaises(ValueError):
                module.report_destination(ROOT / name)

    def test_remap_inputs_are_strictly_bounded_before_any_exe_read(self):
        module = self.module()
        native = object.__new__(module.NativeBulletVisual)
        for color, sprite in ((-1, 0), (16, 0), (0, -1), (0, 4), (True, 0), (0, 10**100)):
            with self.subTest(color=color, sprite=sprite), self.assertRaises(ValueError):
                native.remap(color, sprite)

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE not configured')
    def test_real_callback_and_profile_with_no_native_windows_cpu_claim(self):
        module = self.module()
        source = Path(os.environ['TH128_BULLET_EXE'])
        before = hashlib.sha256(source.read_bytes()).hexdigest()
        report = module.probe(source)
        self.assertEqual(report['caseCount'], 64)
        self.assertEqual(report['profile']['script'], 43)
        self.assertEqual(report['profile']['secondaryScript'], 0)
        self.assertFalse(report['nativeCpuTrace'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertTrue(report['executable']['unchanged'])
        for sample in report['cases']:
            with self.subTest(sample=sample['id']):
                self.assertEqual(sample['stop']['reason'], 'returned')
                self.assertEqual(sample['result'], sample['input']['remap'][sample['input']['sprite']])
                self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
        self.assertEqual(report['unknownCall']['stop']['reason'], 'unsupported_call')
        self.assertEqual(hashlib.sha256(source.read_bytes()).hexdigest(), before)


if __name__ == '__main__':
    unittest.main()
