"""Frozen-runtime performance: real native rAF and real RTC, no Python tick loop."""
import argparse
import hashlib
import json
import math
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
parser.add_argument('--dense',action='store_true')
parser.add_argument('--drop-every',type=int,default=0)
parser.add_argument('--renderer',choices=['software','hardware'],default='software')
parser.add_argument('--profile',action='store_true')
parser.add_argument('--cold-start',action='store_true')
parser.add_argument('--late-start-ms',type=float,default=0)
parser.add_argument('--direct-touch-p1',action='store_true')
parser.add_argument('--input-delay',type=int,choices=range(9),default=0)
parser.add_argument('--unlocked-presentation',action='store_true')
args=parser.parse_args()
root=Path(__file__).resolve().parents[2]
impairment=root/'third_party/eagler-common/testkit/rtc-input-impairment.cjs'
sources=[Path(__file__),Path(__file__).with_name('performance-driver.mjs'),
         Path(__file__).with_name('smoke.mjs'),impairment]
digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
report={'passed':False,'kind':'native-raf-real-rtc','options':vars(args)|{'output':str(args.output)},
        'sources':{str(p):digest(p) for p in sources},'errors':[]}
def call(page,expression,arg=None):
    return page.evaluate(expression,arg)
def distribution(values):
    values=sorted(values)
    if not values:return None
    def pct(p):return values[min(len(values)-1,math.ceil(len(values)*p)-1)]
    return {'count':len(values),'p50':pct(.5),'p95':pct(.95),'p99':pct(.99),'max':values[-1],
            'over25ms':sum(v>25 for v in values),'over50ms':sum(v>50 for v in values)}
def summarize(data):
    # Omit startup JIT/clock priming and the intentional final exact-input fence.
    rows=[r for r in data['rows'] if r[2]>=data['initialFrame']+120 and r[2]<data['targetFrame']-12]
    assert len(rows)>120,('Insufficient measured callbacks',len(rows))
    first,last=rows[0],rows[-1]
    presents=[t for t in data['presents'] if first[0]<=t<=last[0]]
    assert len(presents)>100,'No actual default-framebuffer presentations observed'
    result={'logicHz':(last[2]-first[2])*1000/(last[0]-first[0]),
      'presentHz':(len(presents)-1)*1000/(presents[-1]-presents[0]),
      'callbackWorkMs':distribution([r[1] for r in rows]),
      'forwardCallbackWorkMs':distribution([b[1] for a,b in zip(rows,rows[1:]) if b[2]>a[2]]),
      'callbackArrivalGapMs':distribution([b[0]-a[0] for a,b in zip(rows,rows[1:])]),
      # End-to-start time outside the measured native callback is separately
      # observable, not evidence by itself of a particular OS/GPU/JS cause.
      'outsideNativeCallbackMs':distribution([max(0,b[0]-b[1]-a[0]) for a,b in zip(rows,rows[1:])]),
      'presentGapMs':distribution([b-a for a,b in zip(presents,presents[1:])]),
      'heapMin':min(r[4] for r in rows),'heapMax':max(r[4] for r in rows),
      'rollbackCount':data['finalMemory']['words'][10],
      'resimulatedFrames':data['finalMemory']['words'][11]}
    result['desktopSmoothnessTargetMet']=result['logicHz']>=58.5 and result['presentGapMs']['p99']<25 and result['presentGapMs']['max']<50
    return result

