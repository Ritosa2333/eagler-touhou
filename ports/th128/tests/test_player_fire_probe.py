import importlib
import os
from pathlib import Path
import sys
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))


class FireProbeTests(unittest.TestCase):
    def test_owned_destination(self):
        module = importlib.import_module('player_fire_probe')
        with self.assertRaises(ValueError):
            module.report_destination(ROOT / 'README.md')

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'original bytes opt-in')
    def test_real_head_and_charge_stop_before_required_calls(self):
        module = importlib.import_module('player_fire_probe')
        report = module.probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'], report['executable']['sha256After'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertEqual(len(report['headCases']), 34)
        self.assertEqual(len(report['chargeCases']), 39)
        self.assertTrue(any(sample['stop']['address'] == '0x0043f25d' for sample in report['headCases']))
        self.assertTrue(any(sample['stop'].get('target') == '0x004101d0' for sample in report['chargeCases']))


if __name__ == '__main__':
    unittest.main()
