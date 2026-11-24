import importlib
import os
from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class PlayerRenderProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_real_outline_thresholds_and_manual_draw_registrations(self):
        report = importlib.import_module('player_render_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual([sample['color'] for sample in report['powerCases']], [0x00ffffff, 0x00ffffff, 0x50ffff80, 0x50ffff80, 0xc0ffff00])
        self.assertTrue(all(sample['outlineSprite'] == sample['frame'] + 79 for sample in report['spriteCases']))
        self.assertEqual([sample['priority'] for sample in report['passes']], [32, 25, 24])


if __name__ == '__main__':
    unittest.main()
