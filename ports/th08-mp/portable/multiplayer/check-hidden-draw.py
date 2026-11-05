"""TH08 real native frame loop at synthetic 120 Hz, full/hidden draw control.
No physical refresh, RTC or phone-performance claim.
"""
import argparse,json,sys
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--baseline',action='store_true')
args=parser.parse_args()
source=Path(__file__).with_name('smoke.mjs').read_text(encoding='utf-8')
source=source.replace('sessionId?3:1','sessionId?5:1').replace('const count=sessionId?17:11;','if(sessionId){while(words.length<17)words.push(0);words.push(0,8,1);}const count=sessionId?20:11;')
source+='''
let pendingFrame=null;
function withRafCapture(fn){const old=window.requestAnimationFrame;window.requestAnimationFrame=cb=>{pendingFrame=cb;return 1;};try{return fn();}finally{window.requestAnimationFrame=old;}}
multiplayerSmoke.rafInit=full=>{core.mp_fixture_full_hidden_draw(+full);withRafCapture(()=>core.sdl_loop_start());};
multiplayerSmoke.raf=now=>withRafCapture(()=>{const cb=pendingFrame;pendingFrame=null;if(!cb)throw Error('Native RAF stopped');cb(now);});
multiplayerSmoke.rafStop=()=>core.sdl_loop_stop();
'''
report={'passed':False,'scope':__doc__}
with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        context=browser.new_context()
        context.route('**/smoke.mjs',lambda route:route.fulfill(status=200,content_type='text/javascript',body=source))
        page=context.new_page();page.goto(args.url)
        page.wait_for_function('window.multiplayerSmoke!==undefined',timeout=120000)
        report['result']=page.evaluate('''async()=>{
          const worlds=[];
          for(let i=0;i<4;i++){
            const el=document.createElement('iframe');el.src='/';document.body.append(el);
            while(!el.contentWindow.multiplayerSmoke)await new Promise(r=>setTimeout(r,20));
            const w=el.contentWindow.multiplayerSmoke;await w.start(1234,[0,1],i%2,[0x31313131,0x42424242]);worlds.push(w);
          }
          for(const phase of [1,2]){
            const packets=worlds.map(w=>w.sessionPacket(phase));
            for(let i=0;i<4;i++)if(worlds[i].applyWire(packets[i^1]))throw Error('Handshake rejected');
            if(phase===1)for(const w of worlds)if(!w.markReady())throw Error('Ready rejected');
          }
          worlds.forEach((w,i)=>w.rafInit(i<2||CONTROL));
          const checkpoints=[];let captured=-1;
          for(let sample=0;sample<1800;sample++){
            const f=worlds[0].netStatus()[3];
            if(f>captured){
              for(let i=0;i<4;i++)if(!worlds[i].captureInput(f,f<180?0:1|((Math.floor(f/90)+i)%2?64:128),f===0&&i%2===0?2:0,f===0&&i%2===0?8188:0,f===0&&i%2===0?-8192:0,0))throw Error('Capture rejected '+f);
              for(let i=0;i<4;i++)if(worlds[i].applyWire(worlds[i^1].inputPacket(i%2,f,f+1)))throw Error('Input rejected');
              captured=f;
            }
            for(const w of worlds)w.raf(sample*1000/120);
            if(worlds.some(w=>w.netStatus()[3]!==worlds[0].netStatus()[3]))throw Error('Cadence diverged');
            for(let seat=0;seat<2;seat++){
              if(JSON.stringify(worlds[seat].canonical())!==JSON.stringify(worlds[seat+2].canonical()))throw Error('Canonical divergence '+sample+' frame '+f+' seat '+seat);
              if(JSON.stringify(worlds[seat].rngState())!==JSON.stringify(worlds[seat+2].rngState()))throw Error('RNG divergence '+sample);
            }
            if(sample%120===119){checkpoints.push({sample,frame:worlds[0].netStatus()[3],state:worlds[0].fixtureStatus()[0]});await new Promise(r=>setTimeout(r,0));}
          }
          worlds.forEach(w=>w.rafStop());
          if(checkpoints.at(-1).frame<800||!checkpoints.some(row=>row.state===1))throw Error('Gameplay coverage incomplete');
          return {checkpoints,identity:worlds[0].identity(),canonical:worlds[0].canonical()};
        }'''.replace('CONTROL','true' if args.baseline else 'false'))
        report['passed']=True
    finally:
        browser.close()
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
print('TH08 120 Hz full/hidden draw canonical and RNG control: PASS')
