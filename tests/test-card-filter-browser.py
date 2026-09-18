"""Category navigation always returns home and keeps only the last request."""
import argparse
import json

from playwright.sync_api import sync_playwright


def home(page, category, counts):
    page.wait_for_function("!document.querySelector('#main').classList.contains('card-filter-motion')")
    state = page.evaluate("""() => ({
      selected: document.querySelectorAll('.game.selected').length,
      expanded: document.querySelector('#main').classList.contains('has-selection'),
      inert: document.querySelector('#main').inert,
      toolsInert: document.querySelector('.tools').inert,
      tools: document.querySelector('.tools').getAttribute('aria-hidden'),
      category: document.querySelector('[data-card-filter][aria-pressed=true]').dataset.cardFilter,
      count: document.querySelectorAll('.game:not([hidden])').length,
      opacity: getComputedStyle(document.querySelector('#main')).opacity,
      route: new URL(location.href).searchParams.has('game'),
      overflow: document.documentElement.scrollWidth > innerWidth
    })""")
    assert state == dict(selected=0, expanded=False, inert=False, toolsInert=False, tools="true", category=category,
                         count=counts[category], opacity="1",
                         route=False, overflow=False), state


def change(page, category, counts):
    page.evaluate("c => document.querySelector(`[data-card-filter=${c}]`).click()", category)
    home(page, category, counts)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("url", nargs="?", default="http://127.0.0.1:8130/")
    parser.add_argument("--test-build", action="store_true", help="Retained for compatibility; TH10 is formal in all builds")
    args = parser.parse_args()
    with sync_playwright() as p:
        browser = p.chromium.launch()
        context = browser.new_context(viewport={"width": 1280, "height": 800}, service_workers="block")
        context.add_init_script("localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1')")
        page = context.new_page()
        errors = []
        page.on("pageerror", lambda error: errors.append(str(error)))
        page.goto(args.url)
        page.wait_for_function("window.__eaglerBoot?.done === true")
        catalog = page.evaluate("""() => {
          const cards = [...document.querySelectorAll('.game')];
          const products = cards.map(card => card.dataset.product || card.dataset.game).filter(Boolean);
          return {
            products,
            counts: {
              all: cards.length,
              original: cards.filter(card => !card.classList.contains('game-multiplayer')).length,
              multiplayer: cards.filter(card => card.classList.contains('game-multiplayer')).length,
            },
          };
        }""")
        products = catalog["products"]
        counts = catalog["counts"]
        assert len(products) == len(set(products)) and counts["all"] == len(products), catalog
        assert counts["original"] > 0 and counts["multiplayer"] > 0, catalog
        page.wait_for_timeout(1000)
        # Returning home and reselecting the same product must immediately
        # restore tools hit-testing. Edge could leave the visible tools parent
        # above its descendants in hit testing until a different card was picked.
        first_product = products[0]
        page.evaluate("p => [...document.querySelectorAll('.game')].find(c => (c.dataset.product || c.dataset.game) === p).click()", first_product)
        page.wait_for_timeout(80)
        assert page.evaluate("!document.querySelector('.tools').inert && document.querySelector('.tools').getAttribute('aria-hidden') === 'false'")
        page.locator("#gamePackageImport").click()
        page.locator("#gameDataImportWindow").wait_for(state="visible", timeout=5000)
        page.locator("#gameDataImportClose").click()
        change(page, "all", counts)
        page.evaluate("p => [...document.querySelectorAll('.game')].find(c => (c.dataset.product || c.dataset.game) === p).click()", first_product)
        page.wait_for_timeout(80)
        page.locator("#gamePackageImport").click()
        page.locator("#gameDataImportWindow").wait_for(state="visible", timeout=5000)
        page.locator("#gameDataImportClose").click()
        change(page, "all", counts)
        # Includes same-category clicks and destinations containing the old game.
        for product in products:
            for category in ["all", "original", "multiplayer"]:
                change(page, "all", counts)
                page.evaluate("p => [...document.querySelectorAll('.game')].find(c => (c.dataset.product || c.dataset.game) === p).click()", product)
                page.wait_for_timeout(60)
                change(page, category, counts)
        # Last click wins both before the layout commit and during entrance.
        for gap in [40, 200]:
            for category in ["all", "original", "multiplayer"]:
                page.evaluate("c => document.querySelector(`[data-card-filter=${c}]`).click()", category)
                page.wait_for_timeout(gap)
            home(page, "multiplayer", counts)
        page.reload()
        page.wait_for_timeout(1000)
        home(page, "multiplayer", counts)
        page.emulate_media(reduced_motion="reduce")
        change(page, "original", counts)
        page.emulate_media(reduced_motion="no-preference")
        page.locator('#lessMotionToggle').click()
        change(page, "all", counts)
        assert not page.evaluate("document.querySelector('#main').getAnimations().length")
        page.locator('#lessMotionToggle').click()
        page.set_viewport_size({"width": 390, "height": 844})
        page.evaluate("document.querySelector('.game-th07').click()")
        page.wait_for_timeout(250)
        page.evaluate("window.scrollTo(0, document.documentElement.scrollHeight)")
        change(page, "original", counts)
        assert page.evaluate("scrollY") == 0
        assert not errors, errors
        print(json.dumps({"browser": browser.version, "product_category_cases": len(products) * 3,
                          "rapid_switch_reload_mobile_reduced_motion": "PASS"}))
        browser.close()


if __name__ == "__main__":
    main()
