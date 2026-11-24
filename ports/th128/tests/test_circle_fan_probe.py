import importlib
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class CircleFanProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_original104_and105_modes_and_untextured_vertex_buffers(self):
        report = importlib.import_module('circle_fan_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual([sample['mode'] for sample in report['modeCases']], [16, 17])
        self.assertEqual(report['negativeSprite']['sprite'], 313)
        for sample in report['cases']:
            self.assertEqual(sample['stop'], {'reason': 'boundary', 'address': '0x00462a9e'})
            self.assertEqual(len(sample['vertices']), sample['input'][4] + 2)


if __name__ == '__main__':
    unittest.main()
