"""Freeze TH08 assets and measure serial RTC/rAF pairs in separate browsers.

CPU throttling is a laboratory load, not a phone model. RTC impairment delays
application sends (including repair), not wire RTT. Exact controls wait for
remote input: compare their CPU per logical tick, not their lockstep FPS.
"""
import argparse, functools, hashlib, http.server, json, math, os, shutil
import socket, subprocess, sys, threading, time, uuid
from pathlib import Path
from playwright.sync_api import sync_playwright

ROOT=Path(__file__).resolve().parents[2]
WORKSPACE=ROOT.parents[1]
MODES={'frontier':0,'frontier-late':0,'frontier-legacy':0,
       'frontier-target-legacy':0,'frontier-collision-legacy':0,'frontier-both-legacy':0,
       'frontier-cache-legacy':0,'always':1,'exact':2,'exact-snapshots':3}

def digest(path):return hashlib.sha256(path.read_bytes()).hexdigest()
def distribution(values):
    values=sorted(values)
    if not values:return {'count':0}
    return {'count':len(values),'mean':sum(values)/len(values),'p95':values[math.ceil(len(values)*.95)-1],
            'p99':values[math.ceil(len(values)*.99)-1],'max':values[-1],'over50':sum(v>50 for v in values)}
def summarize(peer):
    rows=peer['rows'];first=peer['before'];last=rows[-1]
    # Keep scene retirement/menu lockstep separate from gameplay throughput.
    # Preserve the whole-run result too: transitions remain user-visible.
    scenes=[];start=previous_scene=None
    for row in rows:
        if start is None:start=row
        if previous_scene is not None and row.get('scene')!=start.get('scene'):
            duration=previous_scene['time']-start['time']
            scenes.append({'scene':start.get('scene'),'firstFrame':start['net'][3],
              'lastFrame':previous_scene['net'][3],'elapsedMs':duration,
              'logicalFps':(previous_scene['net'][3]-start['net'][3])*1000/duration if duration else None})
            start=row
        previous_scene=row
    if start is not None:
        duration=last['time']-start['time']
        scenes.append({'scene':start.get('scene'),'firstFrame':start['net'][3],
          'lastFrame':last['net'][3],'elapsedMs':duration,
          'logicalFps':(last['net'][3]-start['net'][3])*1000/duration if duration else None})
    gaps=[];advances=[];last_present=last_advance=None;previous=None
    for row in rows:
        if previous:
            if row['graphics'][5]!=previous['graphics'][5]:
                if last_present is not None:gaps.append(row['time']-last_present)
                last_present=row['time']
            if row['net'][3]!=previous['net'][3]:
                if last_advance is not None:advances.append(row['time']-last_advance)
                last_advance=row['time']
        previous=row
    frames=last['net'][3]-first['net'][3];elapsed=last['time']-first['time']
    profile=[a-b for a,b in zip(last['profile'],first['profile'])]
    chain=[a-b for a,b in zip(last['chain'],first['chain'])]
    bullet=[a-b for a,b in zip(last['bullet'],first['bullet'])]
    bullet_update=[a-b for a,b in zip(last['bulletUpdate'],first['bulletUpdate'])]
    collision=[a-b for a,b in zip(last['collision'],first['collision'])]
    imp=peer['impairment'];sent=imp['sent'];start_imp=first['impairment']
    measured_sent=sent-start_imp['sent']
    result={'logicalFps':frames*1000/elapsed,'frames':frames,'elapsedMs':elapsed,'sceneSpans':scenes,
      'presentGapMs':distribution(gaps),'advanceGapMs':distribution(advances),
      'callbackMs':distribution([x['ms'] for x in rows]),'receiveAgeMs':distribution([x['receiveAge'] for x in rows]),
      'corrections':last['driver'][3]-first['driver'][3],'resimulated':last['driver'][4]-first['driver'][4],
      'captureMs':profile[4],'restoreMs':profile[5],'updateMs':profile[6],'resimDrawMs':profile[7],
      'chainMs':chain,
      'bulletMs':bullet,
      'bulletUpdateSampleMs':bullet_update,
      'collisionChecks':collision,
      'snapshots':profile[2],'skipped':profile[3],'snapshotBytes':profile[8],
      'correctionMs':last['costs'][2]-first['costs'][2],'maxCorrectionMs':last['costs'][3],
      'callbackMsPerLogicalTick':sum(x['ms'] for x in rows[1:])/frames,
      'updateMsPerExecutedTick':profile[6]/(frames+last['driver'][4]-first['driver'][4]),
      'activeBullets':last['profile'][9],'peakBullets':max(x['profile'][9] for x in rows),
      'heapGrowthBytes':last['heap']-first['heap'],'canonical':peer['canonical'],
      'confirmedThrough':peer['after']['net'][5],
      'deliveredMeanMs':(imp['deliveredDelayTotalMs']-start_imp['deliveredDelayTotalMs'])/measured_sent if measured_sent else None,
      'deliveredMaxMs':imp['maxDeliveredDelayMs'],'maxTimerOverrunMs':imp['maxTimerOverrunMs']}
    result['profileAvailable']=peer.get('profileAvailable',True)
    if not result['profileAvailable']:
        for key in ('captureMs','restoreMs','updateMs','resimDrawMs','chainMs','bulletMs',
                    'bulletUpdateSampleMs','collisionChecks','snapshots','skipped','snapshotBytes',
                    'correctionMs','maxCorrectionMs','updateMsPerExecutedTick','activeBullets','peakBullets'):
            result[key]=None
    return result

