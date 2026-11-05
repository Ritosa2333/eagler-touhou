"""Frozen-runtime performance: real native rAF and RTC/WS, no Python tick loop."""
import argparse
import hashlib
import importlib.util
import json
import math
import os
import sys
import time
import uuid
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--relay',required=True)
parser.add_argument('--transport-mode',choices=['rtc','relay'],default='rtc',
                    help='Actual initial route: rtc retains the original lane; relay forces the existing initial WebSocket fallback before transport creation')
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--frames',type=int,default=1800)
parser.add_argument('--players',type=int,choices=[2,3],default=2)
parser.add_argument('--one-way-ms',type=float,default=38.5)
parser.add_argument('--jitter-ms',type=float,default=10)
parser.add_argument('--cpu',type=float,default=1)
parser.add_argument('--music-mode',choices=['off','stream','full'],default='off',
                    help='Preload canonical OGG before startup; measure the unchanged native decoder/mixer during gameplay')
parser.add_argument('--cpu-rates',help='Comma-separated rates in seat order; overrides --cpu')
parser.add_argument('--dense',action='store_true')
parser.add_argument('--drop-every',type=int,default=0)
parser.add_argument('--spike-frame',type=int,default=450)
parser.add_argument('--spike-ms',type=float,default=0)
parser.add_argument('--spike-delay-ms',type=float,default=800)
parser.add_argument('--spike-seat',type=int,choices=range(-1,3),default=-1,help='Sending seat, or -1 for all')
parser.add_argument('--renderer',choices=['software','hardware'],default='software')
parser.add_argument('--headed',action='store_true')
parser.add_argument('--window-layout',choices=['default','overlap','tiled'],default='default',
                    help='Explicit two-window placement; overlap/tiled share a 672x520 HTML viewport and retain the 640x480 game canvas')
parser.add_argument('--window-work-area',help='Explicit layout requires the measured native work area x,y,width,height; no inferred monitor size')
parser.add_argument('--foreground-seat',type=int,choices=[0,1],default=None,
                    help='Request activation of one explicit-layout headed page once before measurement; not proof of OS foreground state')
parser.add_argument('--browser-channel',choices=['default','chromium','chrome'],default='default')
parser.add_argument('--profile',action='store_true')
parser.add_argument('--chrome-trace',action='store_true',help='Symmetric bounded CDP event tracing without CPU sampling; diagnostic timing is not pure performance')
parser.add_argument('--thread-time-metrics',action='store_true',help='Opt-in fixed-boundary main-thread CPU/wall CDP metrics; enables an observer and is diagnostic timing, not pure performance')
parser.add_argument('--fixed-work-probe',action='store_true',help='Private fixed JS/WASM work in the same pages after final state/audio fences, before bulk; leaves CPU rates unchanged')
parser.add_argument('--fixed-work-rate-calibration',action='store_true',help='Opt-in post-fence original-session 4x/1x/4x fixed work; requires --fixed-work-probe and original 4x on every seat')
parser.add_argument('--cold-start',action='store_true')
parser.add_argument('--late-start-ms',type=float,default=0)
parser.add_argument('--direct-touch-p1',action='store_true')
parser.add_argument('--touch-motion-hz',type=float,default=0,help='Wall-clock native touch trajectory on every seat; 0 keeps legacy input')
parser.add_argument('--touch-path',choices=['orbit','separated-lanes'],default='orbit',
                    help='Explicit pointer trajectory; separated lanes provide visible-player coverage without changing native opacity or analysis')
parser.add_argument('--input-delay',type=int,choices=range(9),default=0)
parser.add_argument('--unlocked-presentation',action='store_true')
parser.add_argument('--model-quad-residency',choices=['default','off','on','seat0-on'],default='default',
                    help='Same-WASM fixed-model VBO option; default leaves the runtime setting unchanged')
parser.add_argument('--callback-timing',action='store_true',help='Opt-in native callback phase attribution')
parser.add_argument('--callback-work-timing',action='store_true',help='Opt-in callback-local capture/update/semantic Draw/restore/confirmed output timers; requires --callback-timing')
parser.add_argument('--transport-trace',action='store_true',help='Diagnostic bounded transport metadata; instrumented timing is not clean performance evidence')
parser.add_argument('--transport-trace-interval-ms',type=int,default=20,help='Diagnostic callback sampling minimum interval, 20..1000 ms; no catch-up sampling')
parser.add_argument('--relay-hop-trace',action='store_true',help='Opt-in actual WebSocket hop counters; requires relay mode, transport trace and the owned runner relay observer')
parser.add_argument('--browser-net-log',action='store_true',help='Diagnostic: each owned browser records bounded Default-mode Chromium NetLog; adds network logging overhead')
parser.add_argument('--verify-touch-frontier',action='store_true',help='Correctness diagnostic: real touches between native recovery callbacks and the first subsequent capture')
parser.add_argument('--verify-touch-frontier-seat',type=int,choices=range(3),
                    help='Observe one seat; other seats may use the unchanged wall-clock touch producer to sustain remote correction load')
parser.add_argument('--verify-live-motion',action='store_true',help='Diagnostic: actual submitted quads and asynchronous default-buffer pixels during the unchanged live input run')
parser.add_argument('--verify-live-motion-seat',type=int,choices=[0,1],default=0,
                    help='Observer seat; its real world contains both players and a naturally drawn bullet')
parser.add_argument('--verify-model-quad',action='store_true',help='Diagnostic fixture only: compare exact stopped-world pixels before timing')
parser.add_argument('--camera-math-cache',choices=['default','off','on','seat0-on'],default='default')
parser.add_argument('--camera-math-verify-every',type=int,default=0,help='Verify every Nth cache hit against original math; 0 for performance')
parser.add_argument('--packed-occupancy-scan',choices=['default','off','on','seat0-on'],default='default')
parser.add_argument('--initial-capture-batch',choices=['default','off','on','seat0-on'],default='default')
parser.add_argument('--network-gap-tail',choices=['default','off','on','seat0-on'],default='default',
                    help='Deep information pacing; default uses the loaded runtime choice, selected before input and locked across runs')
parser.add_argument('--network-tail-presentation',choices=['default','off','on','seat0-on'],default='default',
                    help='Independent experimental extra Draw; explicit choices require the split runtime exports')
parser.add_argument('--raf-deadline-guard',choices=['default','off','on','seat0-on'],default='default',
                    help='TH10-only candidate; chosen before the first native loop start and locked for the Module lifetime')
parser.add_argument('--raf-dispatch-delay-ms',type=int,default=0,
                    help='Test fault: defer one actual RAF callback by 1..1000 ms while timers keep running; 0 disables it')
parser.add_argument('--raf-dispatch-delay-frame',type=int,default=300)
parser.add_argument('--raf-dispatch-delay-seat',type=int,choices=[0,1],default=0)
parser.add_argument('--verify-camera-math',action='store_true',help='Diagnostic fixture: exact stopped-world camera cache pixel and numeric oracle')
parser.add_argument('--startup-poll-multiplier',type=int,choices=range(1,17),default=1,
                    help='Diagnostic: extra stopped-loop loading ticks for seat 0, before multiplayer is active')
args=parser.parse_args()
window_work_area=None
if args.window_layout=='default':
    if args.window_work_area is not None:parser.error('--window-work-area requires an explicit --window-layout')
else:
    if not args.headed or args.players!=2 or args.window_work_area is None:
        parser.error('Explicit --window-layout requires --headed, two players and --window-work-area x,y,width,height')
    try:
        window_work_area=[int(value.strip()) for value in args.window_work_area.split(',')]
        if (len(window_work_area)!=4 or any(not -2147483648<=value<=2147483647 for value in window_work_area)
            or window_work_area[2]<=0 or window_work_area[3]<=0):raise ValueError()
    except ValueError:
        parser.error('--window-work-area must be four signed 32-bit integers with positive width and height')
if args.foreground_seat is not None and (not args.headed or args.window_layout=='default'):
    parser.error('--foreground-seat requires --headed and an explicit --window-layout')
if args.chrome_trace and args.profile:
    parser.error('--chrome-trace excludes --profile: this diagnostic must not enable CPU profiling or sampling')
if args.callback_work_timing and not args.callback_timing:
    parser.error('--callback-work-timing requires --callback-timing')
if not 0<=args.raf_dispatch_delay_ms<=1000:
    parser.error('--raf-dispatch-delay-ms must be between 0 and 1000 ms')
if args.raf_dispatch_delay_ms:
    if (args.cold_start or args.players!=2 or not args.callback_timing or
        not args.unlocked_presentation or args.touch_motion_hz<=0 or
        not 179<=args.raf_dispatch_delay_frame<=59+args.frames-120):
        parser.error('RAF dispatch fault requires a warm two-player callback-timed unlocked touch run and a frame inside the measured window with a 120-frame tail')
elif args.raf_dispatch_delay_frame!=300 or args.raf_dispatch_delay_seat!=0:
    parser.error('RAF dispatch fault frame/seat options require a nonzero delay')
try:
    cpu_rates=[float(v) for v in args.cpu_rates.split(',')] if args.cpu_rates else [args.cpu]*args.players
except ValueError:
    parser.error('--cpu-rates must be comma-separated numbers')
if len(cpu_rates)!=args.players or any(not math.isfinite(v) or v<1 or v>100 for v in cpu_rates):
    parser.error('Provide one finite CPU rate from 1 to 100 for each seat')
if args.fixed_work_rate_calibration and (not args.fixed_work_probe or cpu_rates!=[4]*args.players):
    parser.error('--fixed-work-rate-calibration requires --fixed-work-probe and 4x on every seat')
if not 20<=args.transport_trace_interval_ms<=1000 or (args.transport_trace_interval_ms!=20 and not args.transport_trace):
    parser.error('--transport-trace-interval-ms requires transport trace and an interval from 20 to 1000 ms')
if args.relay_hop_trace and (args.transport_mode!='relay' or not args.transport_trace):
    parser.error('--relay-hop-trace requires --transport-mode relay and --transport-trace')
if args.browser_net_log and (not args.relay_hop_trace or args.chrome_trace or args.profile):
    parser.error('--browser-net-log requires --relay-hop-trace and excludes other tracing/profiling')
if not math.isfinite(args.touch_motion_hz) or not 0<=args.touch_motion_hz<=240:
    parser.error('--touch-motion-hz must be finite and between 0 and 240')
if args.touch_motion_hz and args.direct_touch_p1:
    parser.error('--touch-motion-hz and --direct-touch-p1 are different stimuli; choose one')
if args.touch_path=='separated-lanes' and (args.touch_motion_hz<=0 or args.players!=2 or args.verify_touch_frontier):
    parser.error('--touch-path separated-lanes requires two seats, wall-clock touch and no touch-frontier oracle')
if args.verify_touch_frontier_seat is not None and (not args.verify_touch_frontier or args.verify_touch_frontier_seat>=args.players):
    parser.error('--verify-touch-frontier-seat requires the oracle and a participating seat')
if args.verify_touch_frontier and (args.direct_touch_p1 or args.input_delay or args.cold_start or
                                  (args.touch_motion_hz and args.verify_touch_frontier_seat is None)):
    parser.error('--verify-touch-frontier requires its own per-seat stimulus, a confirmed initial fence and zero input delay')
if args.verify_live_motion and (args.players!=2 or args.frames<1100 or args.touch_motion_hz<=0 or
                               not args.unlocked_presentation or args.verify_touch_frontier):
    parser.error('--verify-live-motion requires two seats, at least 1100 frames, wall-clock touch, unlocked presentation and its own diagnostic run')
if (args.spike_frame<0 or args.spike_seat>=args.players or
    any(not math.isfinite(v) or not 0<=v<=10000 for v in [args.spike_ms,args.spike_delay_ms])):
    parser.error('Invalid spike frame, duration, extra delay or sending seat')
if not math.isfinite(args.late_start_ms) or args.late_start_ms<0:
    parser.error('--late-start-ms must be finite and nonnegative')
if not 0<=args.camera_math_verify_every<=0xffffffff:
    parser.error('--camera-math-verify-every must fit u32')
