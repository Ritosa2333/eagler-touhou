import importlib.util
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools' / 'research'))
SOURCE = ROOT / 'tools' / 'research' / 'cancellation_effect_probe.py'
EXE = Path(r'E:\gee\妖精大战争\th128.exe')
AVAILABLE = EXE.is_file() and importlib.util.find_spec('unicorn') is not None


@unittest.skipUnless(AVAILABLE, 'pinned original executable and local Unicorn are required')
class CancellationDriftProbeTests(unittest.TestCase):
    def report(self):
        self.assertTrue(SOURCE.is_file(), 'bounded native cancellation initializer probe is missing')
        specification = importlib.util.spec_from_file_location('cancellation_effect_probe', SOURCE)
        module = importlib.util.module_from_spec(specification)
        specification.loader.exec_module(module)
        return module.probe(EXE)

    def test_original_prefix_uses_separate_float_stores_and_native_double_ten(self):
        report = self.report()
        self.assertEqual(report['doubleConstant']['value'], 10.0)
        self.assertEqual(report['cases'][0]['targetWords'], [0x41200000, 0xc1a00000, 0x40200000])
        for case in report['cases']:
            self.assertEqual(case['stepWords'], case['velocityWords'])
            self.assertEqual(case['prefix']['stop']['reason'], 'boundary')
            self.assertEqual(case['prefix']['stop']['address'], '0x00408b4b')
            self.assertLess(case['prefix']['instructionsExecuted'], case['prefix']['instructionBudget'])

    def test_original_initializer_writes_only_declared_interpolator_fields(self):
        report = self.report()
        for case in report['cases']:
            initialization = case['initializer']
            self.assertEqual(initialization['stop']['reason'], 'returned')
            self.assertEqual(initialization['stackDelta'], 12)
            self.assertLess(initialization['instructionsExecuted'], initialization['instructionBudget'])
            self.assertEqual(initialization['fromWords'], [0, 0, 0])
            self.assertEqual(initialization['toWords'], case['targetWords'])
            self.assertEqual(initialization['slopeWords'], [0] * 6)
            self.assertEqual(initialization['duration'], 30)
            self.assertEqual(initialization['mode'], 6)
            self.assertEqual(initialization['timerWords'], [0xffffffff, 0, 0, 0x4b6f58, 1])
            self.assertTrue(initialization['unchangedOutsideInterpolator'])
        self.assertTrue(report['executable']['unchanged'])
        self.assertEqual(report['executable']['sha256After'], report['executable']['sha256'])
        self.assertFalse(report['windowsGameProcessLaunched'])


if __name__ == '__main__':
    unittest.main()
