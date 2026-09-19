from __future__ import annotations
import os, socket, subprocess, sys, time
from pathlib import Path
from integration_support import require_host_relay
from playwright.sync_api import sync_playwright

ROOT=Path(__file__).resolve().parents[1]
HOST_ROOT, RELAY_SCRIPT = require_host_relay()

def free_port():
    with socket.socket() as s:
        s.bind(('127.0.0.1',0)); return s.getsockname()[1]

def wait_http(url, timeout=10):
    import urllib.request
    end=time.time()+timeout
    while time.time()<end:
        try:
            with urllib.request.urlopen(url,timeout=.5) as r:
                if r.status<400:return
        except Exception: time.sleep(.1)
    raise RuntimeError('http timeout')

def wait_relay(proc, timeout=10):
    end=time.time()+timeout
    while time.time()<end:
        line=proc.stdout.readline()
        if line:
            print('RELAY',line.rstrip())
            if 'LAN relay listening' in line:return
        elif proc.poll() is not None: raise RuntimeError('relay exit')
    raise RuntimeError('relay timeout')

def install_tap(page):
    return page.evaluate('''() => {
      const w=document.getElementById('runtime')?.contentWindow;
      const s=w?.Module?.SDL3;
      const node=s?.audio_playback?.scriptProcessorNode;
      if(!node || typeof node.onaudioprocess!=='function') return false;
      if(w.__audioTapInstalled) return true;
      const original=node.onaudioprocess;
      w.__audioTap={blocks:[], totalFrames:0, sampleRate:Number(s.audioContext?.sampleRate||0)};
      node.onaudioprocess=function(e){
        original.call(this,e);
        const out=e.outputBuffer;
        const n=out.length|0, ch=out.numberOfChannels|0;
        let sum=0, peak=0;
        for(let c=0;c<ch;c++){
          const a=out.getChannelData(c);
          for(let i=0;i<n;i++){ const v=a[i]; sum+=v*v; const av=Math.abs(v); if(av>peak)peak=av; }
        }
        const rms=Math.sqrt(sum/Math.max(1,n*ch));
        w.__audioTap.blocks.push({t:Number(s.audioContext?.currentTime||0),n,ch,rms,peak});
        if(w.__audioTap.blocks.length>4096) w.__audioTap.blocks.shift();
        w.__audioTap.totalFrames+=n;
      };
      w.__audioTapInstalled=true; return true;
    }''')

def snap(page):
    return page.evaluate('''() => {
      const w=document.getElementById('runtime')?.contentWindow, a=w?.__audioTap, bs=a?.blocks||[];
      return {frame:Number(w?.__eaglerNetplayLanFrame||0),rollback:Number(w?.__eaglerNetplayLanRollback||0),installed:!!w?.__audioTapInstalled,sampleRate:Number(a?.sampleRate||0),totalFrames:Number(a?.totalFrames||0),blocks:bs.slice(-1000)};
    }''')

def main():
    hp,rp=free_port(),free_port()
    env=os.environ.copy(); env.update({'TH07_RELAY_HOST':'127.0.0.1','TH07_RELAY_PORT':str(rp),'TH07_RTC_TIMEOUT_MS':'4500','TH07_STUN_URLS':''})
    http=subprocess.Popen([sys.executable,'-m','http.server',str(hp),'--bind','127.0.0.1'],cwd=ROOT,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    relay=subprocess.Popen(['node',str(RELAY_SCRIPT)],cwd=HOST_ROOT,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,bufsize=1)
    browsers=[]
    try:
      wait_http(f'http://127.0.0.1:{hp}/tests/netplay-browser-host.html'); wait_relay(relay)
      room=f'audio-tap-{int(time.time()*1000)}'
      with sync_playwright() as pw:
        pages=[]
        for i in range(2):
          b=pw.chromium.launch(headless=True,args=['--autoplay-policy=no-user-gesture-required','--disable-background-timer-throttling','--disable-backgrounding-occluded-windows','--disable-renderer-backgrounding']); browsers.append(b)
          c=b.new_context(viewport={'width':960,'height':720}); c.add_init_script('delete globalThis.RTCPeerConnection')
          page=c.new_page(); pages.append(page)
          relay_url=f'ws://127.0.0.1:{rp}/?room={room}&player={i}'
          page.goto(f'http://127.0.0.1:{hp}/tests/netplay-browser-host.html?player={i}&players=2&relay={relay_url}&frames=420&input=none&scripted=1',wait_until='load',timeout=30000)
        deadline=time.time()+45; armed=[False,False]
        while time.time()<deadline:
          for i,p in enumerate(pages):
            if not armed[i]:
              try: armed[i]=bool(install_tap(p))
              except Exception: pass
          vals=[snap(p) for p in pages]
          if all(v['frame']>=360 for v in vals) and all(v['totalFrames']>0 for v in vals): break
          time.sleep(.1)
        vals=[snap(p) for p in pages]
        for i,v in enumerate(vals):
          nonzero=[b for b in v['blocks'] if b['peak']>1e-5]
          loud=sorted((b['peak'] for b in nonzero),reverse=True)[:10]
          print(f'P{i+1} frame={v["frame"]} rollback={v["rollback"]} installed={v["installed"]} sr={v["sampleRate"]} totalFrames={v["totalFrames"]} blocks={len(v["blocks"])} nonzero={len(nonzero)} maxPeak={max(loud or [0]):.6f}')
          print('TOP',loud)
          if not v['installed'] or v['sampleRate']<=0 or v['totalFrames']<=0 or not nonzero: raise RuntimeError(f'PCM tap failed P{i+1}')
        print('TH06 final WebAudio PCM tap: PASS')
    finally:
      for b in browsers:
        try:b.close()
        except:pass
      relay.terminate(); http.terminate()
      try:relay.wait(3)
      except:relay.kill()
      try:http.wait(3)
      except:http.kill()
if __name__=='__main__': main()