args.output.parent.mkdir(parents=True,exist_ok=True)
root=Path(__file__).resolve().parents[2]
rtc_impairment=root/'third_party/eagler-common/testkit/rtc-input-impairment.cjs'
websocket_impairment=Path(__file__).with_name('websocket-input-impairment.cjs')
impairment=rtc_impairment if args.transport_mode=='rtc' else websocket_impairment
browser_evidence=Path(__file__).with_name('browser-evidence.cjs')
sources=[Path(__file__),Path(__file__).with_name('performance-driver.mjs'),
         Path(__file__).with_name('graph-storage.mjs'),Path(__file__).with_name('model-quad-oracle.mjs'),
         Path(__file__).with_name('camera-math-oracle.mjs'),
         Path(__file__).with_name('initial-capture-batch.mjs'),
         Path(__file__).with_name('network-gap-tail.mjs'),
         Path(__file__).with_name('raf-deadline-guard.mjs'),
         Path(__file__).with_name('raf-dispatch-delay.mjs'),
         Path(__file__).with_name('fixed-work-probe.mjs'),
         Path(__file__).with_name('callback-work-timing.mjs'),
         Path(__file__).with_name('transport-trace.mjs'),
         Path(__file__).with_name('chrome-trace.py'),
         Path(__file__).with_name('touch-frontier-oracle.mjs'),
         Path(__file__).with_name('live-motion-capture.mjs'),Path(__file__).with_name('live-motion-analysis.mjs'),
         Path(__file__).with_name('live-motion-texture-guards.mjs'),
         Path(__file__).with_name('smoke.mjs'),rtc_impairment,websocket_impairment,browser_evidence]
digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
report={'passed':False,'schemaVersion':2,
        'kind':'native-raf-real-rtc' if args.transport_mode=='rtc' else 'native-raf-real-websocket-relay',
        'options':vars(args)|{'output':str(args.output)},
        'sources':{str(p):digest(p) for p in sources},'errors':[],'cpuRates':cpu_rates,'cpuThrottle':[],
        'rtcTimeoutEnvironment':{k:os.environ.get(k) for k in ['EAGLER_NETPLAY_RTC_TIMEOUT_MS','TH07_RTC_TIMEOUT_MS','TH07_DIRECT_TIMEOUT_MS']},
        'coverage':{'input':'native TouchController/keyboard API; not full Launcher or managed messages',
          'impairment':('shared application RTC send impairment, including reliable input repair; not wire RTT'
            if args.transport_mode=='rtc' else
            'TH10 Input-only application WebSocket send delay on the actual current relay, with FIFO head blocking; non-Input controls pass through. Not wire RTT, full TCP behavior or managed Launcher loading.'),
          'presentation':'default-framebuffer blits only; final-pixel object continuity and input-to-photon unmeasured',
          'passed':'run, route, state and confirmed-audio gates; not a smoothness acceptance'}}
report['coverage']['transport']=('native RTC mode 1 at the existing startup/loop-end gates; does not establish WAN or selected ICE candidate type'
    if args.transport_mode=='rtc' else
    'native WebSocket mode 2 at initial handshake, loop end and exact final fence; RTCPeerConnection disabled before Module creation to exercise initial fallback, not live RTC-to-relay migration')
report['coverage']['transportTrace']='bounded diagnostic metadata; timing is instrumented' if args.transport_trace else 'disabled'
report['coverage']['relayHopTrace']='test-only actual-send and binary-message counters sampled by existing callbacks; relay receive/send evidence is exported separately by the owned runner' if args.relay_hop_trace else 'disabled'
report['coverage']['browserNetLog']='owned-browser Default-mode network metadata, 64 MiB requested bound per seat; no payload capture, wire/ACK or lossless-log claim; diagnostic timing includes logging overhead' if args.browser_net_log else 'disabled'
report['coverage']['chromeTrace']='symmetric bounded browser event trace; no CPU sampling; instrumented timing, thread duration only where actually emitted' if args.chrome_trace else 'disabled'
report['coverage']['threadTimeMetrics']='fixed-boundary renderer main-thread CPU/wall counters; Performance observer adds overhead; diagnostic availability does not change run acceptance' if args.thread_time_metrics else 'disabled'
report['coverage']['callbackWorkTiming']='callback-local native subphases inside InputAndStep only; added clocks are diagnostic overhead, not CPU time or a pure performance run' if args.callback_work_timing else 'disabled'
report['coverage']['rafDispatchDelay']=('Test-only one-shot delayed dispatch of an actual RAF callback with its original timestamp; timers and native touch remain live. Controlled fault response, not normal cadence or an explanation of historical stalls.' if args.raf_dispatch_delay_ms else 'disabled')
report['coverage']['touchFrontierOracle']='independent real between-callback input correctness; timing is diagnostic' if args.verify_touch_frontier else 'disabled'
report['coverage']['liveMotion']='finite natural object windows; actual quad/default-buffer correspondence; timing is diagnostic' if args.verify_live_motion else 'disabled'
report['coverage']['music']='BGM source disabled; native SFX/mixer remain live' if args.music_mode=='off' else (
    'Real canonical OGG preloaded before Application; native '+args.music_mode+
    ' decode/mixer during gameplay, current-selection cursor observations at native start/stop; audible device output and managed loading unmeasured')
def call(page,expression,arg=None):
    return page.evaluate(expression,arg)

class WindowLayoutReceipt:
    """Explicit fixture geometry only; no focus changes, GL commands or hot-loop polling."""
    def __init__(self,mode,work_area,report):
        self.mode=mode;self.area=work_area;self.handles=[];self.baseline=None
        self.report=report['windowLayout']={'schema':'th10-window-layout/1','mode':mode,
            'status':'pending','workArea':dict(zip(['left','top','width','height'],work_area)),
            'workAreaSource':'Explicit caller-supplied native work area; not DOM screen/avail dimensions',
            'viewport':{'width':672,'height':520},'gameCanvas':{'width':640,'height':480},
            'scope':'Overlap/tiled share the smaller HTML viewport. Only native position is requested to change; outer-frame differences are recorded while game viewport and buffer stay fixed. Boundary geometry is not continuous OS occlusion or physical scanout evidence.',
            'positionOnlyRequest':True,'deviceScaleFactorChanged':False,'setup':[],'samples':[],
            'nativeSizePolicy':'Record up to two pixels per-axis placement change and one pixel peer outer-frame difference; per-peer applied bounds and both measured boundaries must remain exact. Game viewport and drawing buffer are unchanged.',
            'cssRectTolerancePx':1/64}

    @staticmethod
    def checked_bounds(raw):
        bounds={key:raw.get(key) for key in ['left','top','width','height','windowState']}
        assert all(type(bounds[key]) is int for key in ['left','top','width','height']),('Invalid native bounds',raw)
        assert bounds['width']>0 and bounds['height']>0 and bounds['windowState']=='normal',('Non-normal native window',raw)
        return bounds

    def validate_bounds(self,bounds,seat):
        expected={'left':710 if self.mode=='tiled' and seat==1 else 10,'top':10}
        assert all(bounds[key]==value for key,value in expected.items()),('Window position rejected or changed',seat,expected,bounds)
        x,y,width,height=self.area
        assert (bounds['left']>=x and bounds['top']>=y and bounds['left']+bounds['width']<=x+width
                and bounds['top']+bounds['height']<=y+height),('Native window outside explicit work area',seat,bounds,self.area)

    def attach(self,page,seat):
        entry={'seat':seat,'hostCallBeforeMs':time.monotonic()*1000}
        self.report['setup'].append(entry)
        try:
            session=page.context.new_cdp_session(page)
            self.handles.append((page,session,entry))
            initial=session.send('Browser.getWindowForTarget');entry['initial']=initial
            window_id=initial.get('windowId');assert type(window_id) is int and window_id>=0,('Invalid window id',initial)
            before=self.checked_bounds(initial.get('bounds',{}))
            entry['windowId']=window_id
            position={'left':710 if self.mode=='tiled' and seat==1 else 10,'top':10}
            entry['requestedPosition']=position
            entry['setResponse']=session.send('Browser.setWindowBounds',{'windowId':window_id,'bounds':position})
            actual=session.send('Browser.getWindowBounds',{'windowId':window_id});entry['applied']=actual
            bounds=self.checked_bounds(actual.get('bounds',{}));self.validate_bounds(bounds,seat)
            delta={key:bounds[key]-before[key] for key in ['width','height']};entry['placementSizeDelta']=delta
            assert all(abs(value)<=2 for value in delta.values()),('Placement changed native size beyond two pixels',seat,before,bounds)
            entry['bounds']=bounds
        except Exception as error:
            entry['error']=str(error)[:600];self.report['status']='failed';raise
        finally:entry['hostCallAfterMs']=time.monotonic()*1000

    def sample(self,stage):
        sample={'stage':stage,'hostCallBeforeMs':time.monotonic()*1000,'peers':[],'passed':False}
        self.report['samples'].append(sample)
        try:
            assert stage in ['before-native','loops-complete'] and len(self.handles)==2,'Invalid layout sampling boundary'
            assert (stage=='before-native')==(self.baseline is None),'Repeated or out-of-order layout boundary'
            for seat,(page,session,setup) in enumerate(self.handles):
                entry={'seat':seat};sample['peers'].append(entry)
                current=session.send('Browser.getWindowForTarget');entry['windowForTarget']=current
                assert current.get('windowId')==setup['windowId'],('Page moved to another native window',seat,current)
                actual=session.send('Browser.getWindowBounds',{'windowId':setup['windowId']});entry['native']=actual
                bounds=self.checked_bounds(actual.get('bounds',{}));entry['bounds']=bounds
                self.validate_bounds(bounds,seat)
                assert bounds==setup['bounds'],('Native window changed after placement',seat,setup['bounds'],bounds)
                dom=call(page,'''()=>{
                    const canvas=document.getElementById('screen');
                    if(!canvas)throw Error('Missing real game canvas');
                    const rect=canvas.getBoundingClientRect(),gl=canvas.getContext('webgl2');
                    if(!gl)throw Error('Missing existing game WebGL2 context');
                    return {timeOrigin:performance.timeOrigin,timeMs:performance.now(),
                        visibilityState:document.visibilityState,hidden:document.hidden,hasFocus:document.hasFocus(),
                        geometry:{screenX:window.screenX,screenY:window.screenY,innerWidth,innerHeight,
                            outerWidth,outerHeight,devicePixelRatio},
                        canvas:{width:canvas.width,height:canvas.height,
                            rect:{x:rect.x,y:rect.y,width:rect.width,height:rect.height,right:rect.right,bottom:rect.bottom}},
                        drawingBuffer:{width:gl.drawingBufferWidth,height:gl.drawingBufferHeight}};
                }''');entry['dom']=dom
                geometry=dom['geometry'];canvas=dom['canvas'];rect=canvas['rect']
                assert (geometry['innerWidth'],geometry['innerHeight'])==(672,520),('Explicit viewport changed',seat,geometry)
                assert (canvas['width'],canvas['height'])==(640,480),('Game canvas changed size',seat,canvas)
                assert abs(rect['width']-640)<=1/64 and abs(rect['height']-480)<=1/64,('Game CSS canvas changed size',seat,canvas)
                assert dom['drawingBuffer']=={'width':640,'height':480},('Game drawing buffer changed size',seat,dom['drawingBuffer'])
                assert all(type(value) in [int,float] and math.isfinite(value) for value in rect.values()),('Invalid canvas rect',seat,rect)
                assert rect['x']>=0 and rect['y']>=0 and rect['right']<=672 and rect['bottom']<=520,('Game canvas clipped by viewport',seat,rect)
                assert math.isfinite(geometry['devicePixelRatio']) and geometry['devicePixelRatio']>0,('Invalid DPR',seat,geometry)
                if self.baseline is not None:
                    previous=self.baseline[seat]['dom']
                    assert all(dom[key]==previous[key] for key in ['timeOrigin','geometry','drawingBuffer']),('DOM geometry or page realm changed during native loops',seat)
                    assert all(abs(rect[key]-previous['canvas']['rect'][key])<=1/64 for key in rect),('CSS canvas rectangle changed during native loops',seat)
            a,b=[entry['bounds'] for entry in sample['peers']]
            sample['peerOuterSizeDelta']={key:b[key]-a[key] for key in ['width','height']}
            assert all(abs(value)<=1 for value in sample['peerOuterSizeDelta'].values()),'Peer native outer sizes differ by more than one pixel'
            nonoverlap=(a['left']+a['width']<=b['left'] or b['left']+b['width']<=a['left'] or
                        a['top']+a['height']<=b['top'] or b['top']+b['height']<=a['top'])
            sample['nonoverlapping']=nonoverlap
            if self.mode=='tiled':assert nonoverlap,('Tiled windows overlap',a,b)
            else:assert a==b,('Overlap windows do not share the same bounds',a,b)
            if self.baseline is None:self.baseline=sample['peers']
            sample['passed']=True
            if stage=='loops-complete':self.report['status']='passed'
        except Exception as error:
            sample['error']=str(error)[:600];self.report['status']='failed';raise
        finally:sample['hostCallAfterMs']=time.monotonic()*1000

