"""Small native product scenes. No replay oracle regeneration or custom game loop."""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

p = argparse.ArgumentParser()
p.add_argument('--url', required=True)
p.add_argument('--output', type=Path, required=True)
args = p.parse_args()
report = {'passed': False, 'identities': [], 'cases': [], 'errors': []}

def call(page, code, arg=None):
    return page.evaluate(code, arg)

def ticks(page, n, keys):
    assert call(page, 'v=>multiplayerSmoke.commit(v)', keys)
    return call(page, 'n=>multiplayerSmoke.ticks(n)', n)

def capture(page, name):
    page.locator('canvas').first.screenshot(path=str(args.output.parent / (name + '.png')))

with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        for count in (2, 3):
            context = browser.new_context(viewport={'width': 640, 'height': 480}, service_workers='block')
            page = context.new_page()
            page.on('pageerror', lambda e: report['errors'].append(str(e)))
            try:
                page.goto(args.url)
                page.wait_for_function('!!window.multiplayerSmoke', timeout=120000)
                call(page, 'n=>multiplayerSmoke.start(1234,Array.from({length:n},(_,i)=>i),0)', count)
                call(page, 'multiplayerSmoke.ticks(120)')
                for _ in range(3):
                    call(page, 'multiplayerSmoke.key("KeyZ",true)')
                    call(page, 'multiplayerSmoke.ticks(2)')
                    call(page, 'multiplayerSmoke.key("KeyZ",false)')
                    call(page, 'multiplayerSmoke.ticks(120)')
                assert call(page, 'multiplayerSmoke.status()')[3] == 1
                report['identities'].append(call(page, 'multiplayerSmoke.identity()'))
                hud = call(page, 'multiplayerSmoke.productHUD()')
                case = {'players': count, 'front': [hud[48+i*7:55+i*7] for i in range(16)], 'gauges': []}
                report['cases'].append(case)
                capture(page, f'{count}p-resources')
                for viewer in range(count):
                    assert call(page, 'v=>multiplayerSmoke.productGauge(v)', viewer)
                    before = call(page, 'multiplayerSmoke.canonical()')
                    assert call(page, 'multiplayerSmoke.productGaugePure()'), 'viewer draw wrote native gauge VM or RNG'
                    call(page, 'multiplayerSmoke.presented(0.25)')
                    call(page, 'multiplayerSmoke.presented(0.75)')
                    after = call(page, 'multiplayerSmoke.canonical()')
                    assert before == after, 'viewer-only draw changed gameplay owners'
                    hud = call(page, 'multiplayerSmoke.productHUD()')
                    assert hud[3] == (-9000, 7500, 2500)[viewer] and hud[5] == 4321, hud[:8]
                    case['gauges'].append({'viewer': viewer, 'displayed': hud[3], 'obsoleteShared': hud[5], 'pure': True})
                    capture(page, f'{count}p-gauge-p{viewer+1}')
                case['passed'] = True
                print('TH08 product HUD/gauge', count, 'PASS', flush=True)
            finally:
                context.close()
        assert not report['errors'], report['errors']
        report['passed'] = True
    except BaseException as e:
        report['error'] = str(e)
        raise
    finally:
        browser.close()
        args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
