"""Real Launcher + production SW, without proprietary games or sibling builds.

The iframe/ESM/WASM fixture is NOT evidence of a Touhou game's first frame,
physical iOS installation, audio resumption, or platform storage persistence.
"""
import argparse
import functools
import json
import subprocess
import tempfile
import threading
import time
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlparse

from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[2]


def build(directory, version):
    result = subprocess.run(
        ["node", "tests/browser/build-pwa-fixture.mjs", str(directory), version],
        cwd=ROOT, text=True, capture_output=True, check=True, timeout=120,
    )
    return json.loads(result.stdout.strip().splitlines()[-1])["build"]


class Handler(SimpleHTTPRequestHandler):
    unavailable = False
    extensions_map = {**SimpleHTTPRequestHandler.extensions_map,
                      ".mjs": "text/javascript", ".js": "text/javascript",
                      ".wasm": "application/wasm", ".webmanifest": "application/manifest+json"}

    def do_GET(self):
        if self.unavailable:
            self.send_error(503, "Simulated server outage")
            return
        super().do_GET()

    def end_headers(self):
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def log_message(self, *_args):
        pass


def boot(page, url):
    page.goto(url, wait_until="load", timeout=45000)
    page.wait_for_function("window.__eaglerBoot?.done === true", timeout=30000)
    assert page.locator("#pwaOpen").count() == 1, "PWA controls missing"
    page.evaluate("document.querySelector('#firstUseNoticeDialog')?.close()")


def status(page):
    return page.evaluate("""async () => {
        const registration = await navigator.serviceWorker.getRegistration('./');
        if (!registration?.active) return null;
        const channel = new MessageChannel();
        return await new Promise((resolve, reject) => {
            const timer = setTimeout(() => { channel.port1.close(); reject(new Error('SW status timeout')); }, 10000);
            channel.port1.onmessage = event => {
                clearTimeout(timer); channel.port1.close(); resolve(event.data);
            };
            registration.active.postMessage({ type: 'GET_APP_SHELL_STATUS' }, [channel.port2]);
        });
    }""")