def thread_time_metrics_raw(value):
    # CDP normally returns finite JSON numbers. Preserve malformed numeric
    # evidence explicitly without emitting nonstandard JSON NaN/Infinity.
    if isinstance(value,float) and not math.isfinite(value):
        return {'nonFiniteNumber':repr(value)}
    if isinstance(value,list):return [thread_time_metrics_raw(v) for v in value]
    if isinstance(value,dict):return {k:thread_time_metrics_raw(v) for k,v in value.items()}
    return value

def thread_time_metrics_values(response):
    values={};reasons=[];seen=set()
    if not isinstance(response,dict) or not isinstance(response.get('metrics'),list):
        return values,['missing-or-invalid-metrics-array']
    for index,item in enumerate(response['metrics']):
        if not isinstance(item,dict) or not isinstance(item.get('name'),str) or not item['name']:
            reasons.append('invalid-metric-entry:'+str(index));continue
        name=item['name'];value=item.get('value')
        if name in seen:reasons.append('duplicate-metric:'+name);continue
        seen.add(name)
        finite=False
        if isinstance(value,(int,float)) and not isinstance(value,bool):
            try:finite=math.isfinite(value)
            except OverflowError:pass
        if not finite:reasons.append('nonfinite-or-nonnumeric-metric:'+name);continue
        values[name]=value
    for name in ['Timestamp','ThreadTime']:
        if name not in values:reasons.append('missing-valid-required-metric:'+name)
        elif values[name]<0:reasons.append('negative-required-metric:'+name)
    return values,reasons

def thread_time_metrics_interval(name,from_stage,to_stage,samples):
    result={'name':name,'fromStage':from_stage,'toStage':to_stage,'status':'unavailable',
        'unavailableReasons':[],'wallSeconds':None,'rendererMainThreadCpuSeconds':None,'cpuToWallRatio':None}
    endpoints=[]
    for stage in [from_stage,to_stage]:
        found=[s for s in samples if s.get('stage')==stage]
        if len(found)!=1:
            result['unavailableReasons'].append(('missing-stage:' if not found else 'duplicate-stage:')+stage)
        else:
            endpoints.append(found[0])
            result['unavailableReasons'] += [stage+':'+r for r in found[0].get('unavailableReasons',[])]
            if found[0].get('status')!='available' and not found[0].get('unavailableReasons'):
                result['unavailableReasons'].append('unavailable-stage:'+stage)
    if result['unavailableReasons']:return result
    before,after=endpoints
    if (not before.get('sessionId') or before['sessionId']!=after.get('sessionId') or
        before.get('seat')!=after.get('seat')):
        result['unavailableReasons'].append('session-or-seat-mismatch')
    if after['sampleIndex']<=before['sampleIndex']:
        result['unavailableReasons'].append('boundary-order-invalid')
    if result['unavailableReasons']:return result
    wall=after['values']['Timestamp']-before['values']['Timestamp']
    cpu=after['values']['ThreadTime']-before['values']['ThreadTime']
    for metric,delta in [('Timestamp',wall),('ThreadTime',cpu)]:
        if before['values'][metric]<=0 or after['values'][metric]<=0:
            result['unavailableReasons'].append('zero-or-invalid-endpoint:'+metric)
        if delta<0:result['unavailableReasons'].append('counter-reset:'+metric)
        elif delta==0:result['unavailableReasons'].append('counter-did-not-grow:'+metric)
        elif not math.isfinite(delta):result['unavailableReasons'].append('nonfinite-delta:'+metric)
    if result['unavailableReasons']:return result
    ratio=cpu/wall
    if not math.isfinite(ratio):
        result['unavailableReasons'].append('nonfinite-cpu-to-wall-ratio');return result
    # Metrics are not read atomically; do not clamp or gate the ratio at 1.
    result.update(status='available',wallSeconds=wall,rendererMainThreadCpuSeconds=cpu,cpuToWallRatio=ratio)
    return result

class ThreadTimeMetrics:
    """Four opt-in boundaries on the retained original throttle CDPSession."""
    def __init__(self,seat,session_id,session):
        self.session=session
        self.report={'seat':seat,'sessionId':session_id,'timeDomain':'timeTicks',
            'originalThrottleSession':True,'enable':None,'samples':[],'counterResets':[]}
        self.previous=None

    def _request(self,stage,method,params=None,permitted=True):
        entry={'seat':self.report['seat'],'sessionId':self.report['sessionId'],'stage':stage,
            'method':method,'params':params or {},'attempted':permitted,
            'hostBeforeMonotonicMs':None,'hostAfterMonotonicMs':None,'rawResponse':None,'error':None}
        response=None
        if permitted:
            entry['hostBeforeMonotonicMs']=time.monotonic()*1000
            try:response=self.session.send(method,params or {})
            except Exception as error:entry['error']=type(error).__name__+': '+str(error)[:600]
            finally:entry['hostAfterMonotonicMs']=time.monotonic()*1000
            entry['rawResponse']=thread_time_metrics_raw(response)
        return entry,response

    def enable(self):
        if self.report['enable'] is not None:return
        entry,response=self._request('enable','Performance.enable',{'timeDomain':'timeTicks'})
        entry['acknowledged']=entry['error'] is None and isinstance(response,dict)
        entry['unavailableReasons']=[] if entry['acknowledged'] else ['performance-enable-failed-or-invalid-response']
        self.report['enable']=entry

    def sample(self,stage):
        duplicate=any(s['stage']==stage for s in self.report['samples'])
        enabled=bool(self.report['enable'] and self.report['enable']['acknowledged'])
        entry,response=self._request(stage,'Performance.getMetrics',permitted=enabled and not duplicate)
        entry['sampleIndex']=len(self.report['samples'])
        reasons=[];values={}
        if duplicate:reasons.append('duplicate-stage:'+stage)
        if not enabled:reasons.append('performance-enable-unavailable')
        if entry['error']:reasons.append('get-metrics-failed:'+entry['error'])
        if entry['attempted'] and not entry['error']:
            values,invalid=thread_time_metrics_values(response);reasons+=invalid
        if not reasons:
            if self.previous is not None:
                previous_stage,previous_values=self.previous
                reset=[k for k in ['Timestamp','ThreadTime'] if values[k]<previous_values[k]]
                if reset:self.report['counterResets'].append({'fromStage':previous_stage,'toStage':stage,
                    'metrics':reset,'before':{k:previous_values[k] for k in reset},'after':{k:values[k] for k in reset}})
            self.previous=(stage,values)
            for name in ['Timestamp','ThreadTime']:
                if values[name]==0:reasons.append('zero-required-counter:'+name)
        # A detected reset invalidates later intervals on this session; no
        # silent rebasing or extra reads are used to manufacture a baseline.
        if self.report['counterResets']:reasons.append('counter-reset-since-enable')
        entry.update(values=values,unavailableReasons=reasons,status='unavailable' if reasons else 'available')
        self.report['samples'].append(entry)

def finish_thread_time_metrics(diagnostic,requested_seats,include_probe):
    pairs=[('native-window','before-native','loops-complete')]
    if include_probe:pairs.append(('post-fence-probe','before-post-fence-probe','after-post-fence-probe'))
    diagnostic['expectedStages']=[stage for _,a,b in pairs for stage in [a,b]]
    seats=[peer['seat'] for peer in diagnostic['peers']]
    diagnostic['unavailableReasons']=['missing-seat:'+str(seat) for seat in range(requested_seats) if seat not in seats]
    diagnostic['unavailableReasons']+=['duplicate-seat:'+str(seat) for seat in set(seats) if seats.count(seat)!=1]
    available=0
    for peer in diagnostic['peers']:
        peer['intervals']=[thread_time_metrics_interval(name,a,b,peer['samples']) for name,a,b in pairs]
        count=sum(v['status']=='available' for v in peer['intervals']);available+=count
        peer['status']='available' if count==len(pairs) else 'partial' if count else 'unavailable'
        peer['unavailableReasons']=[v['name']+':'+reason for v in peer['intervals'] for reason in v['unavailableReasons']]
        diagnostic['unavailableReasons']+=['seat-'+str(peer['seat'])+':'+reason for reason in peer['unavailableReasons']]
    diagnostic['availableIntervals']=available
    diagnostic['requestedIntervals']=requested_seats*len(pairs)
    diagnostic['status']=('available' if available==diagnostic['requestedIntervals'] and
        not diagnostic['unavailableReasons'] else 'partial' if available else 'unavailable')


def fixed_work_calibration_fingerprint(probe,seat):
    assert isinstance(probe,dict) and probe.get('schema')==1 and probe.get('completed') and probe.get('passed'), 'Incomplete fixed work'
    assert probe.get('seat')==seat and not probe.get('errors') and probe.get('threadTimeMeasured') is False, 'Fixed work scope changed'
    assert probe.get('boundary')=='post-state-audio-fence/pre-bulk', 'Fixed work boundary changed'
    origin=probe.get('timeOriginMs')
    assert isinstance(origin,(int,float)) and math.isfinite(origin) and probe.get('finalTimeOriginMs')==origin, 'Fixed work realm changed'
    policy=probe['policy']
    assert [policy.get(k) for k in ['iterations','warmupGroups','measurementGroups','initialSeed','indexMultiplier']]==[2000000,2,5,123456789,0x9e3779b9], 'Fixed work policy changed'
    module=probe['module']
    assert module.get('byteLength')==109 and module.get('imports')==[] and module.get('definedMemories')==0, 'Fixed work module changed'
    assert module.get('exports')==[{'name':'kernel','kind':'function'}] and module.get('sectionIds')==[1,3,7,10], 'Fixed work module changed'
    groups=[]
    for phase,count,offset in [('warmup',2,0),('measurements',5,2)]:
        rows=probe[phase]['groups']
        assert len(rows)==count, 'Fixed work group count changed'
        for index,row in enumerate(rows):
            order=['js','wasm'] if index%2 else ['wasm','js']
            seed=(123456789+(index+offset)*0x9e3779b9)&0xffffffff
            assert [row.get(k) for k in ['phase','index','globalIndex','iterations','seed','order']]==[phase,index,index+offset,2000000,seed,order], 'Fixed work inputs changed'
            runs=row['runs']
            assert len(runs)==2 and [r.get('engine') for r in runs]==order and row.get('matches') is True, 'Fixed work engines differ'
            assert all(r.get('completed') and isinstance(r.get('result'),int) and 0<=r['result']<=0xffffffff and
                       isinstance(r.get('wallMs'),(int,float)) and math.isfinite(r['wallMs']) and r['wallMs']>=0 for r in runs), 'Invalid fixed work result'
            assert runs[0]['result']==runs[1]['result'], 'Fixed work checksums differ'
            groups.append([phase,index,seed,order,[r['result'] for r in runs]])
    return {'policy':policy,'module':module,'groups':groups}


