"""Compare independent native worlds after all input through a boundary is confirmed.

The same-origin parent only drives existing diagnostic/transport APIs. Each
iframe owns a separate native application, memory, local seat and input history.
No state is copied between worlds and no mismatch is repaired by this harness.
"""
import argparse
import json
import uuid
from pathlib import Path
from playwright.sync_api import sync_playwright

parser = argparse.ArgumentParser()
parser.add_argument('--url', required=True)
parser.add_argument('--frames', type=int, default=7200)
parser.add_argument('--delay', type=int, default=4)
parser.add_argument('--interval', type=int, default=60)
parser.add_argument('--players', type=int, choices=(2, 3), default=2)
parser.add_argument('--presentations', type=int, choices=(0, 1, 2), default=0)
parser.add_argument('--trace-draw', action='store_true')
parser.add_argument('--asymmetric', action='store_true')
parser.add_argument('--guest-config', choices=('none', 'slow', 'lives', 'effects', 'visual', 'frameskip', 'shot-slow'), default='none')
parser.add_argument('--route-history', choices=('none', 'a', 'b'), default='none')
parser.add_argument('--relay', help='Use the native connection over this real Relay instead of manually delivering packets')
parser.add_argument('--transport', choices=('rtc', 'relay'), default='rtc')
parser.add_argument('--analog', action='store_true')
parser.add_argument('--collision-stress', action='store_true')
parser.add_argument('--poc-collector', type=int, choices=(0, 1, 2))
parser.add_argument('--poc-crossing', type=int, choices=(0, 1),
                    help='Move this seat across POC using delayed netplay input after frame 180')
parser.add_argument('--ordinary-death', type=int, choices=(0, 1),
                    help='Start this seat in a first ordinary death before delayed rollback traffic')
parser.add_argument('--output', type=Path, default=Path('artifacts/multiplayer-tests/desync.json'))
args = parser.parse_args()
args.output.parent.mkdir(parents=True, exist_ok=True)
report = {'passed': False, 'frames': args.frames, 'delay': args.delay, 'players': args.players,
          'presentations': args.presentations, 'asymmetric': args.asymmetric,
          'guestConfig': args.guest_config, 'transport': args.transport if args.relay else 'packets',
          'analog': args.analog, 'ordinaryDeath': args.ordinary_death}