def freeze(args):
    dest=args.output/'frozen';dest.mkdir(parents=True,exist_ok=False)
    profile='multiplayer' if args.production else 'multiplayer-fixtures'
    build=ROOT/'th08_web/artifacts'/profile
    info=json.loads((build/'build.json').read_text())
    if bool(info['diagnostic'])==args.production or info['variant']!=profile or digest(build/'th08-sdl.wasm')!=info['sha256']:raise ValueError('Wrong measurement build')
    if digest(build/'th08-sdl.mjs')!=info['loaderSha256']:raise ValueError('Stale fixture loader')
    for name,expected in info['sourceFiles'].items():
        if digest(ROOT/name)!=expected:raise ValueError('Stale fixture source '+name)
    source={ROOT/'portable/multiplayer/smoke.html':'index.html',ROOT/'portable/multiplayer/smoke.mjs':'smoke-source.mjs',
      ROOT/'portable/multiplayer/performance-hook.mjs':'performance-hook.mjs',Path(__file__):'measure-smoothness.py',
      build/'th08-sdl.mjs':'th08-sdl.mjs',build/'th08-sdl.wasm':'th08-sdl.wasm',build/'build.json':'runtime-build.json',
      WORKSPACE/'eagler-common/testkit/rtc-input-impairment.cjs':'rtc-input-impairment.cjs',
      args.data:'input/th08.dat',WORKSPACE/'th06-eagler/assets/msgothic.ttc':'fonts/msgothic.ttc',
      WORKSPACE/'th08-eagler/build-eagler/fonts/blend.bin':'fonts/blend.bin',
      WORKSPACE/'th08-eagler/build-eagler/fonts/cp932.bin':'fonts/cp932.bin'}
    launcher=WORKSPACE/'eagler-touhou'
    for name in ['server/netplay-relay.mjs','server/room-probe-policy.mjs',
                 'server/spectator-frame.mjs','lib/contracts/product-catalog.mjs',
                 'lib/contracts/load-compiled-contract.mjs']:
        source[launcher/name]='relay/'+name
    compiled=launcher/'.cache/build/browser/assets/contracts'
    if not compiled.exists():compiled=launcher/'assets/contracts'
    for path in compiled.rglob('*.mjs'):source[path]='relay/assets/contracts/'+path.relative_to(compiled).as_posix()
    for path in (launcher/'node_modules/ws').rglob('*'):
        if path.is_file():source[path]='relay/node_modules/ws/'+path.relative_to(launcher/'node_modules/ws').as_posix()
    for origin,name in source.items():
        target=dest/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(origin,target)
    (dest/'smoke.mjs').write_bytes((dest/'smoke-source.mjs').read_bytes()+b'\n'+(dest/'performance-hook.mjs').read_bytes())
    (dest/'build.json').write_text(json.dumps({'game':'th08','variant':profile,'wasm':info['sha256']}))
    manifest={p.relative_to(dest).as_posix():digest(p) for p in dest.rglob('*') if p.is_file()}
    (args.output/'manifest.json').write_text(json.dumps(manifest,indent=2))
    return dest,manifest

def verify(dest,manifest):
    for name,expected in manifest.items():
        if digest(dest/name)!=expected:raise ValueError('Frozen experiment changed: '+name)

