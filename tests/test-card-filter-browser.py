"""Category navigation always returns home and keeps only the last request."""
import argparse
import json

from playwright.sync_api import sync_playwright


COUNTS = {"all": 6, "original": 4, "multiplayer": 2}


def home(page, category):
    page.wait_for_function("!document.querySelector('#main').classList.contains('card-filter-motion')")
    state = page.evaluate("""() => ({
      selected: document.querySelectorAll('.game.selected').length,
      expanded: document.querySelector('#main').classList.contains('has-selection'),
      inert: document.querySelector('#main').inert,
      tools: document.querySelector('.tools').getAttribute('aria-hidden'),
      category: document.querySelector('[data-card-filter][aria-pressed=true]').dataset.cardFilter,
      count: document.querySelectorAll('.game:not([hidden])').length,
      opacity: getComputedStyle(document.querySelector('#main')).opacity,
      route: new URL(location.href).searchParams.has('game'),
      overflow: document.documentElement.scrollWidth > innerWidth
    })""")
    assert state == dict(selected=0, expanded=False, inert=False, tools="true", category=category,
                         count=COUNTS[category], opacity="1",
                         route=False, overflow=False), state


def change(page, category):
    page.evaluate("c => document.querySelector(`[data-card-filter=${c}]`).click()", category)
    home(page, category)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("url", nargs="?", default="http://127.0.0.1:8130/")
    parser.add_argument("--test-build", action="store_true", help="Retained for compatibility; TH10 is formal in all builds")
    args = parser.parse_args()
    products = ["th06", "th07", "th08", "th06mp", "th07mp"]
    products += ["th10"]
    with sync_playwright() as p:
        browser = p.chromium.launch()
        context = browser.new_context(viewport={"width": 1280, "height": 800}, service_workers="block")
        context.add_init_script("localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1')")
        page = context.new_page()
        errors = []
        page.on("pageerror", lambda error: errors.append(str(error)))
        page.goto(args.url)
        page.wait_for_function("document.querySelectorAll('.card-art-image').length === 10")
        page.wait_for_timeout(1000)
        # Includes same-category clicks and destinations containing the old game.
        for product in products:
            for category in ["all", "original", "multiplayer"]:
                change(page, "all")
                page.evaluate("p => [...document.querySelectorAll('.game')].find(c => (c.dataset.product || c.dataset.game) === p).click()", product)
                page.wait_for_timeout(60)
                change(page, category)
        # Last click wins both before the layout commit and during entrance.
        for gap in [40, 200]:
            for category in ["all", "original", "multiplayer"]:
                page.evaluate("c => document.querySelector(`[data-card-filter=${c}]`).click()", category)
                page.wait_for_timeout(gap)
            home(page, "multiplayer")
        page.reload()
        page.wait_for_timeout(1000)
        home(page, "multiplayer")
        page.emulate_media(reduced_motion="reduce")
        change(page, "original")
        page.emulate_media(reduced_motion="no-preference")
        page.locator('#lessMotionToggle').click()
        change(page, "all")
        assert not page.evaluate("document.querySelector('#main').getAnimations().length")
        page.locator('#lessMotionToggle').click()
        page.set_viewport_size({"width": 390, "height": 844})
        page.evaluate("document.querySelector('.game-th07').click()")
        page.wait_for_timeout(250)
        page.evaluate("window.scrollTo(0, document.documentElement.scrollHeight)")
        change(page, "original")
        assert page.evaluate("scrollY") == 0
        assert not errors, errors
        print(json.dumps({"browser": browser.version, "product_category_cases": len(products) * 3,
                          "rapid_switch_reload_mobile_reduced_motion": "PASS"}))
        browser.close()


if __name__ == "__main__":
    main()