def run_fixed_work_rate_calibration(enabled,report,pages,sessions,native_done,fence_passed):
    # Never reset the throttler before the original 4x probes. Reusing those
    # probes is essential: even a redundant rate command could disturb a slow state.
    if not enabled:return None
    diagnostic={'schema':1,'completed':False,'passed':False,'errors':[],'peers':[],
        'scope':'Post-fence diagnostic on the original loaded pages and original throttle CDP sessions. '
                'Original 4x probe, then 1x and restored 4x; fresh private module/instance for each probe. '
                'Wall times only; no performance threshold, gameplay correction or JIT/tiering exclusion.',
        'originalProbeSource':'fixedWorkProbe','rateOrder':[4,1,4]}
    report['fixedWorkRateCalibration']=diagnostic
    context_expression='''()=>({done:multiplayerSmoke.performanceDone(),
        timeOriginMs:performance.timeOrigin,nowMs:performance.now(),
        visibility:document.visibilityState,hidden:document.hidden,focused:document.hasFocus()})'''
    try:
        assert native_done and fence_passed, 'Calibration requires completed native loops and final state/audio fence'
        count=len(pages)
        assert count and len(sessions)==count and report.get('cpuRates')==[4]*count, 'Calibration requires original 4x sessions'
        assert len({id(s) for s in sessions})==count, 'Calibration requires distinct original seat sessions'
        baselines=report.get('fixedWorkProbe',[])
        assert len(baselines)==count and len(report.get('cpuThrottle',[]))==count, 'Calibration requires every original probe'
        fingerprints=[]
        # Check BOTH pages before the first mutation; a partial baseline is not a calibration.
        for seat,page in enumerate(pages):
            throttle=report['cpuThrottle'][seat]
            assert throttle.get('seat')==seat and throttle.get('rate')==4 and throttle.get('acknowledged'), 'Original throttle not acknowledged'
            fingerprints.append(fixed_work_calibration_fingerprint(baselines[seat],seat))
            context=call(page,context_expression)
            assert context['done'] and context['timeOriginMs']==baselines[seat]['timeOriginMs'], 'Calibration page no longer at stopped fence'
            diagnostic['peers'].append({'seat':seat,'originalThrottleSession':True,
                'sessionId':throttle.get('sessionId'),'beforeContext':context,'afterContext':None,
                'originalProbeIndex':seat,'commands':[],'segments':[],'errors':[],
                'restored4xAcknowledged':False,'completed':False,'passed':False})
        for seat,page in enumerate(pages):
            peer=diagnostic['peers'][seat];session=sessions[seat]
            may_have_changed=False;restored=False
            def set_rate(rate,stage):
                command={'stage':stage,'method':'Emulation.setCPUThrottlingRate','params':{'rate':rate},
                    'hostBeforeMonotonicMs':time.monotonic()*1000,'hostAfterMonotonicMs':None,
                    'rawResponse':None,'error':None,'acknowledged':False}
                peer['commands'].append(command)
                try:
                    response=session.send(command['method'],command['params'])
                    command['rawResponse']=thread_time_metrics_raw(response)
                    assert isinstance(response,dict), 'Malformed CPU rate acknowledgment'
                    command['acknowledged']=True
                except Exception as error:
                    command['error']=repr(error);raise
                finally:command['hostAfterMonotonicMs']=time.monotonic()*1000
            def probe_at_rate(rate,stage):
                segment={'stage':stage,'rate':rate,'probe':None,
                    'hostBeforeMonotonicMs':time.monotonic()*1000,'hostAfterMonotonicMs':None,'error':None}
                peer['segments'].append(segment)
                try:
                    probe=call(page,'options=>multiplayerSmoke.fixedWorkProbe(options)',{
                        'enabled':True,'boundary':'post-state-audio-fence/pre-bulk',
                        'seat':seat,'label':'rate-calibration-'+stage+'-seat-'+str(seat)})
                    segment['probe']=probe
                    assert fixed_work_calibration_fingerprint(probe,seat)==fingerprints[seat], 'Calibration fixed work changed'
                    assert probe['timeOriginMs']==baselines[seat]['timeOriginMs'], 'Calibration realm changed'
                    return probe
                except Exception as error:
                    segment['error']=repr(error);raise
                finally:segment['hostAfterMonotonicMs']=time.monotonic()*1000
            try:
                # Mark this BEFORE sending: a failed CDP reply may follow a real rate change.
                may_have_changed=True
                set_rate(1,'set-1x')
                probe_at_rate(1,'1x')
                set_rate(4,'restore-4x')
                restored=True
                probe_at_rate(4,'restored-4x')
                context=call(page,context_expression);peer['afterContext']=context
                assert context['done'] and context['timeOriginMs']==baselines[seat]['timeOriginMs'], 'Calibration fence or realm changed'
                peer['completed']=peer['passed']=True
            except Exception as error:
                peer['errors'].append(repr(error));raise
            finally:
                if may_have_changed and not restored:
                    try:
                        set_rate(4,'finally-restore-4x');restored=True
                    except Exception as error:peer['errors'].append('Final restore failed: '+repr(error))
                peer['restored4xAcknowledged']=restored
        diagnostic['completed']=True
        diagnostic['passed']=all(p['passed'] and p['restored4xAcknowledged'] for p in diagnostic['peers'])
        return diagnostic
    except Exception as error:
        diagnostic['errors'].append(repr(error));raise

def gap_tail_option(seat):
    if args.network_gap_tail=='default':return None
    return args.network_gap_tail=='on' or (args.network_gap_tail=='seat0-on' and seat==0)
def tail_presentation_option(seat):
    if args.network_tail_presentation=='default':return None
    return args.network_tail_presentation=='on' or (args.network_tail_presentation=='seat0-on' and seat==0)
def distribution(values):
    values=sorted(values)
    if not values:return None
    def pct(p):return values[min(len(values)-1,math.ceil(len(values)*p)-1)]
    return {'count':len(values),'p50':pct(.5),'p95':pct(.95),'p99':pct(.99),'max':values[-1],
            'over25ms':sum(v>25 for v in values),'over50ms':sum(v>50 for v in values),
            'over100ms':sum(v>100 for v in values)}
