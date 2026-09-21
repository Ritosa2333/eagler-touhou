"""Latest-first + automatic usable rollback, through the real Launcher/SW.

Uses incompatible synthetic JS/Wasm, not proprietary TH08 or physical Safari.
Invoked by the explicit PWA browser lane, sharing its transport-fault fixture.
"""
import argparse
import functools
import importlib.util
import json
import tempfile
import threading
from http.server import ThreadingHTTPServer
from pathlib import Path
from playwright.sync_api import sync_playwright

SPEC = importlib.util.spec_from_file_location("pwa_boot", Path(__file__).with_name("test-pwa-boot.py"))
boot_fixture = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(boot_fixture)


def launch(page, origin, version, frame_id):
    page.evaluate("""([url, id]) => {
        const frame = document.createElement('iframe');
        frame.id = id; frame.src = url; document.body.append(frame);
    }""", [origin + "runtime/pwa-test/runtime.html", frame_id])
    page.wait_for_function("""([id, version]) =>
        document.getElementById(id)?.contentWindow.fixtureVersion === version""",
        arg=[frame_id, version], timeout=30000)


def value(page, frame_id, method):
    return page.evaluate("([id, method]) => document.getElementById(id).contentWindow[method]()", [frame_id, method])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--browser", choices=["chromium", "webkit", "firefox"], required=True)
    args = parser.parse_args()
    with tempfile.TemporaryDirectory(prefix="runtime-recovery-") as temp:
        work = Path(temp)
        site = work / "site"
        build_a = boot_fixture.build(site, "a")
        wasm_a = (site / "runtime/pwa-test/empty.wasm").read_bytes()
        glue_a = (site / "runtime/pwa-test/glue.mjs").read_text(encoding="utf-8")
        probe = site / "pwa-probe"
        probe.mkdir()
        (probe / "index.html").write_text("<!doctype html><title>Independent SW probe</title>", encoding="utf-8")
        (probe / "sw.js").write_text("""self.addEventListener('fetch', event => {
            if (new URL(event.request.url).pathname.endsWith('/literal'))
                event.respondWith(new Response('<!doctype html><h1>Literal worker response</h1>',
                    { headers: { 'Content-Type': 'text/html' } }));
        });""", encoding="utf-8")
        handler = boot_fixture.Handler
        server = ThreadingHTTPServer(("127.0.0.1", 0), functools.partial(handler, directory=str(site)))
        thread = threading.Thread(target=server.serve_forever, daemon=True)
        thread.start()
        origin = f"http://127.0.0.1:{server.server_port}/"
        errors = []
        try:
            with sync_playwright() as playwright:
                engine = getattr(playwright, args.browser)
                emulation = boot_fixture.probe_offline_emulation(engine, origin)
                def outage(context, enabled):
                    if emulation:
                        context.set_offline(enabled)
                    else:
                        handler.drop_connections = enabled
                context = engine.launch_persistent_context(str(work / "profile"), headless=True)
                context.on("page", lambda p: p.on("pageerror", lambda error: errors.append(str(error))))
                page = context.new_page()
                boot_fixture.boot(page, origin)
                boot_fixture.wait_async(page, "async () => (await navigator.serviceWorker.getRegistration('./'))?.active?.state === 'activated'")
                boot_fixture.boot(page, origin)
                page.evaluate("window.__runtimeLaunchSentinel = 'not-reloaded'")
                launch(page, origin, "a", "A")
                boot_fixture.mark("matching-EM_ASM-glue-wasm-A-starts")
                boot_fixture.build(site, "b")
                # Legacy labels are not reliable: reproduce the old glue/new
                # Wasm cache without modifying the already committed snapshot.
                page.evaluate("""async glue => {
                    const cache = await caches.open('eagler-touhou-app-shell-poisoned');
                    await cache.put(new URL('runtime/pwa-test/glue.mjs', location.href).href, new Response(glue));
                    await cache.put(new URL('runtime/pwa-test/empty.wasm', location.href).href,
                        await fetch('runtime/pwa-test/empty.wasm', {cache: 'no-store'}));
                }""", glue_a)
                launch(page, origin, "b", "B")
                assert boot_fixture.status(page)["build"] == build_a, "Runtime update must not require Launcher SW activation"
                assert page.evaluate("window.__runtimeLaunchSentinel") == "not-reloaded"
                assert value(page, "A", "fixtureLate") == "a"
                assert value(page, "A", "fixtureWorker") == "a"
                assert value(page, "B", "fixtureWorker") == "b"
                boot_fixture.mark("new-Runtime-immediate-old-window-and-worker-pinned")
                boot_fixture.build(site, "c")
                (site / "runtime/pwa-test/empty.wasm").write_bytes(wasm_a)
                launch(page, origin, "b", "badC")
                assert value(page, "badC", "fixtureWorker") == "b"
                boot_fixture.mark("bad-new-Wasm-automatically-starts-complete-B")
                outage(context, True)
                launch(page, origin, "b", "offlineB")
                boot_fixture.assert_network_unavailable(page, origin)
                boot_fixture.mark("offline-starts-complete-B")
                # Delete B's cached Wasm but keep A complete. The next start must
                # roll back the WHOLE set before returning even its HTML.
                removed = page.evaluate("""async () => {
                    let count = 0;
                    const key = new URL('runtime/pwa-test/empty.wasm', location.href).href;
                    for (const name of await caches.keys()) {
                        if (!name.startsWith('eagler-touhou-runtime-') || name.endsWith('-state')) continue;
                        const cache = await caches.open(name);
                        const glue = await cache.match(new URL('runtime/pwa-test/glue.mjs', location.href).href);
                        if (glue && (await glue.text()).includes('466320:')) {
                            if (await cache.delete(key)) count++;
                        }
                    }
                    return count;
                }""")
                assert removed > 0
                launch(page, origin, "a", "fallbackA")
                assert value(page, "fallbackA", "fixtureWorker") == "a"
                boot_fixture.mark("partial-B-offline-starts-entire-A")
                outage(context, False)
                boot_fixture.build(site, "d")
                launch(page, origin, "d", "D")
                assert page.evaluate("window.__runtimeLaunchSentinel") == "not-reloaded"
                boot_fixture.mark("network-restored-next-start-immediately-uses-D")
                other = context.new_page()
                boot_fixture.boot(other, origin)
                boot_fixture.build(site, "e")
                launch(other, origin, "e", "E")
                assert value(page, "D", "fixtureLate") == "d"
                assert value(page, "D", "fixtureWorker") == "d"
                boot_fixture.mark("second-tab-new-E-does-not-switch-live-D")
                context.close()
                cold = engine.launch_persistent_context(str(work / "profile"), headless=True)
                cold.on("page", lambda p: p.on("pageerror", lambda error: errors.append(str(error))))
                outage(cold, True)
                cold_page = cold.new_page()
                boot_fixture.boot(cold_page, origin)
                launch(cold_page, origin, "e", "coldE")
                assert value(cold_page, "coldE", "fixtureWorker") == "e"
                boot_fixture.assert_network_unavailable(cold_page, origin)
                cold.close()
                assert not errors, json.dumps(errors, ensure_ascii=False)
                print(json.dumps({"browser": args.browser, "runtime_recovery": "PASS",
                    "outage_mode": "browser-offline-emulation" if emulation else "origin-connection-drop",
                    "physical_device": False, "real_Touhou_game": False}), flush=True)
        finally:
            handler.drop_connections = False
            handler.unavailable = False
            server.shutdown()
            server.server_close()
            thread.join(timeout=5)


if __name__ == "__main__":
    main()
