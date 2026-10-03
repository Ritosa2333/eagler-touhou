"""Exercise the actual multiplayer WASM against local retail resources.

Run serve.mjs first. This is a native owner/input smoke test, not a transport
or rollback acceptance gate. It never changes production saves.
"""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

parser = argparse.ArgumentParser()
parser.add_argument('--url', default='http://127.0.0.1:8138/')
parser.add_argument('--output', type=Path, default=Path('artifacts/multiplayer-tests/native-smoke.json'))
args = parser.parse_args()
report = {'passed': False, 'scope': 'native multiplayer owners and committed input', 'cases': []}
with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        for loadouts, local, difficulty in [
            ([[0, 0], [0, 1], [0, 2]], 0, 1),
            ([[1, 0], [1, 1], [1, 2]], 2, 1),
            ([[1, 0], [0, 1]], 1, 4),
        ]:
            context = browser.new_context(service_workers='block')
            page = context.new_page()
            errors = []
            page.on('pageerror', lambda error: errors.append(str(error)))
            page.goto(args.url)
            page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=120000)
            page.evaluate('(x)=>multiplayerSmoke.start(x[0],x[1],x[2])', [loadouts, local, difficulty])
            status = None
            # Native title resource loading waits for the opening's 300 Draws.
            for _ in range(60):
                status = page.evaluate('multiplayerSmoke.ticks(10)')
                if status[7] >= 35:
                    break
            assert status[3] == 1 and status[7] >= 35, status
            assert status[1] == len(loadouts) and status[2] == local, status
            for seat, loadout in enumerate(loadouts):
                row = status[8 + seat * 12:20 + seat * 12]
                assert row[:2] == loadout and row[2] == 2 and row[4] == 1, row
                assert row[3] == (80 if difficulty == 4 else 0), row
            moved = page.evaluate('multiplayerSmoke.ticks(5,[128,64,16])')
            assert moved[13] > status[13] and moved[25] < status[25], (status, moved)
            if len(loadouts) == 3:
                assert moved[38] < status[38] and moved[37] == status[37], (status, moved)
            if difficulty == 4:
                bomb = page.evaluate('multiplayerSmoke.ticks(1,[2,0])')
                assert bomb[11] == 60 and bomb[23] == 80, bomb
                assert bomb[16] != 0 and bomb[28] == 0, bomb
            assert not errors, errors
            report['cases'].append({'loadouts': loadouts, 'local': local, 'difficulty': difficulty, 'initial': status, 'moved': moved})
            context.close()
        report['passed'] = True
    finally:
        browser.close()
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
print(json.dumps({'passed': report['passed'], 'cases': len(report['cases'])}))