class QuietHandler(http.server.SimpleHTTPRequestHandler):
    def log_message(self,*args):pass

def run(args,origin,relay,dest,mode):
    report={'passed':False,'mode':mode,'production':args.production,'cpuRate':args.cpu_rate,'players':args.players,
      'denseBullets':args.bullets,'difficulty':args.difficulty,'limit60':args.cap60,'wallClockTouch':args.wall_clock_touch,'frames':args.frames,'oneWayMs':args.one_way_ms,'jitterMs':args.jitter_ms,
      'scope':'separate Chromium processes; native rAF; RTC application-send impairment; CPU throttle is not a phone model',
      'peers':[],'errors':[]}
    with sync_playwright() as pw:
        browsers=[];pages=[]
        try:
            room='smooth-'+uuid.uuid4().hex[:12]
            for seat in range(args.players):
                browser=pw.chromium.launch(headless=True,channel=None if args.browser_channel=='default' else args.browser_channel,args=['--disable-background-timer-throttling',
                  '--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding','--enable-unsafe-swiftshader'])
                browsers.append(browser)
                if seat==0:
                    gpu=browser.new_browser_cdp_session().send('SystemInfo.getInfo')['gpu']
                    report['browser']={'version':browser.version,'gpu':gpu}
                context=browser.new_context(viewport={'width':800,'height':600},service_workers='block')
                options={'oneWayMs':args.one_way_ms,'jitterMs':args.jitter_ms,'inputLabel':'th08-input','controlLabel':'th08-control','seed':7135+seat}
                context.add_init_script((dest/'rtc-input-impairment.cjs').read_text()+'\nglobalThis.__th08Impairment=installRtcInputImpairment('+json.dumps(options)+');')
                page=context.new_page();pages.append(page)
                page.on('pageerror',lambda e,seat=seat:report['errors'].append({'seat':seat,'error':str(e)}))
                page.goto(origin);page.wait_for_function('window.th08Performance!==undefined',timeout=90000)
                page.evaluate('(o)=>multiplayerSmoke.start(1234,Array.from({length:o.players},(_,i)=>i),o.seat,[12345,67890],null,o.difficulty)',{'seat':seat,'players':args.players,'difficulty':args.difficulty})
            for page in pages:
                if not page.evaluate('(url)=>multiplayerSmoke.connect(url)',relay+'/?room='+room):raise AssertionError('connect')
            deadline=time.monotonic()+60
            while True:
                states=[page.evaluate('()=>{multiplayerSmoke.pollNetwork();return multiplayerSmoke.netStatus()}') for page in pages]
                if all(s[2] for s in states):break
                if time.monotonic()>deadline:raise TimeoutError('RTC barrier')
                time.sleep(.05)
            if not all(page.evaluate('multiplayerSmoke.driverStatus()[12]===1') for page in pages):raise AssertionError('Not RTC')
            def wait_phase():
                deadline=time.monotonic()+args.timeout
                while True:
                    states=[page.evaluate('th08Performance.status()') for page in pages]
                    if report['errors'] or any(s['issues'] for s in states):raise AssertionError({'states':states,'errors':report['errors']})
                    if all(s['done'] for s in states):break
                    if time.monotonic()>deadline:raise TimeoutError(json.dumps(states))
                    time.sleep(.5)
                for page in pages:page.evaluate('th08Performance.stop()')
            for seat,page in enumerate(pages):page.evaluate('(seat)=>th08Performance.start(180,999999,seat)',seat)
            wait_phase()
            if len({json.dumps(p.evaluate('multiplayerSmoke.canonical()')) for p in pages})!=1:raise AssertionError('Warmup divergence')
            for seat,page in enumerate(pages):
                page.evaluate('(o)=>th08Performance.configure(o.mode,o.bullets,o.cap60,o.wallTouch,o.earlyInput,o.liveBullets,o.targetFilter,o.broadphase,o.cache)',{'mode':MODES[mode],'bullets':args.bullets,'cap60':args.cap60,'wallTouch':args.wall_clock_touch,'earlyInput':mode!='frontier-late','liveBullets':mode!='frontier-legacy','targetFilter':mode not in ('frontier-target-legacy','frontier-both-legacy'),'broadphase':mode not in ('frontier-collision-legacy','frontier-both-legacy'),'cache':mode not in ('frontier-cache-legacy','frontier-both-legacy')})
                if seat==args.players-1:
                    context=page.context;session=context.new_cdp_session(page)
                    session.send('Emulation.setCPUThrottlingRate',{'rate':args.cpu_rate})
            # Arm against the same wall deadline, instead of starting P1 while
            # synchronous CDP setup is still blocking the other processes.
            start_at=time.time()*1000+600
            for seat,page in enumerate(pages):page.evaluate('(o)=>setTimeout(()=>th08Performance.start(o.frames,240,o.seat),Math.max(0,o.at-Date.now()))',{'at':start_at,'frames':args.frames,'seat':seat})
            # The old warmup done flag stays true until the armed start fires.
            time.sleep(.8);wait_phase()
            report['peers']=[page.evaluate('th08Performance.report()') for page in pages]
            for peer in report['peers']:
                imp=peer['impairment']
                if not imp['matched'] or not imp['sent'] or imp['errors'] or imp['overflow']:raise AssertionError('RTC impairment not applied cleanly')
                if not peer['rows'] or peer['after']['net'][3]!=args.frames:raise AssertionError('Incomplete run')
                if peer['limit60']!=args.cap60:raise AssertionError('Presentation limit not applied')
                if peer['wallClockTouch']!=args.wall_clock_touch:raise AssertionError('Touch workload not applied')
                if peer['earlyInput']!=(mode!='frontier-late'):raise AssertionError('Input send order not applied')
                if peer['liveBullets']!=(mode!='frontier-legacy'):raise AssertionError('Bullet backend not applied')
                if peer['targetFilter']!=(mode not in ('frontier-target-legacy','frontier-both-legacy')):raise AssertionError('Target filter not applied')
                if peer['broadphase']!=(mode not in ('frontier-collision-legacy','frontier-both-legacy')):raise AssertionError('Collision broadphase not applied')
                if peer['barrierCache']!=(mode not in ('frontier-cache-legacy','frontier-both-legacy')):raise AssertionError('Barrier cache mode not applied')
                if peer['identity']['fixtureBuild']==args.production or peer['profileAvailable']==args.production:raise AssertionError('Wrong production/fixture lane')
                if args.wall_clock_touch and (peer['touch']['events']<30 or peer['touch']['pathPixels']<10 or peer['touch']['maxX']-peer['touch']['minX']<1):raise AssertionError('Native touch did not move the player')
                if peer['after']['driver'][2] or peer['after']['driver'][15]:raise AssertionError('Driver/channel failure')
                if mode in ('exact','exact-snapshots') and peer['after']['driver'][3]:raise AssertionError('Exact control rolled back')
            if len({json.dumps(p['canonical']) for p in report['peers']})!=1:raise AssertionError('Canonical divergence')
            report['summary']=[summarize(p) for p in report['peers']]
            if mode not in ('exact','exact-snapshots') and not all(p['corrections']>0 for p in report['summary']):raise AssertionError('No measured rollback')
            report['passed']=True
        except Exception as error:
            report['error']=str(error)
            if not report['peers']:
                for page in pages:
                    try:report['peers'].append(page.evaluate('th08Performance.report()'))
                    except Exception:pass
        finally:
            for browser in reversed(browsers):browser.close()
    return report

