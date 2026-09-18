from __future__ import annotations

import json
import threading
import time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlparse

from playwright.sync_api import sync_playwright


PROJECT = Path(__file__).resolve().parents[2]
RUNTIME_PATHS = [
    "runtime/game.html",
    "runtime/shell.mjs",
    "runtime/eagler-host.mjs",
    "runtime/game.wasm",
]
MANIFEST = [{"url": path, "revision": f"current-{index}"} for index, path in enumerate(RUNTIME_PATHS)]


def worker_source() -> bytes:
    source = (PROJECT / "src" / "app-shell-sw.js").read_text(encoding="utf-8")
    source = source.replace("__APP_SHELL_BUILD_ID__", "browserfallback")
    source = source.replace("__APP_SHELL_DEFERRED_PATHS__", json.dumps(RUNTIME_PATHS))
    return (f"self.__WB_MANIFEST = {json.dumps(MANIFEST)};\n" + source).encode()


class Handler(BaseHTTPRequestHandler):
    wasm_requests = 0

    def log_message(self, _format: str, *_args) -> None:
        return

    def do_GET(self) -> None:
        parsed = urlparse(self.path)
        path = parsed.path
        if path == "/":
            body = b"<!doctype html><meta charset=utf-8><title>Runtime cache fallback test</title>"
            content_type = "text/html; charset=utf-8"
        elif path == "/sw.js":
            body = worker_source()
            content_type = "text/javascript; charset=utf-8"
        elif path == "/runtime/game.wasm":
            type(self).wasm_requests += 1
            if parsed.query == "slow=1":
                time.sleep(1.0)
            body = b"new-runtime-wasm"
            content_type = "application/wasm"
        elif path == "/runtime/game.html":
            body = b"new-runtime-html"
            content_type = "text/html; charset=utf-8"
        elif path == "/runtime/shell.mjs":
            body = b"new-runtime-shell"
            content_type = "text/javascript; charset=utf-8"
        elif path == "/runtime/eagler-host.mjs":
            body = b"new-runtime-host"
            content_type = "text/javascript; charset=utf-8"
        else:
            self.send_error(404)
            return
        self.send_response(200)
        self.send_header("Content-Type", content_type)
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


def command(page, kind: str, paths=None):
    return page.evaluate(
        """async ({kind, paths}) => {
          const registration = await navigator.serviceWorker.ready;
          const worker = navigator.serviceWorker.controller || registration.active;
          return await new Promise((resolve, reject) => {
            const channel = new MessageChannel();
            const timer = setTimeout(() => reject(new Error(`${kind} timed out`)), 5000);
            channel.port1.onmessage = event => { clearTimeout(timer); resolve(event.data); };
            worker.postMessage({ type: kind, ...(paths ? { paths } : {}) }, [channel.port2]);
          });
        }""",
        {"kind": kind, "paths": paths},
    )


def seed_previous_cache(page) -> None:
    page.evaluate(
        """async paths => {
          const absolute = Object.fromEntries(paths.map(path => [path, new URL(path, location.href).href]));
          const cache = await caches.open('eagler-touhou-app-shell-browser-old');
          await cache.put(absolute['runtime/game.html'], new Response(
            `<script>const protocol='eagler-touhou/1';const epoch=Number(new URLSearchParams(location.search).get('runtimeEpoch'));window.parent.__eaglerPrepareManagedRuntimeDataV1({game:'th10',generation:'old',epoch});</script>`));
          await cache.put(absolute['runtime/shell.mjs'], new Response(
            `const protocol='eagler-touhou/1';const epoch=Number(new URLSearchParams(location.search).get('runtimeEpoch'));`));
          await cache.put(absolute['runtime/eagler-host.mjs'], new Response(
            `const epoch=Number(query.get('runtimeEpoch'));parentWindow.__eaglerPrepareManagedRuntimeDataV1({game,generation,epoch});`));
          await cache.put(absolute['runtime/game.wasm'], new Response('old-runtime-wasm'));
          await cache.put(new URL('/__app-shell-meta__/old-browser', location.href).href, new Response(JSON.stringify({
            createdAt: 50,
            entries: Object.fromEntries(paths.map((path, index) => [absolute[path], `old-${index}`])),
          }), { headers: { 'Content-Type': 'application/json' } }));
        }""",
        RUNTIME_PATHS,
    )


def run_browser(browser_type, base_url: str) -> None:
    browser = browser_type.launch(headless=True)
    context = browser.new_context(service_workers="allow")
    page = context.new_page()
    page.goto(base_url, wait_until="load", timeout=30_000)
    page.evaluate(
        """async () => {
          await navigator.serviceWorker.register('./sw.js', { scope: './', updateViaCache: 'none' });
          await navigator.serviceWorker.ready;
          if (!navigator.serviceWorker.controller) {
            await new Promise(resolve => navigator.serviceWorker.addEventListener('controllerchange', resolve, { once: true }));
          }
        }"""
    )
    seed_previous_cache(page)

    probe = command(page, "PROBE_RUNTIME_CACHE_FALLBACK", RUNTIME_PATHS)
    assert probe.get("available") is True, f"{browser_type.name}: fallback probe failed: {probe}"

    page.evaluate(
        """() => {
          window.__slowRuntimeFetch = fetch('./runtime/game.wasm?slow=1').then(response => response.text());
        }"""
    )
    page.wait_for_timeout(150)
    restored = command(page, "RESTORE_RUNTIME_CACHE_FALLBACK", RUNTIME_PATHS)
    assert restored.get("ok") is True, f"{browser_type.name}: fallback restore failed: {restored}"

    slow_result = page.evaluate("() => window.__slowRuntimeFetch")
    assert slow_result == "new-runtime-wasm", f"{browser_type.name}: delayed network request did not complete"
    cached_after_race = page.evaluate(
        """async () => {
          const current = (await caches.keys()).find(name => name.includes('browserfallback'));
          return await (await (await caches.open(current)).match(new URL('./runtime/game.wasm', location.href).href)).text();
        }"""
    )
    assert cached_after_race == "old-runtime-wasm", (
        f"{browser_type.name}: late network response overwrote fallback cache: {cached_after_race}"
    )

    cleared = command(page, "CLEAR_RUNTIME_CACHE_FALLBACK")
    assert cleared.get("cleared") == len(RUNTIME_PATHS), f"{browser_type.name}: fallback cleanup failed: {cleared}"
    next_launch = page.evaluate("async () => await (await fetch('./runtime/game.wasm')).text()")
    assert next_launch == "new-runtime-wasm", f"{browser_type.name}: next launch did not retry network"
    cached_after_retry = page.evaluate(
        """async () => {
          const current = (await caches.keys()).find(name => name.includes('browserfallback'));
          return await (await (await caches.open(current)).match(new URL('./runtime/game.wasm', location.href).href)).text();
        }"""
    )
    assert cached_after_retry == "new-runtime-wasm", f"{browser_type.name}: network retry was not persisted"
    context.close()
    browser.close()


def main() -> int:
    server = ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    base_url = f"http://127.0.0.1:{server.server_port}/"
    try:
        with sync_playwright() as playwright:
            for browser_type in (playwright.chromium, playwright.webkit):
                Handler.wasm_requests = 0
                run_browser(browser_type, base_url)
                assert Handler.wasm_requests >= 2, f"{browser_type.name}: expected slow request plus next-launch network retry"
                print(f"{browser_type.name}: runtime cache fallback PASS")
    finally:
        server.shutdown()
        server.server_close()
        thread.join(timeout=5)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
