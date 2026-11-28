import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class EnemyDrop410ProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_actual410_changes_only_base_drop_word_without_clearing_additional_drops(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_drop410_probe'), 'original410 base-drop evidence is missing')
        report = importlib.import_module('enemy_drop410_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'], report['executable']['sha256After'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['cases']), 8)
        for sample in report['cases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x0041c52c'})
            self.assertEqual(sample['afterWords'][0], sample['value'] & 0xffffffff)
            self.assertEqual(sample['afterWords'][1:], sample['beforeWords'][1:])


if __name__ == '__main__':
    unittest.main()
