import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class ExtensionQueueProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_whole_original_update_preserves_waiting_and_concurrent_extension_records(self):
        report = importlib.import_module('bullet_extension_queue_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['cases']), 7)
        for sample in report['cases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x00408d09'})
        self.assertEqual(report['cases'][0]['expected'][8:10], [2, 1])
        self.assertEqual(report['cases'][1]['expected'][8:10], [3, 5])
        self.assertEqual(report['cases'][3]['expected'][8:10], [3, 4])


if __name__ == '__main__':
    unittest.main()
