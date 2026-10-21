"""Production stage Replay shells with independently captured BGM/SFX output."""
import argparse, hashlib, json, mimetypes, threading, time
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import urlsplit
from playwright.sync_api import sync_playwright

p = argparse.ArgumentParser()
p.add_argument('--game', choices=['th08', 'th10', 'th11', 'th15'], default='th08')
p.add_argument('--multiplayer', action='store_true')
p.add_argument('--runtime', type=Path, required=True)
p.add_argument('--data', type=Path, required=True)
p.add_argument('--fixtures', type=Path, required=True)
p.add_argument('--music', type=Path, required=True)
p.add_argument('--font', type=Path, required=True)
p.add_argument('--unifont', type=Path, required=True)
p.add_argument('--output', type=Path, required=True)
p.add_argument('--browser', default='C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe')
a = p.parse_args()
fixtures = sorted([*a.fixtures.glob('*.rpy'), *a.fixtures.glob('*.rpyx')])
assert fixtures, 'Provide original Demo or native recorded Replay files'
number = str(int(a.game[2:])) if a.game == 'th08' else a.game[2:]
extension = '.rpyx' if a.game == 'th08' and a.multiplayer else '.rpy'
replays = [(fixture, 'replay/th'+number+'_'+str(index+1).zfill(2)+extension) for index, fixture in enumerate((fixtures*2)[:2])]
save_root = '/saves'+a.game+(('-multiplayer' if a.game in ['th08', 'th10'] else 'mp') if a.multiplayer else '')+('/jp' if a.game == 'th10' else '')
files = {'/data': a.data, '/shared/msgothic.ttc': a.font, '/shared/unifont.otf': a.unifont}
files.update({'/fixture/'+f.name: f for f in fixtures})
files.update({'/music/'+f.name: f for f in a.music.glob('*.ogg')})
resources = [{'path': ('/bgm-ogg/' if a.game in ['th08', 'th10'] else '/music/')+f.name, 'url': '/music/'+f.name} for f in a.music.glob('*.ogg')]
assert resources, 'Provide the explicit OGG music directory'
host = '''<!doctype html><button id="launch">Launch</button><button id="play">Play</button>
<script>
window.events=[];window.__eaglerPrepareManagedRuntimeDataV1=async()=>({buffer:await(await fetch('/data')).arrayBuffer()});
addEventListener('message',e=>{if(e.origin===location.origin&&e.source===document.querySelector('iframe').contentWindow)events.push(e.data)});
window.rpc=(command,payload={})=>new Promise((resolve,reject)=>{const request=crypto.randomUUID(),frame=document.querySelector('iframe');
 const timer=setTimeout(()=>{removeEventListener('message',receive);reject(Error('Timeout '+command))},120000);
 function receive(e){if(e.origin!==location.origin||e.source!==frame.contentWindow||e.data?.request!==request)return;clearTimeout(timer);removeEventListener('message',receive);e.data.ok?resolve(e.data):reject(Error(e.data.error));}
 addEventListener('message',receive);frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game:'th08',epoch:1,command,request,...payload},location.origin)});
document.querySelector('#launch').onclick=()=>{window.launched=rpc('launch')};
document.querySelector('#play').onclick=()=>{window.opened=rpc('playback',window.selection)};
</script><iframe allow="autoplay; fullscreen" style="width:640px;height:480px" src="/runtime/th08.html?managedData=1&gameGeneration=audio-test&runtimeEpoch=1"></iframe>'''
host = host.replace('th08', a.game)
if a.multiplayer: host = host.replace('runtimeEpoch=1', 'runtimeVariant=multiplayer&runtimeEpoch=1')

