"""Private local browser gate; pack/font are inputs, never copied into source."""
import argparse, json, re, threading
from pathlib import Path
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from playwright.sync_api import sync_playwright

p=argparse.ArgumentParser()
inputs=p.add_mutually_exclusive_group(required=True)
inputs.add_argument('--pack', type=Path)
inputs.add_argument('--assets', type=Path)
p.add_argument('--font', required=True, type=Path)
p.add_argument('--browser', default='C:/Program Files/Google/Chrome/Application/chrome.exe')
p.add_argument('--build', type=Path)
a=p.parse_args()
root=Path(__file__).resolve().parent.parent
build=a.build or root/'build-web-pr-review-on'
files={}
if a.assets:
    for name in ['紅魔郷CM.DAT','紅魔郷ED.DAT','紅魔郷IN.DAT','紅魔郷MD.DAT','紅魔郷ST.DAT','紅魔郷TL.DAT']:
        files['/'+name]=(a.assets/name).read_bytes()
else:
    source=(a.pack/'th06.js').read_text(encoding='utf-8')
    entries=re.findall(r'filename:"([^"]+)",start:(\d+),end:(\d+)',source[source.index('loadPackage({'):])
    if not entries: raise ValueError('Pack has no readable asset inventory')
    blob=(a.pack/'th06.data').read_bytes()
    for name,lo,hi in entries:
        data=blob[int(lo):int(hi)]
        suffix=re.search(r'(CM|ED|IN|MD|ST|TL)\.DAT$',name,re.I)
        files['/紅魔郷'+suffix[1]+'.DAT' if suffix else name]=data
files['/unifont.otf']=a.font.read_bytes()
files['/msgothic.ttc']=(a.font.parent/'msgothic.ttc').read_bytes()

class Handler(BaseHTTPRequestHandler):
    def log_message(self,*args): pass
    def do_GET(self):
        from urllib.parse import unquote,urlsplit
        path=unquote(urlsplit(self.path).path)
        mime='application/octet-stream'
        if path=='/':
            data=b'<html><body style="margin:0"><iframe id="runtime" style="width:100vw;height:100vh;border:0" src="/th06.html?hosted=1&runtimeEpoch=1"></iframe></body></html>'
            mime='text/html'
        elif path=='/favicon.ico': data=b''
        elif path in files: data=files[path]
        elif path in ['/th06.html','/th06.js','/th06.wasm']:
            data=(build/path[1:]).read_bytes()
            mime={'html':'text/html','js':'text/javascript','wasm':'application/wasm'}[path.rsplit('.',1)[1]]
        else:
            self.send_error(404); return
        self.send_response(200); self.send_header('Content-Type',mime)
        self.send_header('Content-Length',str(len(data))); self.end_headers(); self.wfile.write(data)

