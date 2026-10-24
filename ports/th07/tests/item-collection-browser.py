"""Exercise actual item motion and rewards in two- and three-player th07 worlds."""
import argparse
import hashlib
import importlib.util
import json
import os
import threading
import time
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlencode, urlsplit
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
PROJECT = ROOT.parents[1]
os.environ.setdefault('EAGLER_WORKSPACE', str(PROJECT))
os.environ.setdefault('EAGLER_RELAY_ROOT', str(PROJECT / 'services/relay'))
spec = importlib.util.spec_from_file_location('spectator_test', ROOT / 'tests/netplay-spectator-stage2-browser.py')
support = importlib.util.module_from_spec(spec)
spec.loader.exec_module(support)

parser = argparse.ArgumentParser()
parser.add_argument('--runtime-directory', required=True, type=Path)
parser.add_argument('--font', required=True, type=Path)
parser.add_argument('--relay', default='ws://127.0.0.1:18142')
parser.add_argument('--output', required=True, type=Path)
args = parser.parse_args()
runtime = args.runtime_directory.resolve()
font = args.font.resolve()
assert font.is_file()
host = (ROOT / 'tests/netplay-spectator-browser-host.html').read_text().replace(
    '{ character: 1, shot: 1 }', '{ character: 0, shot: 1 }')

class Handler(SimpleHTTPRequestHandler):
    def log_message(self, *args):
        pass

    def do_GET(self):
        if urlsplit(self.path).path in ('/host.html', '/setup.html'):
            data = host.encode() if urlsplit(self.path).path == '/host.html' else b'<!doctype html>'
            self.send_response(200)
            self.send_header('Content-Type', 'text/html; charset=utf-8')
            self.send_header('Content-Length', str(len(data)))
            self.end_headers()
            self.wfile.write(data)
        else:
            super().do_GET()

    def translate_path(self, path):
        path = urlsplit(path).path
        if path == '/assets/msgothic.ttc':
            return str(font)
        if path.startswith('/runtime/'):
            target = (runtime / path[len('/runtime/'):]).resolve()
            if target.is_relative_to(runtime):
                return str(target)
        return str(runtime / '__missing__')

server = ThreadingHTTPServer(('127.0.0.1', 0), Handler)
threading.Thread(target=server.serve_forever, daemon=True).start()
report = {'passed': False, 'wasm': hashlib.sha256((runtime / 'th07.wasm').read_bytes()).hexdigest(),
          'scope': 'native th07 item motion and rewards | Reimu A/B | 2P and 3P', 'cases': []}
try:
    with sync_playwright() as pw:
        browser = pw.chromium.launch(headless=True, args=[
            '--enable-unsafe-swiftshader', '--autoplay-policy=no-user-gesture-required'])
        try:
            for count in (2, 3):
                context = browser.new_context()
                try:
                    base = 'http://127.0.0.1:' + str(server.server_port)
                    setup = context.new_page()
                    setup.goto(base + '/setup.html')
                    # The setup document only hosts the real room sockets.
                    room = 'item-fixture-' + str(time.time_ns())
                    support.admit_lobby(setup, args.relay, room, 'item_viewer', count)
                    pages = [context.new_page() for _ in range(count)]
                    for seat, page in enumerate(pages):
                        page.add_init_script('delete globalThis.RTCPeerConnection')
                        query = {'runtimeBuild': 'runtime', 'player': seat, 'players': count,
                                 'relay': args.relay + '/?room=' + room + '&run=1&member=member_p' +
                                          str(seat + 1) + '_00000' + str(seat + 1)}
                        page.goto(base + '/host.html?' + urlencode(query), wait_until='domcontentloaded')
                    page = pages[0]
                    page.wait_for_function("""() => {
                      const w=document.getElementById('runtime').contentWindow;
                      if (!w.Module?._th07_item_collection_fixture) return false;
                      w.Module._th07_item_collection_fixture(-1); return w.__itemFixtureReady;
                    }""", timeout=120000)
                    values = page.evaluate("""() => {
                      const w=document.getElementById('runtime').contentWindow;
                      return [0,1,2,3,4,5].map(i => {
                        w.Module._th07_item_collection_fixture(i); return w.__itemFixtureResult;
                      });
                    }""")
                    report['cases'].append({'players': count, 'values': values})
                    assert all(v['ready'] and v['collected'] and v['finite'] for v in values), values
                    assert values[0]['power'][1] == values[1]['power'][1] == 65, values
                    assert values[0]['frames'] > 1 and values[1]['frames'] > 1, values
                    assert values[2]['type'] == values[3]['type'] == 7, values
                    assert values[4]['power'][:2] == [101, 64], values[4]
                    assert values[5]['power'][:2] == [128, 72], values[5]
                    assert all(v['slots'][2] == (count == 3) for v in values), values
                    print(f'th07 {count}P native Power collection: PASS', flush=True)
                finally:
                    context.close()
            report['passed'] = True
        finally:
            browser.close()
finally:
    server.shutdown()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