class Handler(BaseHTTPRequestHandler):
    def log_message(self, *args): pass
    def do_GET(self):
        path = urlsplit(self.path).path
        if path == '/': data, kind = host.encode(), 'text/html'
        else:
            file = files.get(path)
            if path.startswith('/runtime/'):
                candidate = (a.runtime/path.removeprefix('/runtime/')).resolve()
                if candidate.is_relative_to(a.runtime.resolve()): file = candidate
            if not file or not file.is_file(): self.send_error(404); return
            data = file.read_bytes()
            kind = 'application/wasm' if file.suffix == '.wasm' else 'text/javascript' if file.suffix in ['.mjs', '.js'] else mimetypes.guess_type(file.name)[0] or 'application/octet-stream'
        self.send_response(200); self.send_header('Content-Type', kind); self.send_header('Content-Length', str(len(data))); self.end_headers(); self.wfile.write(data)

server = ThreadingHTTPServer(('127.0.0.1', 0), Handler)
threading.Thread(target=server.serve_forever, daemon=True).start()
wasm = next(a.runtime.glob('*.wasm'))
report = {'scope': 'Edge | production iframe | explicitly supplied Replay | WebAudio graph capture',
          'game': a.game, 'variant': 'multiplayer' if a.multiplayer else 'normal',
          'wasm': hashlib.sha256(wasm.read_bytes()).hexdigest(),
          'fixtures': [{'file': str(f), 'sha256': hashlib.sha256(f.read_bytes()).hexdigest()} for f in fixtures], 'cases': []}
