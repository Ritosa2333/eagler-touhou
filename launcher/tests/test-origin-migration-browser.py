import json
from pathlib import Path
from urllib.parse import urlparse

from playwright.sync_api import sync_playwright


PROJECT = Path(__file__).resolve().parents[1]
MIGRATION_HTML = (PROJECT / "public" / "migrate.html").read_text(encoding="utf-8")


SEED_SCRIPT = r"""
async ({ storage, databases }) => {
  for (const [key, value] of Object.entries(storage)) localStorage.setItem(key, value);
  for (const [name, value] of Object.entries(databases)) {
    await new Promise((resolve, reject) => {
      const request = indexedDB.deleteDatabase(name);
      request.onsuccess = () => resolve();
      request.onerror = () => reject(request.error);
    });
    const db = await new Promise((resolve, reject) => {
      const request = indexedDB.open(name, 1);
      request.onupgradeneeded = () => request.result.createObjectStore('files');
      request.onsuccess = () => resolve(request.result);
      request.onerror = () => reject(request.error);
    });
    await new Promise((resolve, reject) => {
      const tx = db.transaction('files', 'readwrite');
      tx.objectStore('files').put(value, 'slot');
      tx.oncomplete = () => resolve();
      tx.onerror = tx.onabort = () => reject(tx.error);
    });
    db.close();
  }
}
"""


READ_DB = r"""
name => new Promise((resolve, reject) => {
  const request = indexedDB.open(name);
  request.onsuccess = () => {
    const db = request.result;
    const tx = db.transaction('files', 'readonly');
    const get = tx.objectStore('files').get('slot');
    get.onsuccess = () => { const value = get.result; db.close(); resolve(value); };
    get.onerror = () => { db.close(); reject(get.error); };
  };
  request.onerror = () => reject(request.error);
})
"""


def main() -> int:
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True)
        context = browser.new_context(ignore_https_errors=True)

        def route_request(route):
            parsed = urlparse(route.request.url)
            if parsed.path == "/migrate.html":
                route.fulfill(
                    status=200,
                    body=MIGRATION_HTML,
                    content_type="text/html; charset=utf-8",
                    headers={"Cache-Control": "no-cache"},
                )
            elif parsed.path == "/seed":
                route.fulfill(status=200, body="<!doctype html><title>seed</title>", content_type="text/html")
            else:
                route.fulfill(status=204, body="")

        context.route("**/*", route_request)
        page = context.new_page()

        page.goto("https://migration.test/seed")
        page.evaluate(SEED_SCRIPT, {
            "storage": {
                "eagler-touch-help-seen-v8": "new-setting",
                "eagler-new-only": "keep-me",
            },
            "databases": {
                "/savesth07": "new-th07",
                "/savesth08": "new-th08",
            },
        })

        page.goto("http://migration.test/seed")
        page.evaluate(SEED_SCRIPT, {
            "storage": {
                "eagler-touch-help-seen-v8": "old-setting",
                "eagler-old-only": "bring-me",
            },
            "databases": {
                "/savesth06": "old-th06",
                "/savesth07": "old-th07",
                "/savesth08": "old-th08",
            },
        })

        page.goto("http://migration.test/migrate.html?target=https://migration.test/migrate.html")
        page.wait_for_function("() => !document.getElementById('migrate').disabled")
        with context.expect_page() as receiver_info:
            page.locator("#migrate").click()
        receiver = receiver_info.value
        receiver.wait_for_load_state("load")
        receiver.locator("#conflictPanel").wait_for(state="visible", timeout=20_000)

        # Explicitly replace TH08 only. TH07 and the same-name setting stay on
        # the HTTPS side; TH06 and old-only settings are non-conflicting and
        # therefore migrate automatically.
        receiver.locator(".conflict-option", has_text="TH08").locator("input").check()
        receiver.locator("#overwriteTarget").click()
        receiver.wait_for_function("() => document.getElementById('status')?.classList.contains('ok')", timeout=30_000)

        result = {
            "settingConflict": receiver.evaluate("localStorage.getItem('eagler-touch-help-seen-v8')"),
            "newOnly": receiver.evaluate("localStorage.getItem('eagler-new-only')"),
            "oldOnly": receiver.evaluate("localStorage.getItem('eagler-old-only')"),
            "th06": receiver.evaluate(READ_DB, "/savesth06"),
            "th07": receiver.evaluate(READ_DB, "/savesth07"),
            "th08": receiver.evaluate(READ_DB, "/savesth08"),
        }
        assert result["settingConflict"] == "new-setting", result
        assert result["newOnly"] == "keep-me", result
        assert result["oldOnly"] == "bring-me", result
        assert result["th06"] == "old-th06", result
        assert result["th07"] == "new-th07", result
        assert result["th08"] == "old-th08", result

        print(json.dumps({"originMigrationBrowser": "PASS", **result}, ensure_ascii=False))
        browser.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
