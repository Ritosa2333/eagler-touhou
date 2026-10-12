"""Frozen-runtime performance: real native rAF and real RTC, no Python tick loop."""
import argparse
import hashlib
import json
import math
import os
import time
import uuid
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--relay',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--frames',type=int,default=1800)
parser.add_argument('--players',type=int,choices=[2,3],default=2)
parser.add_argument('--one-way-ms',type=float,default=38.5)
parser.add_argument('--jitter-ms',type=float,default=10)
parser.add_argument('--cpu',type=float,default=1)
parser.add_argument('--cpu-rates',help='Comma-separated rates in seat order; overrides --cpu')
parser.add_argument('--dense',action='store_true')
parser.add_argument('--drop-every',type=int,default=0)
parser.add_argument('--spike-frame',type=int,default=450)
parser.add_argument('--spike-ms',type=float,default=0)
parser.add_argument('--spike-delay-ms',type=float,default=800)
parser.add_argument('--spike-seat',type=int,choices=range(-1,3),default=-1,help='Sending seat, or -1 for all')
parser.add_argument('--renderer',choices=['software','hardware'],default='software')
parser.add_argument('--headed',action='store_true')
parser.add_argument('--browser-channel',choices=['default','chromium','chrome'],default='default')
parser.add_argument('--profile',action='store_true')
parser.add_argument('--cold-start',action='store_true')
parser.add_argument('--late-start-ms',type=float,default=0)
parser.add_argument('--direct-touch-p1',action='store_true')
parser.add_argument('--touch-motion-hz',type=float,default=0,help='Wall-clock native touch trajectory on every seat; 0 keeps legacy input')
parser.add_argument('--input-delay',type=int,choices=range(9),default=0)
parser.add_argument('--unlocked-presentation',action='store_true')
parser.add_argument('--startup-poll-multiplier',type=int,choices=range(1,17),default=1,
                    help='Diagnostic: extra stopped-loop loading ticks for seat 0, before multiplayer is active')
args=parser.parse_args()
try:
    cpu_rates=[float(v) for v in args.cpu_rates.split(',')] if args.cpu_rates else [args.cpu]*args.players
except ValueError:
    parser.error('--cpu-rates must be comma-separated numbers')
if len(cpu_rates)!=args.players or any(not math.isfinite(v) or v<1 or v>100 for v in cpu_rates):
    parser.error('Provide one finite CPU rate from 1 to 100 for each seat')
if not math.isfinite(args.touch_motion_hz) or not 0<=args.touch_motion_hz<=240:
    parser.error('--touch-motion-hz must be finite and between 0 and 240')
if args.touch_motion_hz and args.direct_touch_p1:
    parser.error('--touch-motion-hz and --direct-touch-p1 are different stimuli; choose one')
if (args.spike_frame<0 or args.spike_seat>=args.players or
    any(not math.isfinite(v) or not 0<=v<=10000 for v in [args.spike_ms,args.spike_delay_ms])):
    parser.error('Invalid spike frame, duration, extra delay or sending seat')
if not math.isfinite(args.late_start_ms) or args.late_start_ms<0:
    parser.error('--late-start-ms must be finite and nonnegative')
args.output.parent.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[2]
impairment=root/'third_party/eagler-common/testkit/rtc-input-impairment.cjs'
browser_evidence=Path(__file__).with_name('browser-evidence.cjs')
sources=[Path(__file__),Path(__file__).with_name('performance-driver.mjs'),
         Path(__file__).with_name('smoke.mjs'),impairment,browser_evidence]
digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
report={'passed':False,'schemaVersion':2,'kind':'native-raf-real-rtc','options':vars(args)|{'output':str(args.output)},
        'sources':{str(p):digest(p) for p in sources},'errors':[],'cpuRates':cpu_rates,'cpuThrottle':[],
        'rtcTimeoutEnvironment':{k:os.environ.get(k) for k in ['EAGLER_NETPLAY_RTC_TIMEOUT_MS','TH07_RTC_TIMEOUT_MS','TH07_DIRECT_TIMEOUT_MS']},
        'coverage':{'input':'native TouchController/keyboard API; not full Launcher or managed messages',
          'impairment':'shared application RTC send impairment, including reliable input repair; not wire RTT',
          'presentation':'default-framebuffer blits only; final-pixel object continuity and input-to-photon unmeasured',
          'passed':'run, route, state and confirmed-audio gates; not a smoothness acceptance'}}
def call(page,expression,arg=None):
    return page.evaluate(expression,arg)
