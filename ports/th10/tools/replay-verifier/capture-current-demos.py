"""Capture TH10's title-owned Demo rotation from a diagnostic Browser Runtime.

The game selects and advances its own Demo index.  This driver only advances
complete 1/60 logic transactions and reads the diagnostic trace afterwards.
"""

import argparse
import json
from pathlib import Path

from playwright.sync_api import sync_playwright


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_OUT = ROOT / "artifacts/replay-verifier/current-demo"


BOOTSTRAP = r"""() => {
  const controller=presentationLab.controller;
  controller.freeze();
  const runtime=controller.runtime,core=controller.core;
  const state={rows:[],demos:[],completed:0,active:null,applicationTicks:0,done:false,tail:[]};
  const read=()=>{
    const pointer=core.replay_verifier_trace(runtime.app);
    if(!pointer||pointer%4)throw Error('Invalid Replay verifier trace pointer');
    const words=Array.from(new Uint32Array(core.memory.buffer,pointer,24));
    const floats=new Float32Array(core.memory.buffer,pointer,24);
    return {words,player:[floats[18],floats[19]]};
  };
  const finish=reason=>{
    if(!state.active)return;
    state.demos.push({rotationIndex:state.completed++,reason,rows:state.rows});
    state.active=null;state.rows=[];
    if(state.completed===4)state.done=true;
  };
  const sample=()=>{
    const {words:w,player}=read();
    const replay=w[14]===1,stable=w[23]===1,session=w[22]===1;
    if(replay&&stable&&session){
      const identity=`${w[15]}:${w[16]}`;
      if(state.active===null)state.active={last:null};
      if(identity!==state.active.last){
        state.active.last=identity;
        state.rows.push({frame:w[16],stage:w[15],stageFrames:w[4],sectionFrames:w[5],flags:w[6],score:w[7],power:w[8]|0,itemValue:w[9],lives:w[10]|0,rank:w[11]|0,rng:w[12],rngCalls:w[13],keys:w[17],player,playerState:w[20],sessionFlags:w[21]});
      }
    }else if(state.active!==null&&state.rows.length)finish('returned-from-demo');
    return w;
  };
  const tick=()=>{
    const result=core.presentation_lab_tick(runtime.app);state.applicationTicks++;
    const words=sample();
    state.tail.push({applicationTick:state.applicationTicks,result,screen:words[1],pending:words[2],stage:words[3],replayMode:words[14],activeStage:words[15],replayFrame:words[16],session:words[22],stable:words[23]});
    if(state.tail.length>32)state.tail.shift();
    if(result!==0&&!state.done)throw Error('Diagnostic logic tick failed: '+result);
  };
  window.replayVerifierCapture={
    run(count){for(let i=0;i<count&&!state.done;i++)tick();const demos=state.demos.splice(0);return {demos,done:state.done,applicationTicks:state.applicationTicks,tail:state.tail};},
  };
  return {build:presentationLab.identity,initial:read().words};
}"""


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--url", default="http://127.0.0.1:8134/")
    parser.add_argument("--max-ticks", type=int, default=100_000)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUT)
    args = parser.parse_args()
    if not 10_000 <= args.max_ticks <= 500_000:
        raise ValueError("Invalid tick bound")
    args.output.mkdir(parents=True, exist_ok=True)

    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True, args=["--enable-unsafe-swiftshader"])
        context = browser.new_context(viewport={"width": 1280, "height": 960}, service_workers="block")
        page = context.new_page()
        page.set_default_timeout(120_000)
        errors = []
        page.on("pageerror", lambda error: errors.append(str(error)))
        try:
            page.goto(args.url, wait_until="load")
            page.wait_for_function("window.presentationLab !== undefined")
            page.evaluate("() => presentationLab.boot()")
            initial = page.evaluate(BOOTSTRAP)
            demos = []
            application_ticks = 0
            tail = []
            done = False
            while not done and application_ticks < args.max_ticks:
                batch = page.evaluate("count => replayVerifierCapture.run(count)", 300)
                application_ticks = batch["applicationTicks"]
                tail = batch["tail"]
                for demo in batch["demos"]:
                    demos.append(demo)
                    print(json.dumps({"rotationIndex": demo["rotationIndex"], "ticks": len(demo["rows"]), "last": demo["rows"][-1] if demo["rows"] else None}), flush=True)
                done = batch["done"]
            result = {
                "schema": "th10/current-title-demo-capture/v1",
                "complete": done and len(demos) == 4 and not errors,
                "build": initial["build"],
                "initial": initial["initial"],
                "applicationTicks": application_ticks,
                "errors": errors,
                "tail": tail,
                "demos": demos,
            }
            path = args.output / "suite.json"
            path.write_text(json.dumps(result, separators=(",", ":")), encoding="utf-8")
            if not result["complete"]:
                raise RuntimeError(f"TH10 Demo capture incomplete; evidence: {path}")
        finally:
            context.close()
            browser.close()


if __name__ == "__main__":
    main()
