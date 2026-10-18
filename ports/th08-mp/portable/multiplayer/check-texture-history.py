"""Exact framebuffer restore, CPU texture writers, capture lifetime and reuse."""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--url', required=True)
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()
report = {'passed': False, 'cases': [], 'errors': []}
with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        for count in (2, 3):
            page = browser.new_page()
            page.on('pageerror', lambda error: report['errors'].append(str(error)))
            page.goto(args.url)
            page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
            page.evaluate('n=>multiplayerSmoke.start(1234,Array.from({length:n},(_,i)=>i))', count)
            page.evaluate('multiplayerSmoke.ticks(120)')
            result = page.evaluate('''()=>({identity:multiplayerSmoke.identity(),
                texture:multiplayerSmoke.textureRestore(),gpu:multiplayerSmoke.gpuHistory()})''')
            report['cases'].append({'players': count, **result})
            assert result['texture'] and result['gpu'], result
            page.close()
        assert not report['errors'], report['errors']
        report['passed'] = True
    finally:
        browser.close()
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
print('Texture history: PASS')
