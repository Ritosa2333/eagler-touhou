"""Compatibility entry point: library return and number navigation replace filters."""
import argparse
import json
from playwright.sync_api import sync_playwright


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('url', nargs='?', default='http://127.0.0.1:8130/')
    parser.add_argument('--test-build', action='store_true')
    args = parser.parse_args()
    with sync_playwright() as p:
        browser = p.chromium.launch()
        context = browser.new_context(viewport={'width': 1280, 'height': 800}, service_workers='block')
        # Retired preferences must never hide an entire shelf after upgrade.
        context.add_init_script("localStorage.setItem('eagler-touhou-card-filter-v1','multiplayer');localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');localStorage.setItem('eagler-touhou-site-notice-enabled-v1','0')")
        page = context.new_page()
        errors = []
        page.on('pageerror', lambda error: errors.append(str(error)))
        page.goto(args.url)
        page.wait_for_function('window.__eaglerBoot?.done === true')
        assert page.locator('#cardFilterBar').count() == 0
        assert page.locator('.game-shelf:not([hidden])').count() == 2
        shelf = page.locator('[data-shelf=singleplayer]')
        toggle = shelf.locator('.minimap-toggle').first
        assert shelf.locator('.minimap-panel').count() == 0
        toggle.hover()
        assert shelf.locator('.minimap-toggle.is-current').count() == 1
        assert shelf.locator('.game-rail').evaluate('e => e.scrollLeft') == 0
        assert toggle.locator('.minimap-index').inner_text() == '06'
        toggle.click()
        assert page.locator('body').evaluate("e => e.classList.contains('library-tools-open')")
        page.locator('#libraryBack').click()
        # Wheels never pan covers; deliberate mouse holds drag without link/text DnD.
        rail = shelf.locator('.game-rail')
        rail_box = rail.bounding_box()
        drag_x, drag_y = rail_box['x'] + 400, rail_box['y'] + 100
        page.mouse.move(drag_x, drag_y)
        page.mouse.wheel(180, 0)
        page.wait_for_timeout(120)
        assert rail.evaluate('e => e.scrollLeft') == 0
        page.mouse.down()
        page.wait_for_timeout(360)
        assert rail.evaluate("e => e.classList.contains('is-dragging')")
        page.mouse.move(drag_x - 180, drag_y, steps=6)
        page.mouse.up()
        assert rail.evaluate('e => e.scrollLeft') > 0
        assert page.evaluate("window.getSelection().toString()") == ''
        assert not rail.locator('a,img').evaluate_all('nodes => nodes.some(e => e.draggable)')
        assert not page.locator('body').evaluate("e => e.classList.contains('library-tools-open')")
        page.mouse.down()
        page.wait_for_timeout(360)
        page.mouse.move(drag_x + 100, drag_y, steps=6)
        page.mouse.up()
        assert rail.evaluate('e => e.scrollLeft') == 0
        # Native mouse capture: held horizontal scrubbing must keep working
        # between buttons and below their hit boxes, including reverse travel.
        visible = shelf.locator('.minimap-toggle:not([hidden])')
        first_box, last_box = visible.first.bounding_box(), visible.last.bounding_box()
        first_x = first_box['x'] + first_box['width'] / 2
        last_x = last_box['x'] + last_box['width'] / 2
        y = first_box['y'] + first_box['height'] / 2
        last_id = visible.last.get_attribute('data-minimap-preview')
        for hold_ms in (460, 0):
            page.mouse.move(first_x, y)
            page.mouse.down()
            if hold_ms:
                page.wait_for_timeout(hold_ms)
            page.mouse.move(last_x, y, steps=4)
            assert shelf.locator('.game.nav-preview').get_attribute('data-game') == last_id
            page.wait_for_timeout(180)
            assert visible.first.evaluate('e => getComputedStyle(e).backgroundColor') == 'rgba(0, 0, 0, 0)'
            assert visible.last.evaluate('e => getComputedStyle(e).backgroundColor') == 'rgb(241, 228, 230)'
            page.mouse.move(first_x, y + 36, steps=4)
            assert shelf.locator('.game.nav-preview').get_attribute('data-game') == 'th06'
            page.mouse.move(last_x, y + 36, steps=4)
            page.mouse.up()
            assert shelf.locator('.game.nav-preview').get_attribute('data-game') == last_id
            assert shelf.locator('.minimap-toggle.is-current').count() == 1
            assert not page.locator('body').evaluate("e => e.classList.contains('library-tools-open')")
        # A new short press is not swallowed by the previous drag's click guard.
        toggle.click()
        assert not page.locator('body').evaluate("e => e.classList.contains('library-tools-open')")
        assert toggle.get_attribute('aria-current') == 'true'
        toggle.click()
        assert page.locator('body').evaluate("e => e.classList.contains('library-tools-open')")
        for selector in ('.tools', '.library-backdrop', '.game-library'):
            assert page.locator(selector).evaluate('e => getComputedStyle(e).backdropFilter') == 'none'
            assert page.locator(selector).evaluate('e => getComputedStyle(e).filter') == 'none'
        page.locator('#libraryBack').click()
        box = toggle.bounding_box()
        page.mouse.move(box['x'] + box['width']/2, box['y'] + box['height']/2)
        page.mouse.down()
        page.wait_for_timeout(460)
        page.mouse.up()
        assert shelf.locator('.minimap-panel').count() == 0
        assert toggle.get_attribute('aria-current') == 'true'
        assert not page.locator('body').evaluate("e => e.classList.contains('library-tools-open')")
        page.set_viewport_size({'width': 390, 'height': 844})
        toggle.focus()
        toggle.press('End')
        page.wait_for_function('document.querySelector(".game-rail").scrollLeft > 0')
        expected = visible.last.get_attribute('data-minimap-preview')
        assert shelf.locator('.game.nav-preview').get_attribute('data-game') == expected
        assert not page.locator('#main').evaluate("e => e.classList.contains('has-selection')")
        # Covers share the same select-first policy as the number navigation.
        shelf.locator('.game:not([hidden])').first.click()
        assert not page.locator('body').evaluate("e => e.classList.contains('library-tools-open')")
        assert toggle.get_attribute('aria-current') == 'true'
        # Same-product return must preserve settings hit-testing.
        for _ in range(2):
            before = shelf.locator('.game-rail').bounding_box()
            shelf.locator('.game:not([hidden])').first.click()
            assert shelf.locator('.game-rail').bounding_box()['width'] == before['width']
            assert page.locator('.tools').evaluate("e => getComputedStyle(e).position") == 'fixed'
            page.locator('#gamePackageImport').click()
            page.locator('#gameDataImportWindow').wait_for(state='visible')
            page.locator('#gameDataImportClose').click()
            page.locator('#libraryBack').click()
        assert page.locator('.game-shelf:not([hidden])').count() == 2
        assert page.evaluate('document.documentElement.scrollWidth <= innerWidth')
        page.emulate_media(reduced_motion='reduce')
        toggle.focus()
        toggle.press('Home')
        assert toggle.evaluate("e => getComputedStyle(e).transitionDuration") == '0s'
        toggle.press('Escape')
        assert shelf.locator('.minimap-toggle.is-current').count() == 1
        assert not errors, errors
        print(json.dumps({'library_navigation_return_mobile_reduced_motion': 'PASS'}))
        browser.close()


if __name__ == '__main__':
    main()
