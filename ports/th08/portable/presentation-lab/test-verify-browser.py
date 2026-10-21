import importlib.util
import sys
import types
import unittest
from pathlib import Path


try:
    from playwright.sync_api import sync_playwright  # noqa: F401
except ModuleNotFoundError:
    playwright = types.ModuleType('playwright')
    sync_api = types.ModuleType('playwright.sync_api')
    sync_api.sync_playwright = lambda: None
    playwright.sync_api = sync_api
    sys.modules['playwright'] = playwright
    sys.modules['playwright.sync_api'] = sync_api

MODULE_PATH = Path(__file__).with_name('verify-browser.py')
spec = importlib.util.spec_from_file_location('presentation_verify_browser', MODULE_PATH)
verify = importlib.util.module_from_spec(spec)
spec.loader.exec_module(verify)


def check(adaptation_id, status='passed', complete=True, machine_complete=True):
    return {'id': adaptation_id, 'status': status, 'complete': complete,
            'machineComplete': machine_complete, 'evidence': {'game': 'th08', 'variant': 'normal'}}


def pool_capture():
    retired = {'owner': 5, 'object': 8, 'generation': 4, 'tick': 10, 'operation': 2}
    born = {'owner': 5, 'object': 8, 'generation': 5, 'tick': 11, 'operation': 1}
    report = {
        'schema': 'th08/presentation-audit/1', 'valid': True, 'dropped': 0,
        'lifecycle': {'status': 'complete', 'dropped': 0},
        'label': 'display05-pool-reuse-boundary', 'tick': 11,
        'disappeared': [{'key': '5:8:4:0:0:0', 'ownerId': 5, 'generation': 4, 'status': 'lifecycle'}],
        'objects': [{'key': '5:8:5:0:0:0', 'ownerId': 5, 'object': 8, 'generation': 5,
                     'identityFlags': 3, 'status': 'unobserved',
                     'fields': [{'id': 'position', 'status': 'unobserved'}]}],
    }
    return {'event': {'retired': retired, 'born': born}, 'report': report}, [retired, born]


class PresentationVerifyTests(unittest.TestCase):
    def test_pool_reuse_requires_raw_report_and_real_generation_boundary(self):
        capture, events = pool_capture()
        boundary, passed = verify.pool_reuse_boundary(capture, events)
        self.assertTrue(passed)
        self.assertEqual(boundary['status'], 'passed')

        changed = {**capture, 'report': {**capture['report'], 'objects': [
            {**capture['report']['objects'][0], 'identityFlags': 1}]} }
        self.assertFalse(verify.pool_reuse_boundary(changed, events)[1])
        self.assertFalse(verify.pool_reuse_boundary(capture, [events[0]])[1])
        missing = {'event': capture['event']}
        self.assertFalse(verify.pool_reuse_boundary(missing, events)[1])

    def test_pool_reuse_requires_the_actual_analyzer_schema(self):
        capture, events = pool_capture()
        wrong = {**capture, 'report': {**capture['report'], 'schema': 'presentation-lab/report/1'}}
        self.assertFalse(verify.pool_reuse_boundary(wrong, events)[1])

    def test_scene_cut_uses_native_scene_and_loading_slots(self):
        title = {'scene': {'diagnostics': [1, 1, 0, 0] + [0] * 12,
                           'status': [1, 1, 0, 3, 0, 0, 0, 1, 0, 12]}}
        game = {'scene': {'diagnostics': [2, 2, 1, 0] + [0] * 12,
                          'status': [2, 1, 0, 3, 0, 0, 0, 1, 0, 253]}}
        evidence, passed = verify.scene_transition_evidence(title, game)
        self.assertTrue(passed)
        self.assertEqual(evidence['from']['activeScene'], 1)
        self.assertEqual(evidence['to']['activeScene'], 2)
        self.assertEqual(evidence['to']['loading'], 0)

        still_loading = {'scene': {'diagnostics': [2, 2, 1, 1] + [0] * 12,
                                   'status': [2, 1, 0, 3, 0, 0, 0, 0, 0, 253]}}
        self.assertFalse(verify.scene_transition_evidence(title, still_loading)[1])

    def test_paired_first_frame_and_read_checks_are_authoritative(self):
        pair = {'checks': [check('DISPLAY-07'), check('VERIFY-07')]}
        display = {'checks': [check('DISPLAY-07', 'incomplete', False, False),
                              check('VERIFY-07', 'failed', False, False)],
                   'humanReview': {'status': 'pending', 'groups': []}}
        report = verify.combined_acceptance(pair, display)
        rows = {row['id']: row for row in report['checks']}
        for adaptation_id in ('DISPLAY-07', 'VERIFY-07'):
            self.assertEqual(rows[adaptation_id]['status'], 'passed')
            self.assertTrue(rows[adaptation_id]['complete'])
            self.assertTrue(rows[adaptation_id]['machineComplete'])
            self.assertEqual(rows[adaptation_id]['evidence']['authoritativeSource'], 'pairedReplayRuns')

        for adaptation_id in ('DISPLAY-07', 'VERIFY-07'):
            no_pair = {'checks': [row for row in pair['checks'] if row['id'] != adaptation_id]}
            unpaired = verify.combined_acceptance(no_pair, {'checks': [check(adaptation_id)]})
            row = next(row for row in unpaired['checks'] if row['id'] == adaptation_id)
            self.assertEqual(row['status'], 'incomplete')
            self.assertFalse(row['complete'])
            self.assertFalse(row['machineComplete'])


if __name__ == '__main__':
    unittest.main()
