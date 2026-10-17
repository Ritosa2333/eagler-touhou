"""Exercise selected-file Replay through the real TH06/TH07 shell protocol."""
import argparse, hashlib, json, threading, time, subprocess, re
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from playwright.sync_api import sync_playwright
p=argparse.ArgumentParser();p.add_argument('--game',choices=['th06','th07'],required=True);p.add_argument('--variant',default='normal');p.add_argument('--fixture');p.add_argument('--fixtures',type=Path);p.add_argument('--eof',action='store_true');p.add_argument('--seek-benchmark',type=int,default=0);p.add_argument('--runtime-dir',type=Path);p.add_argument('--workspace',type=Path,default=Path('D:/workspace/eagler'));p.add_argument('--output',type=Path,required=True)
a=p.parse_args();root=a.workspace;build=a.runtime_dir or root/(a.game+'-eagler')/('build-web-release-'+a.variant)
repo=root/(a.game+'-eagler')
def owner_git(*args):return subprocess.check_output(['git','-c','safe.directory='+repo.as_posix(),'-C',str(repo),*args],text=True).strip()
owner={'repository':str(repo),'branch':owner_git('branch','--show-current'),'commit':owner_git('rev-parse','HEAD'),'dirty':bool(owner_git('status','--porcelain'))}
if not a.runtime_dir:
 cache=(build/'CMakeCache.txt').read_text(encoding='utf-8');flag=re.search(r'^TH_ENABLE_MULTIPLAYER_GAMEPLAY:BOOL=(ON|OFF)$',cache,re.M)
 if not flag or (flag[1]=='ON')!=(a.variant=='multiplayer'):p.error(f'{build}: native multiplayer compile flag does not match --variant {a.variant}')
 owner['multiplayerCompileFlag']=flag[1]
host='''<!doctype html><iframe id="runtime" width="640" height="480"></iframe><script>
const game=GAME,frame=document.querySelector('iframe'),pending=new Map();let serial=0;window.errors=[];
window.send=(command,payload={})=>new Promise((resolve,reject)=>{const request=String(++serial);pending.set(request,{resolve,reject});setTimeout(()=>{if(pending.delete(request))reject(Error(command+' timeout'));},30000);frame.contentWindow.postMessage({protocol:'eagler-touhou/1',game,epoch:1,command,request,...payload},location.origin);});
addEventListener('message',async e=>{if(e.source!==frame.contentWindow||e.origin!==location.origin)return;const m=e.data;if(m.request&&pending.has(m.request)){const p=pending.get(m.request);pending.delete(m.request);m.ok?p.resolve(m):p.reject(Error(m.error));return;}
 if(m.event==='first-frame')window.firstFrame=true;
 if(m.event==='runtime-info'&&m.playback)window.playback=m.playback;
 if(m.event==='error'||m.event==='notice'&&String(m.message).startsWith('Error:'))errors.push(m.error||m.message);
 if(m.event==='ready'){try{await send('configure',{music:'none',resources:[],thpracCatalog:[],sharedResources:[{path:'/msgothic.ttc',url:'/font'},{path:'/unifont.otf',url:'/unifont'}],options:{replayViewer:true,touchEnabled:false,thpracEnabled:false}});await send('launch');window.launched=true;}catch(e){errors.push(String(e));}}
});frame.src='/runtime/'+game+'.html?hosted=1&runtimeEpoch=1&runtimeVariant=VARIANT';</script>'''.replace('GAME',json.dumps(a.game)).replace('VARIANT',a.variant)
files={'/font':root/'games'/a.game/'msgothic.ttc','/unifont':root/'dependencies/unifont-15.1.05/unifont-15.1.05.otf'}
fixtures=sorted((a.fixtures or Path(__file__).resolve().parents[2]/'artifacts/replay-workspace/fixtures'/a.game).glob('*.rpy*'))
if a.fixture:fixtures=[path for path in fixtures if path.name==a.fixture]
assert fixtures,'Extract the original built-in Demo fixtures first'
files.update({'/fixtures/'+path.name:path for path in fixtures})
for suffix in ['html','js','wasm','data']:files['/runtime/'+a.game+'.'+suffix]=build/(a.game+'.'+suffix)
class Handler(SimpleHTTPRequestHandler):
 def log_message(self,*args):pass
 def do_GET(self):
  path=self.path.split('?')[0]
  if path=='/':content=host.encode();kind='text/html'
  elif path in files:
   content=files[path].read_bytes();kind='text/html' if path.endswith('.html') else 'text/javascript' if path.endswith('.js') else 'application/wasm' if path.endswith('.wasm') else 'application/octet-stream'
  else:self.send_error(404);return
  self.send_response(200);self.send_header('Content-Type',kind);self.send_header('Content-Length',str(len(content)));self.end_headers();self.wfile.write(content)
