"""Record real admitted peers, save through native API, use the actual Replay menu."""
import argparse
import json
import time
import uuid
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True);parser.add_argument('--relay',required=True)
parser.add_argument('--output',type=Path,required=True);parser.add_argument('--mode',default='replay')
parser.add_argument('--players',choices=['2','3','all'],default='all')
parser.add_argument('--frames',type=int,default=500)
args=parser.parse_args()
report={'passed':False,'scope':'Production all-seat Replay, native menu and fresh offline world; no fixtures',
        'cases':[],'errors':[]}
def save():args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
def call(p,js,arg=None):return p.evaluate(js,arg)
def request(p,command,**fields):return call(p,'v=>host.request(v.command,v.fields)',{'command':command,'fields':fields})
def state(p):return call(p,'host.snapshot()')
def sample(seat,gen,frame):
    buttons=1 if frame>=175 else 0
    if frame>=175:buttons|=(64 if (frame//12+seat)%2 else 128)|(4 if frame%30<15 else 0)
    if gen==0 and seat==0 and frame==230:buttons=8
    if gen==0 and seat==0 and frame==260:buttons=16384
    if 185<=frame<200:return [buttons,2,.5*(1 if seat%2 else -1),-.25,2]
    if frame==215:return [buttons|2,2,5,-1,6]
    return [buttons,0,0,0,0]

with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        for count in ([2,3] if args.players=='all' else [int(args.players)]):
            contexts=[];players=[];viewer=None;case={'players':count,'passed':False,'phase':'boot'}
            report['cases'].append(case);save()
            def boot():
                context=browser.new_context(service_workers='block',viewport={'width':640,'height':480});contexts.append(context)
                page=context.new_page();page.on('pageerror',lambda e:report['errors'].append(str(e)))
                page.goto(args.url+'host.html');call(page,'host.open()');page.wait_for_function('host.ready()',timeout=120000)
                return page
            try:
                for _ in range(count):players.append(boot())
                room='audit-'+uuid.uuid4().hex[:16]
                started=call(players[0],'v=>host.lobby(...v)',[args.relay,room,count])
                for seat,p in enumerate(players):
                    options={'runtimeVariant':'multiplayer','netplayMode':'lan','netplayUrl':args.relay+'/?room='+room+'&run='+str(started['serial']),
                             'netplayPlayer':seat,'netplayPlayerCount':count,'netplayDifficulty':1,'netplaySeed':1234,
                             'netplayLoadouts':[{'character':i,'shot':0} for i in range(count)],'netplayIceServers':[],
                             'touchEnabled':False,'thpracEnabled':False}
                    request(p,'configure',options=options,music='none');request(p,'launch')
                case['phase']='recording';save();trace=[];deadline=time.monotonic()+240
                assert 1<=args.frames<=500
                for global_frame in range(args.frames):
                    while True:
                        values=[]
                        for seat,p in enumerate(players):
                            s=state(p);n=s['net'];r=s['replay']
                            # Never advance beyond this global input row. In
                            # the generation handoff, a real Step may retire
                            # the old epoch without advancing a logical frame.
                            if r[7]+n[3]>=global_frame+1:
                                s=call(p,'t=>host.tickTo(t)',n[4])
                            else:
                                if n[2]:call(p,'v=>host.nativeSample(...v)',[n[3],sample(seat,n[10],n[3])])
                                s=call(p,'host.physicalTick()')
                            values.append(s)
                        if all(s['replay'][4]==global_frame+1 and s['net'][6]==0xffffffff for s in values):break
                        assert time.monotonic()<deadline,{'frame':global_frame,'values':values}
                        players[0].wait_for_timeout(1)
                    assert all(s['canonical']==values[0]['canonical'] for s in values),{'recordFrame':global_frame,'values':values}
                    trace.append({'canonical':values[-1]['canonical'],'state':values[-1]['state'],
                                  'audio':values[-1]['driver'][6:9],'generation':values[-1]['net'][10]})
                    if global_frame%100==99:case['recordedFrames']=global_frame+1;save()
                if args.frames>260:assert trace[-1]['generation']==1,trace[-1]
                assert call(players[-1],'v=>host.saveReplay(...v)',[1,'ReplayUT'])
                request(players[-1],'sync')
                recording=request(players[-1],'read',path='replay/th8_01.rpyx')['bytes']
                cfg=request(players[-1],'read',path='th08.cfg')['bytes'];cfg[24]=6
                score=request(players[-1],'read',path='score.dat')['bytes']
                record_path=args.output.parent/f'recording-{count}p.rpyx';record_path.write_bytes(bytes(recording))
                (args.output.parent/f'trace-{count}p.json').write_text(json.dumps(trace),encoding='utf-8')
                case.update({'recording':str(record_path),'recordedFrames':len(trace),'recordedPlayer':count-1,'phase':'viewer-boot'});save()
                viewer=boot();case['phase']='viewer-import';save()
                request(viewer,'write',path='score.dat',bytes=score)
                request(viewer,'write',path='th08.cfg',bytes=cfg)
                corrupt=recording.copy();corrupt[-1]^=1
                rejected=call(viewer,"async bytes=>{try{await host.request('write',{path:'replay/th8_02.rpyx',bytes});return '';}catch(e){return String(e);}}",corrupt)
                assert 'Invalid TH08 multiplayer Replay' in rejected,rejected
                request(viewer,'write',path='replay/th8_01.rpyx',bytes=recording)
                request(viewer,'configure',options={'runtimeVariant':'multiplayer','replayViewer':True,'touchEnabled':False},music='none')
                request(viewer,'launch');case['phase']='native-menu';save()
                def start_from_menu():
                    for i in range(420):
                        if call(viewer,'host.replayRequest()'):break
                        if i%50==20:request(viewer,'keyboard',code='KeyZ',down=True)
                        if i%50==22:request(viewer,'keyboard',code='KeyZ',down=False)
                        call(viewer,'host.physicalTick()')
                    path=call(viewer,'host.replayRequest()');assert path=='replay/th8_01.rpyx',(path,state(viewer))
                    call(viewer,'host.finishCallback()')
                    viewer.wait_for_function("(()=>{const failure=host.events.find(e=>e.event==='error');if(failure)throw Error(failure.message||failure.error);const s=host.snapshot();return s.app&&s.active&&s.replay?.[2]===1;})()",timeout=120000)
                start_from_menu();case['phase']='playback';save()
                assert state(viewer)['replay'][4]==0
                write_probe=call(viewer,'host.spectatorWriteProbe()')
                assert write_probe=={'local':0,'analog':0,'ready':0,'hello':0,'packet':0,'wire':1,'practice':0},write_probe
                assert not call(viewer,'v=>host.saveReplay(...v)',[2,'Forbidden'])
                for frame,expected in enumerate(trace):
                    if frame==200:request(viewer,'keyboard',code='KeyX',down=True)
                    for callback in range(5):
                        actual=call(viewer,'host.physicalTick()')
                        if actual['replay'][4]>frame:break
                        # Native generation retirement is a barrier, not an
                        # input frame. The recording lane above uses the same
                        # logical cursor rather than counting host callbacks.
                        assert actual['replay'][4]==frame,actual
                    assert actual['replay'][4]==frame+1,{'frame':frame,'actual':actual}
                    assert actual['canonical']==expected['canonical'],{'firstDivergence':frame,'expected':expected,'actual':actual}
                    assert actual['state']==expected['state'],{'firstStateDivergence':frame,'expected':expected,'actual':actual}
                    assert actual['driver'][6:9]==expected['audio'],{'firstAudioDivergence':frame,'expected':expected,'actual':actual}
                    assert actual['driver'][9]==0,actual
                    if frame%100==99:case['framesCompared']=frame+1;save()
                assert state(viewer)['replay'][11]==1
                # The loaded codec must preserve the exact portable file bytes.
                assert call(viewer,'host.replayExport()')==recording
                call(viewer,'host.physicalTick()');call(viewer,'host.finishCallback()')
                viewer.wait_for_function("(()=>{const s=host.snapshot();return s.app&&s.active&&s.replay?.[3]===1&&!s.replay[2];})()",timeout=120000)
                assert request(viewer,'read',path='score.dat')['bytes']==score
                assert request(viewer,'read',path='th08.cfg')['bytes']==cfg
                # Reopening the same native menu then Escape is a lifecycle
                # probe, not a second complete screening of the recording.
                start_from_menu()
                for _ in range(20):call(viewer,'host.physicalTick()')
                request(viewer,'keyboard',code='Escape',down=True);call(viewer,'host.physicalTick()');call(viewer,'host.finishCallback()')
                viewer.wait_for_function("(()=>{const s=host.snapshot();return s.app&&s.active&&s.replay?.[3]===1&&!s.replay[2];})()",timeout=120000)
                assert not any(e.get('event')=='exit' for e in call(viewer,'host.events'))
                assert call(players[0],'host.lobbyOpen()')
                assert request(viewer,'read',path='score.dat')['bytes']==score
                case.update({'passed':True,'phase':'complete','framesCompared':len(trace),'writeProbe':write_probe,
                             'differentLocalLives':6,'bootFilesIsolated':True,'nativeMenu':True,'escape':True,
                             'generation':trace[-1]['generation']});save()
                print('TH08 multiplayer Replay',count,'players: PASS',flush=True)
            except BaseException:
                if viewer:case['viewer']=state(viewer);case['events']=call(viewer,'host.events')
                case['playersState']=[state(p) for p in players];raise
            finally:
                for c in contexts:c.close()
        assert not report['errors'],report['errors'];report['passed']=True
    except BaseException as error:report['error']=str(error);raise
    finally:browser.close();save()
