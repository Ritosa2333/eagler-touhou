import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class DrawPipelineProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_original_draw_registration_places29_before_enemy_and_hud_and_binds_actual_at_r_entry2(self):
        self.assertIsNotNone(importlib.util.find_spec('draw_pipeline_probe'), 'original layer/target pipeline evidence is missing')
        report = importlib.import_module('draw_pipeline_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['executable']['sha256'], report['executable']['sha256After'])
        priorities = {item['layer']: item['priority'] for item in report['layers']}
        self.assertEqual(len(priorities), 31)
        self.assertEqual([priorities[layer] for layer in (0, 4, 5, 6, 7, 8, 16, 23, 29, 30)],
                         [4, 11, 13, 16, 17, 18, 36, 56, 14, 41])
        self.assertEqual(report['stageSurface']['captureEndPriority'], 12)
        self.assertEqual(report['stageSurface']['sourceTextureEntry'], 2)
        self.assertEqual(report['stageSurface']['mainScript'], 82)
        self.assertEqual(report['stageSurface']['compositePriority'], 14)
        self.assertFalse(report['allocationExecuted'])
        self.assertFalse(report['graphicsApiExecuted'])
        self.assertEqual(report['evidenceKind'], 'original-byte static callsite recovery, not a rendered/native frame trace')


if __name__ == '__main__':
    unittest.main()