with sync_playwright() as p:
    def launch_browser():
        flags=['--disable-background-timer-throttling','--disable-renderer-backgrounding',
          '--disable-backgrounding-occluded-windows','--autoplay-policy=no-user-gesture-required']
        options={'headless':True}
        if args.renderer=='hardware':
            options['channel']='chromium';flags+=['--use-angle=d3d11','--enable-gpu']
        else:flags+=['--enable-unsafe-swiftshader','--use-angle=swiftshader']
        return p.chromium.launch(args=flags,**options)
    browser=launch_browser()
    pages=[];browsers=[browser];profilers=[]
    try:
        report['browser']=browser.version
        room='perf-th10-'+uuid.uuid4().hex[:16]
        relay=args.relay+'?room='+room+'&run=1'
        script=impairment.read_text(encoding='utf-8')
        for seat in range(args.players):
            # Independent browser processes: do not hide transport/lifetime
            # problems behind two worlds in one runtime or shared GPU process.
            peer_browser=browser if seat==0 else launch_browser()
            if seat:browsers.append(peer_browser)
            context=peer_browser.new_context(service_workers='block',viewport={'width':800,'height':600})
            settings={'oneWayMs':args.one_way_ms,'jitterMs':args.jitter_ms,'seed':7135+seat,
                      'dropEvery':args.drop_every,'inputLabel':'th10-input','controlLabel':'th10-control'}
            context.add_init_script(script+'\nglobalThis.perfImpairment=installRtcInputImpairment('+json.dumps(settings)+');')
            page=context.new_page();pages.append(page)
            page.on('pageerror',lambda error:report['errors'].append(str(error)))
            page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
            report.setdefault('identities',[]).append(call(page,'multiplayerSmoke.identity()'))
            call(page,'x=>multiplayerSmoke.startNet(...x)',[[[0,0],[1,1],[0,2]][:args.players],seat,1,1234,0x71456789,0x18273645,args.input_delay])
            assert call(page,'u=>multiplayerSmoke.connect(u)',relay)
        deadline=time.monotonic()+60
        while not args.cold_start and not all(call(page,'multiplayerSmoke.canStart()') for page in pages):
            for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')
            assert time.monotonic()<deadline,'Actual transport handshake timeout'
            pages[0].wait_for_timeout(5)
        deadline=time.monotonic()+120
        while not args.cold_start:
            states=[call(page,'multiplayerSmoke.peerAdvance(59,0)') for page in pages]
            if all(v['net'][3]==59 and v['net'][4]>=59 and v['net'][5]==-1 for v in states):break
            assert time.monotonic()<deadline,('Initial confirmed fence timeout',states)
            pages[0].wait_for_timeout(5)
        if args.dense:
            assert args.players==2,'Dense fixture declares two seats'
            for page in pages:assert call(page,'multiplayerSmoke.fixture(7)')
        for seat,page in enumerate(pages):
            if args.cpu!=1:page.context.new_cdp_session(page).send('Emulation.setCPUThrottlingRate',{'rate':args.cpu})
            if args.profile and seat==0:
                profiler=page.context.new_cdp_session(page);profiler.send('Profiler.enable')
                profiler.send('Profiler.setSamplingInterval',{'interval':500});profiler.send('Profiler.start')
                profilers.append((seat,profiler))
            call(page,'o=>multiplayerSmoke.performanceStart(o)',{'targetFrame':(args.frames-1 if args.cold_start else 59+args.frames),'seat':seat,
                  'limit60':not args.unlocked_presentation,'lateStartMs':args.late_start_ms if seat==args.players-1 else 0,
                  'directTouchP1':args.direct_touch_p1})
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
        for data,imp in zip(report['peers'],report['impairment']):
            assert data['passed'],data.get('error')
            renderer=(data.get('unmaskedRenderer') or '').lower()
            assert renderer,'Renderer identity is required for performance evidence'
            if args.renderer=='hardware':
                assert not any(v in renderer for v in ['swiftshader','software','basic render','warp']), ('Hardware lane silently fell back',renderer)
            else:assert 'swiftshader' in renderer,('Software lane changed',renderer)
            assert imp['matched']>0 and imp['sent']>0,('Real RTC lane was not impaired',imp)
            assert imp['errors']==0 and imp['overflow']==0,imp
            assert data['transport'][13]==1,('Not real RTC',data['transport'])
            data['summary']=summarize(data)
        first=report['comparison'][0]
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
        for peer_browser in browsers:peer_browser.close()
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'passed':report['passed'],'summaries':[v['summary'] for v in report['peers']]}))
