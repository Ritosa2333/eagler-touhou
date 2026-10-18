"""Visit real Replay scenes and sample the presentation without advancing inputs.

Fixtures are the four named replays extracted by prepare.py, never generated
game state or arbitrary workspace search results. All output is local evidence.
"""
import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / 'artifacts/presentation-lab/evidence'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', default='http://127.0.0.1:8132/')
    parser.add_argument('--fixtures', type=int, default=4)
    parser.add_argument('--ticks', type=int, default=1200)
    args = parser.parse_args()
    assert 1 <= args.fixtures <= 4 and 120 <= args.ticks <= 3600
    OUT.mkdir(parents=True, exist_ok=True)
    results = []
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        try:
            for fixture in range(args.fixtures):
                context = browser.new_context(viewport={'width': 1440, 'height': 1120}, service_workers='block')
                page = context.new_page()
                page.set_default_timeout(120000)
                errors = []
                page.on('pageerror', lambda e: errors.append(str(e)))
                page.goto(args.url, wait_until='load')
                page.wait_for_function('window.presentationLab !== undefined')
                page.locator('#boot').click()
                page.wait_for_function('!!window.presentationLab?.controller')
                result = page.evaluate('''async ({fixture,ticks}) => {
                  const response=await fetch(`/input/demo${fixture}.rpy`);
                  if(!response.ok)throw Error('Missing named Replay fixture');
                  const bytes=Array.from(new Uint8Array(await response.arrayBuffer()));
                  await presentationLab.boot({replayBytes:bytes});
                  const c=presentationLab.controller;
                  for(let i=0;i<12&&c.core.status(c.runtime.app,9)!==4;i++)c.press('ArrowDown');
                  if(c.core.status(c.runtime.app,9)!==4)throw Error('Replay menu not reached');
                  const navigation=[];
                  for(let i=0;i<5&&c.core.status(c.runtime.app,0)!==2;i++){
                    c.press('KeyZ');navigation.push([c.core.status(c.runtime.app,0),c.core.status(c.runtime.app,8),c.core.status(c.runtime.app,9)]);
                  }
                  if(c.core.status(c.runtime.app,0)!==2||!(c.trace()[4]&8))throw Error('Real Replay playback did not start: '+JSON.stringify(navigation));
                  c.clearKeys();
                  // Let the authored loading transition finish before trying
                  // world interpolation. Capture reports retain gate eligibility.
                  c.step(120);
                  const scan=await c.scan({ticks,stride:30,onWindow:r=>presentationLab.renderReport(r)});
                  const unchanged=scan.reports.every(r=>JSON.stringify(r.traceBefore)===JSON.stringify(r.traceAfter));
                  return {fixture,navigation,scan,unchanged,lastTrace:c.trace(),build:presentationLab.identity};
                }''', {'fixture': fixture, 'ticks': args.ticks})
                result['errors'] = errors
                page.screenshot(path=str(OUT / f'replay-{fixture}.png'), full_page=True)
                results.append(result)
                (OUT / f'replay-{fixture}.json').write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding='utf-8')
                print(json.dumps({'fixture': fixture, 'stage': result['lastTrace'][1],
                                  'windows': result['scan']['windows'], 'completedTicks': result['scan']['completedTicks'],
                                  'pure': not result['scan']['stoppedForPurity'], 'unchangedTrace': result['unchanged'],
                                  'errors': errors}, ensure_ascii=False), flush=True)
                context.close()
                assert not errors
                assert result['unchanged'], 'Presentation sweep consumed or changed Replay state'
                assert all(r['valid'] for r in result['scan']['reports']), 'Incomplete Replay sample'
                assert not result['scan']['stoppedForPurity'], 'Authoritative state changed; inspect the captured first failure'
            (OUT / 'replay-verification.json').write_text(json.dumps({
                'passed': True, 'browser': browser.version, 'fixtures': len(results),
                'windows': sum(r['scan']['windows'] for r in results),
                'stages': sorted({r['lastTrace'][1] for r in results}),
                'files': [f'replay-{i}.json' for i in range(len(results))],
                'scope': 'named Replay windows only; not the entire campaign',
            }, ensure_ascii=False, indent=2), encoding='utf-8')
        finally:
            browser.close()


if __name__ == '__main__':
    main()
