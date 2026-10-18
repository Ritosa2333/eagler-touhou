"""Capture every active Replay tick from the current diagnostic Browser Runtime."""

import argparse
import json
from pathlib import Path
from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_OUT = ROOT / 'artifacts/replay-verifier/current-demo'


BOOTSTRAP = r'''async fixture => {
  const response=await fetch(`/input/demo${fixture}.rpy`);
  if(!response.ok)throw Error('Missing named Replay fixture');
  const replayBytes=Array.from(new Uint8Array(await response.arrayBuffer()));
  await presentationLab.boot({replayBytes});
  const controller=presentationLab.controller;
  controller.freeze();
  const core=controller.core,runtime=controller.runtime,state={previous:null,rows:[],tail:[],applicationTicks:0,stopOnTitle:false,done:false};
  const capture=()=>{
    const pointer=core.trace(runtime.app),u32=new Uint32Array(core.memory.buffer,pointer,68),f32=new Float32Array(core.memory.buffer,pointer,68);
    const diagnostics=new Int32Array(core.memory.buffer,core.diagnostics(runtime.app),16);
    // A scene transition can break the calculation chain while the browser
    // shell still performs one draw/present. The original Present oracle has
    // no such half-transaction, so publish only stable-owner logic ticks.
    if(diagnostics[0]!==2||diagnostics[1]!==2||(u32[4]&12)!==12||!u32[0])return;
    const identity=`${u32[1]}:${u32[0]}`;if(identity===state.previous)return;state.previous=identity;
    state.rows.push({frame:u32[0],stage:u32[1],gameFrames:u32[2],controlFrames:u32[3],flags:u32[4],rng:u32[5],rngCalls:u32[6],
      player:[f32[7],f32[8]],playerState:u32[9],keys:u32[10],numbers:Array.from(new Uint8Array(core.memory.buffer,pointer+44,228))});
  };
  const tick=()=>{
    const receipt=controller.driver.advanceOneTick();state.applicationTicks++;
    const trace=Array.from(new Uint32Array(core.memory.buffer,core.trace(runtime.app),11));
    const diagnostics=Array.from(new Int32Array(core.memory.buffer,core.diagnostics(runtime.app),16));
    state.tail.push({applicationTick:state.applicationTicks,receipt,scene:core.status(runtime.app,0),trace,diagnostics});
    if(state.tail.length>32)state.tail.shift();
    if(receipt.status==='advanced')capture();else state.done=true;
    if(state.stopOnTitle&&core.status(runtime.app,0)===1)state.done=true;
    return receipt;
  };
  const step=count=>{for(let i=0;i<count&&!state.done;i++)tick();};
  const press=(code,wait=40)=>{controller.key(code,true);step(3);controller.key(code,false);step(wait);};
  for(let i=0;i<12&&core.status(runtime.app,9)!==4;i++)press('ArrowDown');
  if(core.status(runtime.app,9)!==4)throw Error('Replay menu not reached');
  const navigation=[];
  for(let i=0;i<5&&core.status(runtime.app,0)!==2;i++){press('KeyZ');navigation.push([core.status(runtime.app,0),core.status(runtime.app,8),core.status(runtime.app,9)]);}
  if(core.status(runtime.app,0)!==2||!(new Uint32Array(core.memory.buffer,core.trace(runtime.app),5)[4]&8))
    throw Error('Real Replay playback did not start: '+JSON.stringify(navigation));
  state.stopOnTitle=true;
  controller.clearKeys();
  window.replayVerifierCapture={
    run(count){state.rows=[];step(count);if(core.status(runtime.app,0)===1)state.done=true;return {rows:state.rows,done:state.done,scene:core.status(runtime.app,0),tail:state.tail};},
    navigation,
  };
  const rows=state.rows;state.rows=[];
  return {rows,navigation,build:presentationLab.identity};
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
                rows = initial['rows']
                done = False
                tail = []
                while not done and len(rows) < args.max_ticks:
                    batch = page.evaluate('count => replayVerifierCapture.run(count)', 300)
                    rows.extend(batch['rows'])
                    done = batch['done']
                    tail = batch['tail']
                    if len(rows) and len(rows) % 3000 < len(batch['rows']):
                        print(json.dumps({'fixture': fixture, 'captured': len(rows), 'lastFrame': rows[-1]['frame']}), flush=True)
                result = {
                    'schema': 'th08/current-replay-capture/v1', 'fixture': fixture,
                    'complete': done, 'errors': errors, 'navigation': initial['navigation'],
                    'build': initial['build'], 'tail': tail, 'rows': rows,
                }
                path = args.output / f'demo{fixture}.json'
                path.write_text(json.dumps(result, separators=(',', ':')), encoding='utf-8')
                suite.append({'fixture': fixture, 'complete': done, 'ticks': len(rows), 'errors': errors, 'file': path.name})
                print(json.dumps(suite[-1]), flush=True)
                context.close()
                if errors or not done:
                    raise RuntimeError(f'Demo {fixture} did not complete cleanly')
        finally:
            browser.close()
    (args.output / 'results.json').write_text(json.dumps({'schema': 'th08/current-replay-capture-suite/v1', 'passed': True, 'fixtures': suite}, indent=2), encoding='utf-8')


if __name__ == '__main__':
    main()
