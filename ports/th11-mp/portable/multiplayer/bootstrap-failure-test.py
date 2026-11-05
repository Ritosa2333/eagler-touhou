"""Exercise the shipped TH11 shell + Emscripten loader over real HTTP.

Only the WASM response is deliberately invalid; there is no request interception,
replacement Module/WebAssembly, copied bootstrap algorithm, or gameplay launch.
Run against frozen packages before and after the owner repackages shell.mjs:
  python portable/multiplayer/bootstrap-failure-test.py --output REPORT.json
The broken baseline exits 1 after receiving each bad response without a standard
Runtime error. The repaired packages must report the error promptly in both modes.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import mimetypes
import re
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, unquote, urlsplit

from playwright.sync_api import sync_playwright, TimeoutError as PlaywrightTimeout

ROOT = Path(__file__).resolve().parents[2]
PARENT = """<!doctype html><meta charset=utf-8><title>TH11 bootstrap failure fixture</title>
<iframe id=runtime style=width:640px;height:480px></iframe><script>
const query=new URLSearchParams(location.search),frame=document.querySelector('iframe');
window.bootstrapReport={messages:[],dataCalls:0};
window.__eaglerPrepareManagedRuntimeDataV1=()=>{
 bootstrapReport.dataCalls++;throw Error('DATA must not be requested after WASM failure');
};
addEventListener('message',event=>{
 if(event.source===frame.contentWindow&&event.origin===location.origin&&
    event.data?.protocol==='eagler-touhou/1'&&event.data.game==='th11'&&event.data.epoch===7)
  bootstrapReport.messages.push({at:performance.now(),...event.data});
});
frame.src='/case/'+query.get('variant')+'/'+query.get('fault')+
 '/th11.html?hosted=1&managedData=1&gameGeneration=bootstrap-negative&runtimeEpoch=7';