server=ThreadingHTTPServer(('127.0.0.1',0),Handler)
threading.Thread(target=server.serve_forever,daemon=True).start()
out=build/'browser-check'; out.mkdir(exist_ok=True)
errors=[]
try:
    with sync_playwright() as pw:
        browser=pw.chromium.launch(executable_path=a.browser,headless=True,
            args=['--use-angle=swiftshader','--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required'])
        page=browser.new_page(viewport={'width':960,'height':720})
        page.on('pageerror',lambda e:errors.append(str(e)))
        page.on('response',lambda r: print('HTTP',r.status,r.url,flush=True) if r.status>=400 else None)
        page.goto(f'http://127.0.0.1:{server.server_port}/')
        frame=page.frame_locator('#runtime')
        def console(m):
            if m.type=='error':
                print('browser:',m.text,flush=True)
                if 'initialization failed' in m.text or 'render loop exited' in m.text or 'Aborted(' in m.text: errors.append(m.text)
        page.on('console',console)
        page.wait_for_timeout(5000)
        page.wait_for_function("typeof document.querySelector('iframe').contentWindow.Module?.callMain==='function'",timeout=30000)
        def rpc(command,**fields):
            return page.evaluate("""m=>new Promise((resolve,reject)=>{
                let id=crypto.randomUUID(),t=setTimeout(()=>reject(Error('RPC '+m.command)),20000);
                function receive(e){if(e.data?.request===id){clearTimeout(t);window.removeEventListener('message',receive);e.data.ok?resolve(e.data):reject(Error(e.data.error));}}
                window.addEventListener('message',receive);
                document.querySelector('iframe').contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th06',epoch:1,request:id,...m},location.origin);
            })""",{'command':command,**fields})
        rpc('resources',resources=[{'url':name,'path':name} for name in files])
        rpc('configure',music='none',resources=[],sharedResources=[],options={'thpracEnabled':True,'thpracLocale':'zh-CN','touchEnabled':True,'limitPresentationTo60':True})
        rpc('launch')
        page.wait_for_timeout(3500)
        canvas=frame.locator('#canvas'); canvas.click()
        def key(name):
            page.keyboard.down(name); page.wait_for_timeout(90)
            page.keyboard.up(name); page.wait_for_timeout(180)
        key('z'); page.wait_for_timeout(1500)
        key('Backspace')
        page.screenshot(path=str(out/'title-backspace.png'))
        key('F12')
        page.screenshot(path=str(out/'title-f12.png'))
        def click(x,y):
            page.mouse.click(x*1.5,y*1.5,delay=80);page.wait_for_timeout(200)
        # Native checkbox coordinates in the 640x480 F12 window.
        click(15,380) # player hitbox
        page.mouse.move(700,500);page.mouse.wheel(0,650);page.wait_for_timeout(300)
        page.screenshot(path=str(out/'advanced-bottom.png'))
        click(80,250);page.screenshot(path=str(out/'statistics.png'))
        click(80,250)
        click(80,278);click(15,306)
        page.screenshot(path=str(out/'blind-setting.png'))
        key('F12'); key('Tab')
        page.screenshot(path=str(out/'title-tab.png'))
        key('Tab');key('Backspace')
        # Enter a normal run, retaining the trainer's source-owned settings.
        for _ in range(8): key('z');page.wait_for_timeout(1200)
        page.wait_for_timeout(2000)
        key('F12');page.screenshot(path=str(out/'game-f12.png'))
        key('F12');key('Tab');page.screenshot(path=str(out/'game-tab.png'))
        rpc('touch-controls',fireEnabled=False,focusEnabled=False,bombSerial=0,escapeSerial=0,
            joystickX=0,joystickY=0,touchSensitivity=100,thpracMobilePanelsEnabled=True,thpracMobilePanelsVisible=True)
        page.wait_for_timeout(400);page.screenshot(path=str(out/'mobile-collapsed.png'))
        # The carrier path is independent of SDL's synthesized mouse stream.
        def tap(x,y):
            for event_type in ['down','up']:
                page.evaluate("""message=>document.querySelector('iframe').contentWindow.postMessage({
                    protocol:'eagler-touhou/1',game:'th06',epoch:1,command:'direct-touch',
                    type:message.type,id:17,x:message.x,y:message.y
                },location.origin)""",{'type':event_type,'x':x/640,'y':y/480})
            page.wait_for_timeout(200)
        tap(65,23);tap(18,64)
        page.screenshot(path=str(out/'mobile-checkbox.png'))
        assert not errors, errors
        assert not any('render loop exited' in e or 'initialization failed' in e for e in errors), errors
        report={'passed':True,'errors':errors,'checks':['private pack mount','title Backspace','title F12','title Tab','normal run','game F12/Tab','mobile collapse and checkbox clicks'],
                'limitations':['headless Chrome; not a physical mobile-device or retail replay oracle']}
        (out/'report.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
        print(json.dumps(report)); browser.close()
finally: server.shutdown(); server.server_close()
