"""Collector bookkeeping only; these mocks are not native gameplay evidence."""
import contextlib
import importlib.util
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import MagicMock, patch


SOURCE = Path(__file__).with_name('capture-current-demos.py')
SPEC = importlib.util.spec_from_file_location('th10_demo_collector', SOURCE)
COLLECTOR = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(COLLECTOR)


class CaptureEvidenceTests(unittest.TestCase):
    def run_collector(self, *, navigation_error=None, evaluations=None):
        with tempfile.TemporaryDirectory() as directory:
            playwright = MagicMock()
            browser = playwright.chromium.launch.return_value
            context = browser.new_context.return_value
            page = context.new_page.return_value
            page.goto.side_effect = navigation_error
            page.evaluate.side_effect = evaluations
            with patch.object(COLLECTOR, 'sync_playwright') as factory, patch(
                'sys.argv', [str(SOURCE), '--output', directory]
            ), contextlib.redirect_stdout(io.StringIO()):
                factory.return_value.__enter__.return_value = playwright
                failure = None
                try:
                    COLLECTOR.main()
                except RuntimeError as error:
                    failure = error
            report = json.loads((Path(directory) / 'suite.json').read_text())
            context.close.assert_called_once()
            browser.close.assert_called_once()
            return report, failure

    def test_navigation_failure_leaves_incomplete_evidence(self):
        report, failure = self.run_collector(navigation_error=RuntimeError('connection refused'))
        self.assertIsNotNone(failure)
        self.assertFalse(report['complete'])
        self.assertEqual(report['phase'], 'navigation')
        self.assertEqual(report['demos'], [])
        self.assertIn('connection refused', report['failure'])

    def test_boot_failure_is_not_an_unattempted_gate(self):
        report, failure = self.run_collector(evaluations=[RuntimeError('Runtime preparation timeout')])
        self.assertIsNotNone(failure)
        self.assertFalse(report['complete'])
        self.assertEqual(report['phase'], 'runtime-boot')
        self.assertEqual(report['applicationTicks'], 0)
        self.assertIn('preparation timeout', report['failure'])

    def test_interrupted_capture_preserves_completed_and_partial_rows(self):
        demo = {'rotationIndex': 0, 'rows': [{'frame': 1}]}
        partial = {'rotationIndex': 1, 'rows': [{'frame': 1}, {'frame': 2}]}
        report, failure = self.run_collector(evaluations=[
            {'build': {'wasm': 'test-only'}, 'initial': [0]},
            {'demos': [demo], 'done': False, 'applicationTicks': 300,
             'tail': [{'applicationTick': 300}], 'partial': partial},
            RuntimeError('native tick failed'),
        ])
        self.assertIsNotNone(failure)
        self.assertFalse(report['complete'])
        self.assertEqual(report['phase'], 'capture')
        self.assertEqual(report['demos'], [demo])
        self.assertEqual(report['partial'], partial)
        self.assertEqual(report['applicationTicks'], 300)

    def test_completion_remains_separate_from_the_golden_comparator(self):
        demos = [{'rotationIndex': i, 'rows': [{'frame': n} for n in range(3000)]}
                 for i in range(4)]
        report, failure = self.run_collector(evaluations=[
            {'build': {'wasm': 'test-only'}, 'initial': [0]},
            {'demos': demos, 'done': True, 'applicationTicks': 16000,
             'tail': [], 'partial': None},
        ])
        self.assertIsNone(failure)
        self.assertTrue(report['complete'])
        self.assertEqual(report['phase'], 'completed')
        self.assertNotIn('passed', report)
        self.assertEqual([len(x['rows']) for x in report['demos']], [3000] * 4)

    def test_boot_and_freeze_share_one_browser_evaluation(self):
        bootstrap = COLLECTOR.BOOTSTRAP
        self.assertIn('async () =>', bootstrap)
        self.assertLess(bootstrap.index('await presentationLab.boot()'),
                        bootstrap.index('controller.freeze()'))
        self.assertIn('if(initial[14]===1)throw', bootstrap)


if __name__ == '__main__':
    unittest.main()
