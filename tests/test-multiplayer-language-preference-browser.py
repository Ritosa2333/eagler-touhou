"""A direct TH06 room link must restore a language after the Host catalog arrives."""

import argparse
import json
from urllib.parse import urljoin

from playwright.sync_api import sync_playwright


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("url", help="Launcher development server URL")
    args = parser.parse_args()

    pack = {
        "url": "games/th06/language/lang_zh-hans.zip",
        "bytes": 1,
        "sha256": "0" * 64,
        "runtimeVersion": "test",
        "files": 1,
    }
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True)
        cases = (
            ("lang_zh-hans", None, True, "lang_zh-hans"),
            ("ja", "lang_zh-hans", False, "lang_zh-hans"),
            ("unsupported", None, True, "ja"),
        )
        for single_language, multiplayer_language, share_settings, expected in cases:
            context = browser.new_context(service_workers="block")
            context.add_init_script(
                "localStorage.setItem('eagler-touhou-language-v1-th06', "
                f"{json.dumps(single_language)});"
                + (
                    "" if share_settings else
                    "localStorage.setItem('eagler-touhou-th06mp-share-singleplayer-settings-v1', '0');"
                    "localStorage.setItem('eagler-touhou-language-v1-th06mp', "
                    f"{json.dumps(multiplayer_language)});"
                )
            )

            def publish_language(route):
                manifest = route.fetch().json()
                manifest["games"]["th06"]["languages"] = [
                    {"id": "lang_zh-hans", "title": "中文（简体）", "pack": pack}
                ]
                manifest["games"]["th06"]["languageOptions"] = [
                    {"id": "ja", "title": "日本語(原版)", "pack": None},
                    {"id": "lang_zh-hans", "title": "中文（简体）", "pack": pack},
                ]
                route.fulfill(status=200, content_type="application/json", body=json.dumps(manifest))

            context.route("**/host-manifest.json", publish_language)
            page = context.new_page()
            page.goto(urljoin(args.url, "?game=th06mp&room=123456"), wait_until="load")
            page.wait_for_function("window.__eaglerBoot?.done === true")
            actual = page.locator("#mpLanguageSelect").input_value()
            assert actual == expected, {
                "single": single_language,
                "multiplayer": multiplayer_language,
                "share": share_settings,
                "expected": expected,
                "actual": actual,
            }
            context.close()
        browser.close()
    print(json.dumps({"th06mpDirectRoomLanguagePreference": "PASS"}))


if __name__ == "__main__":
    main()
