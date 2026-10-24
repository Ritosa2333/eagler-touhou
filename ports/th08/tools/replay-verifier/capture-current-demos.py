"""Capture every active Replay tick from the current diagnostic Browser Runtime."""

import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_OUT = ROOT / 'artifacts/replay-verifier/current-demo'


BOOTSTRAP = r'''async fixture => {
  const {bootReplayCapture}=await import('/replay-capture.mjs');
  return bootReplayCapture({presentationLab,fixture});
}'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', default='http://127.0.0.1:8132/')
    parser.add_argument('--fixtures', type=int, default=4)
    parser.add_argument('--max-ticks', type=int, default=20000)
    parser.add_argument('--output', type=Path, default=DEFAULT_OUT)
    args = parser.parse_args()
    if not 1 <= args.fixtures <= 4 or not 1000 <= args.max_ticks <= 200000:
        raise ValueError('Invalid fixture or tick bound')
    args.output.mkdir(parents=True, exist_ok=True)
    suite = []
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        try:
            for fixture in range(args.fixtures):
                context = browser.new_context(viewport={'width': 1280, 'height': 960}, service_workers='block')
                page = context.new_page()
                page.set_default_timeout(120000)
                errors = []
                page.on('pageerror', lambda error: errors.append(str(error)))
                page.goto(args.url, wait_until='load')
                page.wait_for_function('window.presentationLab !== undefined')
                initial = page.evaluate(BOOTSTRAP, fixture)
                rows = initial['rows']; ledger = initial['ledger']; journal = initial['journal']
                application_ticks = initial['applicationTicks']
                done = initial['done']
                tail = []
                stop_reason = None
                terminal_scene = None
                while not done and application_ticks < args.max_ticks:
                    batch = page.evaluate('count => replayVerifierCapture.run(count)', min(300, args.max_ticks - application_ticks))
                    rows.extend(batch['rows']); ledger.extend(batch['ledger'])
                    application_ticks = batch['applicationTicks']; journal = batch['journal']
                    done = batch['done']
                    tail = batch['tail']
                    stop_reason = batch['stopReason']
                    terminal_scene = batch['terminalScene']
                    if len(rows) and len(rows) % 3000 < len(batch['rows']):
                        print(json.dumps({'fixture': fixture, 'captured': len(rows), 'lastFrame': rows[-1]['frame']}), flush=True)
                if not done:
                    stop_reason = 'maximum-ticks'
                    terminal_scene = page.evaluate('() => presentationLab.controller.core.status(presentationLab.controller.runtime.app,0)')
                complete = done and stop_reason == 'returned-to-title' and terminal_scene == 1 and not errors
                result = {
                    'schema': 'th08/current-replay-capture/v3', 'fixture': fixture,
                    'complete': complete, 'stopReason': stop_reason, 'terminalScene': terminal_scene,
                    'replay': initial['replay'], 'errors': errors, 'navigation': initial['navigation'],
                    'build': initial['build'], 'tail': tail, 'rows': rows,
                    'applicationTicks': application_ticks, 'journal': journal, 'cursorLedger': ledger,
                }
                path = args.output / f'demo{fixture}.json'
                path.write_text(json.dumps(result, separators=(',', ':')), encoding='utf-8')
                suite.append({'fixture': fixture, 'complete': complete, 'stopReason': stop_reason, 'ticks': len(rows), 'errors': errors, 'file': path.name})
                print(json.dumps(suite[-1]), flush=True)
                context.close()
                if not complete:
                    raise RuntimeError(f'Demo {fixture} did not return naturally to the title')
        finally:
            browser.close()
    (args.output / 'results.json').write_text(json.dumps({'schema': 'th08/current-replay-capture-suite/v3', 'passed': all(item['complete'] for item in suite), 'fixtures': suite}, indent=2), encoding='utf-8')


if __name__ == '__main__':
    main()
