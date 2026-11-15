import importlib
import os
from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools/research'))


class BossHudProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'pinned original EXE opt-in')
    def test_original_hp_timer_and_ring_cpu_buffers(self):
        report = importlib.import_module('boss_hud_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned'])
        self.assertEqual(len(report['fillCases']), 5)
        self.assertEqual([case['irq'] for case in report['timerCases']], [7, 8, 9, 9, 0, 0, 0, 0, 0, 0])
        self.assertEqual([case['sound'] for case in report['timerCases']], [-1, 11, 12, 12, -1, -1, -1, -1, -1, -1])
        self.assertEqual([len(case['vertices']) for case in report['ringCases']], [76] * 5)
        self.assertEqual([case['fireSuppressed'] for case in report['fireGateCases']], [False, True])
        self.assertEqual(report['ignoredOpcode440']['target'],0x41c52c)
        self.assertEqual([case['ratioBits'] for case in report['clearCases']],[[0]*5]*3)
        self.assertEqual([case['colors'] for case in report['clearCases']],[[0xffff8080]*4]*3)


if __name__ == '__main__':
    unittest.main()
