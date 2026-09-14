"""Touch layout settings remain shared when the selected game changes."""
import json
import sys

from playwright.sync_api import sync_playwright


def main() -> int:
    url = sys.argv[1] if len(sys.argv) > 1 else "http://127.0.0.1:8130/"
    errors = []
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True)
        context = browser.new_context(viewport={"width": 1280, "height": 800})
        context.add_init_script(
            "localStorage.setItem('eagler-touhou-changelog-seen-20260822-1','1')"
        )
        page = context.new_page()
        page.emulate_media(reduced_motion="reduce")
        page.on("pageerror", lambda error: errors.append(str(error)))
        page.goto(url, wait_until="load", timeout=30000)
        page.wait_for_function("window.__eaglerBoot?.done === true", timeout=30000)
        # The content-derived changelog decision resolves asynchronously after
        # boot and may open between the first state check and the next click.
        page.wait_for_timeout(500)
        if page.locator("#changelogDialog").get_attribute("open") is not None:
            page.locator("#changelogCloseHint").click()
            page.wait_for_function("document.querySelector('#changelogDialog')?.open === false")

        page.locator('.game[data-game="th06"]:not([data-product])').click()
        page.locator("#mobileOptionsToggle").click()
        page.wait_for_function("document.querySelector('#mobileOptions').classList.contains('open')")
        page.evaluate("document.querySelector('#touchLayoutEdit').click()")
        page.wait_for_timeout(1500)
        assert page.locator("#touchLayoutEditor").evaluate("el => !el.hidden"), {
            "errors": errors,
            "status": page.locator("#status").text_content(),
        }
        assert page.locator("#touchRestart").evaluate(
            "el => el.hidden && getComputedStyle(el).display === 'none'"
        ), "disabled R must be actually hidden in the layout editor"

        assert page.locator("#restartButtonToggle").evaluate(
            "r => !!(r.closest('.touch-layout-setting-row').compareDocumentPosition("
            "document.querySelector('#thpracTouchControlsToggle').closest('.touch-layout-setting-row')) "
            "& Node.DOCUMENT_POSITION_FOLLOWING)"
        )
        page.locator("#touchSensitivity").evaluate(
            "input => { input.value = '200'; input.dispatchEvent(new Event('input', {bubbles:true})); "
            "input.dispatchEvent(new Event('change', {bubbles:true})); }"
        )
        page.locator("#touchFocusMode").select_option("toggle-button")
        page.locator("#doubleTapBombToggle").click()
        page.locator("#restartButtonToggle").click()
        assert page.locator("#restartButtonToggle").get_attribute("aria-checked") == "true"
        restart_geometry = page.evaluate("""() => {
          const escapeRect = document.querySelector('#touchEscape').getBoundingClientRect();
          const restart = document.querySelector('#touchRestart');
          const restartRect = restart.getBoundingClientRect();
          return {
            hidden: restart.hidden,
            display: getComputedStyle(restart).display,
            escapeBottom: escapeRect.bottom,
            restartTop: restartRect.top,
            leftDelta: Math.abs(escapeRect.left - restartRect.left),
          };
        }""")
        assert restart_geometry["hidden"] is False, restart_geometry
        assert restart_geometry["display"] != "none", restart_geometry
        assert restart_geometry["restartTop"] >= restart_geometry["escapeBottom"], restart_geometry
        assert restart_geometry["leftDelta"] <= 1, restart_geometry
        page.locator("#restartButtonToggle").click()
        page.locator("#touchMovementMode").select_option("joystick-free")
        page.wait_for_function("document.querySelector('#decisionDialog')?.open === true")
        page.evaluate(
            "document.querySelector('.decision-window').requestSubmit(document.querySelector('#decisionConfirm'))"
        )
        page.wait_for_function("document.querySelector('#decisionDialog')?.open === false")

        page.locator("#touchLayoutExit").click()
        page.wait_for_timeout(100)
        if page.locator("#decisionDialog").evaluate("el => el.open"):
            page.evaluate(
                "document.querySelector('.decision-window').requestSubmit(document.querySelector('#decisionConfirm'))"
            )
        page.wait_for_selector("#touchLayoutEditor", state="hidden")
        page.locator('.game[data-game="th07"]:not([data-product])').click()
        if not page.locator("#mobileOptions").evaluate("el => el.classList.contains('open')"):
            page.locator("#mobileOptionsToggle").click()
        page.wait_for_function("document.querySelector('#mobileOptions').classList.contains('open')")
        page.evaluate("document.querySelector('#touchLayoutEdit').click()")
        page.wait_for_timeout(1500)
        assert page.locator("#touchLayoutEditor").evaluate("el => !el.hidden"), {
            "errors": errors,
            "status": page.locator("#status").text_content(),
        }

        shared = page.evaluate("""() => ({
          movement: document.querySelector('#touchMovementMode').value,
          sensitivity: document.querySelector('#touchSensitivity').value,
          focus: document.querySelector('#touchFocusMode').value,
          doubleTapBomb: document.querySelector('#doubleTapBombToggle').getAttribute('aria-checked'),
          restart: document.querySelector('#restartButtonToggle').getAttribute('aria-checked'),
          restartHidden: document.querySelector('#touchRestart').hidden,
          thpracButtons: document.querySelector('#thpracTouchControlsToggle').getAttribute('aria-checked'),
          stored: JSON.parse(localStorage.getItem('eagler-touhou-touch-options-v1')),
        })""")
        assert shared["movement"] == "joystick-free", shared
        assert shared["sensitivity"] == "200", shared
        assert shared["focus"] == "toggle-button", shared
        assert shared["doubleTapBomb"] == "true", shared
        assert shared["restart"] == "false", shared
        assert shared["restartHidden"] is True, shared
        assert shared["thpracButtons"] == "false", shared
        assert shared["stored"]["restartButtonEnabled"] is False, shared
        assert not errors, errors
        print(json.dumps({"browser": browser.version, "cross_game_touch_settings": "PASS", "restart_above_thprac": "PASS"}))
        browser.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
