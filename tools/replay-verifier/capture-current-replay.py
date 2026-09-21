"""Capture every stable authoritative tick from one ordinary TH08 Replay."""

import argparse
import hashlib
import json
from pathlib import Path

from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[2]


def inspect_replay(raw, start_stage):
    if len(raw) < 104:
        raise ValueError('Invalid TH08 replay size')
    plain = bytearray(raw)
    file_size = int.from_bytes(plain[12:16], 'little')
    if not 104 <= file_size <= len(plain):
        raise ValueError('Invalid TH08 replay file size')
    key = plain[21]
    for index in range(24, file_size):
        plain[index] = (plain[index] - key) & 0xff
        key = (key + 7) & 0xff
    output = bytearray(104 + int.from_bytes(plain[28:32], 'little'))
    output[:104] = plain[:104]
    source = memoryview(plain)[104:file_size]
    dictionary = bytearray(8192)
    cursor = 0
    mask = 0x80
    current = 0
    head = 1
    written = 104

    def bit():
        nonlocal cursor, mask, current
        if mask == 0x80:
            current = source[cursor] if cursor < len(source) else 0
            cursor += 1
        value = 1 if current & mask else 0
        mask >>= 1
        if not mask:
            mask = 0x80
        return value

    def bits(count):
        value = 0
        for _ in range(count):
            value = (value << 1) | bit()
        return value

    def put(value):
        nonlocal head, written
        if written >= len(output):
            raise ValueError('TH08 replay payload exceeds declared size')
        output[written] = value
        written += 1
        dictionary[head] = value
        head = (head + 1) & 8191

    while True:
        if bit():
            put(bits(8))
        else:
            offset = bits(13)
            if not offset:
                break
            for index in range(bits(4) + 3):
                put(dictionary[(offset + index) & 8191])
    stages = []
    for stage in range(start_stage, 9):
        offset = int.from_bytes(output[32 + stage * 4:36 + stage * 4], 'little')
        if offset:
            stages.append({'stage': stage, 'score': int.from_bytes(output[offset:offset + 4], 'little')})
    if not stages:
        raise ValueError('Replay has no selected stage')
    return {'stages': stages, 'finalScore': stages[-1]['score']}