with sync_playwright() as pw:
    browser = pw.chromium.launch(executable_path=a.browser, headless=True, args=['--enable-unsafe-swiftshader', '--autoplay-policy=document-user-activation-required'])
    report['browser'] = browser.version
    try:
        for music in ['none', 'ogg']:
            context = browser.new_context(); page = context.new_page(); errors = []
            page.on('pageerror', lambda error: errors.append(str(error)))
            cdp = context.new_cdp_session(page)
            def evaluate(expression):
                result = cdp.send('Runtime.evaluate', {'expression': expression, 'userGesture': False, 'awaitPromise': True, 'returnByValue': True})
                if 'exceptionDetails' in result: raise RuntimeError(result['exceptionDetails'])
                return result['result'].get('value')
            try:
                page.goto('http://127.0.0.1:'+str(server.server_port))
                page.wait_for_function("events.some(e=>e.event==='ready')", timeout=120000)
                payload = {'music': music, 'language': 'ja', 'resources': resources if music == 'ogg' else [],
                           'sharedResources': [{'path': '/msgothic.ttc', 'url': '/shared/msgothic.ttc'}, {'path': '/unifont.otf', 'url': '/shared/unifont.otf'}],
                           'options': {'replayViewer': True, 'thpracEnabled': False, 'touchEnabled': False}}
                evaluate('rpc("configure",'+json.dumps(payload)+')')
                page.click('#launch'); evaluate('window.launched')
                child = page.frames[1]
                for fixture, path in replays:
                    child.evaluate("async ({name,path,root})=>{Module.FS.mkdirTree(root+'/replay');Module.FS.writeFile(root+'/'+path,new Uint8Array(await(await fetch('/fixture/'+name)).arrayBuffer()));}", {'name': fixture.name, 'path': path, 'root': save_root})
                # Separate taps capture the SDL music stream and independent SFX.
                child.evaluate('''()=>{const m=Module,c=m.SDL3.audioContext;window.meters={bgm:c.createAnalyser(),sfx:c.createAnalyser(),seen:new WeakSet(),nodes:new WeakSet()};
                 for(const meter of [meters.bgm,meters.sfx]){meter.fftSize=2048;const mute=c.createGain();mute.gain.value=0;meter.connect(mute);mute.connect(c.destination);}
                 window.readAudio=()=>{const node=m.SDL3.audio_playback?.scriptProcessorNode;
                  if(node&&!meters.nodes.has(node)){node.connect(meters.bgm);meters.nodes.add(node);}
                  for(const voice of m.eaglerSfxOutput?.voices.values()??[])if(!meters.seen.has(voice)){voice.merge.connect(meters.sfx);meters.seen.add(voice);}
                  const rms=meter=>{const values=new Float32Array(2048);meter.getFloatTimeDomainData(values);return Math.sqrt(values.reduce((n,v)=>n+v*v,0)/values.length)};
                  return {bgm:rms(meters.bgm),sfx:rms(meters.sfx),context:c.state,sfxPaused:m.eaglerSfxPaused};};}''')
                child.evaluate('''game=>{const c=globalThis['__'+game+'Runtime']?.core??globalThis.core??Module;
                 window.audioCounters=()=>{const fn=c.sdl_audio_stats??c.th11_audio_statistics??c._th11_audio_statistics??c._th15_audio_statistics;
                  if(!fn)throw Error('Missing native audio statistics');const ptr=fn(),values=new Uint32Array((c.memory?.buffer??Module.HEAPU8.buffer),ptr,5);
                  return game==='th10'?[values[0],values[3],values[4]]:Array.from(values.subarray(0,3));};}''', a.game)
                observations = []
                for fixture, path in replays:
                    description = evaluate('rpc("playback",{action:"inspect",path:'+json.dumps(path)+'})')['description']
                    evaluate('window.selection='+json.dumps({'action': 'open', 'path': path, 'stage': description['stages'][0]['id']}))
                    page.click('#play'); evaluate('window.opened')
                    peak = {'bgm': 0, 'sfx': 0}; deadline = time.monotonic()+15
                    while time.monotonic() < deadline:
                        sample = child.evaluate('readAudio()')
                        for key in peak: peak[key] = max(peak[key], sample[key])
                        if peak['sfx'] > 1e-5 and (music == 'none' or peak['bgm'] > 1e-5): break
                        page.wait_for_timeout(50)
                    assert peak['sfx'] > 1e-5, (fixture.name, music, peak)
                    if music == 'ogg': assert peak['bgm'] > 1e-5, peak
                    else: assert peak['bgm'] < 1e-7, peak
                    pumped = child.evaluate('audioCounters()')
                    page.wait_for_timeout(600)
                    sustained = child.evaluate('readAudio()')
                    after = child.evaluate('audioCounters()')
                    assert after[1] > pumped[1] and after[2] > pumped[2], (pumped, after)
                    if music == 'ogg': assert sustained['bgm'] > 1e-5, sustained
                    evaluate('rpc("playback",{action:"pause"})'); page.wait_for_timeout(250)
                    paused = child.evaluate('readAudio()'); assert paused['sfxPaused'] is True
                    assert paused['bgm'] < 1e-7 and paused['sfx'] < 1e-7, paused
                    offset = evaluate("events.filter(e=>e.event==='runtime-info'&&e.playback).at(-1).playback.startFrame")
                    evaluate('rpc("playback",{action:"seek",value:'+str(offset)+'})')
                    page.wait_for_function("events.filter(e=>e.event==='runtime-info'&&e.playback).at(-1)?.playback.seeking===false", timeout=60000)
                    assert child.evaluate('Module.eaglerSfxPaused') is True
                    evaluate('rpc("playback",{action:"play"})'); page.wait_for_timeout(250)
                    assert child.evaluate('Module.eaglerSfxPaused') is False
                    observations.append({'file': fixture.name, 'path': path, 'peak': peak, 'sustained': sustained, 'pumpsBefore': pumped, 'pumpsAfter': after, 'paused': paused})
                assert not errors, errors
                report['cases'].append({'music': music, 'observations': observations, 'errors': errors})
                print(a.game+(' MP' if a.multiplayer else '')+' | '+music+' | switch, output, pause and seek: PASS', flush=True)
            finally: context.close()
        report['passed'] = True
    except BaseException as error:
        report['error'] = str(error)
        raise
    finally:
        browser.close(); server.shutdown(); a.output.parent.mkdir(parents=True, exist_ok=True)
        a.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
