"""Actual BrowserPeerTransport + native TH10 world, never remote-input injection."""
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
parser.add_argument('--players',type=int,choices=[2,3],default=2)
parser.add_argument('--drop-fast',action='store_true')
parser.add_argument('--retry',action='store_true')
parser.add_argument('--p1-death',action='store_true')
parser.add_argument('--p1-rescue-death',action='store_true')
args=parser.parse_args()
report={'passed':False,'scope':'native TH10 over actual browser peer transport','mode':args.mode,
        'players':args.players,'dropFast':args.drop_fast,'checkpoints':[],'errors':[]}

def call(page,expression,arg=None):
    return page.evaluate(expression,arg) if arg is not None else page.evaluate(expression)

def inputs(seat,frame):
    if frame<20:return 0
    direction=0x80 if ((frame//30+seat)%2) else 0x40
    return 1|direction|(4 if frame%40<20 else 0)|(2 if frame==80+seat*10 else 0)

DROP=r'''(() => {
  const send=RTCDataChannel.prototype.send;
  globalThis.__testFastSends=0;globalThis.__testFastDropped=0;
  RTCDataChannel.prototype.send=function(data){
    if(this.label.endsWith('-input')&&++globalThis.__testFastSends%3===0){
      ++globalThis.__testFastDropped;return;
    }
    return send.call(this,data);
  };
})();'''

with sync_playwright() as p:
    browser=p.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    contexts=[];pages=[]
    try:
        report['browser']=browser.version
        room='audit-th10-'+uuid.uuid4().hex[:16]
        url=args.relay+('?room='+room+'&run=1')
        for seat in range(args.players):
            context=browser.new_context(service_workers='block');contexts.append(context)
            if args.mode=='relay':context.add_init_script("Object.defineProperty(globalThis,'RTCPeerConnection',{value:undefined,configurable:true});")
            if args.drop_fast:context.add_init_script(DROP)
            page=context.new_page();pages.append(page)
            page.on('pageerror',lambda error,i=seat:report['errors'].append({'seat':i,'error':str(error)}))
            page.goto(args.url);page.wait_for_function('window.multiplayerSmoke !== undefined',timeout=120000)
            report.setdefault('identities',[]).append(call(page,'multiplayerSmoke.identity()'))
            loadouts=[[0,0],[1,1],[0,2]][:args.players]
            call(page,'v=>multiplayerSmoke.startNet(...v)',[loadouts,seat,1,1234,0x71625344,0x18273645])
            assert call(page,'u=>multiplayerSmoke.connect(u)',url)
        deadline=time.monotonic()+60
        while not all(call(page,'multiplayerSmoke.canStart()') for page in pages):
            for page in pages:
                assert call(page,'multiplayerSmoke.pollNetwork()'),call(page,'multiplayerSmoke.networkError()')
            assert time.monotonic()<deadline,('handshake timeout',[call(page,'multiplayerSmoke.transportStatus()') for page in pages])
            pages[0].wait_for_timeout(5)
        mode=1 if args.mode=='rtc' else 2
        assert all(call(page,'multiplayerSmoke.transportStatus()')[13]==mode for page in pages)
        print('network-check: actual route',args.mode,args.players,'players, frame-zero gate complete',flush=True)

        for target in ([59] if (args.retry or args.p1_death or args.p1_rescue_death) else [59,119,179]):
            deadline=time.monotonic()+150
            while True:
                ready=True
                for seat,page in enumerate(pages):
                    current=call(page,'multiplayerSmoke.netStatus()')
                    value=call(page,'v=>multiplayerSmoke.peerAdvance(...v)',[target,inputs(seat,current[2])])
                    assert value['net'][3]<=target,value
                    ready=ready and value['net'][3]==target and value['net'][5]==-1 and value['transport'][9]!=0xffffffff and value['transport'][9]>=target
                if ready:break
                assert time.monotonic()<deadline,('native network deadline',target,[call(page,'multiplayerSmoke.netStatus()') for page in pages],[call(page,'multiplayerSmoke.transportStatus()') for page in pages])
                pages[0].wait_for_timeout(2)
            states=[call(page,'multiplayerSmoke.status()') for page in pages]
            hashes=[call(page,'multiplayerSmoke.canonical()') for page in pages]
            audio=[call(page,'multiplayerSmoke.audioStatus()') for page in pages]
            checkpoint={'frame':target,'states':states,'hashes':hashes,'audio':audio,
                        'network':[call(page,'multiplayerSmoke.transportStatus()') for page in pages]}
            report['checkpoints'].append(checkpoint)
            for seat in range(1,args.players):
                # Status index 2 labels this endpoint's local seat, not gameplay.
                assert states[0][:2]+states[0][3:]==states[seat][:2]+states[seat][3:],checkpoint
                assert hashes[0][1]==hashes[seat][1],checkpoint
                assert audio[0]==audio[seat],checkpoint
            print('network-check: native confirmed frame',target,'state/ANM/RNG/audio match',flush=True)

        if args.p1_death:
            assert args.players==2,'P1 final-death fixture currently owns two seats'
            for page in pages:
                assert call(page,'multiplayerSmoke.fixture(11)')
            deadline=time.monotonic()+90
            target=239
            while True:
                frontier=[]
                for page in pages:
                    current=call(page,'multiplayerSmoke.netStatus()')
                    frontier.append(call(page,'v=>multiplayerSmoke.peerAdvance(...v)',[target,0 if current[2]<=target else 0]))
                ready=all(value['net'][3]==target and value['net'][5]==-1 and
                          value['transport'][9]!=0xffffffff and value['transport'][9]>=target
                          for value in frontier)
                if ready:break
                assert time.monotonic()<deadline,('P1 final death network deadline',frontier,
                    [call(page,'multiplayerSmoke.networkError()') for page in pages])
                pages[0].wait_for_timeout(2)
            states=[call(page,'multiplayerSmoke.status()') for page in pages]
            hashes=[call(page,'multiplayerSmoke.canonical()') for page in pages]
            transport=[call(page,'multiplayerSmoke.transportStatus()') for page in pages]
            # Seat 0 is now the non-colliding Spirit with native final-death
            # lives=-1.  P2 stays alive and receives the cooperation life.
            assert all(value[10]==-1 and value[12]==3 and value[17]==2 for value in states),states
            assert all(value[22]>=0 and value[24]!=3 for value in states),states
            assert states[0][:2]+states[0][3:]==states[1][:2]+states[1][3:],states
            assert hashes[0][1]==hashes[1][1],hashes
            assert all(value[3]==0 for value in transport),transport
            report['p1FinalDeath']={'states':states,'hashes':hashes,'network':transport}
            print('network-check: P1 final death stays synchronized over live transport',flush=True)

        if args.p1_rescue_death:
            assert args.players==2,'P1 rescue/death fixture currently owns two seats'
            for page in pages:
                assert call(page,'multiplayerSmoke.fixture(12)')

            # Frames 60..149 are the exact ninety authoritative focus-hold
            # ticks that let P2 spend a life to revive P1. Drive only each
            # endpoint's real local input through BrowserPeerTransport.
            deadline=time.monotonic()+90
            target=154
            while True:
                frontier=[]
                for seat,page in enumerate(pages):
                    current=call(page,'multiplayerSmoke.netStatus()')
                    buttons=4 if seat==1 and current[2]<=149 else 0
                    frontier.append(call(page,'v=>multiplayerSmoke.peerAdvance(...v)',[target,buttons]))
                ready=all(value['net'][3]==target and value['net'][5]==-1 and
                          value['transport'][9]!=0xffffffff and value['transport'][9]>=target
                          for value in frontier)
                if ready:break
                assert time.monotonic()<deadline,('P1 rescue network deadline',frontier,
                    [call(page,'multiplayerSmoke.networkError()') for page in pages])
                pages[0].wait_for_timeout(2)
            rescued=[call(page,'multiplayerSmoke.status()') for page in pages]
            assert all(value[10]==0 and value[12]==1 and value[17]==0 for value in rescued),rescued
            assert all(value[22]==1 and value[24]==1 for value in rescued),rescued
            for page in pages:
                assert call(page,'multiplayerSmoke.fixture(13)')

            deadline=time.monotonic()+90
            target=259
            while True:
                frontier=[]
                for page in pages:
                    frontier.append(call(page,'v=>multiplayerSmoke.peerAdvance(...v)',[target,0]))
                ready=all(value['net'][3]==target and value['net'][5]==-1 and
                          value['transport'][9]!=0xffffffff and value['transport'][9]>=target
                          for value in frontier)
                if ready:break
                assert time.monotonic()<deadline,('rescued P1 death network deadline',frontier,
                    [call(page,'multiplayerSmoke.networkError()') for page in pages])
                pages[0].wait_for_timeout(2)
            states=[call(page,'multiplayerSmoke.status()') for page in pages]
            hashes=[call(page,'multiplayerSmoke.canonical()') for page in pages]
            transport=[call(page,'multiplayerSmoke.transportStatus()') for page in pages]
            assert all(value[10]==-1 and value[12]==3 and value[17]==2 for value in states),states
            assert all(value[22]==2 and value[24]==1 for value in states),states
            assert states[0][:2]+states[0][3:]==states[1][:2]+states[1][3:],states
            assert hashes[0][1]==hashes[1][1],hashes
            assert all(value[3]==0 for value in transport),transport
            report['p1RescueThenDeath']={'rescued':rescued,'states':states,'hashes':hashes,'network':transport}
            print('network-check: rescued P1 can die again without transport failure',flush=True)

        if args.retry:
            assert args.players==2,'retry fixture currently owns two seats'
            old_hello=call(pages[0],'multiplayerSmoke.sessionPacket(1)')
            original=[call(page,'multiplayerSmoke.generationStatus()') for page in pages]
            for page in pages:assert call(page,'multiplayerSmoke.fixture(5)')
            # Explicit native initial state, then only captured local neutral
            # input over the real transport drives wipe/rebuild/handshake.
            deadline=time.monotonic()+90
            while True:
                frontier=[call(page,'multiplayerSmoke.peerUntilGeneration(1)') for page in pages]
                if all(value['transport'][12]==1 for value in frontier):break
                assert time.monotonic()<deadline,('retry did not retire',frontier)
                pages[0].wait_for_timeout(5)
            fresh=[call(page,'multiplayerSmoke.generationStatus()') for page in pages]
            assert fresh[0][4:8]==fresh[1][4:8] and fresh[0][4:6]!=original[0][4:6],(original,fresh)
            assert all(call(page,'multiplayerSmoke.netStatus()')[2:4]==[0,-1] for page in pages)
            while not all(call(page,'multiplayerSmoke.canStart()') for page in pages):
                for page in pages:assert call(page,'multiplayerSmoke.pollNetwork()')
                assert time.monotonic()<deadline,'new network barrier timed out'
                pages[0].wait_for_timeout(5)
            # Replay the captured previous-generation HELLO over the actual
            # DataChannel/relay socket. Do not bypass the title's network pump.
            before_ignored=call(pages[1],'multiplayerSmoke.transportStatus()')[6]
            call(pages[0],'''bytes=>{
              const state=globalThis.__eaglerPeerTransport,data=new Uint8Array(bytes);
              if(state.route==='rtc')state.peers.get(1).controlDc.send(data);
              else state.relay.send(new Uint8Array([0xe7,1,...data]));
            }''',old_hello)
            while call(pages[1],'multiplayerSmoke.transportStatus()')[6]<=before_ignored:
                assert call(pages[1],'multiplayerSmoke.pollNetwork()')
                assert time.monotonic()<deadline,'old network packet was not observed'
                pages[0].wait_for_timeout(5)
            assert call(pages[1],'multiplayerSmoke.netStatus()')[2:4]==[0,-1]
            deadline=time.monotonic()+90
            while True:
                frontier=[]
                for seat,page in enumerate(pages):
                    current=call(page,'multiplayerSmoke.netStatus()')
                    frontier.append(call(page,'v=>multiplayerSmoke.peerAdvance(...v)',[29,inputs(seat,current[2])]))
                if all(value['net'][3]==29 and value['net'][5]==-1 and value['transport'][9]==29 for value in frontier):break
                assert time.monotonic()<deadline,frontier
                pages[0].wait_for_timeout(2)
            states=[call(page,'multiplayerSmoke.status()') for page in pages]
            hashes=[call(page,'multiplayerSmoke.canonical()') for page in pages]
            assert states[0][:2]+states[0][3:]==states[1][:2]+states[1][3:],states
            assert hashes[0][1]==hashes[1][1],hashes
            assert call(pages[0],'multiplayerSmoke.audioStatus()')==call(pages[1],'multiplayerSmoke.audioStatus()')
            report['retry']={'original':original,'fresh':fresh,'states':states,'hashes':hashes,
                             'network':[call(page,'multiplayerSmoke.transportStatus()') for page in pages]}
            print('network-check: native retry, new session gate and stale-wire rejection PASS',flush=True)

        if args.drop_fast:
            report['dropped']=[call(page,'globalThis.__testFastDropped') for page in pages]
            assert all(count>0 for count in report['dropped']),report['dropped']
        assert not report['errors'],report['errors']
        # Closing an actual native endpoint tears down the real connection;
        # its partner must report failure instead of silently playing alone.
        call(pages[-1],'multiplayerSmoke.close()')
        # A relay peer leaving does not close our own WebSocket. Its liveness
        # failure comes from SessionChannel's 15-second confirmed-input guard;
        # RTC instead reports its DataChannel close immediately.
        deadline=time.monotonic()+(20 if args.mode=='relay' else 10)
        while call(pages[0],'multiplayerSmoke.pollNetwork()'):
            assert time.monotonic()<deadline,'closed peer did not fail transport'
            pages[0].wait_for_timeout(10)
        report['disconnect']={'status':call(pages[0],'multiplayerSmoke.transportStatus()'),
                              'error':call(pages[0],'multiplayerSmoke.networkError()'),
                              'detail':call(pages[0],'multiplayerSmoke.errorDetail()')}
        assert report['disconnect']['status'][3]!=0,report['disconnect']
        assert report['disconnect']['error'] in report['disconnect']['detail'],report['disconnect']
        assert 'confirmed [' in report['disconnect']['detail'] and 'recv ' in report['disconnect']['detail'],report['disconnect']
        report['passed']=True
    except BaseException as error:
        report['failure']=str(error)
        raise
    finally:
        for context in contexts:context.close()
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps({'passed':report['passed'],'mode':args.mode,'players':args.players}))
