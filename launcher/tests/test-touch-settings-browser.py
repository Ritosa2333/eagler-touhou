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
            "localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1')"
        )
        page = context.new_page()
        page.emulate_media(reduced_motion="reduce")
        page.on("pageerror", lambda error: errors.append(str(error)))
        page.goto(url, wait_until="load", timeout=30000)
        page.wait_for_function("window.__eaglerBoot?.done === true", timeout=30000)
        # The one-time first-use notice decision resolves asynchronously after
        # boot and may open between the first state check and the next click.
        page.wait_for_timeout(500)
        if page.locator("#firstUseNoticeDialog").get_attribute("open") is not None:
            page.locator("#firstUseNoticeCloseHint").click()
            page.wait_for_function("document.querySelector('#firstUseNoticeDialog')?.open === false")

        def click_real_pointer(selector: str) -> None:
            target = page.locator(selector)
            target.scroll_into_view_if_needed()
            box = target.bounding_box()
            assert box is not None, f"{selector} has no visible box"
            x = box["x"] + box["width"] / 2
            y = box["y"] + box["height"] / 2
            hit = page.evaluate(
                """({selector, x, y}) => {
                  const target = document.querySelector(selector);
                  const hit = document.elementFromPoint(x, y);
                  return {inside: !!target && !!hit && (target === hit || target.contains(hit)),
                    hit: hit?.id || hit?.className || hit?.tagName};
                }""",
                {"selector": selector, "x": x, "y": y},
            )
            assert hit["inside"], f"pointer at {selector} was intercepted by {hit['hit']}"
            page.mouse.click(x, y)

        def verify_touch_exit_focus():
            nonlocal page
            focus_context = browser.new_context(viewport={"width": 844, "height": 390})
            focus_context.add_init_script(
                "localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1')"
            )
            page.close()
            page = focus_context.new_page()
            page.emulate_media(reduced_motion="reduce")
            page.on("pageerror", lambda error: errors.append(str(error)))
            focus_url = url.split("?", 1)[0].rstrip("/") + "/?game=th10"
            try:
                page.goto(focus_url, wait_until="load", timeout=30000)
                page.wait_for_function("window.__eaglerBoot?.done === true", timeout=30000)
                if page.locator("#firstUseNoticeDialog").get_attribute("open") is not None:
                    page.locator("#firstUseNoticeCloseHint").click()
                    page.wait_for_function("document.querySelector('#firstUseNoticeDialog')?.open === false")
                page.wait_for_selector(".tools.is-open", timeout=15000)
                touch_heading = page.locator("#mobileOptionsToggle")
                if touch_heading.is_visible() and touch_heading.get_attribute("aria-expanded") != "true":
                    click_real_pointer("#mobileOptionsToggle")
                click_real_pointer("#touchLayoutEdit")
                page.wait_for_selector("#touchLayoutEditor:not([hidden])")
                # The editor requests browser fullscreen. Release it so
                # Playwright can emulate the physical device rotation below.
                page.evaluate("async () => { if (document.fullscreenElement) await document.exitFullscreen(); }")

                click_real_pointer("#touchSensitivityCustomToggle")
                slider_geometry = page.locator("#touchSensitivity").evaluate("""root => {
                  const rootRect = root.getBoundingClientRect();
                  const trackRect = root.querySelector('.ui-kit-slider-track').getBoundingClientRect();
                  const thumb = root.querySelector('.ui-kit-slider-thumb');
                  const thumbRect = thumb.getBoundingClientRect();
                  const hit = getComputedStyle(thumb, '::before');
                  return {
                    rootWidth: rootRect.width,
                    rootHeight: rootRect.height,
                    trackWidth: trackRect.width,
                    thumbWidth: thumbRect.width,
                    thumbHeight: thumbRect.height,
                    hitInsets: {top: hit.top, right: hit.right, bottom: hit.bottom, left: hit.left},
                    value: thumb.getAttribute('aria-valuenow'),
                  };
                }""")
                assert abs(slider_geometry["rootWidth"] - slider_geometry["trackWidth"]) <= 1, slider_geometry
                assert abs(slider_geometry["rootHeight"] - 44) <= 1, slider_geometry
                assert slider_geometry["hitInsets"] == {"top": "-15px", "right": "0px", "bottom": "-15px", "left": "0px"}, slider_geometry
                assert abs(slider_geometry["thumbHeight"] + 30 - 44) <= 1, slider_geometry
                slider_thumb = page.locator("#touchSensitivity .ui-kit-slider-thumb")
                slider_thumb.focus()
                assert slider_thumb.evaluate("el => document.activeElement === el"), "slider thumb must accept keyboard focus"
                thumb = slider_thumb.bounding_box()
                page.mouse.click(thumb["x"] + thumb["width"] / 2, thumb["y"] - 14)
                assert slider_thumb.evaluate("el => document.activeElement === el"), "expanded vertical slider target must preserve focus"
                assert slider_thumb.get_attribute("aria-valuenow") == slider_geometry["value"], "vertical slider hit area must not change its value"

                click_real_pointer("#touchMovementMode-trigger")
                page.wait_for_selector(".ui-select-content", state="visible")
                page.keyboard.press("Escape")
                page.wait_for_selector(".ui-select-content", state="hidden")

                page.set_viewport_size({"width": 390, "height": 844})
                page.wait_for_timeout(100)
                click_real_pointer("#touchLayoutExit")
                if page.locator("#decisionDialog").evaluate("el => el.open"):
                    click_real_pointer("#decisionConfirm")
                page.wait_for_selector("#touchLayoutEditor", state="hidden")
                page.wait_for_timeout(100)
                return_focus = page.evaluate("""() => {
                  const active = document.activeElement;
                  const options = document.querySelector('.tools.is-open');
                  return {
                    id: active?.id || '',
                    tag: active?.tagName || '',
                    isBody: active === document.body,
                    inOptions: !!options?.contains(active),
                    url: location.href,
                  };
                }""")
                assert return_focus["inOptions"] and not return_focus["isBody"], return_focus
                page.keyboard.press("Escape")
                page.wait_for_function("!document.body.classList.contains('library-tools-open')")
                assert "game=th10" not in page.url, page.url
                assert not page.locator(".tools.is-open").count(), "Escape after editor exit must close the options panel"
                return {"focus": return_focus, "slider": slider_geometry}
            finally:
                focus_context.close()

        if "--focus-only" in sys.argv:
            focus_evidence = verify_touch_exit_focus()
            assert not errors, errors
            print(json.dumps({"browser": browser.version, "rotated_touch_exit_focus": "PASS", **focus_evidence}))
            browser.close()
            return 0

        page.locator('.book-game-link[data-book-product="th06"]').click()
        page.locator("#mobileOptionsToggle").click()
        page.wait_for_function("document.querySelector('#mobileOptions').classList.contains('open')")
        assert page.locator("#touchHelpOpen").get_attribute("aria-label") in ("打开帮助", "Open help")
        assert page.locator("#touchHelpOpen svg").count() == 1

        # Help content follows the enabled input mode, not UA/pointer heuristics.
        # This context has a desktop/fine pointer, so enabling Touch is the
        # regression case that previously kept showing keyboard/gamepad help.
        page.locator("#touchToggle").click()
        page.wait_for_function("document.querySelector('#touchToggle').getAttribute('aria-checked') === 'true'")
        page.evaluate("document.querySelector('#touchHelpOpen').click()")
        page.wait_for_function("document.querySelector('#touchHelp')?.open === true")
        page.wait_for_function("document.querySelector('#help-touch')?.getAttribute('data-state') === 'open'")
        assert page.locator("#help-controls").get_attribute("data-state") == "closed"
        page.evaluate("document.querySelector('#touchHelpClose').click()")
        page.wait_for_function("document.querySelector('#touchHelp')?.open === false")
        page.locator("#touchToggle").click()
        page.wait_for_function("document.querySelector('#touchToggle').getAttribute('aria-checked') === 'false'")
        page.evaluate("document.querySelector('#touchHelpOpen').click()")
        page.wait_for_function("document.querySelector('#touchHelp')?.open === true")
        assert page.locator("#help-touch").count() == 0
        assert page.locator("#help-controls").get_attribute("data-state") == "open"
        page.evaluate("document.querySelector('#touchHelpClose').click()")
        page.wait_for_function("document.querySelector('#touchHelp')?.open === false")

        page.evaluate("document.querySelector('#touchLayoutEdit').click()")
        page.wait_for_timeout(1500)
        assert page.locator("#touchLayoutEditor").evaluate("el => !el.hidden"), {
            "errors": errors,
            "status": page.locator("#status").text_content(),
        }
        # Keep the preview usable: compact by default and out of the way during drag.
        page.evaluate("async () => { if (document.fullscreenElement) await document.exitFullscreen(); }")
        for width, height in ((390, 844), (1280, 800), (844, 390)):
            page.set_viewport_size({"width": width, "height": height})
            page.wait_for_timeout(120)
            panel = page.locator("#touchLayoutEditor").bounding_box()
            assert panel["width"] <= 320 and panel["height"] <= min(520, height - 32), panel
            assert panel["x"] >= 0 and panel["y"] >= 0, panel
            assert panel["x"] + panel["width"] <= width + 1, panel
            assert panel["y"] + panel["height"] <= height + 1, panel
            assert page.locator("#touchLayoutSave").is_visible()
            assert page.locator("#player").evaluate(
                "el => Number(getComputedStyle(el).zIndex) > Number(getComputedStyle(document.querySelector('.tools.is-open')).zIndex)"
            )
        page.set_viewport_size({"width": 390, "height": 844})
        page.wait_for_timeout(120)
        click_real_pointer("#touchLayoutCollapse")
        assert not page.locator("#touchWorkbenchBody").is_visible()
        click_real_pointer("#touchLayoutCollapse")

        def select_movement(value: str) -> None:
            click_real_pointer("#touchMovementMode-trigger")
            option = f".ui-select-content [role=option][data-value='{value}']"
            page.wait_for_selector(option)
            assert page.locator(".ui-select-content").evaluate(
                "el => Number(getComputedStyle(el).zIndex) > Number(getComputedStyle(document.querySelector('#player')).zIndex)"
            )
            click_real_pointer(option)
            page.wait_for_function(
                "value => document.querySelector('#decisionDialog')?.open === true || document.querySelector('#touchMovementMode').value === value",
                arg=value,
            )
            if page.locator("#decisionDialog").evaluate("el => el.open"):
                click_real_pointer("#decisionConfirm")
                page.wait_for_function("document.querySelector('#decisionDialog')?.open === false")
            page.wait_for_function("value => document.querySelector('#touchMovementMode').value === value", arg=value)

        select_movement("joystick")
        select_movement("touch")
        click_real_pointer("[data-touch-sensitivity-preset='150']")
        page.wait_for_function("document.querySelector('#touchSensitivityValue').textContent.includes('150')")
        click_real_pointer("#touchLayoutSave")
        control = page.locator("#touchBomb").bounding_box()
        x, y = control["x"] + control["width"] / 2, control["y"] + control["height"] / 2
        page.mouse.move(x, y)
        page.mouse.down()
        page.mouse.move(x + 20, y - 20, steps=3)
        assert page.locator("#player").evaluate("el => el.classList.contains('touch-layout-manipulating')")
        page.mouse.up()
        assert not page.locator("#player").evaluate("el => el.classList.contains('touch-layout-manipulating')")
        click_real_pointer("#touchLayoutExit")
        if page.locator("#decisionDialog").evaluate("el => el.open"):
            click_real_pointer("#decisionCancel")
            page.wait_for_function("document.querySelector('#decisionDialog')?.open === false")
            assert page.locator("#touchLayoutEditor").evaluate("el => !el.hidden")
            click_real_pointer("#touchLayoutSave")
        if page.locator("#touchLayoutEditor").evaluate("el => !el.hidden"):
            click_real_pointer("#touchLayoutExit")
        page.wait_for_selector("#touchLayoutEditor", state="hidden")
        return_focus = page.evaluate("""() => ({
          id: document.activeElement?.id,
          isBody: document.activeElement === document.body,
          inOptions: !!document.querySelector('.tools.is-open')?.contains(document.activeElement),
          optionsOpen: !!document.querySelector('.tools.is-open'),
        })""")
        assert return_focus["optionsOpen"] and return_focus["inOptions"] and not return_focus["isBody"], return_focus
        page.keyboard.press("Escape")
        page.wait_for_selector("body:not(.library-tools-open)")
        assert not page.locator(".tools.is-open").count(), "Escape from the restored options focus must close options"

        page.locator('.book-game-link[data-book-product="th06"]').click()
        page.wait_for_selector(".tools.is-open")
        if not page.locator("#mobileOptions").evaluate("el => el.classList.contains('open')"):
            click_real_pointer("#mobileOptionsToggle")
        page.evaluate("document.querySelector('#touchLayoutEdit').click()")
        page.wait_for_selector("#touchLayoutEditor:not([hidden])")
        editor_url = page.url
        page.go_back(wait_until="commit")
        page.wait_for_timeout(100)
        if page.locator("#decisionDialog").evaluate("el => el.open"):
            page.evaluate(
                "document.querySelector('.decision-window').requestSubmit(document.querySelector('#decisionConfirm'))"
            )
        page.wait_for_selector("#touchLayoutEditor", state="hidden")
        assert page.url == editor_url, "system Back must close only the editor history entry"
        assert not page.locator("#player").evaluate("el => el.classList.contains('touch-layout-edit')")
        page.emulate_media(reduced_motion="no-preference")
        page.evaluate("document.querySelector('#touchLayoutEdit').click()")
        page.wait_for_function("document.querySelector('#player').getAnimations().some(a => a.playState === 'running')")
        assert page.locator("#touchLayoutEditor").evaluate("el => el.getAnimations().length === 0"), "enter the whole settings scene, not just the floating panel"
        page.wait_for_function("document.querySelector('#player').getAnimations().length === 0")
        assert page.locator("#player").evaluate("el => getComputedStyle(el).opacity === '1' && !el.classList.contains('touch-layout-preparing')")
        page.emulate_media(reduced_motion="reduce")
        page.wait_for_selector("#touchLayoutEditor:not([hidden])")
        assert page.locator("#touchRestart").evaluate(
            "el => el.hidden && getComputedStyle(el).display === 'none'"
        ), "disabled R must be actually hidden in the layout editor"

        assert page.locator("#touchLayoutSettingsPanel").is_visible()
        assert page.locator("#touchLayoutArrangement").is_visible()
        assert page.locator("#restartButtonToggle").evaluate(
            "r => !!(r.closest('.touch-layout-setting-row').compareDocumentPosition("
            "document.querySelector('#thpracTouchControlsToggle').closest('.touch-layout-setting-row')) "
            "& Node.DOCUMENT_POSITION_FOLLOWING)"
        )
        click_real_pointer("#touchSensitivityCustomToggle")
        slider = page.locator("#touchSensitivity").bounding_box()
        page.mouse.click(slider["x"] + slider["width"] / 2, slider["y"] + slider["height"] / 2)
        page.wait_for_function("document.querySelector('#touchSensitivityValue').textContent.includes('200')")
        page.locator("#touchFocusMode").select_option("toggle-button", force=True)
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
        page.locator("#touchMovementMode").select_option("joystick-free", force=True)
        page.wait_for_function(
            "document.querySelector('#touchMovementMode').value === 'joystick-free' || document.querySelector('#decisionDialog')?.open === true"
        )
        if page.locator("#decisionDialog").evaluate("el => el.open"):
            page.evaluate(
                "document.querySelector('.decision-window').requestSubmit(document.querySelector('#decisionConfirm'))"
            )
            page.wait_for_function("document.querySelector('#decisionDialog')?.open === false")
        page.wait_for_function("document.querySelector('#touchMovementMode').value === 'joystick-free'")

        page.locator("#touchLayoutExit").click()
        page.wait_for_timeout(100)
        if page.locator("#decisionDialog").evaluate("el => el.open"):
            page.evaluate(
                "document.querySelector('.decision-window').requestSubmit(document.querySelector('#decisionConfirm'))"
            )
        page.wait_for_selector("#touchLayoutEditor", state="hidden")
        page.locator("#libraryBack").click()
        page.locator('.book-game-link[data-book-product="th07"]').click()
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
          sensitivity: JSON.parse(localStorage.getItem('eagler-touhou-touch-options-v1')).touchSensitivity,
          focus: document.querySelector('#touchFocusMode').value,
          doubleTapBomb: document.querySelector('#doubleTapBombToggle').getAttribute('aria-checked'),
          restart: document.querySelector('#restartButtonToggle').getAttribute('aria-checked'),
          restartHidden: document.querySelector('#touchRestart').hidden,
          thpracButtons: document.querySelector('#thpracTouchControlsToggle').getAttribute('aria-checked'),
          stored: JSON.parse(localStorage.getItem('eagler-touhou-touch-options-v1')),
        })""")
        assert shared["movement"] == "joystick-free", shared
        assert shared["sensitivity"] == 200, shared
        assert shared["focus"] == "toggle-button", shared
        assert shared["doubleTapBomb"] == "true", shared
        assert shared["restart"] == "false", shared
        assert shared["restartHidden"] is True, shared
        assert shared["thpracButtons"] == "false", shared
        assert shared["stored"]["restartButtonEnabled"] is False, shared

        focus_evidence = verify_touch_exit_focus()
        assert not errors, errors
        print(json.dumps({"browser": browser.version, "cross_game_touch_settings": "PASS", "restart_above_thprac": "PASS", "rotated_touch_exit_focus": "PASS", **focus_evidence}))
        browser.close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
