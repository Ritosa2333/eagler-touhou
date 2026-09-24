"""TH10 confirmed-input spectator over the real BrowserPeerTransport/Relay."""
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
parser.add_argument('--mode',choices=['rtc','relay'],default='rtc')
args=parser.parse_args()
report={'passed':False,'scope':'TH10 read-only start-time spectator over real relay',
        'mode':args.mode,'errors':[]}

def call(page,expression,arg=None):
    return page.evaluate(expression,arg) if arg is not None else page.evaluate(expression)

def admit(setup,room,spectator_id):
    return setup.evaluate(
        """async ({relay,room,spectatorId})=>{
          const clients=[];
          const open=id=>new Promise((resolve,reject)=>{
            const ws=new WebSocket(relay+'/?room='+room+'&lobby='+id),queue=[],waiters=[];
            ws.onmessage=e=>{const v=JSON.parse(String(e.data)),w=waiters.shift();w?w(v):queue.push(v);};
            ws.onerror=()=>reject(new Error('lobby socket failed '+id));
            ws.onopen=()=>resolve({ws,next:()=>queue.length?Promise.resolve(queue.shift()):
              new Promise((done,fail)=>{const t=setTimeout(()=>fail(new Error('lobby timeout '+id)),10000);
                waiters.push(v=>{clearTimeout(t);done(v);});})});
          });
          const nextType=async(c,type)=>{for(;;){const v=await c.next();if(v.type==='error')throw Error(v.error||'lobby error');if(v.type===type)return v;}};
          const p1=await open('player_one_0001');clients.push(p1.ws);await p1.next();
          const p2=await open('player_two_0002');clients.push(p2.ws);await p2.next();
          const sp=await open(spectatorId);clients.push(sp.ws);await sp.next();
          p1.ws.send(JSON.stringify({type:'take-seat',seat:0,loadout:0}));
          p2.ws.send(JSON.stringify({type:'take-seat',seat:1,loadout:1}));
          sp.ws.send(JSON.stringify({type:'spectate'}));
          let version=0;
          for(;;){const v=await sp.next();if(v.type==='error')throw Error(v.error||'spectate error');
            if(v.type==='state'&&v.room?.spectatorCount===1&&v.room?.seats?.[0]&&v.room?.seats?.[1]){
              version=Number(v.room.settingsVersion||0);break;}}
          p1.ws.send(JSON.stringify({type:'set-ready',ready:true,settingsVersion:version}));
          p2.ws.send(JSON.stringify({type:'set-ready',ready:true,settingsVersion:version}));
          for(;;){const v=await sp.next(),seats=v.room?.seats||[];
            if(v.type==='error')throw Error(v.error||'ready error');
            if(v.type==='state'&&seats[0]?.ready&&seats[1]?.ready)break;}
          const started=nextType(sp,'start');p1.ws.send(JSON.stringify({type:'start'}));
          const value=await started;globalThis.__th10SpectatorLobbySockets=clients;
          return {serial:Number(value.serial),spectatorCount:Number(value.room?.spectatorCount)};
        }""",{'relay':args.relay,'room':room,'spectatorId':spectator_id})

