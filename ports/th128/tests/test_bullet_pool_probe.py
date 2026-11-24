import importlib
import importlib.util
import os
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools/research'))


class PoolProbeContractTests(unittest.TestCase):
    def test_probe_exists_and_confines_reports(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'), 'original pool selection probe missing')
        module = importlib.import_module('bullet_pool_probe')
        accepted = ROOT / 'local/research/bullet-pool-fixtures.json'
        self.assertEqual(module.report_destination(accepted), accepted.resolve())
        for path in (ROOT / 'local/research/bullet-fixtures.json', ROOT / 'README.md', Path('E:/gee/bullet-pool.json')):
            with self.subTest(path=path), self.assertRaises(ValueError):
                module.report_destination(path)


@unittest.skipUnless(os.environ.get('TH128_BULLET_EXE') and importlib.util.find_spec('unicorn'),
                     'enable original-byte bullet input and Unicorn')
class OriginalPoolTests(unittest.TestCase):
    def test_effect_tail_and_enemy_head_registration_share_the_primary_list(self):
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'anm_registration'),'original head/tail list-domain proof is missing')
        tail=native.anm_registration(False);head=native.anm_registration(True)
        self.assertEqual(tail['stop']['reason'],'returned');self.assertEqual(head['stop']['reason'],'returned')
        self.assertEqual((tail['head'],tail['tail']),(0x390004,0x3a0004))
        self.assertEqual((head['head'],head['tail']),(0x3a0004,0x390004))
    def test_native_constructor_life_threshold_sets_high_flag(self):
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'spawn_life_flags'),'original life-threshold constructor prefix is missing')
        for life,expected in ((999,0x20),(1000,0x80000020),(8000,0x80000020)):
            result=native.spawn_life_flags(life,0x20)
            self.assertEqual(result['stop'],{'reason':'boundary','address':'0x00415180'})
            self.assertEqual(result['flags'],expected)
    def test_sound_ids_resolve_by_original_loader_slot_not_table_row(self):
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'sound_bindings'),'original sound ID/name bindings are missing')
        bindings=native.sound_bindings()
        self.assertEqual(bindings['22']['name'],'se_tan00.wav')
        self.assertEqual(bindings['44']['name'],'se_graze.wav')
        self.assertEqual(bindings['73']['name'],'se_pin00.wav')
        self.assertEqual(bindings['74']['name'],'se_pin01.wav')
        self.assertEqual(bindings['0']['name'],'se_plst00.wav')
        self.assertEqual(bindings['2']['name'],'se_pldead00.wav')
    def test_enemy_destructor_prefix_unlinks_without_spawning_replacement_entities(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'enemy_unlink'),'original enemy removal prefix is missing')
        for selected,expected in ((0,(0x3a145c,0x3a145c)),(1,(0x39145c,0x39145c))):
            result=native.enemy_unlink(selected)
            self.assertEqual(result['stop'],{'reason':'boundary','address':'0x00415f2b'})
            self.assertEqual((result['head'],result['tail']),expected)
            self.assertEqual(result['count'],1)
    def test_explicit_effect_rng_selector_is_not_a_constructor_preservation_proof(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'effect_rng'),'native transient effect RNG domain is unproved')
        for flags,expected in ((0,(0,2)),(2,(2,0))):
            result=native.effect_rng(flags)
            self.assertEqual(result['stop']['reason'],'returned')
            self.assertEqual((result['displayCalls'],result['gameCalls']),expected)

    def test_actual_effect_reset_bind_selects_game_rng_not_pre_reset_display_flag(self):
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'effect_bind_domain'),'effect constructor domain preservation is unproved')
        result=native.effect_bind_domain()
        self.assertEqual(result['stop']['reason'],'unsupported_call')
        self.assertEqual(result['stop']['target'],'0x00407080')
        self.assertEqual(result['flags2']&2,0)
        getter=native.effect_rng(result['flags2'])
        self.assertEqual((getter['displayCalls'],getter['gameCalls']),(0,2))
    def test_spawn_mode_is_an_original_flag_store_not_a_fake_enemy_class(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'spawn_mode_flags'),'native spawn-mode flag prefix is missing')
        for mode,flags,expected in ((0,0x100020,0x20),(1,0x20,0x100020),(1,0x80080020,0x80180020)):
            result=native.spawn_mode_flags(mode,flags)
            self.assertEqual(result['stop'],{'reason':'boundary','address':'0x0041507e'})
            self.assertEqual(result['flags'],expected)
    def test_anm_zero_is_reserved_and_first_stage_include_is_bank_one(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        native=importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native,'anm_binding'),'original ECL ANM bank mapping is not recorded')
        result=native.anm_binding()
        self.assertEqual(result['reservedBulletOffset'],128)
        self.assertEqual(result['firstIncludeOffset'],132)
        self.assertEqual(result['firstIncludeBank'],1)
        self.assertEqual(result['lookupStride'],4)
        self.assertTrue(result['staticOnly'])
    def test_original_draw_position_uses_th128_center_not_th12_center(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        native = importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native, 'draw_position'), 'native bullet viewport transform is missing')
        result = native.draw_position((-128, 4, .125))
        self.assertEqual(result['stop'], {'reason': 'boundary', 'address': '0x00408f88'})
        self.assertEqual(result['position'], [192, 20, .125])
    def test_emitter_position_uses_original_threshold_and_host_coordinates(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        native = importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        self.assertTrue(hasattr(native, 'position'), '501 position policy is not proven')
        for override, expected in (((10, 20, 0), (-125, -28, 2)), ((10, 20, 1), (13, 24, 0))):
            with self.subTest(override=override):
                result = native.position((-128, -32, 2), (3, 4, 0), override, 0)
                self.assertEqual(result['stop']['reason'], 'unsupported_call')
                self.assertEqual(result['stop']['target'], '0x0040a8f0')
                self.assertEqual(result['emitter']['position'], list(expected))

    def test_runtime_profile_is_read_from_the_pinned_original_table(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        module = importlib.import_module('bullet_pool_probe')
        self.assertTrue(hasattr(module, 'prepare_profile'), 'runtime profile preparation is missing')
        result = module.prepare_profile(os.environ['TH128_BULLET_EXE'])
        self.assertEqual(result['bytes'], 288)
        self.assertEqual(result['sourceAddress'], '0x004b2dd0')
        self.assertEqual((ROOT / 'local/runtime/bullet-profile-8.bin').stat().st_size, 288)
        import json
        bindings=json.loads((ROOT/'local/runtime/sound-bindings.json').read_bytes())
        self.assertEqual(len(bindings['sounds']),76,'profile preparation must not downgrade complete native sound bindings')
        self.assertEqual(bindings['sounds']['38']['name'],'se_item00.wav')

    def test_free_skip_and_wrap_are_original_constructor_prefixes(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'), 'original pool selection probe missing')
        module = importlib.import_module('bullet_pool_probe')
        native = module.NativePool(os.environ['TH128_BULLET_EXE'])
        for identifier, cursor, states, expected in (
                ('free', 0, {0: 0}, 0), ('skip-one', 0, {0: 1, 1: 0}, 1),
                ('skip-two', 0, {0: 2, 1: 3, 2: 0}, 2),
                ('wrap', 1999, {1999: 1, 0: 0}, 0),
                ('wrap-skip', 1999, {1999: 2, 0: 1, 1: 0}, 1)):
            with self.subTest(identifier=identifier):
                result = native.select(cursor, states)
                self.assertEqual(result['stop'], {'reason': 'boundary', 'address': '0x004092dd'})
                self.assertEqual(result['selected'], expected)
                self.assertFalse(result['completedEmission'])

    def test_bad_pool_spans_reject_instead_of_mapping_an_unbounded_pool(self):
        self.assertIsNotNone(importlib.util.find_spec('bullet_pool_probe'))
        native = importlib.import_module('bullet_pool_probe').NativePool(os.environ['TH128_BULLET_EXE'])
        for cursor, states in ((2000, {0: 0}), (0, {2000: 0}), (0, {0: 6}), (0, {index: 1 for index in range(10)})):
            with self.subTest(cursor=cursor, count=len(states)), self.assertRaises(ValueError):
                native.select(cursor, states)


if __name__ == '__main__':
    unittest.main()
