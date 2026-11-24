import importlib
import importlib.util
import os
import sys
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools/research'))

class PlayerWindowSafetyTests(unittest.TestCase):
    def module(self):
        self.assertIsNotNone(importlib.util.find_spec('player_window_probe'),'bounded original player timer/bootstrap probe is missing')
        return importlib.import_module('player_window_probe')

    def test_report_is_confined_to_its_private_namespace(self):
        module=self.module()
        valid=ROOT/'local/research/player-window-fixtures.json'
        self.assertEqual(module.report_destination(valid),valid.resolve())
        for name in (ROOT/'README.md',ROOT/'local/research/player-hit-fixtures.json',Path('E:/gee/player-window.json')):
            with self.assertRaises(ValueError):module.report_destination(name)

    def test_timer_limits_are_checked_before_original_access(self):
        native=object.__new__(self.module().NativePlayerWindow)
        for state,focus in ((-1,0),(65536,0),(0,65536),(True,0)):
            with self.assertRaises(ValueError):native.timers(state,focus)

@unittest.skipUnless(os.environ.get('TH128_BULLET_EXE') and importlib.util.find_spec('unicorn'),'enable pinned original and Unicorn')
class OriginalPlayerWindowTests(PlayerWindowSafetyTests):
    def test_directional_animation_selection_is_native_horizontal_sign_logic(self):
        native=self.module().NativePlayerWindow(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'animation_choice'),'native directional ANM transition prefix missing')
        for current,previous,expected in ((-576,0,1),(-576,-576,None),(576,0,3),(576,-576,3),(0,-576,2),(0,576,4),(0,0,None)):
            result=native.animation_choice(current,previous)
            self.assertEqual(result['script'],expected)
            self.assertEqual(result['stop']['reason'],'unsupported_call' if expected is not None else 'boundary')
    def test_original_unit_clock_increments_state_and_focus_once(self):
        native=self.module().NativePlayerWindow(os.environ['TH128_BULLET_EXE'])
        for state,focus in ((0,0),(7,3),(8,120),(65535,65535)):
            result=native.timers(state,focus)
            self.assertEqual(result['stop'],{'reason':'boundary','address':'0x0043bf75'})
            self.assertEqual(result['stateTimer'][1],state+1)
            self.assertEqual(result['focusTimer'][1],focus+1)

    def test_real_constructor_memset_clears_player_flags_and_optional_handles(self):
        result=self.module().NativePlayerWindow(os.environ['TH128_BULLET_EXE']).constructor_clear()
        self.assertEqual(result['stop'],{'reason':'boundary','address':'0x0043a500'})
        self.assertEqual(result['zeroedBytes'],0x43a94)
        self.assertTrue(result['allBytesZero'])
        self.assertEqual(result['fields'],{'flags438ec':0,'optional87a8':0,'optional87ac':0})

    def test_null_animation_detach_executes_the_real_zero_lookup_path(self):
        result=self.module().NativePlayerWindow(os.environ['TH128_BULLET_EXE']).null_detach()
        self.assertEqual(result['stop']['reason'],'returned')
        self.assertIn('0x0046742a',result['instructionTrace'])
        self.assertEqual(result['handleAfter'],0)

if __name__=='__main__':unittest.main()
