from __future__ import annotations
import os, socket, subprocess, sys, time
from pathlib import Path
from playwright.sync_api import sync_playwright
WORKSPACE=Path(__file__).resolve().parents[2]
RELAY_ROOT=WORKSPACE/'th07-eagler'/'tools'/'netplay'

def free_port():
    with socket.socket() as s:s.bind(('127.0.0.1',0));return s.getsockname()[1]

def wait_http(url,timeout=10):
    import urllib.request
    end=time.time()+timeout
    while time.time()<end:
        try:
            with urllib.request.urlopen(url,timeout=.5) as r:
                if r.status<400:return
        except Exception:time.sleep(.1)
    raise RuntimeError('http timeout')

def wait_relay(proc,timeout=10):
    end=time.time()+timeout
    while time.time()<end:
        line=proc.stdout.readline()
        if line:
            print('RELAY',line.rstrip(),flush=True)
            if 'LAN relay listening' in line:return
        elif proc.poll() is not None:raise RuntimeError('relay exit')
    raise RuntimeError('relay timeout')

def run_game(game:str):
    root=WORKSPACE/(game+'-eagler')
    hp,rp=free_port(),free_port()
    env=os.environ.copy();env.update({'TH07_RELAY_HOST':'127.0.0.1','TH07_RELAY_PORT':str(rp),'TH07_RTC_TIMEOUT_MS':'4500','TH07_STUN_URLS':'','TH07_RELAY_DELAY_MS':'0','TH07_RELAY_JITTER_MS':'0'})
    http=subprocess.Popen([sys.executable,'-m','http.server',str(hp),'--bind','127.0.0.1'],cwd=root,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    relay=subprocess.Popen(['node','lan-relay.cjs'],cwd=RELAY_ROOT,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,bufsize=1)
    browsers=[]
    try:
        host='tests/netplay-browser-host.html' if game=='th06' else 'tests/netplay-countdown-audio-host.html'
        wait_http(f'http://127.0.0.1:{hp}/{host}');wait_relay(relay)
        room=f'{game}-countdown-{int(time.time()*1000)}'
        with sync_playwright() as pw:
            pages=[]
            for i in range(2):
                b=pw.chromium.launch(headless=True,args=['--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding']);browsers.append(b)
                c=b.new_context(viewport={'width':960,'height':720});c.add_init_script('delete globalThis.RTCPeerConnection')
                pg=c.new_page();pages.append(pg)
                relay_url=f'ws://127.0.0.1:{rp}/?room={room}&player={i}'
                if game=='th06':
                    url=f'http://127.0.0.1:{hp}/{host}?player={i}&players=2&relay={relay_url}&frames=1200&input=none&scripted=0&dev=1'
                else:
                    url=f'http://127.0.0.1:{hp}/{host}?player={i}&players=2&relay={relay_url}'
                pg.goto(url,wait_until='load',timeout=30000)
            deadline=time.time()+40
            while time.time()<deadline:
                vals=[]
                for pg in pages:
                    try:
                        vals.append(pg.evaluate("""() => {const w=document.getElementById('runtime')?.contentWindow;return {active:!!w?.__eaglerNetplayLanActive,frame:Number(w?.__eaglerNetplayLanFrame||0),hook:typeof w?.Module?._TouhouDebugCountdownAudioStep==='function',audio:!!w?.Module?.SDL3?.audio_playback?.scriptProcessorNode,failed:!!w?.__eaglerNetplayFailed,error:String(w?.__eaglerNetplayError||'')};}"""))
                    except Exception: vals.append({})
                if any(v.get('failed') for v in vals):raise RuntimeError(vals)
                if all(v.get('active') and v.get('hook') and v.get('audio') and v.get('frame',0)>=100 for v in vals):break
                time.sleep(.05)
            else:raise RuntimeError(f'{game} runtime/hook not ready: {vals}')
            page=pages[0]
            ok=page.evaluate("""() => {const w=document.getElementById('runtime')?.contentWindow,s=w?.Module?.SDL3,node=s?.audio_playback?.scriptProcessorNode;if(!node||typeof node.onaudioprocess!=='function')return false;const original=node.onaudioprocess;w.__countdownAudioTap={capture:true,blocks:[]};node.onaudioprocess=function(e){original.call(this,e);const t=w.__countdownAudioTap;if(!t?.capture)return;const x=e.outputBuffer.getChannelData(0);let ss=0,p=0;for(let i=0;i<x.length;i++){const a=Math.abs(x[i]);ss+=x[i]*x[i];if(a>p)p=a;}t.blocks.push({frame:Number(w.__eaglerNetplayLanFrame||0),rms:Math.sqrt(ss/x.length),peak:p});};return true;}""")
            if not ok:raise RuntimeError('tap install failed')
            # Let any pre-existing output drain, then clear the measurement list.
            time.sleep(.35)
            page.evaluate("""() => {const w=document.getElementById('runtime')?.contentWindow;if(w?.__countdownAudioTap)w.__countdownAudioTap.blocks=[];}""")
            seq=[
                (10,11,42,1,1), # initialize phase; no warning
                (9,10,42,1,0),  # first 9: play
                (10,9,42,1,0),  # rollback upwards: no reset
                (9,10,42,1,0),  # revisit 9: must not replay
                (8,9,42,1,0),   # first 8: play
                (9,8,42,1,0),   # rollback upwards one second
                (8,9,42,1,0),   # revisit 8: must not replay
            ]
            returns=[];windows=[]
            for args in seq:
                before=page.evaluate("""() => document.getElementById('runtime')?.contentWindow?.__countdownAudioTap?.blocks?.length||0""")
                ret=page.evaluate("""args => {const w=document.getElementById('runtime')?.contentWindow;return w.Module._TouhouDebugCountdownAudioStep(...args);}""",list(args))
                returns.append(int(ret));time.sleep(.35)
                win=page.evaluate("""before => {const w=document.getElementById('runtime')?.contentWindow,b=(w?.__countdownAudioTap?.blocks||[]).slice(before);return {count:b.length,maxPeak:b.reduce((m,x)=>Math.max(m,x.peak),0),maxRms:b.reduce((m,x)=>Math.max(m,x.rms),0),frames:b.map(x=>x.frame)};}""",before)
                windows.append(win)
            time.sleep(.5)
            blocks=page.evaluate("""() => document.getElementById('runtime')?.contentWindow?.__countdownAudioTap?.blocks||[]""")
            expected=[0,1,0,0,1,0,0]
            print(game,'returns',returns,'expected',expected,flush=True)
            for i,(args,ret,win) in enumerate(zip(seq,returns,windows)):
                print(game,f'step{i} sec={args[0]} last={args[1]} ret={ret} blocks={win["count"]} peak={win["maxPeak"]:.5f} rms={win["maxRms"]:.5f} frames={win["frames"]}',flush=True)
            if returns!=expected:raise RuntimeError(f'{game} countdown duplicate-submit regression: {returns}')
            # At least the two intended warning submissions must reach final PCM.
            for idx in (1,4):
                if windows[idx]['maxPeak']<0.05 or windows[idx]['maxRms']<0.005:
                    raise RuntimeError(f'{game} intended countdown warning missing from final PCM at step {idx}: {windows[idx]}')
            print(game,'countdown warning final WebAudio: PASS',flush=True)
    finally:
        for b in browsers:
            try:b.close()
            except:pass
        relay.terminate();http.terminate()
        try:relay.wait(3)
        except:relay.kill()
        try:http.wait(3)
        except:http.kill()

def main():
    for g in ('th06','th07'):run_game(g)
if __name__=='__main__':main()