def distribution(values):
    values=sorted(values)
    if not values:return None
    def pct(p):return values[min(len(values)-1,math.ceil(len(values)*p)-1)]
    return {'count':len(values),'p50':pct(.5),'p95':pct(.95),'p99':pct(.99),'max':values[-1],
            'over25ms':sum(v>25 for v in values),'over50ms':sum(v>50 for v in values),
            'over100ms':sum(v>100 for v in values)}
def summarize(data):
    # Native LastSimulatedFrame can retreat during correction. Select one
    # contiguous TIME interval between first observed frontier crossings,
    # retaining every intervening callback, including all replay-only slices.
    raw=data['rows']
    assert not data.get('sessionIdentityChanges'), 'Performance interval crossed a netplay session identity'
    high=data['initialFrame'];frontiers=[]
    for row in raw:
        high=max(high,row[2]);frontiers.append(high)
        if len(row)>5:assert row[5]==high,('Observed frontier does not match raw native history',row)
    lower,upper=data['initialFrame']+120,data['targetFrame']-12
    start_index=next((i for i,f in enumerate(frontiers) if f>=lower),None)
    end_index=next((i for i,f in enumerate(frontiers) if start_index is not None and i>start_index and f>=upper),None)
    assert start_index is not None and end_index is not None,('Measured frontier bounds not reached',lower,upper,high)
    anchors=raw[start_index:end_index+1];levels=frontiers[start_index:end_index+1]
    # The first crossing is the time anchor. Work after it through the upper
    # crossing is included; the anchor's prior callback cost is outside time.
    rows=anchors[1:];first,last=anchors[0],anchors[-1]
    assert len(rows)>120,('Insufficient measured callbacks',len(rows))
    assert last[0]>first[0], 'Non-increasing measured time'
    assert levels[-1]>levels[0], 'No newly observed logical progress in measured window'
    presents=[t for t in data['presents'] if first[0]<t<=last[0]]
    assert len(presents)>100,'No actual default-framebuffer presentations observed'
    assert presents[-1]>presents[0], 'Non-increasing presentation time'
    forward=[row for previous,current,row in zip(levels,levels[1:],rows) if current>previous]
    advances=[first[0]]+[row[0] for row in forward]
    result={'window':{'scope':'first high-water crossings; all intervening callback samples retained',
        'requestedLowerFrame':lower,'requestedUpperFrame':upper,
        'firstObservedFrame':levels[0],'lastObservedFrame':levels[-1],
        'startMs':first[0],'endMs':last[0],'startRowExclusive':start_index,'endRowInclusive':end_index,
        'callbacks':len(rows),'newObservedFrames':levels[-1]-levels[0]},
      'logicHz':(levels[-1]-levels[0])*1000/(last[0]-first[0]),
      'presentHz':(len(presents)-1)*1000/(presents[-1]-presents[0]),
      'callbackWorkMs':distribution([r[1] for r in rows]),
      'callbackWorkPerNewFrameMs':sum(r[1] for r in rows)/(levels[-1]-levels[0]),
      'forwardCallbackWorkMs':distribution([r[1] for r in forward]),
      'callbackArrivalGapMs':distribution([b[0]-a[0] for a,b in zip(anchors,anchors[1:])]),
      'outsideNativeCallbackMs':distribution([max(0,b[0]-b[1]-a[0]) for a,b in zip(anchors,anchors[1:])]),
      'presentGapMs':distribution([b-a for a,b in zip(presents,presents[1:])]),
      'advanceGapMs':distribution([b-a for a,b in zip(advances,advances[1:])]),
      'nativeCursorRegressions':sum(b[2]<a[2] for a,b in zip(anchors,anchors[1:])),
      'maxObservedCursorRetreatFrames':max(f-r[2] for f,r in zip(levels,anchors)),
      'heapMin':min(r[4] for r in rows),'heapMax':max(r[4] for r in rows),
      'rollbackCount':data['finalMemory']['words'][10],
      'resimulatedFrames':data['finalMemory']['words'][11]}
    result['desktopCadenceTargetMet']=(result['logicHz']>=58.5 and
        result['presentGapMs']['p99']<25 and result['presentGapMs']['max']<50 and
        result['advanceGapMs']['p99']<25 and result['advanceGapMs']['max']<50)
    result['cadenceTargetScope']='timing only; default blits do not prove final object motion'
    result['motionContinuityAcceptance']='not measured'
    if len(first)>18 and first[14] is not None:
        presented=[r for r in rows if r[17]]
        fractional=[r for r in presented if r[14] and r[15] and 0<r[16]<1]
        result['nativePresentation']={'highRefreshCallbacks':sum(bool(r[14]) for r in rows),
            'presentedCallbacks':len(presented),'fractionalPresentedCallbacks':len(fractional),
            'highRefreshInterpolationObserved':bool(fractional),
            'scope':'native high-refresh, alpha and submission flags; final object motion remains unmeasured'}
    if len(first)>13 and first[8] is not None:
        for row in rows:
            if row[9]:assert row[6]<row[10] and row[11]==row[10], ('Recovery lost its saved frontier',row)
        result['nativeRecovery']={'yieldsInWindow':last[8]-first[8],
            'recoveringCallbacks':sum(bool(r[9]) for r in rows),
            'maxMissingInputs':max(r[12] for r in rows),
            'intervalScaleMin':min(r[13] for r in rows)/1000,
            'intervalScaleMax':max(r[13] for r in rows)/1000,
            'scope':'native host counters in the same measured callback window; no mid-transaction interruption'}
    motion=data.get('touchMotion')
    if motion and motion.get('enabled'):
        assert motion['submittedEvents']>1 and motion['moves']>0,('No sustained native touch stimulus',motion)
        assert motion['accountedEvents']==(motion['submittedEvents']+motion['coalescedEvents']+motion['skippedNotReadyEvents']), 'Touch plan accounting mismatch'
        assert motion['plannedEvents']==motion['accountedEvents']+motion['unservicedAtStop'], 'Unserviced touch plan mismatch'
        result['touchMotion']={k:motion[k] for k in ['targetHz','plannedEvents','accountedEvents',
            'submittedEvents','coalescedEvents','skippedNotReadyEvents','unservicedAtStop',
            'timerCallbacks','begins','moves','cancels','maxTimerLatenessMs','maxLatestTargetAgeMs','traceDropped']}
        span=motion['lastSubmittedMs']-motion['firstSubmittedMs']
        result['touchMotion']['submittedHz']=(motion['submittedEvents']-1)*1000/span if span>0 else None
        result['touchMotion']['recordedTimerLatenessMs']=distribution([r[5] for r in motion['trace']])
        window_events=[r for r in motion['trace'] if first[0]<r[4]<=last[0] and r[8] in [1,2]]
        result['touchMotion']['recordedWindowSubmittedEvents']=len(window_events)
        result['touchMotion']['recordedWindowSubmittedSpanMs']=window_events[-1][4]-window_events[0][4] if len(window_events)>1 else 0
        if not motion['traceDropped']:
            assert len(window_events)>1, 'No sustained touch submissions inside the measured callback window'
        result['touchMotion']['scope']='whole producer lifetime; lateness distribution uses bounded trace, counters cover all events'
    return result