server=ThreadingHTTPServer(('127.0.0.1',0),Handler);threading.Thread(target=server.serve_forever,daemon=True).start()
report={'game':a.game,'variant':a.variant,'owner':owner,'wasm':hashlib.sha256((build/(a.game+'.wasm')).read_bytes()).hexdigest(),'passed':False,'files':[]}
with sync_playwright() as pw:
 browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']);page=browser.new_page();page_errors=[];page.on('pageerror',lambda e:page_errors.append(str(e)))
 page.on('console',lambda m:print(m.text,flush=True) if 'Replay workspace:' in m.text else None)
 try:
  page.goto('http://127.0.0.1:'+str(server.server_port));page.wait_for_function('window.launched&&window.firstFrame||errors.length',timeout=120000);assert page.evaluate('!errors.length'),page.evaluate('errors')
  native=page.frames[1]
  for i,fixture_path in enumerate(fixtures):
   fixture=fixture_path.name
   data=list(fixture_path.read_bytes())
   path='replay/th'+str(int(a.game[2:]))+'_ud%04d'%i+fixture_path.suffix
   page.evaluate('({path,bytes})=>send("write",{path,bytes})',{'path':path,'bytes':data})
   print(a.game+' inspect '+fixture,flush=True)
   description=page.evaluate('async path=>(await send("playback",{action:"inspect",path})).description',path)
   expected_players=re.fullmatch(r'recording-([23])p\.rpyx',fixture) if a.variant=='multiplayer' else None
   if expected_players:native.evaluate('() => {globalThis.__eaglerNetplayReplayPlaybackIndependentInputs=false;}')
   stage=description['stages'][0]['id'];print(a.game+' open '+str(stage),flush=True);page.evaluate('({path,stage})=>send("playback",{action:"open",path,stage})',{'path':path,'stage':stage})
   page.wait_for_function('window.playback?.frame>=60||errors.length',timeout=60000);assert page.evaluate('!errors.length'),page.evaluate('errors')
   page.evaluate('send("playback",{action:"pause"})');before=page.evaluate('playback.frame');page.wait_for_timeout(200);assert page.evaluate('playback.frame')==before
   print(a.game+' rewind',flush=True)
   page.evaluate('send("playback",{action:"seek",value:20})');page.wait_for_function('!playback.seeking&&playback.frame===20||errors.length',timeout=60000);assert page.evaluate('!errors.length'),page.evaluate('errors')
   page.evaluate('send("playback",{action:"rate",value:4})');page.evaluate('send("playback",{action:"play"})');page.wait_for_function('playback.frame>=120',timeout=60000)
   native_inputs=None
   if expected_players:
    page.wait_for_function('playback.frame>=150||playback.ended',timeout=60000)
    native_inputs=native.evaluate('({players:globalThis.__eaglerNetplayReplayPlaybackPlayerCount??0,independent:!!globalThis.__eaglerNetplayReplayPlaybackIndependentInputs,frame:globalThis.__eaglerNetplayReplayPlaybackFrame??0})')
    assert native_inputs['players']==int(expected_players[1]) and native_inputs['independent'] and native_inputs['frame']>=150,native_inputs
   benchmark=None
   if a.seek_benchmark:
    page.evaluate('send("playback",{action:"pause"})');page.evaluate('send("playback",{action:"seek",value:20})');page.wait_for_function('!playback.preparing&&!playback.seeking&&playback.frame===20',timeout=60000)
    native.evaluate('''() => {
     window.seekProfile={steps:0,renderedSteps:0,stepMs:0,presents:0,presentMs:0};
     const originals=[];window.restoreSeekProfile=()=>{for(const [key,fn] of originals)Module[key]=fn;};
     for(const [name,count,duration] of [['eagler_replay_step','steps','stepMs'],['eagler_replay_present','presents','presentMs']]) {
      const key='_'+name,original=Module[key];if(!original)continue;originals.push([key,original]);
      Module[key]=(...args)=>{const start=performance.now();try{return original(...args);}finally{seekProfile[count]++;seekProfile[duration]+=performance.now()-start;if(name==='eagler_replay_step'&&args[0])seekProfile.renderedSteps++;}};
     }
    }''')
    start=time.perf_counter();page.evaluate('value=>send("playback",{action:"seek",value})',a.seek_benchmark);page.wait_for_function('value=>!playback.preparing&&!playback.seeking&&playback.frame===value||errors.length',arg=a.seek_benchmark,timeout=180000)
    assert page.evaluate('!errors.length'),page.evaluate('errors');elapsed=(time.perf_counter()-start)*1000
    # Compare the actual paused playfield, excluding wall-clock FPS/HUD text.
    screenshot=native.locator('#canvas').screenshot();image_path=a.output.with_name(a.output.stem+'-'+fixture_path.stem+'.png');image_path.parent.mkdir(parents=True,exist_ok=True);image_path.write_bytes(screenshot)
    from PIL import Image
    import io
    image=Image.open(io.BytesIO(screenshot)).convert('RGBA');pixels=image.crop((32,16,416,464)).tobytes()
    benchmark={'start':20,'target':a.seek_benchmark,'elapsedMs':round(elapsed,2),'playfieldSha256':hashlib.sha256(pixels).hexdigest(),'paused':page.evaluate('playback.paused'),'native':native.evaluate('() => {const result=seekProfile;restoreSeekProfile();return result;}')}
    print(a.game+' seek benchmark '+json.dumps(benchmark),flush=True)
   if a.eof:
    deadline=max(60000,page.evaluate('playback.total')*50+15000)
    page.evaluate('send("playback",{action:"rate",value:0})');page.evaluate('send("playback",{action:"play"})');page.wait_for_function('playback.ended||errors.length',timeout=deadline);assert page.evaluate('!errors.length'),page.evaluate('errors');assert page.evaluate('playback.frame===playback.total&&playback.total>0'),page.evaluate('playback')
   report['files'].append({'fixture':fixture,'description':description,'playback':page.evaluate('playback'),**({'nativeInputs':native_inputs} if native_inputs else {}),**({'seekBenchmark':benchmark} if benchmark else {})})
  assert not page_errors,page_errors;report['passed']=True
 except Exception as e:
  report['failure']=str(e);report['nativeError']=native.evaluate('typeof Module._eagler_replay_error==="function"?UTF8ToString(Module._eagler_replay_error()):"no native error export"');report['state']=page.evaluate('({playback:window.playback,errors})');report['pageErrors']=page_errors;raise
 finally:
  a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8');browser.close();server.shutdown()
print(a.game+' native shell Replay: PASS')