def summarize_callback_work(data,anchors,rows):
    phases=['forwardCapture','historyCapture','forwardUpdate','historyUpdate',
            'forwardSemanticDraw','historySemanticDraw','restore','confirmedCommit']
    schema=['schema','enabled','valid','sequence','epoch','owner','generation',
            'frameBefore','frameAfter','exitReason','nativeInputAndStepMs','residualMs',
            'forwardElidedCalls','historyElidedCalls','diagnosticFlags','reserved']
    schema += [phase+suffix for phase in phases for suffix in ['Calls','Ms']]
    assert data.get('callbackTimingEnabled'), 'Work subphases require the original callback timing'
    assert data.get('callbackWorkTimingSchema')==schema, 'Incomplete work timing schema'
    fields=data['rowSchema']
    col={name:fields.index('callbackWorkTiming.'+name) for name in schema}
    old={name:fields.index('callbackTiming.'+name) for name in data['callbackTimingSchema']}
    integer_fields=[name for name in schema if not name.endswith('Ms')]
    for row in anchors:
        assert len(row)==len(fields), 'Incomplete work timing row'
        assert all(type(row[col[name]]) in (int,float) and math.isfinite(row[col[name]]) for name in schema), 'Non-finite or non-numeric work timing'
        assert all(int(row[col[name]])==row[col[name]] and 0<=row[col[name]]<=2**53-1 for name in integer_fields), 'Inexact work timing identity/count'
        assert all(row[col[name]]<=0xffffffff for name in ['generation','frameBefore','frameAfter']), 'Work timing u32 identity overflow'
        assert [row[col[name]] for name in ['schema','enabled','valid']]==[1,1,1], ('Invalid work timing publication',row[col['sequence']],row[col['diagnosticFlags']])
        assert row[col['diagnosticFlags']]==0 and row[col['reserved']]==0, 'Work timing flags or reserved data'
        assert row[col['sequence']]==row[old['sequence']], 'Work sample belongs to another callback'
        assert row[col['owner']]==data.get('callbackWorkTimingOwner') and row[col['owner']]>0, 'Work timing app identity mismatch'
        assert row[col['frameAfter']]==(int(row[2])&0xffffffff), 'Work timing frame mismatch'
        assert row[col['exitReason']]==row[old['exitReason']], 'Work timing exit mismatch'
        assert row[col['nativeInputAndStepMs']]>=0, 'Negative work input interval'
        assert abs(row[col['nativeInputAndStepMs']]-row[old['nativeInputAndStepMs']])<=1.e-6, 'Work timing input interval mismatch'
        values=[row[col[phase+'Ms']] for phase in phases]
        assert all(value>=0 for value in values), 'Negative work subphase'
        assert all(row[col[phase+'Calls']]>0 or row[col[phase+'Ms']]==0 for phase in phases), 'Work subphase duration without a call'
        residual=row[col['residualMs']]
        assert residual>=-1.e-6, ('Overlapping work subphases',row[col['sequence']],residual)
        assert abs(sum(values)+residual-row[old['nativeInputAndStepMs']])<=1.e-6, 'Work subphase accounting mismatch'
    assert len({row[col['epoch']] for row in anchors})==1 and len({row[col['generation']] for row in anchors})==1, 'Work interval crossed loop or generation'
    assert data.get('callbackWorkTimingDisableResult')==0, 'Work timing was not disabled after loop completion'
    def describe(selected):
        total=sum(row[col['nativeInputAndStepMs']] for row in selected)
        detail={}
        for phase in phases:
            calls=sum(row[col[phase+'Calls']] for row in selected)
            ms=sum(row[col[phase+'Ms']] for row in selected)
            detail[phase]={'calls':calls,'totalMs':ms,'meanMsPerCall':ms/calls if calls else None,
                           'callbackMs':distribution([row[col[phase+'Ms']] for row in selected])}
        return {'callbacks':len(selected),'nativeInputAndStepTotalMs':total,
                'nativeInputAndStepCallbackMs':distribution([row[col['nativeInputAndStepMs']] for row in selected]),
                'residualTotalMs':sum(row[col['residualMs']] for row in selected),
                'residualCallbackMs':distribution([row[col['residualMs']] for row in selected]),
                'forwardElidedCalls':sum(row[col['forwardElidedCalls']] for row in selected),
                'historyElidedCalls':sum(row[col['historyElidedCalls']] for row in selected),'phases':detail}
    groups={}
    correcting=fields.index('nativeCorrecting')
    for previous,row in zip(anchors,anchors[1:]):
        replayed=row[old['cumulativeResimulatedFrames']]-previous[old['cumulativeResimulatedFrames']]
        rolled=row[old['cumulativeRollbacks']]-previous[old['cumulativeRollbacks']]
        assert replayed>=0 and rolled>=0, 'Work classification counters regressed'
        recovered=bool(replayed or rolled or previous[correcting])
        if row[correcting]:kind='partial-correction'
        elif row[old['completed']]>0:kind='complete-recovery-plus-forward' if recovered else 'ordinary-forward'
        else:kind='complete-recovery-no-forward' if recovered else 'no-forward-no-correction'
        groups.setdefault(kind,[]).append(row)
    return {'scope':'Original contiguous measured callback bounds, excluding the lower anchor. Eight non-overlapping wall-time subphases inside native InputAndStep; Capture includes journal metadata/copy, Update/Draw include their own first writes; Draw is draw_all core only. ConfirmedCommit covers explicit outer audio/replay/retire calls. Residual includes input, snapshots, flushes, frame bookkeeping and diagnostic overhead. No journal-only CPU attribution.',
            'schema':schema,'all':describe(rows),'groups':{kind:describe(group) for kind,group in groups.items()}}

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
    if data.get('callbackTimingEnabled'):
        fields=data['rowSchema']
        index=lambda name:fields.index('callbackTiming.'+name)
        col={name:index(name) for name in data['callbackTimingSchema']}
        timing_version=data.get('callbackTimingVersion',1)
        assert timing_version in [1,2],('Unsupported native callback timing schema',timing_version)
        assert len(data['callbackTimingSchema'])==(25 if timing_version==2 else 19),'Incomplete callback timing schema'
        assert all(r[col['schema']]==timing_version and r[col['finishCalled']]==1 for r in anchors),'Missing current native callback timing'
        assert all(b[col['sequence']]>a[col['sequence']] for a,b in zip(anchors,anchors[1:])), 'Stale callback timing sample'
        assert all(abs(r[col['nativeTotalMs']]-r[1])<.01 for r in anchors),'Phase sample belongs to another callback'
        phase_names=['networkPrepareMs','nativeInputAndStepMs','flushPresentationMs','audioPumpMs','loadingMs','replaySeekBatchMs']
        phases={name:{'totalMs':sum(r[col[name]] for r in rows),'callbackMs':distribution([r[col[name]] for r in rows])} for name in phase_names}
        ordinary=[];historical=[]
        for previous,row in zip(anchors,anchors[1:]):
            replayed=row[col['cumulativeResimulatedFrames']]-previous[col['cumulativeResimulatedFrames']]
            assert replayed>=0,'Resimulation counter regressed'
            if replayed:historical.append(row)
            elif row[col['completed']]>0:ordinary.append(row)
        result['callbackPhases']={'scope':'same contiguous measured callbacks; opt-in native timers, no world scans',
            'phases':phases,'completedForwardTicks':sum(r[col['completed']] for r in rows),
            'rollbacksInWindow':last[col['cumulativeRollbacks']]-first[col['cumulativeRollbacks']],
            'resimulatedFramesInWindow':last[col['cumulativeResimulatedFrames']]-first[col['cumulativeResimulatedFrames']],
            'ordinaryForwardCallbackMs':distribution([r[1] for r in ordinary]),
            'callbacksWithResimulationMs':distribution([r[1] for r in historical]),
            'maximumSingleStepMs':max(r[col['maxSingleApplicationStepMs']] for r in rows),
            'accountedPhaseTotalMs':sum(v['totalMs'] for v in phases.values()),
            'nativeTotalMs':sum(r[1] for r in rows)}
    if data.get('callbackWorkTimingEnabled'):
        result['callbackWorkPhases']=summarize_callback_work(data,anchors,rows)
    counters0=data.get('initialRendererCounters',{});counters1=data.get('finalRendererCounters',{})
    if counters0 and counters1:
        counter_delta={'scope':'whole live native-loop interval, including warmup and tail; not the trimmed timing window'}
        for key in ['renderer']:
            a,b=counters0.get(key),counters1.get(key)
            if a is not None and b is not None:counter_delta[key]=[(y-x)&0xffffffff for x,y in zip(a,b)]
        a,b=counters0.get('audio'),counters1.get('audio')
        if a is not None and b is not None:
            counter_delta['audio']={name:(b[i]-a[i])&0xffffffff for name,i in
                [('pumps',3),('mixedFrames',4),('emptyChecks',6),('advancedAudioMillis',11)]}
            counter_delta['audio']['finalQueuedFrames']=b[5]
            counter_delta['audio']['finalMinimumQueuedFrames']=b[7]
            counter_delta['audio']['error']=b[8]
        a,b=counters0.get('modelQuad'),counters1.get('modelQuad')
        if a is not None and b is not None:
            assert a[0]==b[0]==1 and a[1]==b[1],'Model-quad option changed during measurement'
            delta=[(y-x)&0xffffffff for x,y in zip(a[2:7],b[2:7])]
            eligible,uploads,hits,uploaded,reused=delta
            assert eligible==uploads+hits and uploaded==uploads*80 and reused==hits*80,'Model-quad counters disagree'
            if data.get('requestedModelQuadResidency') is not None:
                assert b[1]==int(data['requestedModelQuadResidency']),'Requested model-quad option did not apply'
            counter_delta['modelQuad']={'enabled':bool(b[1]),'eligible':eligible,'uploads':uploads,'hits':hits,
                'uploadedBytes':uploaded,'reusedBytes':reused,'residentBytes':b[7],
                'hitRate':hits/eligible if eligible else None}
        a,b=counters0.get('cameraMath'),counters1.get('cameraMath')
        if a is not None and b is not None:
            assert len(a)==len(b)==19 and a[:4]==b[:4] and b[0]==1,'Camera math mode changed during measurement'
            assert all(math.isfinite(v) and int(v)==v and 0<=v<=2**53-1 for v in a+b),'Inexact camera math counters'
            if data.get('requestedCameraMathCache') is not None:
                assert b[1]==int(data['requestedCameraMathCache']),'Requested camera math option did not apply'
            assert b[3]==data.get('cameraMathVerifyEvery',0),'Camera verification interval did not apply'
            camera={'enabled':bool(b[1]),'supported':bool(b[2]),'verifyEvery':b[3],
                    'invalidations':b[4]-a[4]}
            for label,at in [('lookAt',5),('perspective',12)]:
                delta=[y-x for x,y in zip(a[at:at+7],b[at:at+7])]
                calls,hits,misses,evictions,bypasses,verified,mismatches=delta
                assert all(v>=0 for v in delta),'Camera math counters regressed'
                assert calls==hits+misses+bypasses and evictions<=misses and verified<=hits,'Camera counter partition failed'
                assert b[at+6]==0 and mismatches==0,('Camera math reference mismatch',label,data.get('cameraMathFirstMismatch'))
                if b[1] and b[3]>0:assert verified>0,('Requested numeric verification never ran',label)
                camera[label]=dict(zip(['calls','hits','misses','evictions','bypasses','verified','mismatches'],delta))
                camera[label]['hitRate']=hits/(hits+misses) if hits+misses else None
            counter_delta['cameraMath']=camera
        a,b=counters0.get('packedOccupancyScan'),counters1.get('packedOccupancyScan')
        if a is not None and b is not None:
            assert a==b and b in [0,1],'Packed occupancy mode changed during measurement'
            if data.get('requestedPackedOccupancyScan') is not None:
                assert b==int(data['requestedPackedOccupancyScan']),'Requested packed occupancy option did not apply'
            counter_delta['packedOccupancyScan']=bool(b)
        a,b=counters0.get('initialCaptureBatch'),counters1.get('initialCaptureBatch')
        if a is not None and b is not None:
            assert len(a)==len(b)==14 and a[0]==b[0]==1 and a[1:3]==b[1:3],'Initial capture batch schema/option changed'
            assert all(math.isfinite(v) and int(v)==v and 0<=v<=2**53-1 for v in a+b),'Inexact initial capture counters'
            for values in [a,b]:
                assert values[3:6]==[0,0,0],'Initial capture escaped its synchronous scope'
                assert values[7]==values[9] and values[8]==values[10],'Initial capture payload not fully copied'
                assert 0<=values[12]<=values[11]<=values[7] and values[13]<=values[7],'Initial capture counter partition failed'
            if data.get('requestedInitialCaptureBatch') is not None:
                assert b[1]==int(data['requestedInitialCaptureBatch']),'Requested initial capture option did not apply'
            delta=[y-x for x,y in zip(a[6:],b[6:])]
            assert all(v>=0 for v in delta),'Initial capture counters regressed'
            names=['scopes','deferredBlocks','deferredBytes','copiedBlocks','copiedBytes','flushes','batchImports','maxBlocksPerFlushIncrease']
            batch={'enabled':bool(b[1]),'supported':bool(b[2]),**dict(zip(names,delta)),
                   'maxBlocksPerFlush':b[13]}
            if b[1]:
                assert batch['scopes']>0 and 0<batch['batchImports']<batch['deferredBlocks'],'Initial capture batching never exercised'
            else:
                assert all(v==0 for v in delta),'Disabled initial capture batching did work'
            counter_delta['initialCaptureBatch']=batch
        result['nativeCounterDeltas']=counter_delta
    if len(first)>18 and first[14] is not None:
        presented=[r for r in rows if r[17]]
        fractional=[r for r in presented if r[14] and r[15] and 0<r[16]<1]
        result['nativePresentation']={'highRefreshCallbacks':sum(bool(r[14]) for r in rows),
            'presentedCallbacks':len(presented),'fractionalPresentedCallbacks':len(fractional),
            'highRefreshInterpolationObserved':bool(fractional),
            'scope':'native high-refresh, alpha and submission flags; final object motion remains unmeasured'}
    live=data.get('liveMotion') or {}
    if live.get('enabled'):
        result['liveMotion']={k:live.get(k) for k in ['scope','stats','windows','dropped','errors','nativeTextureGuards']}
        # Full observations and every comparison remain once in data.liveMotion.
        # Console/summary/error copies must not duplicate tens of MB of evidence.
        analysis=live.get('analysis') or {}
        result['liveMotion']['analysis']={k:analysis.get(k) for k in ['passed','captureComplete','coverage','accounting','errors']}
        result['liveMotion']['analysis']['violationCount']=len(analysis.get('violations',[]))
        result['motionContinuityAcceptance']='finite live object windows passed' if (live.get('analysis') or {}).get('passed') else 'live object windows incomplete or failed'
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
    net_log_directory=None
    if args.browser_net_log:
        net_log_directory=args.output.resolve().with_name(args.output.name+'.browser-netlog')
        net_log_directory.mkdir(exist_ok=False)
        report['browserNetLog']={'schema':'th10-browser-net-log/1','enabled':True,
            'captureModeRequested':'Default','maxSizeMiBRequested':64,'peers':[],
            'collectionStage':'after-owned-browser-close',
            'scope':'Chromium network events; not wire delivery or acknowledgment',
            'completeness':'Unproven: Chromium queue/file bounds can omit events without a lost-event counter; JSON parsing is not a lossless guarantee',
            'clockScope':'Raw event TimeTicks milliseconds plus recorded timeTickOffset is an approximate epoch projection; retain both and compare broad intervals, not microsecond wire delay'}
    def launch_browser(seat):
        flags=['--disable-background-timer-throttling','--disable-renderer-backgrounding',
          '--disable-backgrounding-occluded-windows','--autoplay-policy=no-user-gesture-required']
        options={'headless':not args.headed}
        if args.renderer=='hardware':
            options['channel']='chromium';flags+=['--ignore-gpu-blocklist','--use-gl=angle','--use-angle=d3d11','--enable-gpu']
        else:flags+=['--enable-unsafe-swiftshader','--use-angle=swiftshader']
        if args.browser_channel!='default':options['channel']=args.browser_channel
        if args.browser_net_log:
            log_path=net_log_directory/('seat-'+str(seat)+'.json')
            if log_path.exists():raise FileExistsError('Refusing to overwrite browser NetLog: '+str(log_path))
            flags+=['--log-net-log='+str(log_path),'--net-log-capture-mode=Default','--net-log-max-size-mb=64']
            report['browserNetLog']['peers'].append({'seat':seat,'path':str(log_path),'parsed':False,'error':None})
        report['browserLaunch']={'headless':options['headless'],'channel':options.get('channel','default'),'args':flags}
        report.setdefault('browserLaunches',[]).append({'seat':seat,**report['browserLaunch']})
        return p.chromium.launch(args=flags,**options)
    browser=launch_browser(0)
    pages=[];browsers=[browser];profilers=[];chrome_traces=[]
    # Keep every original throttle session strongly referenced through the
    # post-fence probe and browser teardown, including when metrics are off.
    throttle_sessions=[];thread_metrics={}
    window_layout=WindowLayoutReceipt(args.window_layout,window_work_area,report) if args.window_layout!='default' else None
    if args.thread_time_metrics:
        report['threadTimeMetrics']={'schema':'th10-thread-time-metrics/1','enabled':True,'status':'pending','peers':[],
            'timeDomain':'timeTicks','affectsAcceptance':False,
            'sessionIdScope':'Host-assigned token for each retained original throttle CDPSession; not a renderer PID or wire session ID',
            'hostClockScope':'Python time.monotonic() milliseconds, before and after each synchronous CDP send; independent of page clocks',
            'rawEncoding':'Raw response names/order/values retained; invalid nonfinite floats are tagged as nonFiniteNumber strings',
            'cpuScope':'ThreadTime delta is all work on that renderer main thread, including CDP, native/WASM, JS and audio; not isolated game CPU',
            'wallScope':'Timestamp delta is that renderer monotonic wall time; counters are not sampled atomically',
            'nativeWindowScope':'Before each performanceStart call through host-observed completion of all native loops; includes startup and completion-observation delay',
            'probeScope':'Each same-page post-state/audio-fence probe, before bulk; includes module compilation, warmup, all measured work and CDP control; not per-kernel CPU time',
            'otherCounters':'TaskDuration, ScriptDuration, LayoutDuration, RecalcStyleDuration, V8CompileDuration and DevToolsCommandDuration use wall time in timeTicks mode, overlap and must not be summed; ProcessTime is whole-process CPU and is not game CPU',
            'observerOverhead':'Performance.enable installs task/script instrumentation until the owned browser closes; four boundary reads do not eliminate this observer cost',
            'interpretation':'Preserves existing CPU rates without domain changes, polling or navigation. A usable post-fence probe does not establish that gameplay was smooth.'}
    native_loops_complete=False;final_state_audio_fence_passed=False;primary_error=None
    def collect_browser_evidence(stage):
        values=[]
        for page in pages:
            try:values.append(call(page,'s=>{const e=globalThis.__th10BrowserEvidence;e?.mark(s);return e?.read()??null;}',stage))
            except Exception as error:values.append({'collectionError':str(error)})
        return values
    try:
        report['browser']=browser.version
        room='perf-th10-'+uuid.uuid4().hex[:16]
        if args.chrome_trace:
            spec=importlib.util.spec_from_file_location('th10_chrome_trace',Path(__file__).with_name('chrome-trace.py'))
            assert spec and spec.loader,'Could not load the Chrome trace helper'
            chrome_trace_module=importlib.util.module_from_spec(spec)
            spec.loader.exec_module(chrome_trace_module)
            report['chromeTrace']={'schema':chrome_trace_module.SCHEMA,'enabled':True,'complete':False,
                'requestedSeats':args.players,'independentBrowserProcesses':True,
                'nativeLoopsComplete':False,'finalStateAudioFencePassed':False,'peers':[],
                'drainPolicy':'Stop requests after all native loops complete; stream IO/analysis after final state/audio fence, or after owned pages close on early failure',
                'stopObservation':'Existing 200 ms nominal performanceDone polling plus CDP/host scheduling; not a bounded native callback timestamp'}
        relay=args.relay+'?room='+room+'&run=1'
        script=impairment.read_text(encoding='utf-8')
        observer_script=browser_evidence.read_text(encoding='utf-8')+'\ninstallTh10BrowserEvidence('+json.dumps({'relayHopTrace':args.relay_hop_trace})+');\n'
        for seat in range(args.players):
            # Independent browser processes: do not hide transport/lifetime
            # problems behind two worlds in one runtime or shared GPU process.
            peer_browser=browser if seat==0 else launch_browser(seat)
            if seat:browsers.append(peer_browser)
            if window_layout:
                context=peer_browser.new_context(service_workers='block',viewport={'width':672,'height':520})
            else:
                context=peer_browser.new_context(service_workers='block',viewport={'width':800,'height':600})
            settings={'oneWayMs':args.one_way_ms,'jitterMs':args.jitter_ms,'seed':7135+seat,
                      'dropEvery':args.drop_every,'inputLabel':'th10-input','controlLabel':'th10-control',
                      'spikeFrame':args.spike_frame,
                      'spikeMs':args.spike_ms if args.spike_seat in [-1,seat] else 0,
                      'spikeDelayMs':args.spike_delay_ms}
            if args.transport_mode=='rtc':
                context.add_init_script(observer_script+script+'\nglobalThis.perfImpairment=installRtcInputImpairment('+json.dumps(settings)+');')
            else:
                # The evidence observer sees the real browser constructors
                # first. Then use check-network's proven initial fallback
                # trigger, before the game Module or transport can be created.
                context.add_init_script(observer_script+script+
                    "\nObject.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});"+
                    '\nglobalThis.perfImpairment=installWebSocketInputImpairment('+json.dumps(settings)+');')
            page=context.new_page();pages.append(page)
            if window_layout:window_layout.attach(page,seat)
            page.on('pageerror',lambda error:report['errors'].append(str(error)))
            page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
            setup=call(page,'mode=>multiplayerSmoke.prepareMusic(mode)',args.music_mode)
            report.setdefault('musicSetup',[]).append(setup)
            assert setup['mode']==args.music_mode and setup['preloadedBeforeApplication'],'Music preparation contract'
            if args.music_mode!='off':
                assert setup['files']==18 and setup['totalBytes']>0,'Real music assets missing'
                assert setup['observation'] and setup['observation']['enabled']==1,'Native music source not enabled'
            report.setdefault('identities',[]).append(call(page,'multiplayerSmoke.identity()'))
            call(page,'x=>multiplayerSmoke.startNet(...x)',[[[0,0],[1,1],[0,2]][:args.players],seat,1,1234,0x71456789,0x18273645,args.input_delay,gap_tail_option(seat),tail_presentation_option(seat)])
            setup=call(page,'multiplayerSmoke.networkGapTailStatus()')
            report.setdefault('networkGapTailSetup',[]).append(setup)
            if gap_tail_option(seat) is not None:
                assert setup and setup[0]==1 and setup[1]==int(gap_tail_option(seat)) and setup[2]==0,('Network gap tail was not configured before input',seat,setup)
            presentation_setup=call(page,'multiplayerSmoke.networkTailPresentationStatus()')
            report.setdefault('networkTailPresentationSetup',[]).append(presentation_setup)
            if tail_presentation_option(seat) is not None:
                assert presentation_setup and presentation_setup[0]==1 and presentation_setup[1]==int(tail_presentation_option(seat)) and presentation_setup[2]==0,('Network tail presentation was not configured before input',seat,presentation_setup)
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
            report['browserEvidenceAtHandshake']=collect_browser_evidence('rtc-barrier' if args.transport_mode=='rtc' else 'relay-barrier')
            if args.transport_mode=='rtc':
                assert all(n[13]==1 for n in report['routeAtHandshake']), ('Not RTC at initial barrier',report['routeAtHandshake'])
            else:
                report['routeAtHandshakeScope']='native canStart barrier before the initial confirmed frame fence'
                assert all(len(n)==15 and n[0]==1 and n[13]==2 for n in report['routeAtHandshake']), ('Not WebSocket at initial barrier',report['routeAtHandshake'])
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
            initial=report['initialComparison'][0]
            for other in report['initialComparison'][1:]:
                assert initial['canonical']==other['canonical'],'Initial full 44-word canonical differs'
                assert initial['audio']==other['audio'],'Initial confirmed audio differs'
        if args.verify_model_quad:
            assert not args.cold_start,'Model-quad pixel oracle requires the confirmed initial fence'
            report['modelQuadOracle']=call(pages[0],'multiplayerSmoke.verifyModelQuad()')
            assert report['modelQuadOracle']['passed'],'Model-quad pixel/state oracle failed'
        if args.verify_camera_math:
            assert not args.cold_start,'Camera pixel oracle requires the confirmed initial fence'
            report['cameraMathOracle']=call(pages[0],'multiplayerSmoke.verifyCameraMath()')
            assert report['cameraMathOracle']['passed'],'Camera math pixel/numeric/state oracle failed'
        if args.dense:
            assert args.players==2,'Dense fixture declares two seats'
            for page in pages:assert call(page,'multiplayerSmoke.fixture(7)')
        if args.foreground_seat is not None:
            foreground_request=report['windowLayout']['foregroundRequest']={
                'seat':args.foreground_seat,'hostBefore':time.monotonic()*1000,'hostAfter':None,
                'hostClock':'Python time.monotonic() milliseconds',
                'api':'Playwright Page.bringToFront; activation request only, not OS foreground proof'}
            try:pages[args.foreground_seat].bring_to_front()
            finally:foreground_request['hostAfter']=time.monotonic()*1000
        if window_layout:window_layout.sample('before-native')
        if args.chrome_trace:
            # Start every independent browser trace before any native loop.
            # These small calls never read the recorded event stream.
            for seat,peer_browser in enumerate(browsers):
                trace=chrome_trace_module.ChromeTrace(seat,args.output,room,report['identities'][seat])
                chrome_traces.append(trace)
                report['chromeTrace']['peers'].append(trace.report)
                trace.start(peer_browser)
            for trace,page in zip(chrome_traces,pages):trace.mark(page,'before-native-start')
            assert all(trace.ready for trace in chrome_traces),'Chrome trace could not start on every seat; see chromeTrace.peers errors'
        for seat,page in enumerate(pages):
            rate=cpu_rates[seat]
            verify_touch_here=args.verify_touch_frontier and args.verify_touch_frontier_seat in [None,seat]
            throttle=page.context.new_cdp_session(page)
            throttle_sessions.append(throttle)
            acknowledgement=throttle.send('Emulation.setCPUThrottlingRate',{'rate':rate})
            report['cpuThrottle'].append({'seat':seat,'rate':rate,'acknowledged':True,'response':acknowledgement})
            if args.thread_time_metrics:
                token=room+'-throttle-seat-'+str(seat)
                report['cpuThrottle'][-1]['sessionId']=token
                metrics=ThreadTimeMetrics(seat,token,throttle);thread_metrics[seat]=metrics
                report['threadTimeMetrics']['peers'].append(metrics.report)
                metrics.enable()
            if args.profile and seat==0:
                profiler=page.context.new_cdp_session(page);profiler.send('Profiler.enable')
                profiler.send('Profiler.setSamplingInterval',{'interval':500});profiler.send('Profiler.start')
                profilers.append((seat,profiler))
            if args.transport_mode=='relay':
                report.setdefault('impairmentAtNativeStart',[]).append(call(page,'perfImpairment.stats'))
            if args.thread_time_metrics:thread_metrics[seat].sample('before-native')
            call(page,'o=>multiplayerSmoke.performanceStart(o)',{'targetFrame':(args.frames-1 if args.cold_start else 59+args.frames),'seat':seat,
                  'limit60':not args.unlocked_presentation,'lateStartMs':args.late_start_ms if seat==args.players-1 else 0,
                  'directTouchP1':args.direct_touch_p1,'touchMotionHz':0 if verify_touch_here else args.touch_motion_hz,
                  'touchPath':args.touch_path,
                  'modelQuadResidency':None if args.model_quad_residency=='default' else
                    args.model_quad_residency=='on' or (args.model_quad_residency=='seat0-on' and seat==0),
                  'cameraMathCache':None if args.camera_math_cache=='default' else
                    args.camera_math_cache=='on' or (args.camera_math_cache=='seat0-on' and seat==0),
                  'cameraMathVerifyEvery':args.camera_math_verify_every,
                  'packedOccupancyScan':None if args.packed_occupancy_scan=='default' else
                    args.packed_occupancy_scan=='on' or (args.packed_occupancy_scan=='seat0-on' and seat==0),
                  'initialCaptureBatch':None if args.initial_capture_batch=='default' else
                    args.initial_capture_batch=='on' or (args.initial_capture_batch=='seat0-on' and seat==0),
                  'networkGapTail':gap_tail_option(seat),
                  'networkTailPresentation':tail_presentation_option(seat),
                  'rafDeadlineGuard':None if args.raf_deadline_guard=='default' else
                    args.raf_deadline_guard=='on' or (args.raf_deadline_guard=='seat0-on' and seat==0),
                  'rafDispatchDelayMs':args.raf_dispatch_delay_ms if seat==args.raf_dispatch_delay_seat else 0,
                  'rafDispatchDelayFrame':args.raf_dispatch_delay_frame,
                  'callbackTiming':args.callback_timing,'callbackWorkTiming':args.callback_work_timing,'transportTrace':args.transport_trace,
                  'transportTraceIntervalMs':args.transport_trace_interval_ms,'relayHopTrace':args.relay_hop_trace,
                  'verifyTouchFrontier':verify_touch_here,'inputDelay':args.input_delay,
                  'verifyLiveMotion':args.verify_live_motion and seat==args.verify_live_motion_seat})
        deadline=time.monotonic()+max(120,args.frames/60*6)
        while True:
            done=[call(page,'multiplayerSmoke.performanceDone()') for page in pages]
            if all(done):break
            assert not report['errors'],report['errors']
            assert time.monotonic()<deadline,('Native performance deadline',done)
            pages[0].wait_for_timeout(200)
        native_loops_complete=True
        post_run=report['postRun']={'schemaVersion':1,
            'hostClockScope':'Python time.monotonic() milliseconds in this checker process; independent of every page clock',
            'pageClockScope':'Each completion pageClock and native startedAtPerformanceMs/elapsedMs use that page performance realm; timeOrigin is retained',
            'nativeElapsedScope':'Unchanged driver elapsedMs is recorded after sdl_loop_stop and before final hashing; not an exact native stop timestamp',
            'bulkReportPolicy':'Complete native reports stay in their pages until the final exact state/audio fence, or are recovered after failure',
            'hostMarksMs':{'nativeLoopsObservedComplete':time.monotonic()*1000}}
        if args.chrome_trace:
            report['chromeTrace']['nativeLoopsComplete']=True
            for trace,page in zip(chrome_traces,pages):trace.mark(page,'native-loops-complete')
            # Send all stop requests before performanceResult or final settle.
            # Do not await completion or drain either stream at this boundary.
            for trace in chrome_traces:trace.end()
        if args.thread_time_metrics:
            for metrics in thread_metrics.values():metrics.sample('loops-complete')
        if window_layout:window_layout.sample('loops-complete')
        # Project inside each page. Serializing complete rows/pixels here can
        # spend the active session's confirmed-input watchdog while both native
        # loops are stopped. No measured input or timeout is changed to settle.
        post_run['hostMarksMs']['completionReadStarted']=time.monotonic()*1000
        report['completions']=[call(page,'''()=>{
            const data=multiplayerSmoke.performanceResult();
            if(!data)return null;
            return {done:data.done,passed:data.passed,error:data.error,
                nativeError:data.nativeError,networkError:data.networkError,
                finalNet:data.finalNet,firstForward:data.firstForward,
                startedAtPerformanceMs:data.startedAtPerformanceMs,elapsedMs:data.elapsedMs,
                pageClock:{timeOrigin:performance.timeOrigin,timeMs:performance.now(),epochMs:Date.now()}};
        }''') for page in pages]
        post_run['hostMarksMs']['completionReadCompleted']=time.monotonic()*1000
        if args.chrome_trace:
            for trace,data in zip(chrome_traces,report['completions']):
                trace.report['nativeRunClock']={'scope':'Unmodified driver values in the same page performance realm as the trace markers',
                    'startedAtPerformanceMs':(data or {}).get('startedAtPerformanceMs'),'elapsedMs':(data or {}).get('elapsedMs')}
        for data in report['completions']:
            assert data and data.get('done'),'Missing completed native-loop boundary'
            assert data['passed'],(data.get('error'),data.get('nativeError'),data.get('networkError'))
            if args.cold_start:
                first=data.get('firstForward')
                assert first and first['net'][4]>=args.input_delay,('Frame zero ran before all peers supplied their first real input',first)
                assert first['atMs']>=args.late_start_ms-100,('Gameplay ran before the slow peer started',first)
        if args.transport_mode=='relay' and args.cold_start:
            # Keep cold-start scheduling unchanged: the driver already records
            # the first real forward with its actual native transport status.
            report['routeAtHandshake']=[data['firstForward']['transport'] for data in report['completions']]
            report['routeAtHandshakeScope']='driver first real forward after native handshake; cold start deliberately has no host barrier'
            assert all(len(n)==15 and n[0]==1 and n[13]==2 for n in report['routeAtHandshake']), ('Cold start did not use initial WebSocket fallback',report['routeAtHandshake'])
        for seat,profiler in profilers:
            profile=profiler.send('Profiler.stop')['profile']
            path=args.output.with_name('peer-'+str(seat)+'.cpuprofile')
            path.write_text(json.dumps(profile),encoding='utf-8')
            report.setdefault('profiles',[]).append(str(path))
        report['impairment']=[call(page,'perfImpairment.stats') for page in pages]
        comparison=max(data['finalNet'][3] for data in report['completions'])+16
        post_run['comparisonTarget']=comparison
        post_run['hostMarksMs']['firstSettleStarted']=time.monotonic()*1000
        post_run['settleIterations']=0
        post_run['lastSettleStates']=[None]*len(pages)
        deadline=time.monotonic()+60
        while True:
            states=[]
            post_run['settleIterations']+=1
            for seat,page in enumerate(pages):
                post_run['lastSettleAttempt']={'seat':seat,'hostMonotonicMs':time.monotonic()*1000}
                state=call(page,'t=>multiplayerSmoke.performanceSettle(t)',comparison)
                post_run['lastSettleStates'][seat]=state
                states.append(state)
            if all(n[3]==comparison and n[4]>=comparison and n[5]==-1 for n in states):break
            assert time.monotonic()<deadline,('Final exact fence timeout',states)
            pages[0].wait_for_timeout(5)
        post_run['hostMarksMs']['exactFrameFenceCompleted']=time.monotonic()*1000
        report['comparison']=[call(page,'multiplayerSmoke.performanceFinal()') for page in pages]
        if any(v['canonical'][1]!=report['comparison'][0]['canonical'][1] for v in report['comparison'][1:]):
            report['sceneOwners']=[call(page,'multiplayerSmoke.sceneOwners()') for page in pages]
            report['graphStorage']=[call(page,'multiplayerSmoke.graphStorage()') for page in pages]
            if all(report['sceneOwners']):
                report['sceneDifferences']=[]
                for a,b in zip(report['sceneOwners'][0],report['sceneOwners'][1]):
                    differences=[i for i,(x,y) in enumerate(zip(a['bytes'],b['bytes'])) if x!=y]
                    if differences or len(a['bytes'])!=len(b['bytes']):
                        report['sceneDifferences'].append({'name':a['name'],'pointers':[a['pointer'],b['pointer']],
                           'lengths':[len(a['bytes']),len(b['bytes'])],'count':len(differences),
                           'first':[[i,a['bytes'][i],b['bytes'][i]] for i in differences[:64]]})
        first=report['comparison'][0]
        for final in report['comparison']:
            if final.get('smoothness'):assert final['smoothness'][2]==0, ('Final comparison is still correcting',final['smoothness'])
            if final.get('cameraMath'):
                assert final['cameraMath'][11]==final['cameraMath'][18]==0,('Camera mismatch during final settle',final.get('cameraMathFirstMismatch'))
        for other in report['comparison'][1:]:
            assert first['state'][:2]+first['state'][3:]==other['state'][:2]+other['state'][3:], 'Final authoritative state differs'
            assert first['canonical']==other['canonical'],('Final full 44-word canonical differs',first['canonical'],other['canonical'])
            assert first['audio']==other['audio'],'Confirmed audio history differs'
        final_state_audio_fence_passed=True
        post_run['hostMarksMs']['stateAudioFenceCompleted']=time.monotonic()*1000
        if args.transport_mode=='relay':
            report['routeAtFinalFence']=[call(page,'multiplayerSmoke.transportStatus()') for page in pages]
            report['impairmentAtFinalFence']=[call(page,'perfImpairment.stats') for page in pages]
            assert all(len(n)==15 and n[0]==1 and n[13]==2 for n in report['routeAtFinalFence']), ('Not WebSocket at final exact fence',report['routeAtFinalFence'])
        if args.fixed_work_probe:
            post_run['hostMarksMs']['fixedWorkProbeStarted']=time.monotonic()*1000
            report['fixedWorkProbeScope']='Same loaded pages and existing CPU rates; final state/audio fence already passed, bulk not started. Game trace has already ended and does not cover this probe.'
            report['fixedWorkProbeCpuRates']=cpu_rates[:]
            report['fixedWorkProbe']=[]
            for seat,page in enumerate(pages):
                if args.thread_time_metrics:thread_metrics[seat].sample('before-post-fence-probe')
                try:
                    probe=call(page,'options=>multiplayerSmoke.fixedWorkProbe(options)',{
                        'enabled':True,'boundary':'post-state-audio-fence/pre-bulk',
                        'seat':seat,'label':'post-fence-seat-'+str(seat)})
                finally:
                    if args.thread_time_metrics:thread_metrics[seat].sample('after-post-fence-probe')
                report['fixedWorkProbe'].append(probe)
                assert probe and probe['schema']==1 and probe['completed'] and probe['passed'],('Fixed work probe failed',seat,probe)
                assert not probe['threadTimeMeasured'] and probe['boundary']=='post-state-audio-fence/pre-bulk',('Fixed work probe scope changed',seat)
            post_run['hostMarksMs']['fixedWorkProbeCompleted']=time.monotonic()*1000
        if args.fixed_work_rate_calibration:
            post_run['hostMarksMs']['fixedWorkRateCalibrationStarted']=time.monotonic()*1000
            run_fixed_work_rate_calibration(True,report,pages,throttle_sessions,
                native_loops_complete,final_state_audio_fence_passed)
            post_run['hostMarksMs']['fixedWorkRateCalibrationCompleted']=time.monotonic()*1000
        post_run['bulkReportReadStage']='after-final-state-audio-fence'
        post_run['hostMarksMs']['bulkReportReadStarted']=time.monotonic()*1000
        report['peers']=[call(page,'multiplayerSmoke.performanceResult()') for page in pages]
        post_run['hostMarksMs']['bulkReportReadCompleted']=time.monotonic()*1000
        for seat,(data,imp) in enumerate(zip(report['peers'],report['impairment'])):
            assert data['passed'],data.get('error')
            setup=report['networkGapTailSetup'][seat]
            before,after=data.get('initialNetworkGapTail'),data.get('finalNetworkGapTail')
            if setup is not None:
                assert before and after and before[0]==after[0]==1,('Missing network gap tail observations',seat)
                assert before[1]==after[1]==setup[1] and after[2]==1,('Network gap tail mode changed or did not lock',seat,setup,before,after)
                if not args.cold_start:assert before[2]==1,('Network gap tail did not lock at startup',seat,before)
                assert report['comparison'][seat]['networkGapTail'][1:3]==after[1:3],('Network gap tail changed during final settle',seat)
            else:
                assert before is None and after is None,('Unexpected network gap tail availability change',seat)
            presentation_setup=report['networkTailPresentationSetup'][seat]
            before,after=data.get('initialNetworkTailPresentation'),data.get('finalNetworkTailPresentation')
            if presentation_setup is not None:
                assert before and after and before[0]==after[0]==1,('Missing network tail presentation observations',seat)
                assert before[1]==after[1]==presentation_setup[1] and after[2]==1,('Network tail presentation changed or did not lock',seat,presentation_setup,before,after)
                if not args.cold_start:assert before[2]==1,('Network tail presentation did not lock at startup',seat,before)
                assert report['comparison'][seat]['networkTailPresentation'][1:3]==after[1:3],('Network tail presentation changed during final settle',seat)
            else:
                assert before is None and after is None and report['comparison'][seat].get('networkTailPresentation') is None,('Unexpected network tail presentation availability change',seat)
            before,after=data.get('initialRafDeadlineGuard'),data.get('finalRafDeadlineGuard')
            if before is not None:
                assert after and before[0]==after[0] and after[0] in [1,2],('Missing RAF deadline guard observations',seat)
                assert before[2:4]==[0,0] and after[2:4]==[1,0],('RAF guard startup/stop lock failed',seat,before,after)
                assert before[1]==after[1] and after[6:8]==[0,0] and after[9]==0,('RAF guard changed mode or left pending work',seat,before,after)
                if data.get('requestedRafDeadlineGuard') is not None:
                    assert after[1]==int(data['requestedRafDeadlineGuard']),('Requested RAF guard option did not apply',seat)
                assert report['comparison'][seat]['rafDeadlineGuard'][1:4]==after[1:4],('RAF guard changed during final settle',seat)
                if after[1]==0:
                    assert after[12:21]==[0]*9 and after[30:32]==[0,0],('Disabled RAF guard dispatched work',seat,after)
                else:
                    assert after[12]>0 and after[14]>0,('Enabled RAF guard never serviced a real RAF',seat,after)
                    assert after[16]<=after[15] and after[30]==after[14]+after[16],('RAF guard service accounting failed',seat,after)
                    if after[0]==1:
                        assert after[16]<=after[12],('Schema-1 RAF guard compensated more than once per request',seat,after)
            else:
                assert after is None,('Unexpected RAF guard availability change',seat)
            fault=data.get('rafDispatchDelay')
            expected_delay=args.raf_dispatch_delay_ms if seat==args.raf_dispatch_delay_seat else 0
            assert fault and fault.get('schema')=='th10-raf-dispatch-delay/1',('Missing RAF dispatch fault evidence',seat)
            assert fault.get('enabled')==bool(expected_delay) and fault.get('delayMs')==expected_delay,('RAF dispatch fault mode changed',seat,fault)
            assert fault.get('valid') and not fault.get('errors'),('Invalid RAF dispatch fault evidence',seat,fault)
            if expected_delay:
                assert fault.get('targetFrame')==args.raf_dispatch_delay_frame and fault.get('installed') and fault.get('restored'),('RAF dispatch fault install/restore failed',seat,fault)
                assert fault.get('heldCount')==fault.get('releasedCount')==1 and fault.get('cancelledHeldCount')==0 and fault.get('pendingAfterRestore')==0,('RAF dispatch fault not completed exactly once',seat,fault)
                event=fault.get('event') or {}
                assert all(isinstance(event.get(k),(int,float)) and math.isfinite(event[k]) for k in ['rafTimestamp','arrivedAtMs','heldAtMs','dueAtMs','releasedAtMs','frameAtHold','frameAtRelease','deliveredTimestamp']),('Invalid RAF dispatch event',seat,event)
                assert event['deliveredTimestamp']==event['rafTimestamp'] and event['frameAtHold']>=args.raf_dispatch_delay_frame,('RAF timestamp or target frame changed',seat,event)
                assert event['arrivedAtMs']<=event['heldAtMs']<=event['dueAtMs']<=event['releasedAtMs']+1 and abs(event['dueAtMs']-event['heldAtMs']-expected_delay)<0.01,('RAF hold duration changed',seat,event)
            else:
                assert not fault.get('installed') and fault.get('heldCount')==fault.get('releasedCount')==fault.get('cancelledHeldCount')==fault.get('pendingAfterRestore')==0,('Disabled RAF fault performed work',seat,fault)
            renderer=(data.get('unmaskedRenderer') or '').lower()
            assert renderer,'Renderer identity is required for performance evidence'
            if args.renderer=='hardware':
                assert not any(v in renderer for v in ['swiftshader','software','basic render','warp']), ('Hardware lane silently fell back',renderer)
            else:assert 'swiftshader' in renderer,('Software lane changed',renderer)
            if args.transport_mode=='rtc':
                assert imp['matched']>0 and imp['sent']>0,('Real RTC lane was not impaired',imp)
                assert imp['errors']==0 and imp['overflow']==0,imp
                if args.spike_ms and args.spike_seat in [-1,seat]:
                    assert imp['spikeDelayed']>0,('Requested RTC spike was not observed',seat,imp)
                assert data['transport'][13]==1,('Not real RTC',data['transport'])
            else:
                before=report['impairmentAtNativeStart'][seat]
                after=report['impairmentAtFinalFence'][seat]
                for value in [before,imp,after]:
                    assert value['schema']=='th10-websocket-input-impairment/1' and value['transportMode']=='relay',('Wrong WebSocket impairment',seat,value)
                    assert value['oneWayMs']==args.one_way_ms and value['jitterMs']==args.jitter_ms and value['dropEvery']==args.drop_every,('WebSocket delay settings changed',seat,value)
                    assert value['errors']==value['overflow']==0,('WebSocket impairment failed',seat,value)
                    assert value['matched']==value['broadcastInputs']+value['targetedInputs'],('WebSocket input partition differs',seat,value)
                    assert value['matched']==value['scheduled']+value['dropped']+value['overflow']+value['scheduleErrors'],('WebSocket scheduling partition differs',seat,value)
                    assert value['scheduled']==value['sent']+value['canceled']+value['sendErrors']+value['pendingJobs'],('WebSocket delivery partition differs',seat,value)
                    assert 0<=value['queuedBytes']<=value['maxQueuedBytes'] and 0<=value['pendingJobs']<=value['maxPendingJobs'],('WebSocket queue exceeded bounds',seat,value)
                cumulative=['matched','scheduled','sent','dropped','canceled','broadcastInputs','targetedInputs','ackOnlyInputs',
                    'requestedDelayTotalMs','plannedDelayTotalMs','fifoDelayed','fifoDelayTotalMs','deliveredDelayTotalMs','timerOverrunTotalMs']
                assert all(math.isfinite(v[k]) and v[k]>=0 for v in [before,imp,after] for k in cumulative),('Invalid WebSocket counters',seat)
                assert all(before[k]<=imp[k]<=after[k] for k in cumulative),('WebSocket counters regressed',seat)
                assert all(imp[k]>before[k] for k in ['matched','scheduled','sent']),('Actual WebSocket Input messages were not impaired during native loops',seat,before,imp)
                data['relayImpairmentDuringNativeLoops']={'scope':'Counter differences from just before this native loop start through loop-completion read; includes queued startup deliveries, excludes the later exact fence',
                    **{k:imp[k]-before[k] for k in cumulative}}
                if args.spike_ms and args.spike_seat in [-1,seat]:
                    assert imp['spikeDelayed']>0,('Requested WebSocket Input spike was not observed',seat,imp)
                assert len(data['transport'])==15 and data['transport'][0]==1 and data['transport'][13]==2,('Not real WebSocket at native loop end',data['transport'])
            if args.transport_trace:
                trace=data.get('transportTrace') or {}
                assert trace.get('schema')=='th10-transport-trace/1',('Missing transport trace schema',seat)
                assert trace.get('valid') and trace.get('complete') and trace.get('rows') and trace.get('dropped')==0,('Incomplete transport diagnostic evidence',seat,trace.get('firstReadError'))
                assert trace.get('sampling',{}).get('intervalMs')==args.transport_trace_interval_ms,('Transport sample interval changed',seat)
                hop=trace.get('relayHop')
                if args.relay_hop_trace:
                    assert hop and hop.get('schema')=='th10-relay-hop/1' and hop.get('enabled') and hop.get('valid') and hop.get('complete'),('Incomplete browser relay hop evidence',seat,hop)
                    assert len(hop.get('rows',[]))==len(trace['rows']),('Relay hop sample coverage differs',seat)
                else:assert hop is None,('Unexpected relay hop observer',seat)
            data['summary']=summarize(data)
            if args.music_mode!='off':
                before,after=data.get('initialMusic'),data.get('finalMusic')
                assert before and after,('Missing native music observations',seat)
                for observation in [before,after]:
                    assert observation['schema']==1 and observation['enabled']==1,('Music disabled',seat,observation)
                    assert observation['fullDecode']==int(args.music_mode=='full'),('Wrong native music mode',seat,observation)
                active_observations=[after] if args.cold_start else [before,after]
                for observation in active_observations:
                    assert observation['streams']>0 and observation['waiting']==0 and observation['trackMask']>0,('Music has missing assets or no selected track',seat,observation)
                if args.music_mode=='stream':
                    assert all(value['validDecoders']>0 for value in active_observations),('No real streaming decoder',seat,before,after)
                    assert before['fullReady']==after['fullReady']==0,('Unexpected full decode cache',seat)
                    assert after['decoderFrames']>0 and after['decoderFrames']!=before['decoderFrames'],('Decoder did not progress during native measurement',seat,before,after)
                else:
                    assert all(value['fullReady']>0 for value in active_observations) and after['fullPCMBytes']>0,('No decoded real PCM',seat,before,after)
                assert after['cursorBytes']!=before['cursorBytes'],('Music source did not advance',seat,before,after)
                data['summary']['music']={'mode':args.music_mode,'initial':before,'final':after,
                    'sameTrackMask':before['trackMask']==after['trackMask'],
                    'scope':'Current-selection positions at native-loop boundaries; not cumulative decode work, PCM equality, or audible device output'}
        if args.verify_touch_frontier:
            for seat,data in enumerate(report['peers']):
                if args.verify_touch_frontier_seat not in [None,seat]:continue
                oracle=data.get('touchFrontierOracle') or {}
                assert oracle.get('schemaVersion')==1 and oracle.get('enabled'),('Missing live touch frontier oracle',seat)
                assert oracle.get('passed') and oracle.get('status')=='pass',('Live touch frontier correctness not covered or failed',seat,oracle)
        if args.verify_live_motion:
            live=report['peers'][args.verify_live_motion_seat].get('liveMotion') or {}
            assert live.get('schema')=='th10-live-motion-capture/1' and live.get('enabled'),'Missing real live motion capture'
            assert live.get('nativeStoppedBeforeAnalysis') and live.get('nonblockingGpuWait'),'Live motion measurement boundary changed'
            assert (live.get('nativeTextureGuards') or {}).get('passed'), 'Native texture getter boundary probes incomplete or failed'
            assert not live.get('errors') and live.get('dropped')==0,('Live motion sample loss or error',live.get('stats'),live.get('errors'))
            analysis=live.get('analysis') or {}
            assert analysis.get('passed'),('Live motion continuity not covered or failed',
                {'coverage':analysis.get('coverage'),'captureComplete':analysis.get('captureComplete'),
                 'violations':len(analysis.get('violations',[])),'errors':analysis.get('errors')})
        assert not report['errors'],report['errors']
        assert all(digest(p)==report['sources'][str(p)] for p in sources),'Harness changed during run'
        report['passed']=True
    except BaseException as error:
        primary_error=error
        report['error']=str(error)
        if not report.get('peers'):
            if native_loops_complete:
                # A failed fence must retain the same complete native reports,
                # including the slow/bad interval, without replacing its error.
                post_run=report['postRun']
                post_run['bulkReportReadStage']='failure-after-native-loops'
                post_run['hostMarksMs']['failureBulkReportReadStarted']=time.monotonic()*1000
                report['peers']=[]
                for seat,page in enumerate(pages):
                    try:report['peers'].append(call(page,'multiplayerSmoke.performanceResult()'))
                    except Exception as read_error:
                        report['peers'].append({'seat':seat,'resultReadError':str(read_error)[:600]})
                post_run['hostMarksMs']['failureBulkReportReadCompleted']=time.monotonic()*1000
            else:
                for page in pages:
                    try:report.setdefault('partial',[]).append(call(page,'multiplayerSmoke.performanceResult?.() || multiplayerSmoke.netStatus()'))
                    except Exception:pass
        raise
    finally:
        if args.thread_time_metrics:
            diagnostic=report['threadTimeMetrics']
            diagnostic['nativeLoopsComplete']=native_loops_complete
            diagnostic['finalStateAudioFencePassed']=final_state_audio_fence_passed
            try:finish_thread_time_metrics(diagnostic,args.players,args.fixed_work_probe)
            except Exception as error:
                # Diagnostic parsing/availability must not replace the run's
                # primary outcome or invent a zero CPU measurement.
                diagnostic['status']='unavailable'
                diagnostic.setdefault('unavailableReasons',[]).append('diagnostic-finalization-failed:'+str(error)[:600])
        cleanup_errors=[]
        def cleanup_error(stage,error):
            cleanup_errors.append({'stage':stage,'message':str(error)[:600]})
        try:report['browserEvidence']=collect_browser_evidence('final-collection')
        except BaseException as error:cleanup_error('browser-evidence',error)
        if args.chrome_trace:
            chrome_report=report.setdefault('chromeTrace',{'enabled':True,'complete':False,'peers':[]})
            chrome_report['nativeLoopsComplete']=native_loops_complete
            chrome_report['finalStateAudioFencePassed']=final_state_audio_fence_passed
            if not native_loops_complete:
                # The helper owns browser-level sessions: close active pages
                # before any large failure-path drain, keeping those sessions alive.
                chrome_report['earlyFailurePageCleanup']=True
                for seat,page in enumerate(pages):
                    try:page.close()
                    except BaseException as error:cleanup_error('trace-page-close-seat-'+str(seat),error)
            for trace in chrome_traces:
                try:trace.end()
                except BaseException as error:cleanup_error('trace-end-seat-'+str(trace.seat),error)
            drain_stage='after-final-state-audio-fence' if final_state_audio_fence_passed else 'failure-finally'
            for trace in chrome_traces:
                try:trace.finish(drain_stage)
                except BaseException as error:cleanup_error('trace-finish-seat-'+str(trace.seat),error)
            chrome_report['complete']=(len(chrome_traces)==args.players and
                all(trace.report['passed'] for trace in chrome_traces) and not cleanup_errors)
            if not chrome_report['complete']:
                cleanup_error('chrome-trace','Requested trace is incomplete; see chromeTrace.peers errors and marker/data-loss fields')
        for seat,peer_browser in enumerate(browsers):
            try:peer_browser.close()
            except BaseException as error:cleanup_error('browser-close-seat-'+str(seat),error)
        if args.browser_net_log:
            for entry in report['browserNetLog']['peers']:
                try:
                    path=Path(entry['path'])
                    entry['bytes']=path.stat().st_size
                    if not 0<entry['bytes']<=65*1024*1024:
                        raise ValueError('NetLog file missing, empty or above diagnostic read bound')
                    raw=path.read_bytes()
                    entry['sha256']=hashlib.sha256(raw).hexdigest()
                    log=json.loads(raw)
                    constants,events=log.get('constants'),log.get('events')
                    if not isinstance(constants,dict) or not isinstance(events,list) or not events:
                        raise ValueError('Missing NetLog constants/events')
                    entry['captureModeActual']=constants.get('logCaptureMode')
                    if entry['captureModeActual']!='Default':
                        raise ValueError('NetLog did not report the requested Default capture mode')
                    offset=float(constants['timeTickOffset'])
                    ticks=[float(event['time']) for event in events]
                    if not math.isfinite(offset) or any(not math.isfinite(tick) for tick in ticks):
                        raise ValueError('Invalid NetLog clock')
                    types=constants['logEventTypes']
                    wanted=['WEBSOCKET_SENT_FRAME_HEADER','WEBSOCKET_RECV_FRAME_HEADER',
                        'SOCKET_BYTES_SENT','SOCKET_BYTES_RECEIVED','SOCKET_READ_ERROR','SOCKET_WRITE_ERROR']
                    if not isinstance(types,dict) or any(type(types.get(name)) is not int for name in wanted):
                        raise ValueError('Missing or invalid NetLog event type constants')
                    counts={name:sum(event.get('type')==types.get(name) for event in events) for name in wanted}
                    byte_events={types[name] for name in ['SOCKET_BYTES_SENT','SOCKET_BYTES_RECEIVED',
                        'SSL_SOCKET_BYTES_SENT','SSL_SOCKET_BYTES_RECEIVED'] if name in types}
                    if any(event.get('type') in byte_events and 'bytes' in (event.get('params') or {}) for event in events):
                        raise ValueError('Unexpected raw socket payload in Default-mode NetLog')
                    entry.update({'parsed':True,'events':len(events),'timeTickOffsetRaw':constants['timeTickOffset'],
                        'firstTimeTicksMs':min(ticks),'lastTimeTicksMs':max(ticks),
                        'firstProjectedEpochMs':min(ticks)+offset,'lastProjectedEpochMs':max(ticks)+offset,
                        'eventCounts':counts,'fileBelowRequestedCap':entry['bytes']<64*1024*1024})
                    del raw,log,events,ticks
                except BaseException as error:
                    entry['error']=str(error)[:600]
                    cleanup_error('browser-net-log-seat-'+str(entry['seat']),error)
        if cleanup_errors:
            report['cleanupErrors']=cleanup_errors
            report['passed']=False
            if primary_error is None:report['error']='Post-run diagnostic or cleanup failed; see cleanupErrors and chromeTrace'
        try:
            args.output.parent.mkdir(parents=True,exist_ok=True)
            args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
        except BaseException as error:
            if primary_error is None:raise
            try:print('Report write failed while preserving primary failure: '+str(error)[:600],file=sys.stderr)
            except Exception:pass
        if cleanup_errors and primary_error is None:raise RuntimeError(report['error'])
print(json.dumps({'passed':report['passed'],'summaries':[v['summary'] for v in report['peers']]}))
