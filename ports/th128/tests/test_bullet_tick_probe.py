import importlib
import importlib.util
import os
from pathlib import Path
import sys
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))


class BulletTickProbeTests(unittest.TestCase):
    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'read-only original EXE is opt-in')
    def test_hit_effects_continuation_is_an_independent_real_tail_not_a_fake_effect_return(self):
        module=self.module()
        self.assertTrue(hasattr(module,'probe_hit_suffix'),'original after-hit tail evidence is missing')
        report=module.probe_hit_suffix(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['caseCount'],6)
        for sample in report['cases']:
            self.assertEqual(sample['expected']['entry'],'0x00408c93')
            self.assertTrue(sample['expected']['independentAfterHitSuffix'])
            self.assertFalse(sample['expected']['completedTick'])
            self.assertNotIn('0x0040882b',sample['expected']['instructionTrace'])
            if sample['expected']['continuation']!='Removed':
                self.assertEqual(sample['expected']['next']['positionBits'],sample['pending']['next']['positionBits'])
                self.assertEqual(sample['expected']['next']['lifeClock'],sample['pending']['next']['lifeClock'])
    def test_hit_motion_state_uses_original_half_speed_without_repeating_collision(self):
        module=self.module()
        self.assertEqual(module.frame_input(motionState=3)['motionState'],3)
    def test_report_destinations_are_all_validated_before_any_probe_or_write(self):
        module=self.module()
        same=ROOT/'local/research/bullet-tick-resume-fixtures.json'
        with patch.object(module,'probe',side_effect=AssertionError('probe ran before output validation')):
            with self.assertRaises(ValueError):
                module.main(['--exe','unused','--output',str(same),'--resume-output',str(same)])
            with self.assertRaises(ValueError):
                module.main(['--exe','unused','--resume-output',str(ROOT/'README.md')])
    def test_motion_domain_accepts_tiny_real_velocity_and_cancellation_residue(self):
        module=self.module()
        result=module.frame_input(position=(-128,2**-22,.1),velocity=(-4.371138828673793e-8,1,0))
        self.assertNotEqual(result['positionBits'][1],0)
        self.assertNotEqual(result['velocityBits'][0],0)
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_tick_probe'),
                             'bounded original-byte bullet tick probe is not implemented')
        return importlib.import_module('bullet_tick_probe')

    def test_output_is_confined_to_owned_private_reports(self):
        module = self.module()
        destination = ROOT / 'local/research/bullet-tick-fixtures.json'
        self.assertEqual(module.report_destination(destination), destination.resolve())
        for destination in (ROOT / 'README.md', ROOT / 'local/research/bullet-visual-fixtures.json',
                            ROOT / 'local/research/nested/bullet-tick-fixtures.json',
                            Path('E:/gee/bullet-tick.json')):
            with self.subTest(destination=destination), self.assertRaises(ValueError):
                module.report_destination(destination)

    def test_input_rejection_precedes_executable_access(self):
        module = self.module()
        native = object.__new__(module.NativeBulletTick)
        for changes in ({'age': -1}, {'age': 4097}, {'motionState': 4}, {'type': 9},
                        {'clock': .5}, {'clock': float('nan')}, {'position': (1, 2)},
                        {'position': (float('inf'), 0, 0)}, {'velocity': (10**100, 0, 0)},
                        {'velocity': (10**1000, 0, 0)}, {'position': [0] * 4096},
                        {'spriteWidth': -1}, {'flags': -1}, {'activeExtensions': 2**32},
                        {'hasSprite': 1}, {'lifeClock': True}, {'grace': -4097}):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                native.tick(**changes)

        with self.assertRaises(ValueError):
            module.frame_input(spriteWidth=2**-60)

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_actual_body_stops_before_animation_or_unimplemented_calls(self):
        module = self.module()
        report = module.probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['nativeCpuTrace'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertTrue(report['executable']['unchanged'])
        self.assertGreaterEqual(report['caseCount'], 50)
        categories = set()
        for sample in report['cases']:
            with self.subTest(sample=sample['id']):
                categories.add(sample['continuation'])
                self.assertIn(sample['stop']['reason'], ('boundary', 'unsupported_call', 'returned'))
                self.assertLess(sample['instructionsExecuted'], sample['instructionBudget'])
                self.assertFalse(sample['completedTick'])
                self.assertNotIn('0x00408d0c', sample['instructionTrace'])
                self.assertNotIn('0x00459d90', sample['instructionTrace'])
                self.assertEqual(sample['next']['integer0'], sample['input']['integer0'])
                if sample['continuation'] != 'Removed':
                    self.assertEqual(sample['next']['age'], sample['input']['age'])
        self.assertEqual(categories, {'NeedsAnimation', 'NeedsCollision', 'NeedsExtension', 'Removed'})
        self.assertEqual(report['scheduler']['bulletPriority'], 24)
        self.assertEqual(report['scheduler']['enemyPriority'], 21)
        transition = next(sample for sample in report['cases'] if sample['id'] == 'transition-1-17')
        self.assertIn('0x00408727', transition['instructionTrace'])
        self.assertIn('0x004088a6', transition['instructionTrace'])
        self.assertIn('0x00409a90', transition['instructionTrace'])
        self.assertIn('0x00472905', transition['instructionTrace'])
        self.assertIn('0x004089e2', transition['instructionTrace'])
        unknown = next(sample for sample in report['cases'] if sample['id'] == 'transition-unknown-empty-list')
        self.assertEqual(unknown['continuation'], 'NeedsExtension')
        self.assertNotIn('0x00409a90', unknown['instructionTrace'])

    @unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'), 'read-only original EXE is opt-in')
    def test_resume_uses_actual_helper_zero_returns_without_a_stub(self):
        report = self.module().probe_resume(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(report['caseCount'], 17)
        self.assertFalse(report['nativeCpuTrace'])
        self.assertFalse(report['windowsGameProcessLaunched'])
        self.assertTrue(report['executable']['unchanged'])
        boundaries = set()
        for sample in report['cases']:
            with self.subTest(sample=sample['id']):
                measurement = sample['helperMeasurement']
                self.assertEqual(measurement['helperReturn'], 0)
                self.assertEqual(measurement['registers']['eax'], 0)
                self.assertEqual(measurement['stop']['reason'], 'boundary')
                self.assertTrue({'0x0043c410', '0x0043c570'} & set(measurement['instructionTrace']))
                self.assertNotIn('0x0043d0c0', measurement['instructionTrace'])
                pending, expected = sample['pending'], sample['expected']
                self.assertEqual(measurement['collisionInput']['centerBits'],pending['next']['positionBits'][:2])
                boundaries.add(pending['boundary'])
                self.assertEqual(pending['continuation'], 'NeedsCollision')
                self.assertFalse(expected['completedTick'])
                if expected['continuation'] != 'Removed':
                    self.assertEqual(pending['next']['lifeClock'], expected['next']['lifeClock'])
                    self.assertEqual(pending['next']['age'], expected['next']['age'])
                self.assertNotIn('0x00408d0c', expected['instructionTrace'])
        self.assertEqual(boundaries, {0x4087b4, 0x4087f0, 0x40880f, 0x408a44, 0x408a80, 0x408a9f})


if __name__ == '__main__':
    unittest.main()
