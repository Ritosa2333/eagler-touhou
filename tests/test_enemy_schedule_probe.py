import importlib
import importlib.util
import os
import sys
import unittest
from pathlib import Path
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools' / 'research'))
ORIGINAL = os.environ.get('TH128_ENEMY_SCHEDULE_EXE')


class EnemyScheduleContractTests(unittest.TestCase):
    def setUp(self):
        self.assertIsNotNone(importlib.util.find_spec('enemy_schedule_probe'),
                             'bounded enemy scheduling probe is missing')
        self.probe = importlib.import_module('enemy_schedule_probe')

    def test_destination_is_confined_to_new_private_scheduling_json(self):
        for destination in (ROOT / 'README.md', ROOT / 'local/research/enemy-fixtures.json',
                            ROOT / 'local/research/enemy-scheduling/nested.json',
                            ROOT / 'local/research/enemy-scheduling.txt',
                            Path('E:/gee/enemy-scheduling.json')):
            with self.subTest(destination=destination), self.assertRaises(ValueError):
                self.probe.report_destination(destination)
        destination = ROOT / 'local/research/enemy-scheduling.json'
        self.assertEqual(self.probe.report_destination(destination), destination.resolve())

    def test_wrong_hash_is_rejected_before_pe_or_emulator_use(self):
        with patch.object(Path, 'read_bytes', return_value=b'not the original executable'):
            with self.assertRaisesRegex(ValueError, 'SHA-256'):
                self.probe.probe(Path('unused.exe'))

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'optional Unicorn is not installed')
    def test_unknown_call_stops_before_any_invented_return(self):
        result = self.probe.emulate([(0x410000, bytes.fromhex('e8fbff0f00c3'))], 0x410000)
        self.assertEqual(result['stop']['reason'], 'unsupported_call')
        self.assertEqual(result['stop']['target'], '0x00510000')
        self.assertEqual(result['registers']['esp'], self.probe.STACK_POINTER)
        self.assertEqual(result['instructionTrace'], [])

    @unittest.skipUnless(importlib.util.find_spec('unicorn'), 'optional Unicorn is not installed')
    def test_budget_cannot_silently_become_an_unbounded_run(self):
        with self.assertRaisesRegex(ValueError, 'bounded'):
            self.probe.emulate([(0x410000, bytes.fromhex('ebfe'))], 0x410000, budget=0)
        result = self.probe.emulate([(0x410000, bytes.fromhex('ebfe'))], 0x410000, budget=3)
        self.assertEqual(result['stop']['reason'], 'budget_exhausted')
        self.assertEqual(result['instructionsExecuted'], 3)


@unittest.skipUnless(ORIGINAL and importlib.util.find_spec('unicorn'),
                     'set TH128_ENEMY_SCHEDULE_EXE and install optional Unicorn')
class OriginalEnemyScheduleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.probe = importlib.import_module('enemy_schedule_probe')
        cls.report = cls.probe.probe(Path(ORIGINAL))
        cls.cases = {case['id']: case for case in cls.report['cases']}

    def test_static_edges_are_decoded_from_real_instruction_boundaries(self):
        edges = {edge['call']: edge['target'] for edge in self.report['directCalls']}
        expected = {'0x00415004': '0x0047296c', '0x00415016': '0x00415a70',
                    '0x00415180': '0x00416160', '0x004153c0': '0x004681d0',
                    '0x00415927': '0x00416160', '0x00415a52': '0x004158f0',
                    '0x00416200': '0x0041eb00', '0x004164e1': '0x0046fe20',
                    '0x0046fe52': '0x0046e100'}
        for location, target in expected.items():
            with self.subTest(location=location):
                self.assertEqual(edges[location], target)
        self.assertEqual(self.report['layout']['managerGlobal'], '0x004b8948')
        self.assertEqual(self.report['layout']['nodeOffset'], '0x145c')
        self.assertFalse(self.report['fullNativeScheduleProven'])
        self.assertFalse(self.report['fullSpawnExecuted'])

    def test_static_constructor_continuation_does_not_branch_on_initial_update_result(self):
        self.assertIn('constructor_after_initial_update', self.report['code'])
        continuation = self.report['code']['constructor_after_initial_update']['instructions']
        self.assertEqual(continuation[0], {'address': '0x00415185', 'mnemonic': 'mov',
                                           'operands': 'eax, dword ptr [ebx + 0x11c0]'})
        self.assertIn('lower_constructor_remaining', self.report['code'])
        tail = self.report['code']['lower_constructor_remaining']['instructions']
        self.assertEqual(tail[-1], {'address': '0x00415eb0', 'mnemonic': 'ret', 'operands': '4'})

    def test_catalog_marks_truncated_inherited_helper_as_partial(self):
        for name, record in self.report['code'].items():
            with self.subTest(name=name):
                self.assertIn('decodedBytes', record, 'catalog must disclose undecoded original bytes')
                self.assertEqual(record['decodedBytes'] + record['undecodedBytes'],
                                 int(record['endExclusive'], 16) - int(record['start'], 16))
                self.assertEqual(record['completeDecoding'], record['undecodedBytes'] == 0)
        normalize = self.report['code']['normalize']
        self.assertFalse(normalize['completeDecoding'])
        self.assertEqual(normalize['undecodedBytes'], 1)
        self.assertEqual(normalize['evidence'], 'partial-static-decoding')

    def test_constructor_allocator_is_an_explicit_unexecuted_boundary(self):
        case = self.cases['spawn-entry-allocation-boundary']
        self.assertEqual(case['stop'], {'reason': 'unsupported_call',
                                       'address': '0x00415004', 'target': '0x0047296c'})
        self.assertNotIn('0x00415180', case['instructionTrace'])

    def test_original_constructor_state_clear_resets_the_guard(self):
        case = self.cases['constructor-state-clear']
        self.assertEqual(case['stop']['reason'], 'boundary')
        self.assertEqual(case['observations']['enemyAFlags']['beforeU32'], 0xA5A5A5A5)
        self.assertEqual(case['observations']['enemyAFlags']['afterU32'], 0)
        self.assertIn('0x0047bb30', case['instructionTrace'])

    def test_original_node_initializer_stores_owner_and_null_links(self):
        case = self.cases['constructor-node-init']
        self.assertEqual(case['stop']['reason'], 'boundary')
        for field in ('enemyANodeOwner', 'enemyANext', 'enemyAPrev'):
            self.assertEqual(case['observations'][field]['beforeU32'], 0xA5A5A5A5)
        self.assertEqual(case['observations']['enemyANodeOwner']['afterU32'], self.probe.ENEMY_A)
        self.assertEqual(case['observations']['enemyANext']['afterU32'], 0)
        self.assertEqual(case['observations']['enemyAPrev']['afterU32'], 0)

    def test_initial_update_reaches_ecl_before_enemy_is_linked(self):
        case = self.cases['prepared-constructor-to-ecl']
        self.assertEqual(case['entry'], '0x00415021')
        self.assertEqual(case['stop'], {'reason': 'boundary', 'address': '0x004164e1'})
        self.assertIn('0x00415180', case['instructionTrace'])
        self.assertIn('0x00416330', case['instructionTrace'])
        self.assertNotIn('0x004151fc', case['instructionTrace'])
        self.assertEqual(case['observations']['managerHead']['afterU32'], 0)
        self.assertEqual(case['observations']['managerCount']['afterU32'], 0)
        self.assertEqual(case['observations']['enemyAFlags']['afterU32'] & 0x80000, 0x80000)

    def test_empty_list_and_tail_append_use_real_constructor_suffix(self):
        node_a = self.probe.ENEMY_A + 0x145C
        node_b = self.probe.ENEMY_B + 0x145C
        empty = self.cases['insert-empty']
        self.assertEqual(empty['stop']['reason'], 'returned')
        self.assertEqual(empty['observations']['managerHead']['afterU32'], node_a)
        self.assertEqual(empty['observations']['managerTail']['afterU32'], node_a)
        self.assertEqual(empty['observations']['managerCount']['afterU32'], 1)
        append = self.cases['insert-tail']
        self.assertEqual(append['stop']['reason'], 'returned')
        self.assertEqual(append['observations']['managerHead']['afterU32'], node_a)
        self.assertEqual(append['observations']['managerTail']['afterU32'], node_b)
        self.assertEqual(append['observations']['enemyANext']['afterU32'], node_b)
        self.assertEqual(append['observations']['enemyBPrev']['afterU32'], node_a)
        self.assertEqual(append['observations']['enemyBNext']['afterU32'], 0)
        self.assertEqual(append['observations']['managerCount']['afterU32'], 2)

    def test_guarded_manager_visits_clear_guard_without_motion_or_ecl(self):
        case = self.cases['manager-two-guarded']
        self.assertEqual(case['stop'], {'reason': 'boundary', 'address': '0x00415954'})
        self.assertEqual(case['instructionTrace'].count('0x00415927'), 2)
        self.assertEqual(case['instructionTrace'].count('0x004728fd'), 2)
        self.assertEqual(case['observations']['enemyAFlags']['afterU32'], 0x20)
        self.assertEqual(case['observations']['enemyBFlags']['afterU32'], 0)
        self.assertNotIn('0x0041618e', case['instructionTrace'])
        self.assertNotIn('0x00416330', case['instructionTrace'])
        self.assertNotIn('0x004164e1', case['instructionTrace'])

    def test_eligible_manager_path_interpolates_then_moves_before_ecl(self):
        case = self.cases['manager-eligible-to-ecl']
        self.assertEqual(case['stop'], {'reason': 'boundary', 'address': '0x004164e1'})
        trace = case['instructionTrace']
        ordered = ['0x0041591c', '0x00415927', '0x0041618e', '0x00416200',
                   '0x00416330', '0x004163b6', '0x004163eb', '0x004163f0']
        self.assertEqual([trace.index(location) for location in ordered],
                         sorted(trace.index(location) for location in ordered))
        self.assertEqual(case['observations']['enemyAInterpolationCurrent']['afterI32'], 1)
        self.assertEqual(case['observations']['enemyAAbsoluteSpeed']['afterF32'], 1.0)
        self.assertEqual(case['observations']['enemyAAbsoluteX']['afterF32'], -127.0)
        self.assertEqual(case['observations']['enemyACombinedX']['afterF32'], -127.0)
        self.assertEqual(case['observations']['enemyAFlags']['afterU32'] & 0x80000, 0x80000)

    def test_manager_caches_successor_before_state_update(self):
        for identifier, expected in (('cache-next-tail', 0),
                                      ('cache-next-nontail', self.probe.ENEMY_B + 0x145C)):
            with self.subTest(identifier=identifier):
                case = self.cases[identifier]
                self.assertEqual(case['stop'], {'reason': 'boundary', 'address': '0x00415927'})
                self.assertEqual(case['registers']['ebx'], expected)
                self.assertNotIn('0x00416160', case['instructionTrace'])

    def test_cached_null_does_not_reread_a_newly_appended_successor(self):
        case = self.cases['cached-null-after-append']
        self.assertEqual(case['entry'], '0x0041594d')
        self.assertEqual(case['stop'], {'reason': 'boundary', 'address': '0x00415953'})
        self.assertEqual(case['observations']['enemyANext']['afterU32'], self.probe.ENEMY_B + 0x145C)
        self.assertEqual(case['observations']['enemyBFlags']['afterU32'], 0x80000)
        self.assertNotIn('0x00415927', case['instructionTrace'])

    def test_cached_old_successor_can_lead_to_new_tail_but_only_clear_guard(self):
        case = self.cases['cached-next-after-append']
        self.assertEqual(case['entry'], '0x0041594d')
        self.assertEqual(case['instructionTrace'].count('0x00415927'), 2)
        self.assertEqual(case['observations']['enemyBFlags']['afterU32'], 0)
        self.assertEqual(case['observations']['enemyCFlags']['afterU32'], 0)
        self.assertNotIn('0x00416330', case['instructionTrace'])

    def test_deletion_branch_does_not_fabricate_a_virtual_destructor(self):
        case = self.cases['manager-removal-boundary']
        self.assertEqual(case['stop']['reason'], 'unsupported_call')
        self.assertEqual(case['stop']['address'], '0x0041594b')
        self.assertNotIn('0x00415927', case['instructionTrace'])

    def test_wrapper_gate_stops_without_advancing_or_clearing_enemies(self):
        for identifier in ('wrapper-missing-control', 'wrapper-blocked-1', 'wrapper-blocked-2',
                            'wrapper-blocked-4', 'wrapper-blocked-400'):
            with self.subTest(identifier=identifier):
                case = self.cases[identifier]
                self.assertEqual(case['stop']['reason'], 'returned')
                self.assertEqual(case['registers']['eax'], 1)
                self.assertEqual(case['observations']['enemyAFlags']['afterU32'], 0x80000)
                self.assertNotIn('0x004158f0', case['instructionTrace'])
        case = self.cases['wrapper-active']
        self.assertIn('0x00415a52', case['instructionTrace'])
        self.assertEqual(case['observations']['enemyAFlags']['afterU32'], 0)

    def test_callback_registration_and_scheduler_boundary_are_distinct(self):
        registered = self.cases['register-manager-callback']
        self.assertEqual(registered['observations']['callbackPriority']['afterI32'], 21)
        self.assertEqual(registered['observations']['callbackFunction']['afterU32'], 0x415A20)
        self.assertEqual(registered['observations']['callbackContext']['afterU32'], self.probe.MANAGER)
        self.assertEqual(registered['observations']['schedulerHead']['afterU32'], self.probe.CALLBACK + 0x14)
        dispatched = self.cases['scheduler-callback-boundary']
        self.assertEqual(dispatched['stop']['reason'], 'unsupported_call')
        self.assertEqual(dispatched['stop']['address'], '0x0046838d')
        self.assertEqual(dispatched['registers']['edx'], 0x415A20)
        self.assertEqual(dispatched['registers']['ecx'], self.probe.MANAGER)
        self.assertNotIn('0x00415a20', dispatched['instructionTrace'])

    def test_literal_main_request_is_not_proof_of_initializer_or_browser_frame_order(self):
        self.assertTrue('rootCreation' in self.report, 'root initialization evidence is missing')
        root = self.report['rootCreation']
        self.assertFalse(root['insideManagerInitializerProven'])
        self.assertFalse(root['initializerToRootOrderProven'])
        self.assertFalse(self.report['browserFrameCadenceProven'])
        self.assertEqual(root['name'], 'main')
        self.assertEqual(root['literalNameAddress'], '0x004a28b8')
        edges = {edge['call']: edge['target'] for edge in self.report['directCalls']}
        self.assertEqual(edges['0x00415882'], '0x00415310')
        self.assertEqual(edges['0x00426ae9'], '0x00414ff0')
        self.assertEqual(edges['0x00426c6a'], '0x00414ff0')
        initializer = self.report['code']['manager_initializer']['instructions']
        self.assertFalse(any(item['mnemonic'] == 'call' and item['operands'] == '0x414ff0'
                             for item in initializer))
        case = self.cases['literal-main-request-allocation-boundary']
        self.assertEqual(case['stop'], {'reason': 'unsupported_call',
                                       'address': '0x00415004', 'target': '0x0047296c'})
        self.assertIn('0x00426ae9', case['instructionTrace'])
        stack_head = bytes.fromhex(case['stackHead'])
        name_offset = case['registers']['ebp'] + 8 - case['registers']['esp']
        parameter_offset = case['registers']['ebp'] + 12 - case['registers']['esp']
        self.assertEqual(int.from_bytes(stack_head[name_offset:name_offset + 4], 'little'), 0x4A28B8)
        parameters = case['stackSnapshots']['rootSpawnData']
        self.assertEqual(parameters['before'], 'a5' * 0x54)
        self.assertEqual(parameters['after'], '00' * 0x54)
        self.assertEqual(int.from_bytes(stack_head[parameter_offset:parameter_offset + 4], 'little'),
                         int(parameters['address'], 16))
        self.assertNotIn('0x00415180', case['instructionTrace'])

    def test_no_fixture_accepts_a_fault_or_silent_instruction_budget_exhaustion(self):
        self.assertEqual(self.report['caseCount'], len(self.cases))
        for case in self.report['cases']:
            with self.subTest(identifier=case['id']):
                self.assertIn(case['stop']['reason'], ('returned', 'boundary', 'unsupported_call'))
                self.assertGreater(case['instructionsExecuted'], 0)
                self.assertLess(case['instructionsExecuted'], case['instructionBudget'])


if __name__ == '__main__':
    unittest.main()