def launch_fixture(page, origin, version):
    page.evaluate("""url => {
        document.querySelector('#pwaFixtureFrame')?.remove();
        const frame = document.createElement('iframe');
        frame.id = 'pwaFixtureFrame'; frame.src = url; document.body.append(frame);
    }""", origin + "runtime/pwa-test/runtime.html?runtimeEpoch=1")
    page.wait_for_function("""version => document.querySelector('#pwaFixtureFrame')?.contentWindow?.fixtureVersion === version""",
                           arg=version, timeout=20000)
    page.locator("#pwaFixtureFrame").evaluate("element => element.remove()")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--browser", choices=["chromium", "webkit", "firefox"], required=True)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="eagler-pwa-") as temporary:
        work = Path(temporary)
        site = work / "site"
        build_a = build(site, "a")
        build(site / "nested", "a")
        server = ThreadingHTTPServer(("127.0.0.1", 0), functools.partial(Handler, directory=str(site)))
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        origin = f"http://127.0.0.1:{server.server_port}/"
        errors = []
        try:
            with sync_playwright() as playwright:
                engine = getattr(playwright, args.browser)
                context = engine.launch_persistent_context(str(work / "profile"), headless=True)
                context.on("page", lambda page: page.on("pageerror", lambda error: errors.append(str(error))))
                page = context.new_page()
                boot(page, origin)
                page.wait_for_function("""async () => {
                    const r = await navigator.serviceWorker.getRegistration('./');
                    return r?.active?.state === 'activated';
                }""", timeout=45000)
                # No clients.claim(): do not switch a network-loaded module graph.
                assert not page.evaluate("!!navigator.serviceWorker.controller")
                boot(page, origin)
                assert page.evaluate("!!navigator.serviceWorker.controller")
                assert status(page)["build"] == build_a
                icons = page.evaluate("""async () => {
                    const manifest = await (await fetch('site.webmanifest')).json();
                    return await Promise.all(manifest.icons.map(async icon => {
                        const image = new Image(); image.src = icon.src; await image.decode();
                        return [image.naturalWidth, image.naturalHeight];
                    }));
                }""")
                assert [192, 192] in icons and [512, 512] in icons
                for path in ["en.html", "?game=th07", "?game=th08", "?game=th10"]:
                    boot(page, origin + path)
                # Simulate optional APIs being restricted in a separate clean profile.
                restricted = engine.launch_persistent_context(str(work / "restricted"), headless=True)
                restricted.add_init_script("""(() => {
                    Object.defineProperty(navigator, 'storage', { configurable: true, get() { throw new DOMException('blocked', 'SecurityError'); } });
                    if ('serviceWorker' in navigator) navigator.serviceWorker.register = () => { throw new DOMException('blocked', 'SecurityError'); };
                })();""")
                restricted_page = restricted.new_page()
                restricted_page.on("pageerror", lambda error: errors.append(str(error)))
                boot(restricted_page, origin)
                restricted_page.locator("#mastheadMenuToggle").click()
                restricted_page.locator("#pwaOpen").click()
                restricted_page.wait_for_function("document.querySelector('#pwaDialog').open")
                restricted.close()

                boot(page, origin)
                # Cache, but do not launch, the fixture before going offline.
                result = page.evaluate("""async () => {
                    const r = await navigator.serviceWorker.getRegistration('./');
                    const paths = ['runtime.html', 'boot.mjs', 'empty.wasm'].map(p => 'runtime/pwa-test/' + p);
                    const channel = new MessageChannel();
                    return await new Promise((resolve, reject) => {
                        const timer = setTimeout(() => reject(new Error('Runtime cache timeout')), 15000);
                        channel.port1.onmessage = e => { clearTimeout(timer); channel.port1.close(); resolve(e.data); };
                        r.active.postMessage({ type: 'CACHE_APP_SHELL_PATHS', paths }, [channel.port2]);
                    });
                }""")
                assert result["ok"]
                context.set_offline(True)
                boot(page, origin)
                launch_fixture(page, origin, "a")
                context.set_offline(False)
                # Establish another SW scope; root cache GC must not delete it.
                nested = context.new_page()
                boot(nested, origin + "nested/")
                nested.wait_for_function("""async () => (await navigator.serviceWorker.getRegistration('./'))?.active?.state === 'activated'""", timeout=45000)
                boot(nested, origin + "nested/")
                nested.close()
                other = context.new_page()
                boot(other, origin)
                build_b = build(site, "b")
                page.evaluate("async () => (await navigator.serviceWorker.getRegistration('./')).update()")
                page.wait_for_function("""async () => !!(await navigator.serviceWorker.getRegistration('./'))?.waiting""", timeout=45000)
                assert status(page)["build"] == build_a
                assert status(other)["build"] == build_a
                # A refresh and closing just one window must not activate B.
                boot(page, origin)
                assert status(page)["build"] == build_a
                page.close()
                assert status(other)["build"] == build_a
                other.close()
                page = context.new_page()
                page.wait_for_timeout(500)
                context.set_offline(True)
                boot(page, origin)
                assert status(page)["build"] == build_b
                launch_fixture(page, origin, "b")  # includes newly introduced helper.mjs
                boot(page, origin + "nested/")
                context.set_offline(False)
                boot(page, origin)
                # HTTP 200 with the wrong bytes must fail the candidate update.
                build(site, "c")
                (site / "runtime/pwa-test/helper.mjs").write_text("export const version = 'corrupt';\n", encoding="utf-8")
                page.evaluate("""async () => {
                    window.__candidateFailed = false;
                    const r = await navigator.serviceWorker.getRegistration('./');
                    const watch = () => {
                        const w = r.installing;
                        if (w) w.addEventListener('statechange', () => { if (w.state === 'redundant') window.__candidateFailed = true; });
                    };
                    r.addEventListener('updatefound', watch); watch(); await r.update();
                }""")
                page.wait_for_function("window.__candidateFailed === true", timeout=45000)
                assert status(page)["build"] == build_b
                Handler.unavailable = True
                boot(page, origin)  # navigator is online but the entire server returns 503
                launch_fixture(page, origin, "b")
                context.close()
                # Persistent profile, fresh browser process, no reachable server.
                cold = engine.launch_persistent_context(str(work / "profile"), headless=True)
                cold.set_offline(True)
                cold_page = cold.new_page()
                cold_page.on("pageerror", lambda error: errors.append(str(error)))
                boot(cold_page, origin)
                launch_fixture(cold_page, origin, "b")
                cold.close()
                assert not errors, json.dumps(errors, ensure_ascii=False)
                print(json.dumps({"browser": args.browser, "pass": True,
                                  "coverage": "real Launcher + production SW + synthetic iframe/ESM/WASM; not real games or physical iOS"}))
        finally:
            Handler.unavailable = False
            server.shutdown()
            server.server_close()
            thread.join(timeout=5)


if __name__ == "__main__":
    main()