with sync_playwright() as p:
    def launch_browser():
        flags=['--disable-background-timer-throttling','--disable-renderer-backgrounding',
          '--disable-backgrounding-occluded-windows','--autoplay-policy=no-user-gesture-required']
        options={'headless':not args.headed}
        if args.renderer=='hardware':
            options['channel']='chromium';flags+=['--ignore-gpu-blocklist','--use-gl=angle','--use-angle=d3d11','--enable-gpu']
        else:flags+=['--enable-unsafe-swiftshader','--use-angle=swiftshader']
        if args.browser_channel!='default':options['channel']=args.browser_channel
        report['browserLaunch']={'headless':options['headless'],'channel':options.get('channel','default'),'args':flags}
        return p.chromium.launch(args=flags,**options)
    browser=launch_browser()
    pages=[];browsers=[browser];profilers=[]
    def collect_browser_evidence(stage):
        values=[]
        for page in pages:
            try:values.append(call(page,'s=>{const e=globalThis.__th10BrowserEvidence;e?.mark(s);return e?.read()??null;}',stage))
            except Exception as error:values.append({'collectionError':str(error)})
        return values
    try:
        report['browser']=browser.version
        room='perf-th10-'+uuid.uuid4().hex[:16]
        relay=args.relay+'?room='+room+'&run=1'
        script=impairment.read_text(encoding='utf-8')
        observer_script=browser_evidence.read_text(encoding='utf-8')+'\ninstallTh10BrowserEvidence();\n'
        for seat in range(args.players):
            # Independent browser processes: do not hide transport/lifetime
            # problems behind two worlds in one runtime or shared GPU process.
            peer_browser=browser if seat==0 else launch_browser()
            if seat:browsers.append(peer_browser)
            context=peer_browser.new_context(service_workers='block',viewport={'width':800,'height':600})
            settings={'oneWayMs':args.one_way_ms,'jitterMs':args.jitter_ms,'seed':7135+seat,
                      'dropEvery':args.drop_every,'inputLabel':'th10-input','controlLabel':'th10-control',
                      'spikeFrame':args.spike_frame,
                      'spikeMs':args.spike_ms if args.spike_seat in [-1,seat] else 0,
                      'spikeDelayMs':args.spike_delay_ms}
            context.add_init_script(observer_script+script+'\nglobalThis.perfImpairment=installRtcInputImpairment('+json.dumps(settings)+');')
            page=context.new_page();pages.append(page)
            page.on('pageerror',lambda error:report['errors'].append(str(error)))
            page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
            report.setdefault('identities',[]).append(call(page,'multiplayerSmoke.identity()'))
            call(page,'x=>multiplayerSmoke.startNet(...x)',[[[0,0],[1,1],[0,2]][:args.players],seat,1,1234,0x71456789,0x18273645,args.input_delay])
        # Match the TH08 reference setup: create both native worlds before
        # connecting them. The relay's existing route deadline stays intact.
        for page in pages:assert call(page,'u=>{__th10BrowserEvidence.mark("connect-call");return multiplayerSmoke.connect(u);}',relay)
        deadline=time.monotonic()+60
        while not args.cold_start and not all(call(page,'multiplayerSmoke.canStart()') for page in pages):
            for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')
            assert time.monotonic()<deadline,'Actual transport handshake timeout'
            pages[0].wait_for_timeout(5)
        if not args.cold_start:
            report['routeAtHandshake']=[call(page,'multiplayerSmoke.transportStatus()') for page in pages]
            report['browserEvidenceAtHandshake']=collect_browser_evidence('rtc-barrier')
            assert all(n[13]==1 for n in report['routeAtHandshake']), ('Not RTC at initial barrier',report['routeAtHandshake'])
        deadline=time.monotonic()+120
        while not args.cold_start:
            states=[]
            for seat,page in enumerate(pages):
                value=call(page,'''multiplier=>{
                    let value,startupCalls=0;
                    for(let i=0;i<multiplier;++i){
                        if(!multiplayerSmoke.netStatus()[9])++startupCalls;
                        value=multiplayerSmoke.peerAdvance(59,0);
                        if(value.net[9])break;
                    }
                    return {...value,startupCalls};
                }''',args.startup_poll_multiplier if seat==0 else 1)
                report.setdefault('startupPollCalls',[0]*args.players)[seat]+=value['startupCalls']
                states.append(value)
            if all(v['net'][3]==59 and v['net'][4]>=59 and v['net'][5]==-1 for v in states):break
            assert time.monotonic()<deadline,('Initial confirmed fence timeout',states)
            pages[0].wait_for_timeout(5)
        # Observe the existing confirmed fence before throttling or starting
        # native measurement, so startup divergence is not blamed on recovery.
        if not args.cold_start:
            report['initialComparison']=[call(page,'multiplayerSmoke.performanceFinal()') for page in pages]
            if any(v['canonical'][1]!=report['initialComparison'][0]['canonical'][1] for v in report['initialComparison'][1:]):
                report['initialSceneOwners']=[call(page,'multiplayerSmoke.sceneOwners()') for page in pages]
        if args.dense:
            assert args.players==2,'Dense fixture declares two seats'
            for page in pages:assert call(page,'multiplayerSmoke.fixture(7)')
        for seat,page in enumerate(pages):
            rate=cpu_rates[seat]
            throttle=page.context.new_cdp_session(page)
            acknowledgement=throttle.send('Emulation.setCPUThrottlingRate',{'rate':rate})
            report['cpuThrottle'].append({'seat':seat,'rate':rate,'acknowledged':True,'response':acknowledgement})
            if args.profile and seat==0:
                profiler=page.context.new_cdp_session(page);profiler.send('Profiler.enable')
                profiler.send('Profiler.setSamplingInterval',{'interval':500});profiler.send('Profiler.start')
                profilers.append((seat,profiler))
            call(page,'o=>multiplayerSmoke.performanceStart(o)',{'targetFrame':(args.frames-1 if args.cold_start else 59+args.frames),'seat':seat,
                  'limit60':not args.unlocked_presentation,'lateStartMs':args.late_start_ms if seat==args.players-1 else 0,
                  'directTouchP1':args.direct_touch_p1,'touchMotionHz':args.touch_motion_hz})
        deadline=time.monotonic()+max(120,args.frames/60*6)
        while True:
            done=[call(page,'multiplayerSmoke.performanceDone()') for page in pages]
            if all(done):break
            assert not report['errors'],report['errors']
            assert time.monotonic()<deadline,('Native performance deadline',done)
            pages[0].wait_for_timeout(200)
        report['peers']=[call(page,'multiplayerSmoke.performanceResult()') for page in pages]
        for data in report['peers']:
            assert data['passed'],(data.get('error'),data.get('nativeError'),data.get('networkError'))
            if args.cold_start:
                first=data.get('firstForward')
                assert first and first['net'][4]>=args.input_delay,('Frame zero ran before all peers supplied their first real input',first)
                assert first['atMs']>=args.late_start_ms-100,('Gameplay ran before the slow peer started',first)
        for seat,profiler in profilers:
            profile=profiler.send('Profiler.stop')['profile']
            path=args.output.with_name('peer-'+str(seat)+'.cpuprofile')
            path.write_text(json.dumps(profile),encoding='utf-8')
            report.setdefault('profiles',[]).append(str(path))
        report['impairment']=[call(page,'perfImpairment.stats') for page in pages]
        comparison=max(data['finalNet'][3] for data in report['peers'])+16
        deadline=time.monotonic()+60
        while True:
            states=[call(page,'t=>multiplayerSmoke.performanceSettle(t)',comparison) for page in pages]
            if all(n[3]==comparison and n[4]>=comparison and n[5]==-1 for n in states):break
            assert time.monotonic()<deadline,('Final exact fence timeout',states)
            pages[0].wait_for_timeout(5)
        report['comparison']=[call(page,'multiplayerSmoke.performanceFinal()') for page in pages]
        if any(v['canonical'][1]!=report['comparison'][0]['canonical'][1] for v in report['comparison'][1:]):
            report['sceneOwners']=[call(page,'multiplayerSmoke.sceneOwners()') for page in pages]
            if all(report['sceneOwners']):
                report['sceneDifferences']=[]
                for a,b in zip(report['sceneOwners'][0],report['sceneOwners'][1]):
                    differences=[i for i,(x,y) in enumerate(zip(a['bytes'],b['bytes'])) if x!=y]
                    if differences or len(a['bytes'])!=len(b['bytes']):
                        report['sceneDifferences'].append({'name':a['name'],'pointers':[a['pointer'],b['pointer']],
                           'lengths':[len(a['bytes']),len(b['bytes'])],'count':len(differences),
                           'first':[[i,a['bytes'][i],b['bytes'][i]] for i in differences[:64]]})
        for seat,(data,imp) in enumerate(zip(report['peers'],report['impairment'])):
            assert data['passed'],data.get('error')
            renderer=(data.get('unmaskedRenderer') or '').lower()
            assert renderer,'Renderer identity is required for performance evidence'
            if args.renderer=='hardware':
                assert not any(v in renderer for v in ['swiftshader','software','basic render','warp']), ('Hardware lane silently fell back',renderer)
            else:assert 'swiftshader' in renderer,('Software lane changed',renderer)
            assert imp['matched']>0 and imp['sent']>0,('Real RTC lane was not impaired',imp)
            assert imp['errors']==0 and imp['overflow']==0,imp
            if args.spike_ms and args.spike_seat in [-1,seat]:
                assert imp['spikeDelayed']>0,('Requested RTC spike was not observed',seat,imp)
            assert data['transport'][13]==1,('Not real RTC',data['transport'])
            data['summary']=summarize(data)
        first=report['comparison'][0]
        for final in report['comparison']:
            if final.get('smoothness'):assert final['smoothness'][2]==0, ('Final comparison is still correcting',final['smoothness'])
        for other in report['comparison'][1:]:
            assert first['state'][:2]+first['state'][3:]==other['state'][:2]+other['state'][3:], 'Final authoritative state differs'
            assert first['canonical'][1]==other['canonical'][1],('Final canonical composite differs',first['canonical'],other['canonical'])
            assert first['audio']==other['audio'],'Confirmed audio history differs'
        assert not report['errors'],report['errors']
        assert all(digest(p)==report['sources'][str(p)] for p in sources),'Harness changed during run'
        report['passed']=True
    except BaseException as error:
        report['error']=str(error)
        for page in pages:
            try:report.setdefault('partial',[]).append(call(page,'multiplayerSmoke.performanceResult?.() || multiplayerSmoke.netStatus()'))
            except Exception:pass
        raise
    finally:
        report['browserEvidence']=collect_browser_evidence('final-collection')
        for peer_browser in browsers:peer_browser.close()
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'passed':report['passed'],'summaries':[v['summary'] for v in report['peers']]}))
