import copy
from pathlib import Path
import runpy
import unittest

compare = runpy.run_path(str(Path(__file__).with_name('compare-resource-traces.py')))['compare']


def pair():
    doc = {'schema': 'th08mp/resource-trace/1', 'runtimeGeneration': 'fixture-identity',
           'wasmSha256': 'fixture-bytes', 'valueFields': ['time', 'P1.power', 'rngCalls'],
           'itemFields': ['slot', 'type', 'owner'], 'inputFields': ['buttons'],
           'metadata': {'localPlayer': 0, 'sessionId': '1', 'generation': 0, 'playerCount': 2,
                        'seed': 1234, 'difficulty': 1, 'loadouts': [0, 1],
                        'confirmedThrough': 102, 'lastFrame': 103, 'rollbackFrame': 0xffffffff,
                        'traceOverflow': False, 'restoreMismatch': None},
           'records': [{'frame': f, 'revision': 1,
                        'attempt': {'complete': True, 'inputs': [0, 0],
                                    'end': {'values': [2000, 80, 123], 'items': [[7, 0, 0]]},
                                    'events': []}} for f in range(100, 104)]}
    other = copy.deepcopy(doc)
    other['metadata']['localPlayer'] = 1
    return doc, other


class CompareTest(unittest.TestCase):
    def test_equal_window(self):
        a, b = pair()
        result = compare(a, b)
        self.assertEqual(result['status'], 'equal-in-retained-window')
        self.assertEqual(result['commonWindow'], [100, 102])

    def test_power_time_owner(self):
        for field in ('power', 'time', 'owner'):
            with self.subTest(field=field):
                a, b = pair()
                end = b['records'][1]['attempt']['end']
                if field == 'owner':
                    end['items'][0][2] = 1
                else:
                    end['values'][1 if field == 'power' else 0] += 20
                result = compare(a, b)
                self.assertEqual(result['status'], 'confirmed-divergence')
                self.assertEqual(result['firstDivergence'], 101)

    def test_predicted_difference_is_not_desync(self):
        a, b = pair()
        b['records'][-1]['attempt']['end']['values'][0] = 0
        self.assertEqual(compare(a, b)['status'], 'equal-in-retained-window')

    def test_latest_revision_wins(self):
        a, b = pair()
        stale = copy.deepcopy(b['records'][1])
        stale['attempt']['end']['values'][1] = 0
        b['records'][1]['revision'] = 2
        b['records'].append(stale)
        self.assertEqual(compare(a, b)['status'], 'equal-in-retained-window')

    def test_overflow_and_missing_are_inconclusive(self):
        a, b = pair()
        b['metadata']['traceOverflow'] = True
        self.assertEqual(compare(a, b)['status'], 'inconclusive')
        a, b = pair()
        del b['records'][1]
        self.assertEqual(compare(a, b)['status'], 'inconclusive')

    def test_wrong_session_rejected(self):
        a, b = pair()
        b['metadata']['sessionId'] = 'another-room'
        self.assertEqual(compare(a, b)['status'], 'inconclusive')

    def test_missing_inputs_rejected(self):
        a, b = pair()
        del a['records'][1]['attempt']['inputs']
        del b['records'][1]['attempt']['inputs']
        self.assertEqual(compare(a, b)['status'], 'inconclusive')

    def test_input_mismatch_is_identified_before_resources(self):
        a, b = pair()
        b['records'][1]['attempt']['inputs'] = [4, 0]
        result = compare(a, b)
        self.assertEqual(result['firstInputDivergence'], 101)
        self.assertIsNone(result['firstResourceDivergence'])


if __name__ == '__main__':
    unittest.main()
