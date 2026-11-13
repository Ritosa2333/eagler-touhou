"""Check real HTTP font compression without changing decoded Runtime bytes."""
from pathlib import Path
from tempfile import TemporaryDirectory
from http.client import HTTPConnection
import gzip
import hashlib
import os
import socket
import subprocess
import time
import unittest

PROJECT = Path(__file__).resolve().parents[1]

class FontTransport(unittest.TestCase):
    def test_th14_font_gzip_identity_and_other_binary_paths(self):
        with TemporaryDirectory(prefix='th14-font-http-') as directory:
            root = Path(directory).resolve()
            generation = 'a' * 64
            font = f'runtime/th14/{generation}/fonts/font4.bin'
            sample = bytes(range(256)) * 1024
            paths = [font, f'runtime/th12/{generation}/fonts/font4.bin', 'downloads/font4.bin',
                     f'runtime/th14/{generation}/fonts/other.bin']
            for name in paths:
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_bytes(sample)
            with socket.socket() as reserved:
                reserved.bind(('127.0.0.1', 0))
                port = reserved.getsockname()[1]
            node = os.environ.get('EAGLER_NODE', 'node')
            child = subprocess.Popen([node, str(PROJECT / 'scripts/serve-static.mjs'), str(root), str(port)],
                                     env={**os.environ, 'EAGLER_TOUHOU_CROSS_ORIGIN_ISOLATION': '1'},
                                     stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            def request(name, encoding=None):
                connection = HTTPConnection('127.0.0.1', port, timeout=3)
                try:
                    headers = {} if encoding is None else {'Accept-Encoding': encoding}
                    connection.request('GET', '/' + name, headers=headers)
                    response = connection.getresponse()
                    return response.status, dict(response.getheaders()), response.read()
                finally:
                    connection.close()
            try:
                deadline = time.monotonic() + 10
                while True:
                    self.assertIsNone(child.poll(), 'Static server exited before readiness')
                    try:
                        status, _, _ = request(font)
                        self.assertEqual(status, 200)
                        break
                    except OSError:
                        if time.monotonic() >= deadline:
                            raise
                        time.sleep(.05)
                status, headers, wire = request(font, 'gzip')
                self.assertEqual(status, 200)
                self.assertEqual(headers['Content-Encoding'], 'gzip')
                self.assertEqual(headers['Vary'], 'Accept-Encoding')
                self.assertIn('no-transform', headers['Cache-Control'])
                self.assertEqual(headers['Cross-Origin-Opener-Policy'], 'same-origin')
                self.assertEqual(headers['Cross-Origin-Embedder-Policy'], 'require-corp')
                self.assertLess(len(wire), len(sample) // 10)
                self.assertEqual(hashlib.sha256(gzip.decompress(wire)).digest(), hashlib.sha256(sample).digest())
                for name in paths:
                    status, headers, body = request(name)
                    self.assertEqual(status, 200)
                    self.assertNotIn('Content-Encoding', headers)
                    self.assertEqual(body, sample)
                for name in paths[1:]:
                    status, headers, body = request(name, 'gzip')
                    self.assertEqual(status, 200)
                    self.assertNotIn('Content-Encoding', headers)
                    self.assertEqual(body, sample)
            finally:
                child.terminate()
                child.wait(timeout=10)
                child.stdout.close()
                child.stderr.close()

if __name__ == '__main__':
    unittest.main()