BOOTSTRAP = r'''async ({bytes,startStage}) => {
  await presentationLab.boot({replayBytes:bytes});
  const controller=presentationLab.controller,core=controller.core,runtime=controller.runtime;
  controller.freeze();
  const state={previous:null,rows:[],tail:[],applicationTicks:0,done:false,started:false,stages:[],finalScore:0};
  const capture=()=>{
    const pointer=core.trace(runtime.app),u32=new Uint32Array(core.memory.buffer,pointer,68),f32=new Float32Array(core.memory.buffer,pointer,68);
    const diagnostics=new Int32Array(core.memory.buffer,core.diagnostics(runtime.app),16);
    if(diagnostics[0]!==2||diagnostics[1]!==2||(u32[4]&12)!==12||!u32[0])return;
    const identity=`${u32[1]}:${u32[0]}`;if(identity===state.previous)return;state.previous=identity;
    if(!state.stages.includes(u32[1]))state.stages.push(u32[1]);
    state.rows.push({frame:u32[0],stage:u32[1],gameFrames:u32[2],controlFrames:u32[3],flags:u32[4],rng:u32[5],rngCalls:u32[6],
      player:[f32[7],f32[8]],playerState:u32[9],keys:u32[10],numbers:Array.from(new Uint8Array(core.memory.buffer,pointer+44,228))});
  };
  const tick=()=>{
    const receipt=controller.driver.advanceOneTick();state.applicationTicks++;
    const traceView=new Uint32Array(core.memory.buffer,core.trace(runtime.app),68),scene=core.status(runtime.app,0);
    if(scene===2)state.finalScore=traceView[13];const trace=Array.from(traceView.slice(0,14));
    const diagnostics=Array.from(new Int32Array(core.memory.buffer,core.diagnostics(runtime.app),16));
    state.tail.push({applicationTick:state.applicationTicks,receipt,scene,trace,diagnostics});if(state.tail.length>32)state.tail.shift();
    if(receipt.status==='advanced')capture();else state.done=true;
    if(state.started&&scene===1)state.done=true;
  };
  const step=count=>{for(let i=0;i<count&&!state.done;i++)tick();};
  const press=(code,wait=40)=>{controller.key(code,true);step(3);controller.key(code,false);step(wait);};
  for(let i=0;i<12&&core.status(runtime.app,9)!==4;i++)press('ArrowDown');
  if(core.status(runtime.app,9)!==4)throw Error('Replay menu not reached');
  press('KeyZ');press('KeyZ');
  for(let i=0;i<10&&core.status(runtime.app,9)!==startStage;i++)press('ArrowDown');
  if(core.status(runtime.app,9)!==startStage)throw Error('Requested Replay stage is absent');
  const navigation=[];
  for(let i=0;i<4&&core.status(runtime.app,0)!==2;i++){press('KeyZ');navigation.push([core.status(runtime.app,0),core.status(runtime.app,8),core.status(runtime.app,9)]);}
  if(core.status(runtime.app,0)!==2||!(new Uint32Array(core.memory.buffer,core.trace(runtime.app),5)[4]&8))
    throw Error('Real Replay playback did not start: '+JSON.stringify(navigation));
  state.started=true;controller.clearKeys();capture();
  window.replayVerifierCapture={run(count){state.rows=[];step(count);return {rows:state.rows,done:state.done,applicationTicks:state.applicationTicks,stages:state.stages,tail:state.tail,finalScore:state.finalScore};}};
  const rows=state.rows;state.rows=[];
  return {rows,navigation,build:presentationLab.identity};
}'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--replay', type=Path, required=True)
    parser.add_argument('--start-stage', type=int, default=0)
    parser.add_argument('--url', default='http://127.0.0.1:8132/')
    parser.add_argument('--max-ticks', type=int, default=200_000)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    if not 0 <= args.start_stage <= 8 or not 300 <= args.max_ticks <= 500_000:
        raise ValueError('Invalid stage or tick bound')
    replay = args.replay.resolve().read_bytes()
    expected = inspect_replay(replay, args.start_stage)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    rows_path = args.output.with_suffix('.rows.jsonl')
    errors = []
    result = None
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        context = browser.new_context(viewport={'width': 1280, 'height': 960}, service_workers='block')
        page = context.new_page(); page.set_default_timeout(180_000)
        page.on('pageerror', lambda error: errors.append(str(error)))
        try:
            page.goto(args.url, wait_until='load')
            page.wait_for_function('window.presentationLab !== undefined')
            initial = page.evaluate(BOOTSTRAP, {'bytes': list(replay), 'startStage': args.start_stage})
            ticks = 0; application_ticks = 0; done = False; tail = []; stages = []; final_score = None
            with rows_path.open('w', encoding='utf-8', buffering=1) as rows:
                for row in initial['rows']:
                    rows.write(json.dumps(row, separators=(',', ':')) + '\n'); ticks += 1
                while not done and application_ticks < args.max_ticks:
                    batch = page.evaluate('count => replayVerifierCapture.run(count)', min(600, args.max_ticks - application_ticks))
                    for row in batch['rows']:
                        rows.write(json.dumps(row, separators=(',', ':')) + '\n'); ticks += 1
                    application_ticks, done, tail, stages = batch['applicationTicks'], batch['done'], batch['tail'], batch['stages']
                    final_score = batch['finalScore']
                    if ticks and ticks % 6000 < len(batch['rows']):
                        print(json.dumps({'ticks': ticks, 'stages': stages, 'last': batch['rows'][-1]}), flush=True)
            complete = done and not errors and stages == [item['stage'] for item in expected['stages']] and final_score == expected['finalScore']
            result = {'schema': 'th08/current-replay-tick-capture/v1', 'complete': complete,
                      'provider': 'th08-eagler/diagnostic-browser', 'replay': {'path': str(args.replay.resolve()), 'bytes': len(replay), 'sha256': hashlib.sha256(replay).hexdigest()},
                      'build': initial['build'], 'rows': {'path': str(rows_path.resolve()), 'ticks': ticks},
                      'applicationTicks': application_ticks, 'navigation': initial['navigation'], 'expected': expected,
                      'stages': stages, 'finalScore': final_score, 'tail': tail, 'errors': errors}
        except Exception as error:
            result = {'schema': 'th08/current-replay-tick-capture/v1', 'complete': False,
                      'replay': {'path': str(args.replay.resolve()), 'sha256': hashlib.sha256(replay).hexdigest()},
                      'rows': {'path': str(rows_path.resolve())}, 'errors': [*errors, repr(error)]}
            raise
        finally:
            args.output.write_text(json.dumps(result, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
            context.close(); browser.close()
    if not result['complete']:
        raise RuntimeError(f'Incomplete TH08 candidate capture; evidence: {args.output}')
    print(json.dumps({'complete': True, 'ticks': result['rows']['ticks'], 'stages': result['stages'], 'output': str(args.output)}), flush=True)


if __name__ == '__main__':
    main()
