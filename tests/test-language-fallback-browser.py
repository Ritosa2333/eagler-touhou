"""A missing optional remote language pack must not block a local game Package."""
import argparse
import json
import os
from urllib.parse import urljoin

from playwright.sync_api import TimeoutError as PlaywrightTimeoutError, sync_playwright


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("url")
    parser.add_argument("package_zip")
    args = parser.parse_args()
    package_zip = os.path.abspath(args.package_zip)
    if not os.path.isfile(package_zip):
        raise FileNotFoundError(package_zip)

    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True)
        context = browser.new_context()
        page = context.new_page()
        language_requests = []
        console_messages = []
        page.on("console", lambda message: console_messages.append(f"{message.type}: {message.text}"))

        def fail_language(route, request):
            language_requests.append(request.url)
            route.fulfill(status=404, content_type="text/plain", body="intentional language fixture 404")

        page.route("**/games/th06/language/*.zip*", fail_language)
        page.goto(urljoin(args.url, "?game=th06"), wait_until="load", timeout=30_000)
        page.wait_for_function("window.__eaglerBoot?.done === true", timeout=30_000)
        page.wait_for_timeout(750)
        page.evaluate("""() => {
          document.querySelector('#changelogDialog')?.close();
          window.__languageFallbackFirstFrame = false;
          addEventListener('message', event => {
            const message = event.data || {};
            if (message.protocol === 'eagler-touhou/1' && message.event === 'first-frame') {
              window.__languageFallbackFirstFrame = true;
            }
          });
        }""")
        page.locator("#languageSelect").select_option("lang_en")
        page.evaluate("""() => {
          const select = document.getElementById('musicSelect');
          select.value = 'none';
          select.dispatchEvent(new Event('change', { bubbles: true }));
        }""")
        selected_language = page.locator("#languageSelect").input_value()
        stored_language = page.evaluate("localStorage.getItem('eagler-touhou-language-v1-th06')")
        assert selected_language == "lang_en", {
            "selected": selected_language,
            "stored": stored_language,
            "url": page.url,
        }
        page.locator("#gamePackageImport").click()
        page.locator("#gameDataImportInput").set_input_files(package_zip)
        page.wait_for_function("""() =>
          document.getElementById('status')?.textContent.includes('游戏包已导入，可以启动游戏') &&
          document.getElementById('transferImport')?.disabled === false
        """, timeout=120_000)
        page.wait_for_timeout(500)
        # Headless Chromium can leave requestFullscreen pending indefinitely.
        # Reject it immediately so this gate measures the launch path itself.
        page.evaluate("""() => {
          document.getElementById('player').requestFullscreen = () =>
            Promise.reject(new Error('headless fullscreen fixture'));
        }""")
        page.locator("#launch").click()
        page.wait_for_function("document.getElementById('player')?.classList.contains('open')", timeout=15_000)
        page.wait_for_function("document.getElementById('decisionDialog')?.open === true", timeout=15_000)
        page.locator("#decisionCancel").click()
        try:
            page.wait_for_function("""() =>
              document.getElementById('playerStatus')?.textContent === '运行中' &&
              window.__languageFallbackFirstFrame === true
            """, timeout=90_000)
        except PlaywrightTimeoutError as error:
            diagnostic = page.evaluate("""() => ({
              status: document.getElementById('status')?.textContent,
              playerStatus: document.getElementById('playerStatus')?.textContent,
              toast: document.getElementById('toast')?.textContent,
              frame: document.getElementById('gameFrame')?.getAttribute('src'),
              firstFrame: window.__languageFallbackFirstFrame,
              transferHidden: document.getElementById('transferWindow')?.hidden,
              language: document.getElementById('languageSelect')?.value,
              storedLanguage: localStorage.getItem('eagler-touhou-language-v1-th06'),
              playerClass: document.getElementById('player')?.className,
              startupError: document.getElementById('startupErrorText')?.textContent,
              decisionOpen: document.getElementById('decisionDialog')?.open,
            })""")
            raise AssertionError({
                "diagnostic": diagnostic,
                "languageRequests": language_requests,
                "console": console_messages[-20:],
            }) from error

        persisted = page.evaluate("localStorage.getItem('eagler-touhou-language-v1-th06')")
        assert persisted == "lang_en", persisted
        assert language_requests, "the remote language fixture was not exercised"
        print(json.dumps({
            "languageFallback": "PASS",
            "remoteStatus": 404,
            "firstFrame": True,
            "preferencePreserved": persisted,
            "requests": language_requests,
        }, ensure_ascii=False))
        browser.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