</script>"""


def package_identity(directory: Path):
    files = {}
    for name in ['manifest.json', 'th11.html', 'shell.mjs', 'th11-sdl.mjs', 'th11-sdl.wasm']:
        raw = (directory / name).read_bytes()
        files[name] = {'bytes': len(raw), 'sha256': hashlib.sha256(raw).hexdigest()}
    return files


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--ordinary', type=Path, default=ROOT / 'build-eagler')
    parser.add_argument('--multiplayer', type=Path, default=ROOT / 'build-eagler-multiplayer')
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--error-timeout-ms', type=int, default=2500)
    args = parser.parse_args()
    packages = {'ordinary': args.ordinary.resolve(), 'multiplayer': args.multiplayer.resolve()}
    identities = {key: package_identity(path) for key, path in packages.items()}
    requests = []
    lock = threading.Lock()

    class Handler(BaseHTTPRequestHandler):
        def log_message(self, *_):
            pass

        def do_GET(self):
            address = urlsplit(self.path)
            if address.path == '/':
                query = parse_qs(address.query)
                if query.get('variant', [''])[0] not in packages or query.get('fault', [''])[0] not in ['http503', 'bad-wasm']:
                    self.send_error(400)
                    return
                body, status, mime = PARENT.encode(), 200, 'text/html; charset=utf-8'
            else:
                match = re.fullmatch(r'/case/(ordinary|multiplayer)/(http503|bad-wasm)/(.+)', unquote(address.path))
                if not match:
                    self.send_error(404)
                    return
                variant, fault, name = match.groups()
                target = (packages[variant] / name).resolve()
                if not target.is_relative_to(packages[variant]) or not target.is_file():
                    self.send_error(404)
                    return
                if name == 'th11-sdl.wasm':
                    body = b'Expected isolated bootstrap test failure' if fault == 'http503' else b'BAD!\x01\x00\x00\x00'
                    status, mime = (503 if fault == 'http503' else 200), 'application/wasm'
                else:
                    body, status = target.read_bytes(), 200
                    mime = 'text/javascript' if target.suffix in ['.js', '.mjs'] else mimetypes.guess_type(name)[0] or 'application/octet-stream'
                with lock:
                    requests.append({'variant': variant, 'fault': fault, 'file': name, 'status': status, 'bytes': len(body)})
            self.send_response(status)
            self.send_header('Content-Type', mime)
            self.send_header('Content-Length', str(len(body)))
            self.send_header('Cache-Control', 'no-store')
            self.end_headers()
            try:
                self.wfile.write(body)
            except (BrokenPipeError, ConnectionResetError):
                pass

    server = ThreadingHTTPServer(('127.0.0.1', 0), Handler)
    server.daemon_threads = True
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    report = {'scope': 'Actual production HTML/shell/Emscripten loader, isolated real HTTP failures; no gameplay or main Launcher acceptance',
              'packages': {key: str(path) for key, path in packages.items()}, 'identities': identities,
              'cases': [], 'passed': False, 'requestInterception': False}
    try:
        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
            try:
                for variant in packages:
                    for fault in ['http503', 'bad-wasm']:
                        context = browser.new_context(service_workers='block')
                        try:
                            page = context.new_page()
                            errors, responses = [], []
                            page.on('pageerror', lambda error: errors.append(str(error)))
                            page.on('response', lambda response: responses.append({'url': response.url, 'status': response.status, 'at': time.monotonic()}) if response.url.endswith('/th11-sdl.wasm') else None)
                            page.goto(f'http://127.0.0.1:{server.server_port}/?variant={variant}&fault={fault}', wait_until='domcontentloaded')
                            deadline = time.monotonic() + 20
                            while not responses and time.monotonic() < deadline:
                                page.wait_for_timeout(25)
                            if not responses:
                                raise AssertionError(f'{variant}/{fault}: did not reach the real WASM request')
                            try:
                                page.wait_for_function("bootstrapReport.messages.some(message=>message.event==='error')", timeout=args.error_timeout_ms)
                            except PlaywrightTimeout:
                                pass
                            observed = page.evaluate("""() => {
                              const runtime=document.querySelector('iframe').contentWindow;
                              return {...bootstrapReport, exposedModule:!!runtime.Module, exposedCore:!!runtime.core,
                                exposedRuntime:!!runtime.__th11Runtime, netplayFailed:runtime.__eaglerNetplayFailed===true,
                                ordinaryError:runtime.document.querySelector('#error')?.textContent||''};
                            }""")
                            runtime_errors = [item for item in observed['messages'] if item['event'] == 'error']
                            elapsed = round((time.monotonic() - responses[0]['at']) * 1000, 1)
                            reached_native = observed['exposedModule'] or observed['exposedCore'] or observed['exposedRuntime'] or observed['dataCalls']
                            message = runtime_errors[0]['error'] if len(runtime_errors) == 1 else ''
                            right_failure = (responses[0]['status'] == 503 and 'HTTP status code' in message) if fault == 'http503' else (responses[0]['status'] == 200 and 'WebAssembly' in message and 'expected magic word' in message)
                            same_owner = observed['netplayFailed'] if variant == 'multiplayer' else bool(observed['ordinaryError'])
                            passed = bool(right_failure and same_owner and not reached_native and len(responses) == 1 and elapsed <= args.error_timeout_ms + 500 and
                                          not any(item['event'] in ['ready', 'first-frame'] for item in observed['messages']))
                            result = {'variant': variant, 'fault': fault, 'passed': passed, 'responseToObservationMs': elapsed,
                                      'responseStatus': responses[0]['status'], 'wasmRequests': len(responses),
                                      'pageErrors': errors, 'observed': observed}
                            report['cases'].append(result)
                            print(('PASS' if passed else 'FAIL') + f' {variant}/{fault}: standardErrors={len(runtime_errors)}, DATA={observed["dataCalls"]}, WASM={len(responses)}, observedAfter={elapsed}ms', flush=True)
                        finally:
                            context.close()
            finally:
                browser.close()
    except Exception as error:
        report['failure'] = repr(error)
    finally:
        server.shutdown()
        server.server_close()
        thread.join(timeout=5)
        report['requests'] = requests
        report['packagesUnchanged'] = all(package_identity(path) == identities[key] for key, path in packages.items())
        report['passed'] = len(report['cases']) == 4 and all(case['passed'] for case in report['cases']) and report['packagesUnchanged'] and 'failure' not in report
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(('PASS' if report['passed'] else 'FAIL') + ': ' + str(args.output), flush=True)
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
