"""Death/item causal measurement, not a cross-device acceptance certificate.

No forced death button, no periodic packet flush, no world-state copying.
Read revisioned native records only through the common confirmed frontier.
Always persist observations, including native failures and incomplete coverage.
"""
import argparse
import json
import os
from pathlib import Path
import subprocess
import time
from urllib.request import urlopen
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[2]
GLOBAL_FIELDS = ['time', 'totalTime', 'timeRequirement', 'pointValue', 'clock', 'score',
                 'points', 'graze', 'rngSeed', 'rngBackup', 'rngCalls', 'stage',
                 'gameFlags', 'spellFlags', 'pendingTime', 'pauseState', 'paused',
                 'retrying', 'enemyFrames', 'bulletCount', 'bulletTimer',
                 'bulletCancelFrames', 'bulletUnknownCounter', 'effectCursor',
                 'effectCount', 'effectFrames', 'supervisorActive', 'supervisorTarget',
                 'playFrames', 'menuSupervisorState']
PILOT_FIELDS = ['power', 'lives', 'bombs', 'gauge', 'deaths', 'life', 'predead', 'lifeTimer',
                'clearFrames', 'contextPower', 'contextTime', 'contextBombs', 'contextGauge',
                'gameFlags', 'focus', 'bombActive', 'bombTimer', 'shootTimer', 'xBits', 'yBits',
                'buttons', 'spirit', 'cancelItem', 'gaugeLock']
FIELDS = GLOBAL_FIELDS + [f'P{s+1}.{key}' for s in range(2) for key in PILOT_FIELDS] + [
    'itemCursor', 'itemCount', 'itemHead', 'itemTail']
ITEM_FIELDS = ['slot', 'active', 'type', 'state', 'age', 'owner', 'recipient', 'xBits',
               'yBits', 'vxBits', 'vyBits', 'targetXBits', 'targetYBits', 'next', 'previous', 'maxValue']

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--victim', type=int, choices=(0, 1), required=True)
parser.add_argument('--scenario', choices=('poc', 'field'), default='poc')
parser.add_argument('--bombs', type=int, choices=(0, 1, 2, 3), default=3)
parser.add_argument('--frames', type=int, default=480)
parser.add_argument('--delay', type=int, default=3)
parser.add_argument('--jitter', type=int, default=3)
parser.add_argument('--presentations', type=int, choices=(0, 1, 2), default=1)
parser.add_argument('--loadouts', default='0,1')
parser.add_argument('--browser', choices=('chromium', 'firefox', 'webkit'), default='chromium')
parser.add_argument('--movement', choices=('keyboard', 'direct-touch', 'fresh-touch'), default='keyboard')
parser.add_argument('--port', type=int, default=8152)
parser.add_argument('--data', type=Path, default=ROOT/'artifacts/multiplayer-tests/runtime-data/games/th08/th08.data')
parser.add_argument('--baseline', type=Path, help='Same-scenario zero-delay .frames.jsonl reference')
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()
if not 300 <= args.frames <= 900 or not 0 <= args.delay <= 16 or not 0 <= args.jitter <= 16:
    parser.error('frames must be 300..900 and delay/jitter 0..16')
loadouts = [int(n) for n in args.loadouts.split(',')]
if len(loadouts) != 2 or any(not 0 <= n < 12 for n in loadouts):
    parser.error('exactly two TH08 loadouts (0..11) required')
args.output.parent.mkdir(parents=True, exist_ok=True)
trace_path = args.output.with_suffix('.frames.jsonl')
report = {'schema': 'th08mp/death-measurement/1', 'passed': False,
          'scope': 'two native worlds; deterministic packet fault injection; not real-device acceptance',
          'config': vars(args) | {'data': str(args.data), 'output': str(args.output),
                                  'baseline': str(args.baseline) if args.baseline else None},
          'valueFields': FIELDS, 'itemFields': ITEM_FIELDS, 'traceFile': str(trace_path)}