def buttons(seat,frame):
    if frame<20:return 0
    return 1|(0x80 if ((frame//30+seat)%2) else 0x40)|(4 if frame%40<20 else 0)

with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    contexts=[];pages=[]
    try:
        report['browser']=browser.version
        setup=browser.new_context().new_page()
        room='audit-th10spec-'+uuid.uuid4().hex[:12]
        spectator_id='spectator_0003'
        started=admit(setup,room,spectator_id)
        assert started=={'serial':1,'spectatorCount':1},started
        run_url=f"{args.relay}/?room={room}&run=1"
        session_low,session_high=0x71625344,0x18273645
        loadouts=[[0,0],[1,1]]
        for seat in range(2):
            context=browser.new_context(service_workers='block');contexts.append(context)
            if args.mode=='relay':
                context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});")
            page=context.new_page();pages.append(page)
            page.on('pageerror',lambda error,i=seat:report['errors'].append({'endpoint':i,'error':str(error)}))
            page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
            call(page,'multiplayerSmoke.setSpectatorCount(1)')
            call(page,'v=>multiplayerSmoke.startNet(...v)',[loadouts,seat,1,1234,session_low,session_high])
            assert call(page,'u=>multiplayerSmoke.connect(u)',run_url)
        deadline=time.monotonic()+60
        while not all(call(page,'multiplayerSmoke.canStart()') for page in pages):
            for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()'),call(page,'multiplayerSmoke.networkError()')
            assert time.monotonic()<deadline,'player handshake timeout'
            pages[0].wait_for_timeout(5)
        expected_route=1 if args.mode=='rtc' else 2
        assert all(call(page,'multiplayerSmoke.transportStatus()')[13]==expected_route for page in pages)

        target_before_join=89
        deadline=time.monotonic()+90
        while True:
            ready=True
            for seat,page in enumerate(pages):
                n=call(page,'multiplayerSmoke.netStatus()')
                value=call(page,'v=>multiplayerSmoke.peerAdvance(...v)',[target_before_join,buttons(seat,n[2])])
                ready=ready and value['net'][3]==target_before_join and value['net'][5]==-1
            if ready:break
            assert time.monotonic()<deadline,'players did not reach delayed-join boundary'
            pages[0].wait_for_timeout(2)
        for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')

        spectator_context=browser.new_context(service_workers='block');contexts.append(spectator_context)
        spectator=spectator_context.new_page();pages.append(spectator)
        spectator.on('pageerror',lambda error:report['errors'].append({'endpoint':'spectator','error':str(error)}))
        spectator.goto(args.url);spectator.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
        call(spectator,'v=>multiplayerSmoke.startNet(...v)',[loadouts,0,1,1234,session_low,session_high])
        assert call(spectator,'v=>multiplayerSmoke.spectatorConnect(...v)',[run_url,spectator_id])
        assert call(spectator,'multiplayerSmoke.canStart()')
        write_probe=call(spectator,'multiplayerSmoke.spectatorWriteProbe()')
        assert write_probe=={'capture':False,'remote':7,'packetBytes':0,'helloBytes':0,'markReady':False},write_probe

        target=179
        deadline=time.monotonic()+120
        while True:
            player_ready=True
            for seat,page in enumerate(pages[:2]):
                n=call(page,'multiplayerSmoke.netStatus()')
                value=call(page,'v=>multiplayerSmoke.peerAdvance(...v)',[target,buttons(seat,n[2])])
                player_ready=player_ready and value['net'][3]==target and value['net'][5]==-1
            spectator_value=call(spectator,'n=>multiplayerSmoke.spectatorAdvance(n)',target)
            if player_ready and spectator_value['net'][3]==target:break
            assert time.monotonic()<deadline,('spectator checkpoint timeout',spectator_value)
            pages[0].wait_for_timeout(2)

        hashes=[call(page,'multiplayerSmoke.canonical()') for page in pages]
        portable=[call(page,'multiplayerSmoke.portableCanonical()') for page in pages]
        states=[call(page,'multiplayerSmoke.status()') for page in pages]
        audio=[call(page,'multiplayerSmoke.audioStatus()') for page in pages]
        enemies=[call(page,'multiplayerSmoke.enemyDebug()') for page in pages]
        spectator_status=call(spectator,'multiplayerSmoke.spectatorStatus()')
        transport=call(spectator,'multiplayerSmoke.transportStatus()')
        replay=call(spectator,'multiplayerSmoke.replayStatus()')
        peer_state=call(spectator,"()=>({route:globalThis.__eaglerPeerTransport?.route||'',peers:globalThis.__eaglerPeerTransport?.peers?.size||0,canSend:typeof globalThis.__eaglerPeerTransport?.send==='function'||typeof globalThis.__eaglerPeerTransport?.sendTo==='function'})")
        report.update({'room':room,'checkpoint':target,'states':states,'hashes':hashes,
                       'portableHashes':portable,'audio':audio,
                       'enemyDebug':enemies,'spectatorStatus':spectator_status,
                       'spectatorTransport':transport,'spectatorPeerState':peer_state,
                       'writeProbe':write_probe})
        # Exact-layout hashes deliberately include graph pointers and therefore
        # differ for the separately bootstrapped spectator. The portable
        # oracle covers gameplay, ECL, authored ANM, RNG and lifecycle state.
        assert len({value[1] for value in portable})==1,(portable,hashes,states,enemies)
        assert states[0][:2]+states[0][3:]==states[1][:2]+states[1][3:]==states[2][:2]+states[2][3:],states
        assert audio[0]==audio[1]==audio[2],audio
        assert spectator_status[1]==1 and transport[13]==3,(spectator_status,transport)
        assert peer_state=={'route':'spectator','peers':0,'canSend':False},peer_state
        assert replay[1]==0 and replay[2]==0 and replay[9]==1,replay
        assert not report['errors'],report['errors']
        report.update({'passed':True,'readOnly':True})
        print(f"TH10 spectator {args.mode}: PASS frame={target} hash={portable[0][1]}",flush=True)
    except BaseException as error:
        report['failure']=str(error);raise
    finally:
        for context in contexts:
            try:context.close()
            except Exception:pass
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
