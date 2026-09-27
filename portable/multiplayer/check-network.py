"""Production TH08 automatic driver, then shared real RTC/Relay transport."""
import argparse
import json
import time
import uuid
from pathlib import Path
from playwright.sync_api import sync_playwright

p=argparse.ArgumentParser()
p.add_argument('--url',required=True);p.add_argument('--relay',required=True)
p.add_argument('--mode',choices=['packets','rtc','relay'],required=True)
p.add_argument('--players',choices=['all','2','3'],default='all')
p.add_argument('--output',type=Path,required=True)
p.add_argument('--analog',action='store_true')
args=p.parse_args()
report={'passed':False,'mode':args.mode,'analog':args.analog,'cases':[],'errors':[]}
def save():args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
def call(page,code,arg=None):return page.evaluate(code,arg)
def apply(page,wire):
    result=call(page,'p=>multiplayerSmoke.applyWire(p)',wire)
    assert result==0,(result,wire)
def input_for(seat,frame):
    if frame<180:return 0
    if seat==0 and frame in (250,265):return 8
    if seat==0 and 250<frame<290:return 0
    return 1|(64 if ((frame//18+seat)%2) else 128)|(4 if frame%80<40 else 0)|(2 if frame in (215,305) else 0)
def observe(page):
    result={'state':call(page,'multiplayerSmoke.status()'),'net':call(page,'multiplayerSmoke.netStatus()'),
            'driver':call(page,'multiplayerSmoke.driverStatus()'),'native':call(page,'multiplayerSmoke.nativeStatus()')}
    if call(page,"typeof multiplayerSmoke.canonical === 'function'"):result['canonical']=call(page,'multiplayerSmoke.canonical()')
    return result
def capture(page,seat,frame):
    buttons=input_for(seat,frame)
    if frame==0 and seat==0:
        # Live v3 frame zero carries the host's route bootstrap; the native
        # driver strips it before gameplay. Keep the packet fixture current.
        return call(page,'multiplayerSmoke.captureInput(0,0,2,8188,-8192,0)')
    if not args.analog or frame<180 or 250<=frame<290:
        return call(page,'v=>multiplayerSmoke.capture(...v)',[frame,buttons])
    buttons&=~(16|32|64|128)
    if frame<240:
        mode=2;x=1.375 if (frame//9+seat)%2 else -2.625;y=-0.25;flags=2|(4 if buttons&2 else 0)
    elif frame<320:
        mode=1;x=0.375 if seat%2 else -0.625;y=0.125;flags=2|(4 if buttons&2 else 0)
    else:
        mode=2;x=12.125 if (frame//8+seat)%2 else -13.625;y=0;flags=3
    return call(page,'v=>multiplayerSmoke.captureInput(...v)',[frame,buttons,mode,x,y,flags])
def equal(values):
    a=values[0]['state'];assert all(v['state'][:2]+v['state'][3:]==a[:2]+a[3:] for v in values),values
    if 'canonical' in values[0]:assert all(v['canonical']==values[0]['canonical'] for v in values),values
    assert all(not v['driver'][2] for v in values),values
    assert all(v['driver'][6:9]==values[0]['driver'][6:9] for v in values),values

with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        for count in ([2,3] if args.players=='all' else [int(args.players)]):
            contexts=[];pages=[]
            case={'players':count,'passed':False,'phase':'boot','checkpoints':[]};report['cases'].append(case);save()
            try:
                for endpoint in range(count+(1 if args.mode=='packets' else 0)):
                    case['bootEndpoint']=endpoint;case['bootStep']='context';save()
                    seat=endpoint if endpoint<count else 0
                    context=browser.new_context(service_workers='block');contexts.append(context)
                    if args.mode=='relay':context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true})")
                    page=context.new_page();pages.append(page)
                    page.on('pageerror',lambda error:report['errors'].append(str(error)))
                    page.goto(args.url);case['bootStep']='module';save()
                    page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
                    case['bootStep']='native-prepare';save()
                    call(page,'v=>multiplayerSmoke.start(...v)',[1234,list(range(count)),seat,[0x12345678,0x10203040]])
                case['identities']=[call(page,'multiplayerSmoke.identity()') for page in pages]
                oracle=pages.pop() if args.mode=='packets' else None
                if args.mode=='packets':
                    for phase in (1,2):
                        packets=[call(page,'p=>multiplayerSmoke.sessionPacket(p)',phase) for page in pages]
                        for target,page in enumerate(pages):
                            for source,wire in enumerate(packets):
                                if source!=target:apply(page,wire)
                        if phase==1:
                            for page in pages:assert call(page,'multiplayerSmoke.markReady()')
                        for source,wire in enumerate(packets):
                            if source!=0:apply(oracle,wire)
                        if phase==1:assert call(oracle,'multiplayerSmoke.markReady()')
                else:
                    room='audit-'+uuid.uuid4().hex[:14]
                    for page in pages:assert call(page,'u=>multiplayerSmoke.connect(u)',args.relay+'/?room='+room)
                    deadline=time.monotonic()+50
                    while not all(call(page,'multiplayerSmoke.netStatus()')[2] for page in pages):
                        for page in pages:call(page,'multiplayerSmoke.pollNetwork()')
                        assert time.monotonic()<deadline,[observe(page) for page in pages]
                        pages[0].wait_for_timeout(5)
                    expected_route=1 if args.mode=='rtc' else 2
                    assert all(call(page,'multiplayerSmoke.driverStatus()')[12]==expected_route for page in pages)
                case['phase']='gameplay';save()
                for page in pages:
                    probe=call(page,'multiplayerSmoke.practiceWriteProbe()')
                    assert not probe['accepted'] and probe['state'][3]==probe['state'][5]==0,probe
                if args.mode=='packets':
                    queue=[]
                    for frame in range(380):
                        for seat,page in enumerate(pages):assert capture(page,seat,frame)
                        assert capture(oracle,0,frame)
                        for seat in range(1,count):apply(oracle,call(pages[seat],'v=>multiplayerSmoke.inputPacket(...v)',[0,frame,frame+1]))
                        call(oracle,'multiplayerSmoke.ticks(1)')
                        delayed=frame>=180 and frame<360
                        for source,page in enumerate(pages):
                            for target,receiver in enumerate(pages):
                                if source==target:continue
                                wire=call(page,'v=>multiplayerSmoke.inputPacket(...v)',[target,frame,frame+1])
                                queue.append((frame+(4 if delayed else 0),target,wire))
                        due=[v for v in queue if v[0]<=frame];queue=[v for v in queue if v[0]>frame]
                        # Deterministic reordering and duplicate delivery through
                        # the real codec, never through a fake game simulator.
                        for _,target,wire in reversed(due):apply(pages[target],wire);apply(pages[target],wire)
                        for page in pages:call(page,'multiplayerSmoke.ticks(1)')
                        if case['identities'][0]['fixtureBuild'] and frame in (220,240,300,320):
                            before=call(pages[-1],'multiplayerSmoke.canonical()')
                            call(pages[-1],'multiplayerSmoke.presented(0.25)');call(pages[-1],'multiplayerSmoke.presented(0.75)')
                            assert call(pages[-1],'multiplayerSmoke.canonical()')==before,'Render-only draw mutated canonical state'
                        if frame in (119,179,379):
                            values=[observe(page) for page in pages+[oracle]];case['checkpoints'].append({'frame':frame,'values':values});save();equal(values)
                    assert all(v['driver'][3]>0 and v['driver'][4]>0 for v in values[:count]),values
                    assert values[-1]['driver'][3]==0,values
                    images=[call(page,'multiplayerSmoke.frameDigest()') for page in pages+[oracle]]
                    case['framebufferHashes']=images;save();assert len(set(images))==1,images
                    # Exhaust the production driver's real prediction window.
                    # Polling the next frame must freeze Update/Draw but keep
                    # servicing the independent real-time MIDI owner.
                    page=pages[0];assert call(page,'multiplayerSmoke.capture(380,1)')
                    call(page,'multiplayerSmoke.ticks(9)')
                    stalled=observe(page);picture=call(page,'multiplayerSmoke.frameDigest()');audio_before=call(page,'multiplayerSmoke.audioServices()')
                    call(page,'multiplayerSmoke.ticks(8)')
                    after=observe(page);audio_after=call(page,'multiplayerSmoke.audioServices()')
                    assert stalled['net'][3]==after['net'][3]==388 and stalled['canonical']==after['canonical'],(stalled,after)
                    assert picture==call(page,'multiplayerSmoke.frameDigest()')
                    assert stalled['driver'][6:9]==after['driver'][6:9],(stalled,after)
                    if audio_before is not None:assert audio_after-audio_before==8,(audio_before,audio_after)
                    case['boundedStall']={'frame':388,'stateFrozen':True,'imageFrozen':True,'audioServices':None if audio_before is None else audio_after-audio_before}
                else:
                    for target in (119,239,379):
                        deadline=time.monotonic()+120
                        while True:
                            ready=True
                            for seat,page in enumerate(pages):
                                call(page,'multiplayerSmoke.pollNetwork()');n=call(page,'multiplayerSmoke.netStatus()')
                                if n[3]<=target:
                                    # Capture once; waiting never changes this frame.
                                    capture(page,seat,n[3])
                                    call(page,'multiplayerSmoke.ticks(1)')
                                else:call(page,'multiplayerSmoke.reconcile()')
                                n=call(page,'multiplayerSmoke.netStatus()')
                                ready=ready and n[4]==target and n[5]>=target and n[6]==0xffffffff
                            if ready:break
                            assert time.monotonic()<deadline,[observe(page) for page in pages]
                            pages[0].wait_for_timeout(1)
                        values=[observe(page) for page in pages];case['checkpoints'].append({'frame':target,'values':values});save();equal(values)
                if args.mode!='packets':
                    # Native pause -> Restart hotkey -> scene teardown -> new
                    # SessionGate on the SAME live transport. No fixture sets
                    # a stage, run generation or player resource directly.
                    case['phase']='native-restart';save()
                    stale=[call(page,'multiplayerSmoke.sessionPacket(1)') for page in pages]
                    deadline=time.monotonic()+120
                    while True:
                        ready=True
                        for seat,page in enumerate(pages):
                            call(page,'multiplayerSmoke.pollNetwork()');n=call(page,'multiplayerSmoke.netStatus()')
                            if n[10]==0 or n[3]<120:
                                buttons=8 if seat==0 and n[10]==0 and n[3]==400 else 16384 if seat==0 and n[10]==0 and n[3]==410 else 0
                                if n[2]:
                                    if n[3]==0 and seat==0:call(page,'multiplayerSmoke.captureInput(0,0,2,8188,-8192,0)')
                                    else:call(page,'v=>multiplayerSmoke.capture(...v)',[n[3],buttons])
                                call(page,'multiplayerSmoke.ticks(1)')
                            else:call(page,'multiplayerSmoke.reconcile()')
                            n=call(page,'multiplayerSmoke.netStatus()')
                            ready=ready and n[10]==1 and n[4]==119 and n[5]>=119 and n[6]==0xffffffff
                        if ready:break
                        assert time.monotonic()<deadline,[observe(page) for page in pages]
                        pages[0].wait_for_timeout(1)
                    restarted=[observe(page) for page in pages];case['restart']=restarted;save();equal(restarted)
                    for seat,page in enumerate(pages):
                        before=call(page,'multiplayerSmoke.netStatus()')
                        assert call(page,'p=>multiplayerSmoke.applyWire(p)',stale[(seat+1)%count])==1
                        assert call(page,'multiplayerSmoke.netStatus()')==before
                    case['staleGenerationRejected']=True
                case['passed']=True;case['phase']='complete';save()
                print('TH08 production driver',args.mode,count,'players PASS',flush=True)
            except BaseException:
                case['latest']=[observe(page) for page in pages];raise
            finally:
                for context in contexts:context.close()
        assert not report['errors'],report['errors'];report['passed']=True
    except BaseException as error:report['error']=str(error);raise
    finally:browser.close();save()
