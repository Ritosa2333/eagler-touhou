"""Compare identical real input timelines with/without diagnostic redraws.

One page/context at a time. This is named-field/trace evidence, not a full-state
determinism proof. It uses the real runtime and does not synthesize game objects.
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
    parser.add_argument('--ticks', type=int, default=1200)
    args = parser.parse_args()
    assert 60 <= args.ticks <= 7200
    OUT.mkdir(parents=True, exist_ok=True)
    lanes = {}
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        try:
            for lane in ['observer-off', 'observer-on', 'redraws']:
                context = browser.new_context(viewport={'width': 1200, 'height': 1000}, service_workers='block')
                page = context.new_page()
                page.set_default_timeout(120000)
                errors = []
                page.on('pageerror', lambda error: errors.append(str(error)))
                page.goto(args.url, wait_until='load')
                page.wait_for_function('window.presentationLab !== undefined')
                page.locator('#boot').click()
                page.wait_for_function('!!window.presentationLab?.controller')
                result = page.evaluate('''async ({lane,ticks}) => {
                  const c=presentationLab.controller, trace=[], findings=[], pure=[];
                  if(lane==='observer-off')c.core.audit_enable(0);
                  for(let i=0;i<5;i++)c.press('KeyZ');
                  c.clearKeys();
                  for(let n=0;n<ticks;n++){
                    // Shot, movement, bomb, and a real pause/resume lifecycle.
                    const phase=n%240;
                    c.key('KeyZ',true);
                    c.key('ArrowRight',phase<45);
                    c.key('ArrowLeft',phase>=120&&phase<165);
                    c.key('KeyX',n===420);
                    c.key('Escape',n===600||n===670);
                    c.step(1);
                    if(n%20===19){
                      if(lane==='redraws'){
                        const r=c.sweep({label:'input-tick-'+(n+1)});
                        pure.push({tick:n+1,ok:r.purity,changes:r.stateChanges});
                        findings.push({tick:n+1,counts:r.counts,groups:r.issueGroups});
                      }
                      trace.push({tick:n+1,trace:c.trace(),state:c.state(),status:c.runtime.status()});
                      await new Promise(resolve=>setTimeout(resolve,0));
                    }
                  }
                  c.clearKeys();return {trace,pure,findings,build:presentationLab.identity};
                }''', {'lane': lane, 'ticks': args.ticks})
                result['errors'] = errors
                lanes[lane] = result
                context.close()
                print(json.dumps({'lane': lane, 'samples': len(result['trace']),
                                  'impureWindows': sum(not row['ok'] for row in result['pure']),
                                  'errors': errors}, ensure_ascii=False), flush=True)
            reference = lanes['observer-off']['trace']
            divergence = []
            for lane in ['observer-on', 'redraws']:
                for a, b in zip(reference, lanes[lane]['trace']):
                    for field in ['trace', 'state', 'status']:
                        if a[field] != b[field]:
                            divergence.append({'lane': lane, 'tick': a['tick'], 'field': field,
                                               'indices': [i for i, (x, y) in enumerate(zip(a[field], b[field])) if x != y]})
                            break
                    if divergence and divergence[-1]['lane'] == lane:
                        break
            report = {'schema': 'th08/presentation-noninterference/1', 'browser': browser.version,
                      'inputTicksPerLane': args.ticks, 'checkpointStride': 20,
                      'evidence': '68-word game trace and nine named state hashes at checkpoints',
                      'divergence': divergence, 'lanes': lanes}
            report['passed'] = not divergence and not any(lane['errors'] for lane in lanes.values())
            (OUT / 'noninterference.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
            print(json.dumps({'passed': report['passed'], 'divergence': divergence}, ensure_ascii=False), flush=True)
            assert report['passed'], 'Diagnostic observation/redraw changed named game state; inspect first divergence'
        finally:
            browser.close()


if __name__ == '__main__':
    main()
