import importlib
import json
import os
from pathlib import Path
import sys
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools/research'));sys.path.insert(0,str(ROOT/'tools'))

@unittest.skipUnless(os.environ.get('TH128_BULLET_EXE'),'read-only original EXE opt-in')
class CombatFocusStageProbeTests(unittest.TestCase):
    def test_actual_shot_tiny_lateral_motion_reaches_native_movement_boundary(self):
        report=importlib.import_module('shot_motion_edge_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(len(report['cases']),22);self.assertFalse(report['apiReturnsAssigned'])
        for case in report['cases']:self.assertIn(case['stop']['reason'],('returned','boundary'))

    def test_original_focus_binding_is_bullet80_and_pixel_offset_is320_16(self):
        report=importlib.import_module('focus_marker_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned']);self.assertEqual(report['gates'][2]['stop']['target'],'0x004670b0')
        for case in report['positions']:
            self.assertEqual(case['pixelPosition'][:2],[case['input'][0]+320,case['input'][1]+16])

    def test_original524_typed4_and_complete_extension1_steps_are_real_code(self):
        report=importlib.import_module('boss_ring_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertFalse(report['apiReturnsAssigned']);self.assertEqual(len(report['geometry']),12)
        self.assertEqual(len(report['extension1Steps']),15)
        for case in report['extension1Steps']:
            self.assertEqual(case['stop']['reason'],'returned');self.assertEqual(case['after']['active'],0 if case['time']>16 else 1)

    def test_original448_subtracts_rank_selected_delay_from_current_clock(self):
        report=importlib.import_module('difficulty_delay_probe').probe(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(len(report['cases']),10)
        for case in report['cases']:
            self.assertEqual(case['clockAfter'],case['clock']-(5 if case['difficulty']<2 else 1))

    def test_preparation_produces_original_bullet4_table_without_external_pe_dependencies(self):
        from th128_assets.bullet_profiles import read_bullet_profiles
        report=read_bullet_profiles(Path(os.environ['TH128_BULLET_EXE']).read_bytes())
        catalog=json.loads((ROOT/'local/catalog.json').read_text(encoding='utf8'))
        current=(ROOT/'local'/catalog['variants']['ja']['manifest']).parent/'animations/bullet-native-profiles.json'
        self.assertEqual(report,json.loads(current.read_text(encoding='utf8')))
    def test_preparation_preserves_original_boss10_table_without_external_pe_dependencies(self):
        from th128_assets.bullet_profiles import read_boss_bullet_profiles
        report=read_boss_bullet_profiles(Path(os.environ['TH128_BULLET_EXE']).read_bytes())
        catalog=json.loads((ROOT/'local/catalog.json').read_text(encoding='utf8'))
        current=(ROOT/'local'/catalog['variants']['ja']['manifest']).parent/'animations/bullet-boss-profiles.json'
        self.assertEqual(report,json.loads(current.read_text(encoding='utf8')))

if __name__=='__main__':unittest.main()