def supervised_run(args,origin,relay,dest,mode,name):
    """Bound even a stuck CDP evaluate; retain failed reports and reap children."""
    output=args.output/name
    job={'args':{key:str(value) if isinstance(value,Path) else value for key,value in vars(args).items()},
         'origin':origin,'relay':relay,'dest':str(dest),'mode':mode,'output':str(output)}
    spec=args.output/(name+'.job');spec.write_text(json.dumps(job))
    with (args.output/(name+'.log')).open('w') as log:
        process=subprocess.Popen([sys.executable,str(dest/'measure-smoothness.py'),'--worker',str(spec)],
                                 stdout=log,stderr=subprocess.STDOUT,start_new_session=os.name!='nt')
        try:process.wait(timeout=args.timeout*2+180)
        except subprocess.TimeoutExpired:
            if os.name=='nt':subprocess.run(['taskkill','/PID',str(process.pid),'/T','/F'],stdout=log,stderr=subprocess.STDOUT)
            else:
                import signal
                os.killpg(process.pid,signal.SIGKILL)
            process.wait(timeout=15)
            output.write_text(json.dumps({'passed':False,'mode':mode,'error':'Worker timeout; browser process tree terminated'}))
    if not output.exists():output.write_text(json.dumps({'passed':False,'mode':mode,'error':'Worker exited '+str(process.returncode)}))
    return json.loads(output.read_text())

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output',type=Path,required=True);p.add_argument('--data',type=Path,required=True)
    p.add_argument('--frames',type=int,default=780);p.add_argument('--players',type=int,choices=(2,3),default=2)
    p.add_argument('--cpu-rate',type=float,default=4);p.add_argument('--bullets',type=int,default=1000)
    p.add_argument('--difficulty',type=int,choices=range(4),default=1)
    p.add_argument('--one-way-ms',type=float,default=38.5);p.add_argument('--jitter-ms',type=float,default=5)
    p.add_argument('--modes',default='always,frontier');p.add_argument('--rounds',type=int,default=2)
    p.add_argument('--timeout',type=float,default=150);p.add_argument('--freeze-only',action='store_true')
    p.add_argument('--browser-channel',choices=('default','chromium','chrome','msedge'),default='chromium')
    p.add_argument('--cap60',action='store_true')
    p.add_argument('--wall-clock-touch',action='store_true')
    p.add_argument('--production',action='store_true',help='Formal WASM without native fixture/profiling exports; requires --bullets 0 --modes frontier')
    args=p.parse_args();args.output=args.output.resolve()
    modes=args.modes.split(',')
    if args.frames<=300 or not all(m in MODES for m in modes) or args.cpu_rate<1:raise ValueError('Invalid settings')
    if args.production and (args.bullets or modes!=['frontier']):raise ValueError('Production lane requires authored bullets and unchanged frontier policy')
    dest,manifest=freeze(args)
    if args.freeze_only:return 0
    server=http.server.ThreadingHTTPServer(('127.0.0.1',0),functools.partial(QuietHandler,directory=str(dest)))
    threading.Thread(target=server.serve_forever,daemon=True).start()
    with socket.socket() as probe:probe.bind(('127.0.0.1',0));relay_port=probe.getsockname()[1]
    env={**os.environ,'EAGLER_NETPLAY_RELAY_HOST':'127.0.0.1','EAGLER_NETPLAY_RELAY_PORT':str(relay_port),'EAGLER_NETPLAY_STUN_URLS':''}
    summaries=[];reference_hashes=None
    with (args.output/'relay.log').open('w') as log:
        relay=subprocess.Popen(['node',str(dest/'relay/server/netplay-relay.mjs')],env=env,stdout=log,stderr=subprocess.STDOUT)
        try:
            time.sleep(1)
            if relay.poll() is not None:raise RuntimeError('Relay startup failed')
            for round_id in range(args.rounds):
                for mode in modes if round_id%2==0 else list(reversed(modes)):
                    verify(dest,manifest)
                    name=f'{round_id+1}-{mode}.json'
                    report=supervised_run(args,'http://127.0.0.1:'+str(server.server_port),'ws://127.0.0.1:'+str(relay_port),dest,mode,name)
                    verify(dest,manifest)
                    if report['passed']:
                        hashes=report['peers'][0]['canonical']
                        if not args.wall_clock_touch and reference_hashes is not None and hashes!=reference_hashes:
                            report['passed']=False;report['error']='Cross-policy final canonical divergence'
                            (args.output/name).write_text(json.dumps(report,indent=2))
                        reference_hashes=hashes
                    summary={k:report[k] for k in ('passed','mode','summary','error') if k in report};summary['file']=name
                    summaries.append(summary)
                    print(json.dumps({'file':name,'passed':report['passed'],'error':report.get('error'),
                      'peers':[{key:peer[key] for key in ('logicalFps','presentGapMs','advanceGapMs','corrections','resimulated')}
                               for peer in report.get('summary',[])]}),flush=True)
                    (args.output/'summary.json').write_text(json.dumps(summaries,indent=2))
                    if not report['passed']:return 1
        finally:
            relay.terminate();relay.wait(timeout=10);server.shutdown()
    return 0
if __name__=='__main__':
    if len(sys.argv)>1 and sys.argv[1]=='--worker':
        job=json.loads(Path(sys.argv[2]).read_text())
        report=run(argparse.Namespace(**job['args']),job['origin'],job['relay'],Path(job['dest']),job['mode'])
        Path(job['output']).write_text(json.dumps(report,indent=2))
        sys.exit(0 if report['passed'] else 1)
    sys.exit(main())