with sync_playwright() as p:
    browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
    try:
        context = browser.new_context(service_workers='block')
        if args.relay and args.transport == 'relay':
            context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true})")
        page = context.new_page()
        # Load the actual smoke origin before creating child worlds. A route-
        # fulfilled synthetic top document may receive an opaque origin in
        # Chromium, which makes otherwise same-URL iframes inaccessible.
        page.goto(args.url)
        page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=90000)
        page.on('console', lambda msg: print(msg.text, flush=True) if msg.text.startswith('DESYNC ') else None)
        result = page.evaluate("""async o => {
          const wait = ms => new Promise(r => setTimeout(r, ms));
          const worlds = [];
          const pocSeat=o.pocCrossing!==null?o.pocCrossing:o.pocCollector;
          const pocLoadouts=pocSeat===0?[1,0]:pocSeat===1?[0,1]:pocSeat===2?[1,1]:null;
          for (let seat=0; seat<o.players; ++seat) {
            const frame=document.createElement('iframe'); frame.width=640; frame.height=480;
            frame.src=o.url; document.body.append(frame);
            const deadline=performance.now()+90000;
            while (!frame.contentWindow?.multiplayerSmoke) {
              if(performance.now()>deadline) throw Error('Module boot timeout at seat '+seat);
              await wait(20);
            }
            const w=frame.contentWindow.multiplayerSmoke;
            const guestConfig={slow:{slowMode:1},lives:{lives:6},effects:{effects:0},
              visual:{options:2|4|64|256|1024},frameskip:{frameskip:2},'shot-slow':{shotSlow:1}}[o.guestConfig];
            await w.start(1234, pocLoadouts||Array.from({length:o.players},(_,i)=>i), seat, [0x12345678,0x10203040],seat?guestConfig:null);
            if(o.routeHistory!=='none'){
              const hostRoute=o.routeHistory==='b'?2:1;
              const localRoute=seat===0?hostRoute:(hostRoute===2?0:2);
              if(!w.fixtureRouteHistory(localRoute))throw Error('Route history fixture rejected seat '+seat);
            }
            worlds.push(w); console.log('DESYNC boot seat '+seat);
          }
          const result={identities:worlds.map(w=>w.identity()), checkpoints:[]};
          const apply=(target,wire)=>{if(worlds[target].applyWire(wire)!==0)throw Error('Wire rejected');};
          if(o.relay){
            for(const w of worlds)if(!w.connect(o.relay+'/?room='+o.room))throw Error('Native connection rejected');
            const deadline=performance.now()+60000;
            while(!worlds.every(w=>w.netStatus()[2])){
              for(const w of worlds)w.pollNetwork();
              if(performance.now()>deadline)throw Error('Real transport handshake timeout');
              await wait(2);
            }
            const expected=o.transport==='rtc'?1:2;
            if(!worlds.every(w=>w.driverStatus()[12]===expected))throw Error('Unexpected real transport route');
            result.transport=o.transport;
          }else for(const phase of [1,2]) {
            const packets=worlds.map(w=>w.sessionPacket(phase));
            for(let i=0;i<worlds.length;i++) for(let j=0;j<worlds.length;j++) if(i!==j)apply(i,packets[j]);
            if(phase===1) for(const w of worlds) if(!w.markReady())throw Error('READY rejected');
          }
          const input=(seat,f)=>{
            if(f<180)return 0;
            if(o.pocCrossing!==null)return seat===o.pocCrossing&&f<205?16:0;
            if(o.ordinaryDeath!==null&&seat===o.ordinaryDeath&&f===180)return 8192;
            if(o.collisionStress)return f%180<120?1:0;
            return 1|((Math.floor(f/90)+seat)%2?64:128)|(f%120<60?4:0)|([620,850,1250,1500,2050].includes(f)?2:0);
          };
          const capture=(seat,f)=>{
            const buttons=input(seat,f);
            if(f===0&&seat===0){
              const route=o.routeHistory==='b'?2:o.routeHistory==='a'?1:0;
              return worlds[seat].captureInput(f,buttons,2,8188+route,-8192,0);
            }
            if(!o.analog||f<180)return worlds[seat].capture(f,buttons);
            const x=(Math.floor(f/90)+seat)%2?1.375:-1.625;
            return worlds[seat].captureInput(f,buttons&~240,2,x,0,2|(buttons&2?4:0));
          };
          let queue=[],pocReady=o.pocCollector===null&&o.pocCrossing===null,deathReady=o.ordinaryDeath===null;
          const owners=['economy','players','rng','enemies','bullets','lasers','items','effects','lifecycle','assets','spell'];
          for(let f=0;f<o.frames;f++) {
            if(o.relay){
              const deadline=performance.now()+30000, sampled=new Set();
              while(!worlds.every(w=>w.netStatus()[3]>f)){
                for(let seat=0;seat<worlds.length;seat++){
                  const w=worlds[seat];w.pollNetwork();
                  if(w.netStatus()[3]===f){
                    if(!sampled.has(seat)){if(!capture(seat,f))throw Error('Capture rejected');sampled.add(seat);}
                    w.ticks(1);
                  }
                }
                if(performance.now()>deadline)throw Error('Real transport stalled at '+f);
                await wait(1);
              }
            }else{
            for(let seat=0;seat<worlds.length;seat++) if(!capture(seat,f))throw Error('Capture rejected '+f+' seat '+seat);
            for(let source=0;source<worlds.length;source++)for(let target=0;target<worlds.length;target++)if(source!==target)
              queue.push({due:f+(f>=180&&(!o.asymmetric||target!==0)?o.delay:0),target,wire:worlds[source].inputPacket(target,f,f+1)});
            const due=queue.filter(p=>p.due<=f);queue=queue.filter(p=>p.due>f);
            for(const p of due.reverse())apply(p.target,p.wire);
            for(const w of worlds)w.ticks(1);
            }
            if(!pocReady&&f<180&&worlds.every(w=>w.fixtureStatus()[0]===1)){
              if(o.pocCrossing!==null){
                if(f===179){
                  for(const w of worlds)if(!w.fixturePocCrossingSetup(o.pocCrossing))throw Error('POC crossing fixture setup rejected');
                  pocReady=true;console.log('DESYNC POC crossing setup collector='+o.pocCrossing+' frame='+f);
                }
              }else{
                for(const w of worlds)if(!w.fixturePocSetup(o.pocCollector))throw Error('POC fixture setup rejected');
                pocReady=true;console.log('DESYNC POC setup collector='+o.pocCollector+' frame='+f);
              }
            }
            if(!deathReady&&f===179){
              for(const w of worlds)if(!w.fixtureOrdinaryDeathSetup(o.ordinaryDeath))throw Error('Ordinary death fixture setup rejected');
              deathReady=true;console.log('DESYNC ordinary death setup seat='+o.ordinaryDeath+' frame='+f);
            }
            if(f>=180)for(let i=0;i<o.presentations;i++){
              const alpha=(i+1)/(o.presentations+1);
              if(o.traceDraw){
                const mutation=worlds.at(-1).enemyDrawPurity(alpha);
                if(mutation[0]!==1){result.firstDivergence=f;result.drawMutation=mutation;result.passed=false;return result;}
              }else worlds.at(-1).presented(alpha);
            }
            if((f+1)%o.interval===0||f===o.frames-1) {
              if(o.relay){
                const deadline=performance.now()+30000;
                do{
                  for(const w of worlds){w.pollNetwork();w.reconcile();}
                  if(worlds.every(w=>w.netStatus()[5]>=f&&w.netStatus()[6]===0xffffffff))break;
                  if(performance.now()>deadline)throw Error('Confirmation timeout at '+f);
                  await wait(1);
                }while(true);
              }
              for(const p of queue)apply(p.target,p.wire);queue=[];
              for(const w of worlds)w.reconcile();
              const states=worlds.map(w=>({net:w.netStatus(),canonical:w.canonical(),state:w.status(),driver:w.driverStatus()}));
              for(const s of states) if(s.net[4]!==f||s.net[5]<f||s.net[6]!==0xffffffff)
                throw Error('Unconfirmed comparison at '+f+': '+JSON.stringify(s.net));
              const different=owners.filter((_,i)=>states.some(s=>s.canonical[i+2]!==states[0].canonical[i+2]));
              if(o.routeHistory!=='none'){
                const expected=o.routeHistory==='b'?2:1;
                if(states.some(s=>s.state[43]!==expected))
                  throw Error('P1 route bootstrap mismatch at '+f+': '+states.map(s=>s.state[43]).join(','));
              }
              if(o.pocCollector!==null&&pocReady&&f>=60){
                const powers=states.map(s=>[s.state[10],s.state[22]]);
                const expected=o.pocCollector===0?[8,0]:o.pocCollector===1?[0,8]:[8,0];
                if(powers.some(value=>value[0]!==expected[0]||value[1]!==expected[1]))
                  throw Error('POC power mismatch at '+f+': '+JSON.stringify(powers)+' expected '+JSON.stringify(expected));
              }
              if(o.pocCrossing!==null&&pocReady&&f>=239){
                const powers=states.map(s=>[s.state[10],s.state[22]]);
                if(powers.some(value=>value[0]!==powers[0][0]||value[1]!==powers[0][1]))
                  throw Error('POC crossing endpoint power mismatch at '+f+': '+JSON.stringify(powers));
                if(powers[0][o.pocCrossing]===0)
                  throw Error('POC crossing did not collect Power by '+f+': '+JSON.stringify(powers));
              }
              if(o.ordinaryDeath!==null&&deathReady&&f>=269){
                const offset=8+o.ordinaryDeath*12;
                const values=states.map(s=>({lives:s.state[offset+1],power:s.state[offset+2],life:s.state[offset+4]}));
                if(values.some(value=>value.lives!==values[0].lives||value.power!==values[0].power||value.life!==values[0].life))
                  throw Error('Ordinary death endpoint state mismatch at '+f+': '+JSON.stringify(values));
                // Native TH08 removes 16 Power on the ordinary death, but the
                // same pilot may legitimately collect later field/death drops.
                // The invariant for this long-running rollback probe is the
                // shared spare-life respawn and endpoint agreement, not a
                // permanently frozen post-death Power value.
                if(values[0].lives!==1||values[0].power<64||values[0].life===4)
                  throw Error('Ordinary death did not complete native spare-life respawn by '+f+': '+JSON.stringify(values));
                result.ordinaryDeathState=values;
              }
              result.checkpoints.push({frame:f,different,states});
              if(o.collisionStress){
                const life=states[0].state.slice(8,44);
                result.lastCollisionState=life;
                if(life.some((value,index)=>index%12===4&&value!==3&&value!==0))
                  console.log('DESYNC collision-state '+f+' '+JSON.stringify(life));
              }
              if(different.length){
                result.firstDivergence=f;result.passed=false;console.log('DESYNC first observed '+f+' '+different.join(','));
                if(different.includes('enemies')&&worlds[0].identity().fixtureBuild){
                  result.enemyDiffs=[];
                  for(let index=0;index<=481;index++){
                    const a=worlds[0].enemyCanonical(index),b=worlds.at(-1).enemyCanonical(index);
                    if(JSON.stringify(a)===JSON.stringify(b))continue;
                    const diffs=[];for(let k=0;k<Math.max(a.bytes.length,b.bytes.length);k++)if(a.bytes[k]!==b.bytes[k])diffs.push([k,a.bytes[k],b.bytes[k]]);
                    result.enemyDiffs.push({index,headers:[a.header,b.header],diffs});
                    if(result.enemyDiffs.length>=8)break;
                  }
                }
                return result;
              }
              if((f+1)%600===0)console.log('DESYNC confirmed '+(f+1)+' rollback='+states.map(s=>s.driver[3]).join(','));
              await wait(0);
            }
          }
          result.passed=true;return result;
        }""", {'url': args.url, 'players': args.players, 'frames': args.frames,
                 'delay': args.delay, 'interval': args.interval, 'presentations': args.presentations,
                 'traceDraw': args.trace_draw, 'asymmetric': args.asymmetric, 'guestConfig': args.guest_config,
                 'relay': args.relay, 'transport': args.transport, 'room': 'desync-'+uuid.uuid4().hex[:14], 'analog': args.analog,
                 'routeHistory': args.route_history, 'collisionStress': args.collision_stress,
                 'pocCollector': args.poc_collector, 'pocCrossing': args.poc_crossing,
                 'ordinaryDeath': args.ordinary_death})
        report.update(result)
    except BaseException as error:
        report['error'] = str(error)
        raise
    finally:
        args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
        browser.close()
        print(json.dumps({'passed': report['passed'], 'firstDivergence': report.get('firstDivergence'),
                          'output': str(args.output)}), flush=True)
if not report['passed']:
    raise SystemExit(1)
