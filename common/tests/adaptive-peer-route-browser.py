"""Exercise the production EM_JS transport in two browser frames.

python common/tests/adaptive-peer-route-browser.py --turn-origin https://touhou.vip
Only short-lived diagnostic credentials are fetched. No room or game is created.
"""
import argparse
import http.server
import json
import pathlib
import threading
import datetime
import subprocess
from playwright.sync_api import sync_playwright

parser = argparse.ArgumentParser()
parser.add_argument('--turn-origin', required=True)
parser.add_argument('--browser', default='msedge')
parser.add_argument('--game', choices=['th09', 'th12', 'th15'], default='th09')
parser.add_argument('--output', type=pathlib.Path)
args = parser.parse_args()
root = pathlib.Path(__file__).resolve().parents[2]
source = (root / 'common/src/netplay/BrowserPeerTransport.cpp').read_text(encoding='utf-8')


def embedded(name, parameters):
    start = source.index(name)
    start = source.index('{', start) + 1
    end = source.index('\n});', start)
    return f'({parameters}) => {{' + source[start:end] + '\n}'


class Handler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        self.send_response(200)
        self.send_header('Content-Type', 'text/html; charset=utf-8')
        self.end_headers()
        self.wfile.write(b'<!doctype html><title>Adaptive routing test</title>')

    def log_message(self, *_):
        pass


