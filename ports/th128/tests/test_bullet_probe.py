import importlib
import importlib.util
import os
import struct
import sys
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools' / 'research'))
ORIGINAL = os.environ.get('TH128_BULLET_EXE')


class BulletProbeSafetyTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.probe = importlib.import_module('bullet_probe')

    def test_output_is_confined_to_owned_bullet_json(self):
        accepted = ROOT / 'local/research/bullet-fixtures.json'
        self.assertEqual(self.probe.report_destination(accepted), accepted.resolve())
        for path in (ROOT / 'README.md', ROOT / 'local/research/player-fixtures.json',
                     Path('E:/gee/bullet-test.json'), accepted.parent / 'nested/bullet-x.json'):
            with self.subTest(path=path), self.assertRaises(ValueError):
                self.probe.report_destination(path)

    def test_code_data_state_and_instruction_limits_are_explicit(self):
        for arguments in ({'budget': 0}, {'budget': 20001},
                          {'chunks': [(0x410000, b'\x90' * 65537)]},
                          {'data': [self.probe.datum('large', 0x300000, bytes(131073))]},
                          {'chunks': [(0x410000, b'\xc3')] * 33}):
            options = {'chunks': [(0x410000, b'\xc3')], 'entry': 0x410000}
            options.update(arguments)
            with self.subTest(arguments=list(arguments)), self.assertRaises(ValueError):
                self.probe.emulate(**options)

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'provide Unicorn')
    def test_unsupported_calls_remain_prefixes(self):
        result = self.probe.emulate([(0x410000, bytes.fromhex('e8fbff0000c3'))], 0x410000)
        self.assertEqual(result['stop']['reason'], 'unsupported_call')
        self.assertEqual(result['stop']['target'], '0x00420000')
        self.assertEqual(result['registers']['esp'], self.probe.STACK_POINTER)

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'provide Unicorn')
    def test_page_padding_is_not_a_declared_field(self):
        result = self.probe.emulate([(0x410000, bytes.fromhex('a104003000c3'))], 0x410000,
                                   data=[self.probe.datum('only', 0x300000, 1)])
        self.assertEqual(result['stop']['reason'], 'outside_data')

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'provide Unicorn')
    def test_readonly_jump_table_can_share_a_code_page_without_becoming_code(self):
        result = self.probe.emulate([(0x410000, bytes.fromhex('a110004100c3'))], 0x410000,
                                   data=[self.probe.datum('table', 0x410010, 37, writable=False)])
        self.assertEqual(result['stop']['reason'], 'returned')
        self.assertEqual(result['registers']['eax'], 37)
        result = self.probe.emulate([(0x410000, bytes.fromhex('eb0e'))], 0x410000,
                                   data=[self.probe.datum('table', 0x410010, b'\xc3', writable=False)])
        self.assertEqual(result['stop']['reason'], 'outside_code')

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'provide Unicorn')
    def test_infinite_loop_is_not_completed_proof(self):
        result = self.probe.emulate([(0x410000, bytes.fromhex('ebfe'))], 0x410000, budget=7)
        self.assertEqual(result['stop']['reason'], 'budget_exhausted')
        self.assertEqual(result['instructionsExecuted'], 7)

    def test_actual_first_sequence_has_distinct_count_and_loop_locals(self):
        resource = self.probe.load_first_emitter()
        choose = next(item for item in resource['commands'] if item['opcode'] == 435)
        count = next(item for item in resource['commands'] if item['opcode'] == 522)
        self.assertEqual(struct.unpack('<5i', bytes.fromhex(choose['raw'])), (8, 3, 3, 5, 7))
        self.assertEqual(count['mask'], 28)
        self.assertEqual(struct.unpack('<9i', bytes.fromhex(count['raw'])),
                         (0, 1, 4, 4, 4, 1, 1, 1, 1))
        self.assertEqual(resource['initialLocals'], {'4': 1, '8': 0})


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'),
                     'set TH128_BULLET_EXE and provide Unicorn for original bytes')
class OriginalBulletTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.report = importlib.import_module('bullet_probe').probe(Path(ORIGINAL))
        cls.cases = {case['id']: case for case in cls.report['cases']}

    def test_dispatch_is_recovered_from_pinned_th128_not_th12(self):
        expected = {435: 0x41a55f, 500: 0x41a67f, 501: 0x41a7bd, 502: 0x41a8c3,
                    504: 0x41aa39, 507: 0x41af99, 509: 0x41b009,
                    521: 0x41aafb, 522: 0x41ab92}
        self.assertEqual({int(key): int(value['handler'], 16)
                          for key, value in self.report['dispatch'].items()}, expected)
        self.assertTrue(self.report['executable']['unchanged'])
        self.assertFalse(self.report['windowsGameProcessLaunched'])

    def test_reset_clears_native_block_and_sets_th128_defaults(self):
        case = self.cases['reset-0']
        self.assertEqual(case['stop']['reason'], 'boundary')
        state = case['emitter']
        self.assertEqual((state['type'], state['color'], state['angle'], state['spacing']), (0, 0, 0, 0))
        self.assertEqual((state['speed'], state['endSpeed'], state['count'], state['layers']), (2, 0, 1, 1))
        self.assertEqual((state['aimMode'], state['flags'], state['shotSound'], state['transformSound']),
                         (0, 35, 22, 40))
        self.assertTrue(case['adjacentEmittersUnchanged'])

    def test_rank_choose_and_emitter_rank_have_different_invalid_rank_behavior(self):
        for difficulty, expected in ((0, 3), (1, 3), (2, 5), (3, 7), (4, 7), (-1, 91), (5, 91)):
            case = self.cases[f'choose-{difficulty}']
            self.assertEqual(case['locals']['8'], expected)
            self.assertEqual(case['stop']['reason'], 'boundary')
        for difficulty in (-1, 4, 5):
            state = self.cases[f'ranked-{difficulty}']['emitter']
            self.assertEqual((state['speed'], state['endSpeed'], state['count'], state['layers']),
                             (4.5, 8.5, -32768, -1))

    def test_first_sequence_is_one_shot_at_each_rank_and_preserves_raw_etex(self):
        for difficulty, speed, limit in ((0, 1, 3), (1, 1, 3), (2, 2, 5), (3, 2, 7), (4, 2, 7)):
            case = self.cases[f'first-{difficulty}']
            state = case['emitter']
            self.assertEqual((state['type'], state['color'], state['count'], state['layers']), (8, 10, 1, 1))
            self.assertEqual((state['speed'], state['endSpeed'], state['aimMode']), (speed, 1, 0))
            self.assertEqual(case['locals']['8'], limit)
            self.assertEqual(state['extension0'], {'parameterR': -999999.0, 'parameterS': -999999.0,
                                                  'parameterA': 1, 'parameterB': -999999, 'type': 2, 'control': 1})
        case = self.cases['extension-distinct-parameters']
        self.assertEqual(case['stop']['reason'], 'boundary')
        self.assertEqual(case['emitter']['extension0'],
                         {'parameterR': .625, 'parameterS': -8.25, 'parameterA': -123456,
                          'parameterB': 987654, 'type': 37, 'control': 42})

    def test_geometry_prefix_is_alternating_and_layer_speed_is_interpolated(self):
        for index, angle in enumerate((1.0, .75, 1.25)):
            case = self.cases[f'geometry-odd-{index}']
            self.assertEqual(case['stop']['reason'], 'boundary')
            self.assertEqual(case['geometry']['rawAngle'], angle)
            self.assertEqual(case['geometry']['speed'], 2.5)
            self.assertFalse(case['completedEmission'])
        for index, angle in enumerate((1.125, .875, 1.375, .625)):
            case = self.cases[f'geometry-even-{index}']
            self.assertEqual(case['geometry']['rawAngle'], angle)

    def test_first_fire_and_aim_are_not_falsely_reported_as_completed(self):
        for difficulty in range(5):
            fire = self.cases[f'fire-prefix-{difficulty}']
            self.assertEqual(fire['stop']['reason'], 'unsupported_call')
            self.assertEqual(fire['stop']['target'], '0x0040a8f0')
            sample = self.cases[f'geometry-first-{difficulty}']
            self.assertEqual(sample['stop']['reason'], 'boundary')
            self.assertAlmostEqual(sample['geometry']['rawAngle'], 1.5707963705062866)
            self.assertEqual(sample['geometry']['speed'], 1 if difficulty < 2 else 2)
            self.assertFalse(sample['completedEmission'])


if __name__ == '__main__':
    unittest.main()
