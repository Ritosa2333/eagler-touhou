"""Capture every stable authoritative tick from one ordinary TH10 Replay."""

import argparse
import hashlib
import json
from pathlib import Path

from playwright.sync_api import sync_playwright


BOOTSTRAP = r'''async bytes => {
  await presentationLab.boot({replayBytes:bytes});
  const controller=presentationLab.controller,core=controller.core,runtime=controller.runtime;
  controller.freeze();
  const state={previous:null,rows:[],tail:[],applicationTicks:0,done:false,started:false,stages:[]};
  const read=()=>{const pointer=core.replay_verifier_trace(runtime.app);if(!pointer||pointer%4)throw Error('Invalid trace pointer');const words=Array.from(new Uint32Array(core.memory.buffer,pointer,24)),floats=new Float32Array(core.memory.buffer,pointer,24);return {words,player:[floats[18],floats[19]]};};
  const capture=()=>{const {words:w,player}=read();if(w[14]!==1||w[23]!==1||w[22]!==1)return false;const identity=`${w[15]}:${w[16]}`;if(identity===state.previous)return true;state.previous=identity;
    if(!state.stages.includes(w[15]))state.stages.push(w[15]);state.rows.push({frame:w[16],stage:w[15],stageFrames:w[4],sectionFrames:w[5],flags:w[6],score:w[7],power:w[8]|0,itemValue:w[9],lives:w[10]|0,rank:w[11]|0,rng:w[12],rngCalls:w[13],keys:w[17],player,playerState:w[20],sessionFlags:w[21]});return true;};
  const tick=()=>{const result=core.presentation_lab_tick(runtime.app);state.applicationTicks++;const active=capture(),{words:w}=read();state.tail.push({applicationTick:state.applicationTicks,result,screen:w[1],pending:w[2],stage:w[3],replayMode:w[14],activeStage:w[15],replayFrame:w[16],session:w[22],stable:w[23]});if(state.tail.length>32)state.tail.shift();if(result!==0)state.done=true;if(state.started&&!active&&w[14]!==1)state.done=true;};
  const step=count=>{for(let i=0;i<count&&!state.done;i++)tick();};
  const press=(code,wait=42)=>{controller.key(code,true);step(3);controller.key(code,false);step(wait);};
  step(450);press('KeyZ');press('ArrowDown');press('ArrowDown');press('KeyZ');press('KeyZ');press('KeyZ');step(300);
  const first=read().words;if(first[14]!==1||first[22]!==1)throw Error('Real Replay playback did not start: '+JSON.stringify(first));
  state.started=true;controller.clearKeys();capture();
  window.replayVerifierCapture={run(count){state.rows=[];step(count);return {rows:state.rows,done:state.done,applicationTicks:state.applicationTicks,stages:state.stages,tail:state.tail};}};
  const rows=state.rows;state.rows=[];return {rows,build:presentationLab.identity,initial:first};
}'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--replay', type=Path, required=True)
    parser.add_argument('--url', default='http://127.0.0.1:8134/')
    parser.add_argument('--max-ticks', type=int, default=200_000)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if not 300 <= args.max_ticks <= 500_000:
        raise ValueError('Invalid tick bound')
    replay = args.replay.resolve().read_bytes(); args.output.parent.mkdir(parents=True, exist_ok=True)
    rows_path = args.output.with_suffix('.rows.jsonl'); errors = []; result = None
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        context = browser.new_context(viewport={'width': 1280, 'height': 960}, service_workers='block')
        page = context.new_page(); page.set_default_timeout(180_000); page.on('pageerror', lambda error: errors.append(str(error)))
        try:
            page.goto(args.url, wait_until='load'); page.wait_for_function('window.presentationLab !== undefined')
            initial = page.evaluate(BOOTSTRAP, list(replay)); ticks = 0; application_ticks = 0; done = False; tail = []; stages = []
            with rows_path.open('w', encoding='utf-8', buffering=1) as rows:
                for row in initial['rows']:
                    rows.write(json.dumps(row, separators=(',', ':')) + '\n'); ticks += 1
                while not done and application_ticks < args.max_ticks:
                    batch = page.evaluate('count => replayVerifierCapture.run(count)', min(600, args.max_ticks - application_ticks))
                    for row in batch['rows']:
                        rows.write(json.dumps(row, separators=(',', ':')) + '\n'); ticks += 1
                    application_ticks, done, tail, stages = batch['applicationTicks'], batch['done'], batch['tail'], batch['stages']
                    if ticks and ticks % 6000 < len(batch['rows']): print(json.dumps({'ticks': ticks, 'stages': stages, 'last': batch['rows'][-1]}), flush=True)
            result = {'schema': 'th10/current-replay-tick-capture/v1', 'complete': done and not errors,
                      'provider': 'th10-eagler/diagnostic-browser', 'replay': {'path': str(args.replay.resolve()), 'bytes': len(replay), 'sha256': hashlib.sha256(replay).hexdigest()},
                      'build': initial['build'], 'rows': {'path': str(rows_path.resolve()), 'ticks': ticks},
                      'applicationTicks': application_ticks, 'initial': initial['initial'], 'stages': stages, 'tail': tail, 'errors': errors}
        except Exception as error:
            result = {'schema': 'th10/current-replay-tick-capture/v1', 'complete': False,
                      'replay': {'path': str(args.replay.resolve()), 'sha256': hashlib.sha256(replay).hexdigest()},
                      'rows': {'path': str(rows_path.resolve())}, 'errors': [*errors, repr(error)]}
            raise
        finally:
            args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
            context.close(); browser.close()
    if not result['complete']: raise RuntimeError(f'Incomplete TH10 candidate capture; evidence: {args.output}')
    print(json.dumps({'complete': True, 'ticks': result['rows']['ticks'], 'stages': result['stages'], 'output': str(args.output)}), flush=True)


if __name__ == '__main__':
    main()
