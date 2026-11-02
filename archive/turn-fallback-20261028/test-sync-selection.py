import importlib.util
import json
import pathlib
import subprocess
import tempfile
import unittest

spec = importlib.util.spec_from_file_location('sync_selection', pathlib.Path(__file__).with_name('sync-selection.py'))
sync = importlib.util.module_from_spec(spec)
spec.loader.exec_module(sync)


class PublicationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        self.active = self.root / 'active.json'
        self.bridge = self.root / 'stunnel.conf'
        self.bridge.write_text('[turn-tls]\nconnect = 192.0.2.1:3478\n')
        self.old = {'urls': ['turn:192.0.2.1:3478?transport=udp'], 'sharedSecret': 'fixture-old'}
        self.active.write_text(json.dumps(self.old))
        self.config = {
            'activeConfig': str(self.active), 'stateFile': str(self.root / 'state.json'),
            'exportFile': str(self.root / 'export.json'), 'exportLock': str(self.root / 'export.lock'),
            'tlsUrl': 'turns:example.invalid:8443?transport=tcp',
            'tlsBridge': {'config': str(self.bridge), 'unit': 'fixture-tls'},
            'nodes': {'backup': {'target': '192.0.2.2:3478',
                                'urls': ['turn:192.0.2.2:3478?transport=udp', 'turn:192.0.2.2:3478?transport=tcp']}}}
        self.value = {'schema': 'eagler/turn-selection/1', 'selected': 'backup',
                      'evaluatedAt': 990, 'urls': self.config['nodes']['backup']['urls'],
                      'sharedSecret': 'fixture-new'}
        self.calls = []

    def reload(self, command, **kwargs):
        self.assertEqual(json.loads(self.active.read_text()), self.old)
        self.assertIn('192.0.2.2:3478', self.bridge.read_text())
        self.calls.append(command)

    def test_node_change_updates_tls_before_credentials_and_exports_matching_target(self):
        result = sync.apply(self.value, self.config, run=self.reload, clock=lambda: 1000)
        self.assertEqual(self.calls, [['systemctl', 'reload', 'fixture-tls']])
        self.assertTrue(result['tlsReloaded'])
        self.assertEqual(json.loads(self.active.read_text())['sharedSecret'], 'fixture-new')
        exported = json.loads((self.root / 'export.json').read_text())
        self.assertEqual(exported['tlsTarget'], '192.0.2.2:3478')
        self.assertEqual(exported['evaluatedAt'], 990)
        self.assertNotIn('fixture-new', json.dumps(result))
        sync.apply(self.value, self.config, run=lambda *a, **k: self.fail('Repeated reload'), clock=lambda: 1001)

    def test_failed_reload_rolls_back_and_preserves_credentials(self):
        before = self.bridge.read_bytes()
        def fail(command, **kwargs):
            self.calls.append(command)
            if kwargs['check']:
                raise subprocess.CalledProcessError(1, command)
        with self.assertRaises(RuntimeError):
            sync.apply(self.value, self.config, run=fail, clock=lambda: 1000)
        self.assertEqual(self.bridge.read_bytes(), before)
        self.assertEqual(json.loads(self.active.read_text()), self.old)
        self.assertFalse((self.root / 'export.json').exists())

    def test_stale_unknown_and_wrong_node_publications_do_not_replace_active(self):
        for change in [{'evaluatedAt': 1}, {'selected': 'unknown'}, {'urls': ['turn:192.0.2.99:3478']}]:
            with self.assertRaises(ValueError):
                sync.apply({**self.value, **change}, self.config, clock=lambda: 1000)
            self.assertEqual(json.loads(self.active.read_text()), self.old)

    def test_mirror_requires_matching_tls_target(self):
        self.config.pop('tlsBridge')
        self.config['requireTlsTarget'] = True
        for target in [None, '192.0.2.1:3478']:
            with self.assertRaises(ValueError):
                sync.apply({**self.value, 'tlsTarget': target}, self.config, clock=lambda: 1000)
        sync.apply({**self.value, 'tlsTarget': '192.0.2.2:3478'}, self.config, clock=lambda: 1000)
        self.assertEqual(json.loads(self.active.read_text())['sharedSecret'], 'fixture-new')

    def test_older_publication_cannot_override_a_newer_one(self):
        sync.apply(self.value, self.config, run=self.reload, clock=lambda: 1000)
        with self.assertRaises(ValueError):
            sync.apply({**self.value, 'evaluatedAt': 989}, self.config, clock=lambda: 1000)


if __name__ == '__main__':
    unittest.main()