baseline = {}
if args.baseline:
    for line in args.baseline.read_text(encoding='utf-8').splitlines():
        row = json.loads(line)
        baseline[row['frame']] = row['worlds'][0]['record']['attempt']['end']
server = None
browser = None
page = None
server_log = args.output.with_suffix('.server.log').open('w', encoding='utf-8')
trace = trace_path.open('w', encoding='utf-8')

def save_frame(row):
    trace.write(json.dumps(row, separators=(',', ':')) + '\n')
    trace.flush()
    reference = baseline.get(row['frame'])
    if reference is not None:
        for index, value in enumerate(row['worlds']):
            if value['record']['attempt']['end'] != reference:
                return {'frame': row['frame'], 'world': index, 'expected': reference,
                        'actual': value['record']['attempt']['end']}
    return None

try:
    env = os.environ | {'TH08_MP_FIXTURES': '1', 'TH08_MP_DATA': str(args.data.resolve()),
                        'PORT': str(args.port)}
    server = subprocess.Popen(['node', 'portable/multiplayer/serve.mjs'], cwd=ROOT, env=env,
                              stdout=server_log, stderr=subprocess.STDOUT)
    url = f'http://127.0.0.1:{args.port}/'
    deadline = time.monotonic() + 15
    while True:
        if server.poll() is not None:
            raise RuntimeError('measurement server failed; see ' + server_log.name)
        try:
            with urlopen(url + 'build.json', timeout=1) as response:
                report['servedIdentity'] = json.load(response)
            break
        except OSError:
            if time.monotonic() >= deadline:
                raise RuntimeError('measurement server startup timeout')
            time.sleep(.1)
    with sync_playwright() as p:
        browser = getattr(p, args.browser).launch(headless=True, args=['--enable-unsafe-swiftshader'] if args.browser == 'chromium' else [])
        page = browser.new_page(service_workers='block')
        page.expose_function('storeMeasurementFrame', save_frame)
        page.on('console', lambda msg: print(msg.text, flush=True) if msg.text.startswith('MEASURE ') else None)
        page.goto(url)
        page.wait_for_function('window.multiplayerSmoke !== undefined', timeout=90000)
        options = {'url': url, 'victim': args.victim, 'scenario': 0 if args.scenario == 'poc' else 1,
                   'bombs': args.bombs, 'frames': args.frames, 'delay': args.delay, 'jitter': args.jitter,
                   'presentations': args.presentations, 'loadouts': loadouts, 'fields': FIELDS,
                   'movement': args.movement,'pilotOffset':len(GLOBAL_FIELDS),'pilotWidth':len(PILOT_FIELDS)}
        result = page.evaluate('''async o => {
          const result=window.__deathMeasurement={passed:false,inputs:[],packets:[],coverage:{},checkpoints:[],firstDivergence:null};
          const worlds=[],wait=ms=>new Promise(r=>setTimeout(r,ms));let queue=[],checked=179;
          const sample=(f,buttons)=>f===0?{buttons,mode:2,x:8188,y:-8192,flags:0}:
            o.movement==='fresh-touch'&&f>=180?{buttons:buttons&~240,mode:f===180?4:3,x:0,y:buttons&16?-2.2:0,flags:2}:
            o.movement==='direct-touch'&&f>=180?{buttons:buttons&~240,mode:2,x:0,y:buttons&16?-2.2:0,flags:2}:
            {buttons,mode:0,x:0,y:0,flags:0};
          const capture=(w,f,buttons,seat)=>{const s=sample(f,buttons);
            if(f===0&&seat!==0)return w.capture(f,buttons);
            return w.captureInput(f,s.buttons,s.mode,s.x,s.y,s.flags);};
          const inputs=(seat,f)=>{
            if(f<180)return 0;
            const n=f-180;
            // Both sides change movement/focus while items are already homing.
            // The victim actually moves into the pre-existing bullet at y-32.
            if(seat===o.victim)return n<24?20:n<100?0:1|(n%40<20?4:0);
            return n<10?0:n<42?20:n<100?4:1|(n%48<24?4:0);
          };
          const apply=p=>{if(worlds[p.target].applyWire(p.wire)!==0)throw Error('Wire rejected');p.delivered=true;};
          const inspect=async()=>{
            const net=worlds.map(w=>w.netStatus());
            if(net.some(n=>n[6]!==0xffffffff))return;
            const frontier=Math.min(...net.map(n=>Math.min(n[4],n[5])));
            while(checked<frontier){
              const f=++checked,row={frame:f,frontier,worlds:worlds.map(w=>w.traceRead(f,true))};
              if(row.worlds.some(t=>!t?.record?.attempt?.complete))throw Error('Missing completed trace '+f);
              if(row.worlds.some(t=>t.traceOverflow))throw Error('Trace overflow; incomplete evidence');
              const oracle=await window.storeMeasurementFrame(row);
              const ends=row.worlds.map(t=>t.record.attempt.end),a=ends[0],b=ends[1];
              if(ends.some(x=>x.values.length!==o.fields.length))throw Error('Native trace field layout changed');
              const valueDiff=a.values.flatMap((v,i)=>v===b.values[i]?[]:[{field:o.fields[i],values:[v,b.values[i]]}]);
              for(let i=0;i<2;i++){
                const record=row.worlds[i].record;
                for(const event of record.attempt.events){
                  if(!event.after)result.coverage[event.kind]=(result.coverage[event.kind]||0)+1;
                  if(event.kind==='time.add'&&event.argument<0&&!event.after)result.coverage.timePenalty=(result.coverage.timePenalty||0)+1;
                }
                if(record.firstAttempt&&record.firstAttempt.end.values[o.pilotOffset+o.victim*o.pilotWidth+5]!==record.attempt.end.values[o.pilotOffset+o.victim*o.pilotWidth+5])
                  result.coverage.correctedDeathState=(result.coverage.correctedDeathState||0)+1;
              }
              if(valueDiff.length||JSON.stringify(a.items)!==JSON.stringify(b.items)||oracle){
                result.firstDivergence=f;result.valueDifferences=valueDiff;result.firstDifference=row;
                result.oracleDifference=oracle;result.failure='confirmed-state-divergence';
                console.log('MEASURE first confirmed divergence '+f+' '+valueDiff.map(x=>x.field).join(','));return false;
              }
              if(row.worlds.some(t=>t.restoreMismatch!==null)){
                result.failure='rollback-restore-mismatch';result.firstDifference=row;
                console.log('MEASURE bad restore '+row.worlds.map(t=>t.restoreMismatch));return false;
              }
              if(f%30===29)result.checkpoints.push({frame:f,values:a.values,items:a.items.length,
                rollback:worlds.map(w=>w.driverStatus()[3]),restoreChecks:row.worlds.map(t=>t.restoreChecks)});
            }
            return true;
          };
          try{
            for(let seat=0;seat<2;seat++){
              const element=document.createElement('iframe');element.width=640;element.height=480;element.src=o.url;document.body.append(element);
              const deadline=performance.now()+90000;
              while(!element.contentWindow?.multiplayerSmoke){if(performance.now()>deadline)throw Error('World boot timeout');await wait(20);}
              const w=element.contentWindow.multiplayerSmoke;
              await w.start(1234,o.loadouts,seat,[0x23232323,0x56565656]);worlds.push(w);
            }
            result.identities=worlds.map(w=>w.identity());
            if(result.identities[0].wasmSha256!==result.identities[1].wasmSha256)throw Error('Build identity mismatch');
            for(const phase of [1,2]){
              const packets=worlds.map(w=>w.sessionPacket(phase));
              apply({target:0,wire:packets[1]});apply({target:1,wire:packets[0]});
              if(phase===1)for(const w of worlds)if(!w.markReady())throw Error('READY rejected');
            }
            for(let f=0;f<o.frames;f++){
              const buttons=worlds.map((_,seat)=>inputs(seat,f));result.inputs.push({frame:f,buttons,samples:buttons.map((b,seat)=>f===0&&seat!==0?{buttons:b,mode:0,x:0,y:0,flags:0}:sample(f,b))});
              for(let seat=0;seat<2;seat++)if(!capture(worlds[seat],f,buttons[seat],seat))throw Error('Capture rejected '+f);
              for(let source=0;source<2;source++){
                const target=1-source,latency=f<180?0:o.delay+(o.jitter?((f*5+Math.floor(f/3)*2+source*3)%(o.jitter+1)):0);
                const packet={source,target,frame:f,due:f+latency,wire:worlds[source].inputPacket(target,f,f+1)};
                queue.push(packet);result.packets.push(packet);
              }
              const due=queue.filter(p=>p.due<=f);queue=queue.filter(p=>p.due>f);
              for(const packet of due.reverse())apply(packet);
              // The order alternates, but captures and network deliveries are
              // recorded independently. No periodic drain or state repair.
              for(const seat of f%2?[1,0]:[0,1]){
                worlds[seat].ticks(1);
                if(worlds[seat].netStatus()[3]!==f+1)throw Error('Prediction window stalled at '+f+' seat '+seat);
              }
              if(f===179){
                if(worlds.some(w=>w.netStatus()[5]!==179))throw Error('Unconfirmed fixture boundary');
                for(const w of worlds){w.deathField(o.victim,o.bombs,o.scenario);w.traceEnable(180);}
                console.log('MEASURE armed victim='+o.victim+' bombs='+o.bombs+' nonzero time; real movement collision');
              }
              if(f>=180){
                for(let i=0;i<o.presentations;i++)worlds[1].presented((i+1)/(o.presentations+1));
                if(await inspect()===false)return result;
              }
              if(f%60===59){console.log('MEASURE frame='+f+' compared='+checked);await wait(0);}
            }
            // Stop sampling first; allow the remaining packets to arrive at
            // their recorded delivery times. This is final settlement only.
            for(let t=o.frames;queue.length&&t<o.frames+64;t++){
              const due=queue.filter(p=>p.due<=t);queue=queue.filter(p=>p.due>t);for(const packet of due.reverse())apply(packet);
              for(const w of worlds)w.reconcile();if(await inspect()===false)return result;
            }
            if(checked!==o.frames-1)throw Error('Incomplete confirmation frontier');
            result.final=worlds.map(w=>({net:w.netStatus(),driver:w.driverStatus(),state:w.status(),canonical:w.canonical(),trace:w.traceRead(checked)}));
            const values=result.final[0].trace.record.attempt.end.values,offset=o.pilotOffset+o.victim*o.pilotWidth;
            result.coverage.oneOrdinaryDeath=values[offset+1]===1&&values[offset+4]===1&&values[offset+21]===0;
            result.passed=!!(result.coverage.oneOrdinaryDeath&&result.coverage['item.collect']&&result.coverage.timePenalty&&
                (o.delay===0&&o.jitter===0||result.coverage.correctedDeathState));
            if(!result.passed)result.failure='coverage-incomplete';
            return result;
          }catch(error){result.error=String(error.stack||error);result.failure='harness-or-native-error';return result;}
        }''', options)
        report.update(result)
        browser.close()
        browser = None
except BaseException as error:
    report['error'] = str(error)
finally:
    if browser:
        browser.close()
    if server and server.poll() is None:
        server.terminate()
        try:
            server.wait(timeout=5)
        except subprocess.TimeoutExpired:
            server.kill()
            server.wait()
    trace.close()
    server_log.close()
    args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps({key: report.get(key) for key in ['passed', 'failure', 'firstDivergence', 'coverage', 'error']}), flush=True)
if not report['passed']:
    raise SystemExit(1)