server = http.server.ThreadingHTTPServer(('127.0.0.1', 0), Handler)
threading.Thread(target=server.serve_forever, daemon=True).start()
try:
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(channel=args.browser, headless=True, args=['--no-proxy-server'])
        page = browser.new_page()
        page.goto(f'http://127.0.0.1:{server.server_port}/')
        # Fetch the same ephemeral ICE contract used by the launcher diagnostic.
        servers = page.evaluate("""async origin => {
            const url = new URL('/eagler-netplay/', origin);
            url.protocol = 'wss:'; url.searchParams.set('diagnostic', '1');
            return await new Promise((resolve,reject) => {
                const ws = new WebSocket(url);
                const timer = setTimeout(() => {ws.close(); reject(Error('diagnostic timeout'));}, 10000);
                ws.onmessage = event => {
                    const m = JSON.parse(event.data);
                    if (m.type !== 'diagnostic-ready') return;
                    clearTimeout(timer); ws.close(); resolve(m.iceServers);
                };
                ws.onerror = () => {clearTimeout(timer); reject(Error('diagnostic unavailable'));};
            });
        }""", args.turn_origin)
        assert any(str(server.get('urls', '')).find('turn:') >= 0 for server in servers), 'TURN credentials missing'
        page.evaluate("""servers => {
            window.testServers = servers;
            window.sockets = [new Map(),new Map()]; window.readySeats = new Set();
            window.registerSocket = (socket,seat,signal) => {
                sockets[seat].set(signal,socket);
                setTimeout(() => {socket.readyState=1; socket.onopen?.();
                    if (signal && sockets.every(m => m.has(true))) for (let p=0;p<2;p++)
                        sockets[p].get(true).onmessage?.({data:JSON.stringify({type:'peers',peers:[1-p],iceServers:servers})});
                },0);
            };
            window.sendSignal = (seat,text) => {
                const message=JSON.parse(text);
                if(message.type==='signal')setTimeout(()=>sockets[1-seat].get(true)?.onmessage?.({data:JSON.stringify({...message,from:seat})}),0);
                if(message.type==='rtc-ready') {
                    readySeats.add(seat);
                    if(readySeats.size===2)for(let p=0;p<2;p++)sockets[p].get(true).onmessage({data:JSON.stringify({type:'route',mode:'rtc'})});
                }
            };
        }""", servers)
        frames = []
        for seat in range(2):
            page.evaluate("""seat => {const frame=document.createElement('iframe');frame.src='/peer?seat='+seat;document.body.append(frame);} """, seat)
            page.wait_for_function('seat => window.frames.length > seat && window.frames[seat].document.readyState === "complete"', arg=seat)
            frame = page.frames[seat + 1]
            frame.evaluate("""({seat,game}) => {
                window.testSeat=seat; window.Module={eaglerOptions:{netplayIceServers:parent.testServers}};
                window.HEAPU8=new Uint8Array(1024);
                HEAPU8.set(new TextEncoder().encode('wss://fixture.test/room'),0);
                HEAPU8.set(new TextEncoder().encode(game),100);
                window.__eaglerNetplayLanActive=true;
                window.errors=[];
                window.addEventListener('error',e=>errors.push(e.message));
                window.dropInput=false;window.holdControl=false;window.held=[];
                const original=RTCDataChannel.prototype.send;
                RTCDataChannel.prototype.send=function(data){
                    if(this.label===game+'-input' && dropInput)return;
                    if(this.label===game+'-control' && holdControl && typeof data!=='string'){
                        const copy=data.slice();held.push(()=>original.call(this,copy));return;
                    }
                    return original.call(this,data);
                };
                window.WebSocket=class {
                    static OPEN=1;static CONNECTING=0;static CLOSING=2;
                    readyState=0;bufferedAmount=0;
                    constructor(url){this.signal=new URL(url).searchParams.get('signal')==='1';parent.registerSocket(this,seat,this.signal);}
                    send(text){if(this.signal)parent.sendSignal(seat,text);}
                    close(){this.readyState=3;this.onclose?.({code:1000});}
                };
            }""", {'seat': seat, 'game': args.game})
            frame.evaluate('window.connectTransport=' + embedded('EM_JS(int, eagler_peer_connect,', 'relayUrlPtr,tagPtr,localPlayer,playerCount,stageSpectatorHistory'))
            frame.evaluate('window.sendControl=' + embedded('EM_JS(int, eagler_peer_send,', 'data,size,control'))
            frame.evaluate('window.sendInput=' + embedded('EM_JS(int, eagler_peer_send_to,', 'peerId,data,size'))
            assert frame.evaluate('seat=>connectTransport(0,100,seat,2,0)', seat) == 1
            frames.append(frame)
        page.wait_for_function("() => Array.from(document.querySelectorAll('iframe'),e=>e.contentWindow).every(f=>f.__eaglerPeerTransport?.route==='rtc' && [...f.__eaglerPeerTransport.peers.values()].every(p=>p.adaptive?.snapshot().standbyReady))", timeout=25000)
        for frame in frames:
            assert frame.evaluate("__eaglerNetplayPath") == 'direct'
        frames[0].evaluate("HEAPU8.set([41,42,43],200);holdControl=true;sendControl(200,3,1);dropInput=true")
        frames[1].evaluate('dropInput=true')
        page.wait_for_function("() => Array.from(document.querySelectorAll('iframe'),e=>e.contentWindow).every(f=>[...f.__eaglerPeerTransport.peers.values()].every(p=>p.adaptive.snapshot().selected==='turn'))", timeout=22000)
        frames[0].evaluate("HEAPU8.set([51,52],210);sendControl(210,2,1);HEAPU8.set([61,62],220);sendInput(1,220,2)")
        page.wait_for_function("() => [...window.frames[1].__eaglerPeerTransport.received].some(p=>p[0]===61)", timeout=5000)
        frames[0].evaluate('holdControl=false;for(const fn of held.splice(0))fn()')
        page.wait_for_timeout(300)
        received = frames[1].evaluate('__eaglerPeerTransport.received.map(p=>[...p])')
        assert [p for p in received if p[0] in [41,51]] == [[41,42,43],[51,52]], received
        assert received.count([61,62]) == 1 and len(received) == 3, received
        results = []
        for frame in frames:
            result = frame.evaluate("""async () => {
                const state=__eaglerPeerTransport,peer=[...state.peers.values()][0];
                const stats=await peer.pc.getStats();let pair;
                for(const row of stats.values())if(row.type==='transport' && row.selectedCandidatePairId)pair=stats.get(row.selectedCandidatePairId);
                const candidates=pair?[stats.get(pair.localCandidateId)?.candidateType,stats.get(pair.remoteCandidateId)?.candidateType]:[];
                return {route:state.route,failed:state.failed,disconnected:state.disconnected,selection:peer.adaptive.snapshot(),candidates,errors};
            }""")
            assert result['candidates'] == ['relay', 'relay'], result['candidates']
            assert not result['failed'] and not result['disconnected'] and not result['errors'], result
            results.append(result)
        for frame in frames:
            frame.evaluate('__eaglerPeerTransport.close()')
        report = {'test':'production EM_JS | real direct and TURN | controlled input loss | reliable control replay',
            'game':args.game, 'timestamp':datetime.datetime.now(datetime.timezone.utc).isoformat(),
            'base':subprocess.check_output(['git','rev-parse','HEAD'],cwd=root,text=True).strip(),
            'received':received,'peers':results}
        if args.output:
            args.output.parent.mkdir(parents=True,exist_ok=True)
            args.output.write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
        print(json.dumps(report, ensure_ascii=False))
        browser.close()
finally:
    server.shutdown()
