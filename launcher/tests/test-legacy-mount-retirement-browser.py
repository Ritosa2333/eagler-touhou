import json
import threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path

from playwright.sync_api import sync_playwright


PROJECT = Path(__file__).resolve().parents[1]
RETIREMENT_WORKER = (PROJECT / "public" / "legacy-mount-retirement-sw.js").read_bytes()

OLD_PAGE = b"""<!doctype html><meta charset=utf-8><title>legacy</title>
<body>legacy</body><script>
(async () => {
  const registration = await navigator.serviceWorker.register(
    '/eagler-touhou/app-shell-sw.js',
    { scope: '/eagler-touhou/', updateViaCache: 'none' }
  );
  await registration.update();
  document.body.dataset.registrationUpdated = '1';
})().catch(error => { document.body.dataset.error = String(error); });
</script>"""

OLD_WORKER = b"""
self.addEventListener('install', event => {
  event.waitUntil(caches.open('legacy-mount-test').then(cache => cache.add('/eagler-touhou/')));
});
self.addEventListener('activate', event => event.waitUntil(self.clients.claim()));
self.addEventListener('fetch', event => {
  event.respondWith(caches.match(event.request).then(hit => hit || fetch(event.request)));
});
"""


class State:
    retired = False
    retirement_worker_requests = 0


class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        path = self.path.split("?", 1)[0]
        if path == "/":
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()
            self.wfile.write(b"<!doctype html><title>root</title><body data-root=1>root</body>")
            return
        if path == "/eagler-touhou/app-shell-sw.js":
            body = RETIREMENT_WORKER if State.retired else OLD_WORKER
            if State.retired:
                State.retirement_worker_requests += 1
            self.send_response(200)
            self.send_header("Content-Type", "application/javascript")
            self.send_header("Cache-Control", "no-store")
            self.send_header("Service-Worker-Allowed", "/eagler-touhou/")
            self.end_headers()
            self.wfile.write(body)
            return
        if path in ("/eagler-touhou", "/eagler-touhou/"):
            if State.retired:
                self.send_response(302)
                self.send_header("Location", "/")
                self.end_headers()
            else:
                self.send_response(200)
                self.send_header("Content-Type", "text/html; charset=utf-8")
                self.send_header("Cache-Control", "no-store")
                self.end_headers()
                self.wfile.write(OLD_PAGE)
            return
        self.send_response(404)
        self.end_headers()

    def log_message(self, _format, *_args):
        pass


def main() -> int:
    server = ThreadingHTTPServer(("127.0.0.1", 0), Handler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    origin = f"http://127.0.0.1:{server.server_port}"
    try:
        with sync_playwright() as playwright:
            browser = playwright.chromium.launch(headless=True)
            context = browser.new_context()
            page = context.new_page()
            page.goto(f"{origin}/eagler-touhou/", wait_until="load")
            page.evaluate("() => navigator.serviceWorker.ready")
            page.reload(wait_until="load")
            page.wait_for_function("() => !!navigator.serviceWorker.controller")

            State.retired = True
            page.reload(wait_until="load")
            page.wait_for_url(f"{origin}/", timeout=15_000)
            page.wait_for_function(
                "() => navigator.serviceWorker.getRegistration('/eagler-touhou/').then(value => !value)",
                timeout=15_000,
            )

            result = {
                "legacyMountRetirement": "PASS",
                "url": page.url,
                "workerUpdates": State.retirement_worker_requests,
                "registrations": page.evaluate(
                    "() => navigator.serviceWorker.getRegistrations().then(items => items.length)"
                ),
            }
            if result["workerUpdates"] < 1 or result["registrations"] != 0:
                raise AssertionError(result)
            print(json.dumps(result, ensure_ascii=False))
            browser.close()
    finally:
        server.shutdown()
        server.server_close()
        thread.join(timeout=5)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
