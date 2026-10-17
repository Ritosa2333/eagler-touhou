"""Integration test: real shell + non-fixture MP binary + recorder export.

Packets are delivered by this harness; this is not real-device desync acceptance.
"""
import argparse
import json
import os
from pathlib import Path
import runpy
import subprocess
import time
from urllib.request import urlopen
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[2]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--port', type=int, default=8154)
parser.add_argument('--output', type=Path, required=True)
args = parser.parse_args()
args.output.parent.mkdir(parents=True, exist_ok=True)
report = {'passed': False, 'scope': __doc__}
build = json.loads((ROOT/'th08_web/artifacts/multiplayer/build.json').read_text())
report['wasmSha256'] = build['sha256']
if any(e['name'].startswith('mp_fixture_') for e in build['exports']):
    raise RuntimeError('fixture exports leaked into real MP build')
log = args.output.with_suffix('.server.log').open('w', encoding='utf-8')
env = os.environ | {'PORT': str(args.port), 'TH08_MP_FIXTURES': '0',
                    'TH08_MP_DATA': str(ROOT/'artifacts/multiplayer-tests/runtime-data/games/th08/th08.data')}
server = subprocess.Popen(['node', 'portable/multiplayer/serve.mjs'], cwd=ROOT, env=env, stdout=log, stderr=subprocess.STDOUT)
url = f'http://127.0.0.1:{args.port}/'
try:
    deadline = time.monotonic()+15
    while True:
        if server.poll() is not None:
            raise RuntimeError('server startup failed')
        try:
            with urlopen(url+'build.json', timeout=1) as response:
                assert json.load(response)['wasm'] == build['sha256']
            break
        except OSError:
            if time.monotonic() > deadline:
                raise RuntimeError('server timeout')
            time.sleep(.1)
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True, args=['--enable-unsafe-swiftshader'])
        context = browser.new_context(service_workers='block', viewport={'width': 390, 'height': 844},
                                      is_mobile=True, has_touch=True)
        page = context.new_page()
        errors = []
        page.on('pageerror', lambda e: errors.append(str(e)))
        # This test-only host lacks a frozen Runtime directory; supply its
        # metadata, never intercept executable bytes or replace the actual shell.
        page.route('**/runtime/manifest.json', lambda route: route.fulfill(json={'execution': {'sha256': build['sha256']}}))
        page.goto(url+'host.html')
        result = page.evaluate('''async options=>{
          const wait=ms=>new Promise(r=>setTimeout(r,ms)),hosts=[],worlds=[];
          for(let seat=0;seat<2;seat++){
            const frame=document.createElement('iframe');frame.src='/host.html?th08Trace=1';document.body.append(frame);
            let deadline=performance.now()+90000;while(!frame.contentWindow?.host||!frame.contentDocument?.body){if(performance.now()>deadline)throw Error('host timeout');await wait(10);}
            const host=frame.contentWindow.host;hosts.push(host);host.open();
            while(!host.ready()){if(performance.now()>deadline)throw Error('shell timeout');await wait(10);}
            await host.request('configure',{music:'none',options:{multiplayerPreflight:true,alwaysHitbox:seat===1}});await host.request('launch');
            const child=frame.contentWindow.document.querySelector('iframe').contentWindow,r=child.__th08Runtime,{core,app}=r;
            if(Object.keys(core).some(k=>k.startsWith('mp_fixture_')))throw Error('fixture binary');
            const hash=Array.from({length:4},(_,i)=>parseInt(options.sha.slice(i*8,i*8+8),16)>>>0);
            const values=[3,2,seat,1,1234,0x12345678,0x10203040,...hash,0,0,1,0,0,0],p=core.allocate(68);
            try{new Uint32Array(core.memory.buffer,p,17).set(values);if(!core.multiplayer_configure(app,p,17))throw Error('session configure');}finally{core.deallocate(p);}
            const packet=(name,...values)=>{const p=core.allocate(2048);try{const n=core[name](app,...values,p,2048);if(!n)throw Error(name);return Array.from(new Uint8Array(core.memory.buffer,p,n));}finally{core.deallocate(p);}};
            worlds.push({r,core,app,child,packet,apply(bytes){const p=core.allocate(bytes.length);try{new Uint8Array(core.memory.buffer,p,bytes.length).set(bytes);if(core.multiplayer_wire_apply(app,p,bytes.length)!==0)throw Error('wire');}finally{core.deallocate(p);}}});
          }
          for(const phase of [1,2]){const packets=worlds.map(w=>w.packet('multiplayer_session_build',phase));worlds[0].apply(packets[1]);worlds[1].apply(packets[0]);if(phase===1)for(const w of worlds)if(!w.core.multiplayer_session_ready(w.app))throw Error('ready');}
          for(let f=0;f<300;f++){
            for(let seat=0;seat<2;seat++){const w=worlds[seat];const ok=f===0&&seat===0?w.core.multiplayer_capture_input(w.app,f,0,2,8188,-8192,0):w.core.multiplayer_capture_local(w.app,f,f>180?1:0);if(!ok)throw Error('capture');}
            const packets=worlds.map((w,i)=>w.packet('multiplayer_input_build',1-i,f,f+1));worlds[0].apply(packets[1]);worlds[1].apply(packets[0]);
            for(let seat=0;seat<2;seat++){const w=worlds[seat];if(w.core.sdl_loop_tick(w.app,1/60,16))throw Error('native tick');hosts[seat].finishCallback();}
            if(f%30===29)await wait(0);
          }
          window.__traceWorlds=worlds;
          return {ready:true};
        }''', {'sha': build['sha256']})
        before = page.evaluate('''() => window.__traceWorlds.map(w =>
          Array.from(new Uint32Array(w.core.memory.buffer,w.core.multiplayer_canonical_state(w.app),13)))''')
        if before[0] != before[1]:
            raise RuntimeError('opposite local alwaysHitbox preferences changed canonical multiplayer world')
        host_frames = [frame for frame in page.frames if 'host.html?th08Trace=1' in frame.url]
        if len(host_frames) != 2:
            raise RuntimeError(f'expected two trace host frames, got {len(host_frames)}')
        downloads = []
        for frame in host_frames:
            button = frame.locator('#th08ResourceTraceExport')
            button.wait_for(state='visible', timeout=10000)
            if button.inner_text() != '导出同步记录':
                raise RuntimeError(f'trace button not ready: {button.inner_text()}')
            with page.expect_download(timeout=10000) as pending:
                button.tap(timeout=10000)
            download = pending.value
            downloads.append(download.suggested_filename)
            if button.inner_text() != '同步记录已导出':
                raise RuntimeError(f'trace button did not freeze: {button.inner_text()}')
        result = page.evaluate('''() => {
          const exports=window.__traceWorlds.map(w=>w.r.resourceTrace.export(false));
          const after=window.__traceWorlds.map(w=>Array.from(new Uint32Array(
            w.core.memory.buffer,w.core.multiplayer_canonical_state(w.app),13)));
          return {exports,after,readOnly:true};
        }''')
        if before != result['after']:
            raise RuntimeError('mobile button export changed native world')
        report['mobileTap'] = True
        report['downloads'] = downloads
        compare = runpy.run_path(str(ROOT/'portable/multiplayer/compare-resource-traces.py'))['compare']
        for seat, doc in enumerate(result['exports']):
            path = args.output.with_suffix(f'.p{seat+1}.json')
            path.write_text(json.dumps(doc), encoding='utf-8')
        report['comparison'] = compare(*result['exports'])
        report['readOnly'] = result['readOnly']
        report['errors'] = errors
        report['passed'] = not errors and report['comparison']['status'] == 'equal-in-retained-window'
        context.close()
        browser.close()
except BaseException as e:
    report['error'] = str(e)
finally:
    server.terminate()
    try:
        server.wait(timeout=5)
    except subprocess.TimeoutExpired:
        server.kill(); server.wait()
    log.close()
    args.output.write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps(report), flush=True)
if not report['passed']:
    raise SystemExit(1)
