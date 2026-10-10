"""Actual TH08 Runtime shell, admitted observer stream, and native read-only path."""
import argparse
import json
import time
import uuid
from pathlib import Path
from playwright.sync_api import sync_playwright

parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--relay',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--mode',choices=['spectator-rtc','spectator-relay'],required=True)
parser.add_argument('--players',choices=['2','3','all'],default='all')
parser.add_argument('--frames',type=int,default=239)
parser.add_argument('--timing',choices=['pure','hybrid'],default='pure')
args=parser.parse_args()
assert args.frames>=239
adonis_mode=1 if args.timing=='pure' else 2
report={'passed':False,'scope':'Production TH08 admitted read-only spectator using actual Runtime host protocol',
        'mode':args.mode,'cases':[],'errors':[]}

def save():args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
def call(page,code,arg=None):return page.evaluate(code,arg)
def request(page,command,**fields):return call(page,'v=>host.request(v.command,v.fields)',{'command':command,'fields':fields})
def snapshot(page,full=True):return call(page,'full=>host.snapshot(full)',full)
def input_at(seat,frame):
    if frame<180:return 0
    return 1|(64 if (seat+frame//12)%2 else 128)|(4 if frame%30<15 else 0)|(2 if frame==215 else 0)

with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        for count in ([2,3] if args.players=='all' else [int(args.players)]):
            contexts=[];pages=[];case={'players':count,'passed':False,'phase':'boot'}
            report['cases'].append(case);save()
            try:
                # Prepare all shell instances before the lobby starts its
                # finite observer connection window. The observer game itself
                # is deliberately launched after the players reach frame 89.
                for endpoint in range(count+1):
                    context=browser.new_context(service_workers='block',viewport={'width':640,'height':480});contexts.append(context)
                    if args.mode=='spectator-relay':
                        context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true})")
                    page=context.new_page();pages.append(page)
                    page.on('pageerror',lambda error:report['errors'].append(str(error)))
                    page.goto(args.url+'host.html');call(page,'host.open()')
                    page.wait_for_function('host.ready()',timeout=120000)
                players=pages[:count];observer=pages[-1]
                room='th08mp-audit-'+uuid.uuid4().hex[:16];observer_id='observer_1234'
                started=call(players[0],'v=>host.lobby(...v)',[args.relay,room,count,observer_id,{'adonisMode':adonis_mode,'inputDelay':2,'inputDelayAuto':False,'predictionReserve':2}])
                assert started['room']['spectatorCount']==1,started
                url=args.relay+'/?room='+room+'&run='+str(started['serial'])
                common={'runtimeVariant':'multiplayer','netplayMode':'lan','netplayUrl':url,
                        'netplayPlayerCount':count,'netplayDifficulty':1,'netplaySeed':1234,
                        'netplayLoadouts':[{'character':seat,'shot':0} for seat in range(count)],
                        'netplayAdonisMode':adonis_mode,'netplayInputDelay':2,'netplayInputDelayAuto':False,'netplayPredictionLimit':8,'netplayPredictionReserve':2,'netplaySpectatorCount':1,'netplayIceServers':[],
                        'touchEnabled':True,'touchMovementMode':'touch','touchSensitivity':100,'thpracEnabled':True}
                for seat,page in enumerate(players):
                    request(page,'configure',options={**common,'netplayPlayer':seat,'netplayUrl':url+'&member=fixture_host_contract_p'+str(seat+1)},music='none')
                    request(page,'launch')
                def advance(target,watch=False):
                    deadline=time.monotonic()+120;iterations=0
                    while True:
                        values=[]
                        for seat,page in enumerate(players):
                            s=snapshot(page,False);n=s['net']
                            if n[2] and n[3]<=target:call(page,'v=>host.buttons(v)',input_at(seat,n[3]))
                            values.append(call(page,'t=>host.tickTo(t,false)',target))
                        if watch:values.append(call(observer,'t=>host.tickTo(t,false)',target))
                        iterations+=1
                        if all(s['net'][4]==target and s['net'][5]>=target and s['net'][6]==0xffffffff for s in values):
                            # Hash the stopped, fully confirmed checkpoint once.
                            # Progress polling need not hash the entire native world.
                            return [snapshot(page) for page in players+([observer] if watch else [])],iterations
                        assert time.monotonic()<deadline,values
                        players[0].wait_for_timeout(1)
                case['phase']='delayed-admission';save()
                before,_=advance(89)
                expected_route=1 if args.mode=='spectator-rtc' else 2
                assert all(s['driver'][12]==expected_route for s in before),before
                # A pre-existing imported record is never changed by observer
                # initialization, gameplay, results or exit, even if decoding
                # the old record yields the native empty-record fallback.
                sentinel=[84,72,48,56,0,0,0,0]+[0]*120
                request(observer,'write',path='score.dat',bytes=sentinel)
                request(observer,'configure',options={**common,'netplayPlayer':-1,'netplaySpectator':True,'netplaySpectatorId':observer_id,'netplayUrl':url+'&member=fixture_'+observer_id},music='none')
                request(observer,'launch')
                writes=call(observer,'host.spectatorWriteProbe()')
                assert writes=={'local':0,'analog':0,'ready':0,'hello':0,'packet':0,'wire':1,'practice':0},writes
                for page in (players[0],observer):
                    rejection=call(page,"async()=>{try{await host.request('resources',{resources:[]});return '';}catch(e){return String(e);}}")
                    assert 'Cannot replace resources' in rejection,rejection
                case['liveResourceReplacementRejected']=True
                assert request(observer,'read',path='score.dat')['bytes']==sentinel
                caught,iterations=advance(119,True)
                case['caughtUp']={'iterations':iterations,'values':caught};save()
                assert iterations<120,'observer did not use bounded native catch-up'
                # Browser controls must not become an extra input participant.
                request(observer,'keyboard',code='KeyX',down=True)
                request(observer,'direct-touch',type='down',id=31,x=.5,y=.7)
                request(observer,'direct-touch',type='move',id=31,x=.8,y=.3)
                request(observer,'touch-controls',controls={'focusEnabled':True,'fireEnabled':True,'bombSerial':1})
                case['checkpoints']=[]
                for target in sorted(set(range(179,args.frames+1,60))|{args.frames}):
                    values,_=advance(target,True)
                    case['checkpoints'].append({'frame':target,'canonical':[s['canonical'] for s in values]});save()
                    assert all(s['canonical']==values[0]['canonical'] for s in values),{'frame':target,'values':values}
                case.update({'phase':'comparison','values':values,'writeProbe':writes,'peer':call(observer,'host.observerPeer()')});save()
                assert all(s['canonical']==values[0]['canonical'] for s in values),values
                assert all(s['state'][:2]+s['state'][3:]==values[0]['state'][:2]+values[0]['state'][3:] for s in values),values
                assert all(s['driver'][6:9]==values[0]['driver'][6:9] for s in values),values
                viewer=values[-1]
                assert viewer['state'][2]==-1 and viewer['driver'][3:6]==[0,0,0],viewer
                assert viewer['driver'][9]==0 and viewer['driver'][12]==3,viewer
                assert case['peer']=={'route':'spectator','peers':0,'canSend':False},case['peer']
                assert request(observer,'read',path='score.dat')['bytes']==sentinel
                assert all(not f['path'].startswith('replay/') for f in request(observer,'list')['files'])
                # Native Pause then Restart, through player input. The observer
                # consumes that confirmed selection and ends this admitted run;
                # the players complete generation one on the same transport.
                case['phase']='restart';save();deadline=time.monotonic()+120;finished=None
                while True:
                    ready=True
                    for seat,page in enumerate(players):
                        s=snapshot(page,False);n=s['net']
                        if n[10]==0 or n[3]<120:
                            buttons=8 if seat==0 and n[10]==0 and n[3]==args.frames+21 else 16384 if seat==0 and n[10]==0 and n[3]==args.frames+36 else 0
                            if n[2]:call(page,'v=>host.buttons(v)',buttons)
                            s=call(page,'host.physicalTick(false)')
                        else:s=call(page,'t=>host.tickTo(t,false)',119)
                        ready=ready and s['net'][10]==1 and s['net'][4]==119 and s['net'][5]>=119 and s['net'][6]==0xffffffff
                    if finished is None:
                        s=call(observer,'host.physicalTick(false)')
                        if s['spectator'][2]:finished=s
                    if ready and finished is not None:break
                    assert time.monotonic()<deadline,{'players':[snapshot(page) for page in players],'observer':snapshot(observer)}
                    players[0].wait_for_timeout(1)
                assert finished['net'][10]==0 and finished['spectator'][1:3]==[1,1],finished
                call(observer,'host.finishCallback()')
                observer.wait_for_function("host.events.filter(e=>e.event==='exit').length===1",timeout=15000)
                assert not snapshot(observer)['app']
                assert request(observer,'read',path='score.dat')['bytes']==sentinel
                assert call(players[0],'host.lobbyOpen()')
                restarted=[snapshot(page) for page in players]
                assert all(s['canonical']==restarted[0]['canonical'] for s in restarted),restarted
                case.update({'passed':True,'phase':'complete','observerFinished':finished,'playersRestarted':restarted,
                             'persistentRecordsUnchanged':True,'lobbyRetained':True});save()
                print('TH08 observer',args.mode,count,'players: PASS',flush=True)
            except BaseException:
                case['latest']=[snapshot(page) for page in pages]
                case['events']=[call(page,'host.events') for page in pages];raise
            finally:
                for context in contexts:context.close()
        assert not report['errors'],report['errors'];report['passed']=True
    except BaseException as error:report['error']=str(error);raise
    finally:browser.close();save()
