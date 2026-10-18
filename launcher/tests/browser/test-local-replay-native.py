"""Real Chromium/WASM local Replay adapter check, with an isolated MEMFS store."""
import argparse, functools, hashlib, json, threading, subprocess
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from playwright.sync_api import sync_playwright

p=argparse.ArgumentParser();p.add_argument('--workspace',type=Path,default=Path('D:/workspace/eagler'))
p.add_argument('--game',default='th11');p.add_argument('--output',type=Path,required=True)
p.add_argument('--data',type=Path)
p.add_argument('--multiplayer',action='store_true');p.add_argument('--repo',type=Path)
p.add_argument('--fixtures',type=Path)
p.add_argument('--runtime',type=Path,help='Explicit packaged Runtime supplying the measured font tables')
p.add_argument('--late-import',action='store_true');p.add_argument('--all-stages',action='store_true')
p.add_argument('--recorded',action='store_true',help='Fixtures are native recordings, rather than software format fixtures')
a=p.parse_args();root=a.workspace;repo=a.repo or root/(a.game+'-eagler');web=repo/'web'
owner=json.loads(subprocess.check_output(['node',str(Path(__file__).resolve().parents[1]/'support/runtime-source-owner.mjs'),'--source='+str(repo),'--game='+a.game,'--variant='+('multiplayer' if a.multiplayer else 'normal')],text=True))
profile=('sdl-multiplayer' if a.game=='th15' else 'multiplayer' if a.game=='th10' else 'multiplayer-diagnostic') if a.multiplayer else 'sdl-release' if a.game=='th15' else 'sdl3'
build=web/'artifacts'/profile;runtime=a.runtime or repo/'build-eagler'
if not (runtime/'fonts').is_dir():runtime=root/(a.game+'-eagler')/'build-eagler'
data=a.data or (root/'games/web-content/th10/th10.data' if a.game=='th10' else next(path for path in (root/(a.game+'-eagler')).glob('*/'+a.game+'.dat') if path.is_file()))
fixtures=sorted((a.fixtures or ((Path(__file__).resolve().parents[2]/'artifacts/replay-workspace/fixtures'/(a.game+('mp' if a.multiplayer else ''))) if a.multiplayer or a.game in ['th08','th10'] else (web/'reference/assets'))).glob('demo*.rpy'))
assert fixtures,'Extract the original built-in Demo fixtures first'
stem=a.game if a.game=='th15' else a.game+'-sdl'
files={'/module.mjs':build/(stem+'.mjs'),'/module.wasm':build/(stem+'.wasm'),'/game.dat':data}
build_identity=json.loads((build/'build.json').read_text(encoding='utf-8'))
if hashlib.sha256(files['/module.wasm'].read_bytes()).hexdigest()!=build_identity.get('sha256'):p.error(f'{build}: WASM does not match its build identity')
declared={**build_identity.get('sourceFiles',{}),**build_identity.get('headers',{}),**(build_identity.get('sources',{}) if isinstance(build_identity.get('sources'),dict) else {})}
native_paths=([f'{a.game}_web/cpp/sdl/GameHost.cpp',f'{a.game}_web/cpp/sdl/LocalReplay.inc'] if a.game in ['th08','th10'] else ['web/cpp/sdl/Application.cpp']+(['web/cpp/sdl/LocalReplay.inc'] if a.multiplayer else []) if a.game=='th11' else ['web/cpp/sdl/BrowserRuntime.cpp','web/cpp/sdl/LocalReplay.inc'])
if a.game=='th08':native_paths+=['web/cpp/game/GameApplication.cpp','web/cpp/game/GameApplication.hpp']
owner['nativeReplaySources']={}
for path in native_paths:
 actual=hashlib.sha256((repo/path).read_bytes()).hexdigest()
 if declared.get(path)!=actual:p.error(f'{build}: rebuild {owner["branch"]}; native Replay source identity differs: {path}')
 owner['nativeReplaySources'][path]=actual
files.update({'/fonts/'+path.name:path for path in (runtime/'fonts').iterdir() if path.is_file()})
files['/fonts/msgothic.ttc']=root/'games/th06/msgothic.ttc'
files.update({'/replay/'+path.name:path for path in fixtures})
common=Path(owner['common']) if owner.get('common') else root/'eagler-common'
files['/local-replay.mjs']=common/'browser/local-replay.mjs'
files['/local-replay-native.mjs']=common/'browser/local-replay-native.mjs'
class Handler(SimpleHTTPRequestHandler):
 def log_message(self,*args):pass
 def do_GET(self):
  if self.path=='/':
   content=b'<!doctype html><canvas id="canvas" width="640" height="480"></canvas>';kind='text/html'
  elif self.path in files:
   content=files[self.path].read_bytes();kind='application/wasm' if self.path.endswith('.wasm') else 'text/javascript' if self.path.endswith('.mjs') else 'application/octet-stream'
  else:self.send_error(404);return
  self.send_response(200);self.send_header('Content-Type',kind);self.send_header('Content-Length',str(len(content)));self.end_headers();self.wfile.write(content)
server=ThreadingHTTPServer(('127.0.0.1',0),Handler);threading.Thread(target=server.serve_forever,daemon=True).start()
report={'game':a.game,'multiplayer':a.multiplayer,'owner':owner,'scope':'Native codec input corpus and actual WASM playback controls; not live-room or retail determinism' if a.multiplayer else 'Original Demo files and actual WASM playback controls; not retail determinism','passed':False,'wasm':hashlib.sha256(files['/module.wasm'].read_bytes()).hexdigest(),'fixtures':[]}
if a.fixtures:report['scope']='Native recorded corpus and actual WASM entry/controls; not a retail determinism proof' if a.recorded else 'Explicit native format corpus and actual WASM entry/controls; not an authentic recorded performance or determinism proof'
def verify_stage(page,fixture,path,metadata,stage):
 page.evaluate('async ({path,stage})=>player.command({action:"open",path,stage})',{'path':path,'stage':stage})
 page.wait_for_function('(!playback.preparing&&!playback.seeking&&playback.frame>=playback.startFrame+60)||playback.ended||window.playbackError',timeout=60000)
 assert page.evaluate('!window.playbackError'),page.evaluate('playbackError')
 assert page.evaluate('playback.stage')==stage,page.evaluate('playback')
 if a.fixtures:page.screenshot(path=str(a.output.with_name(a.output.stem+'-'+fixture.stem+'-'+str(stage)+'.png')))
 page.evaluate('player.command({action:"pause"})');before=page.evaluate('playback.frame');page.wait_for_timeout(200)
 assert page.evaluate('playback.frame')==before,'pause must stop native input consumption'
 offset=page.evaluate('playback.startFrame')
 if offset:
  assert page.evaluate('async value=>{try{await player.command({action:"seek",value});return false;}catch{return true;}}',offset-1),'selected-stage seek must reject positions before its native start'
 target=offset+20 if page.evaluate('playback.total') else offset
 page.evaluate('value=>player.command({action:"seek",value})',target);page.wait_for_function('target=>(!playback.preparing&&!playback.seeking&&playback.frame===target)||window.playbackError',arg=target,timeout=60000)
 assert page.evaluate('!window.playbackError'),page.evaluate('playbackError')
 page.evaluate('player.command({action:"rate",value:4})');page.evaluate('player.command({action:"play"})')
 page.wait_for_function('playback.frame>=playback.startFrame+120||playback.ended||window.playbackError',timeout=60000)
 assert page.evaluate('!window.playbackError'),page.evaluate('playbackError')
 if a.multiplayer:
  # Software-rendered full stages can run near real time. Bound this by native
  # length, instead of giving a 7,520-frame run the short 500-frame deadline.
  deadline=max(60000,page.evaluate('playback.total')*50+15000)
  print(a.game+' '+fixture.name+' stage '+str(stage)+' checking native EOF',flush=True)
  page.evaluate('player.command({action:"rate",value:0})');page.wait_for_function('playback.ended||window.playbackError',timeout=deadline);assert page.evaluate('!window.playbackError'),page.evaluate('playbackError');assert page.evaluate('playback.frame===playback.total&&playback.total>0')
 report['fixtures'].append({'file':fixture.name,'selectedStage':stage,'metadata':metadata,'state':page.evaluate('playback')})
 print(a.game+' '+fixture.name+' stage '+str(stage)+': PASS',flush=True)
with sync_playwright() as pw:
 browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader','--autoplay-policy=no-user-gesture-required']);page=browser.new_page();errors=[]
 page.on('pageerror',lambda error:errors.append(str(error)))
 try:
  page.goto('http://127.0.0.1:'+str(server.server_port))
  page.evaluate('''async ({game,files,multiplayer,buildWords,lateImport})=>{
   const {default:createModule}=await import('/module.mjs');
   window.native=await createModule({canvas:document.querySelector('canvas'),noInitialRun:true,resetBrowserKeyboard(){},
    locateFile:path=>path.endsWith('.wasm')?'/module.wasm':path,
    instantiateWasm(imports,ready){return WebAssembly.instantiateStreaming(fetch('/module.wasm'),imports).then(({instance,module})=>{window.core=instance.exports;ready(instance,module);return instance.exports;});}});
   const fs=native.FS;fs.mkdirTree('/fonts');fs.mkdirTree('/save/replay');fs.mkdirTree('/game');fs.mkdirTree('/savesth10/jp/replay');fs.mkdirTree('/savesth10-multiplayer/jp/replay');fs.mkdirTree('/savesth08/replay');
   const save=game==='th10'?(multiplayer?'/savesth10-multiplayer/jp':'/savesth10/jp'):game==='th08'?'/savesth08':'/save';
   window.saveRoot=save;
   if(multiplayer&&game==='th10')core.sdl_files_variant(1);
   for(const url of files){if(lateImport&&url.startsWith('/replay/'))continue;const target=url==='/game.dat'?(['th08','th10'].includes(game)?'/game/'+game+'.dat':'/'+game+'.dat'):url.startsWith('/replay/')?save+url:url;fs.writeFile(target,new Uint8Array(await(await fetch(url)).arrayBuffer()));}
   if(['th08','th10'].includes(game)){
    const alloc=core.graphics_allocate||core.allocate,free=core.graphics_free||core.deallocate;
    const p=alloc(8);new Uint8Array(core.memory.buffer).set(new Uint8Array([35,99,97,110,118,97,115,0]),p);core.sdl_canvas(p);free(p);core.sdl_music_enabled(0);
    native.runtimePrepare=()=>true;native.runtimeFinish=result=>{if(result)window.nativeLoopResult=result;};
    window.app=game==='th08'?core.sdl_game_open(123):core.sdl_game_open(0,123);if(!app)throw Error(game+' initialization rejected');
    if(game==='th08'){const total=core.sdl_prepare_total();for(let i=0;i<total;i++){if(core.sdl_prepare_next()<0)throw Error('TH08 resource preparation rejected '+i);if(i%12===11)await new Promise(requestAnimationFrame);}if(!core.sdl_game_initialize())throw Error('TH08 game initialization rejected');core.sdl_loop_start();}else core.sdl_loop_start(app);
   }else {core[game+'_music_enabled'](0);if(!core[game+'_initialize']()){const p=core[game+'_error'](),b=native.HEAPU8;throw Error('Initialize rejected: '+new TextDecoder().decode(b.subarray(p,b.indexOf(0,p))));}}
   if(multiplayer&&game==='th11'){const p=core.malloc(16);try{new Uint32Array(core.memory.buffer,p,4).set(buildWords);if(!core.th11_mp_replay_menu(p,4))throw Error('TH11 native build identity rejected');}finally{core.free(p);}}
   const bytesCall=(bytes,call)=>{const alloc=core.malloc||core.eagler_replay_allocate,free=core.free||core.eagler_replay_free,p=alloc(bytes.length);try{new Uint8Array(core.memory.buffer).set(bytes,p);return call(p,bytes.length);}finally{free(p);}};
   const {createNativeLocalReplay}=await import('/local-replay-native.mjs');
   window.player=createNativeLocalReplay({core:()=>core,module:()=>native,physical:path=>save+'/'+path,
    ...(multiplayer&&game==='th10'?{replayState(){const v=new Uint32Array(core.memory.buffer,core.eagler_replay_state(),6);return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4],bootstrapTarget:v[5]};}}:{}),
    ...(multiplayer&&game==='th15'?{openReplay(path,stage){core.th15_mp_close();core.th15_loop_stop();if(!core.th15_initialize())throw Error('TH15 reinitialize rejected');
      if(!bytesCall(native.FS.readFile(save+'/'+path),(p,n)=>bytesCall(new Uint8Array(new Uint32Array(buildWords).buffer),b=>core.th15_mp_replay_load(p,n,b)))||!core.th15_mp_start()||!core.eagler_replay_select_stage(stage))throw Error('TH15 native Replay rejected');},
      replayState(){const v=new Uint32Array(core.memory.buffer,core.eagler_replay_state(),6);return {frame:v[0],total:v[1],ended:!!v[2],stage:v[3],startStage:v[4],bootstrapTarget:v[5]};}}:{}),
    stopLoop:()=>['th08','th10'].includes(game)?core.sdl_loop_stop():core[game+'_loop_stop'](),onState:value=>window.playback=value,onError:error=>{window.playbackError=String(error);}});
  }''', {'game':a.game,'multiplayer':a.multiplayer,'lateImport':a.late_import,'buildWords':[int(report['wasm'][i:i+8],16) for i in range(0,32,8)],'files':['/game.dat']+[path for path in files if path.startswith('/fonts/') or path.startswith('/replay/')]})
  for fixture in fixtures:
   path='replay/late-import.rpy' if a.late_import else 'replay/'+fixture.name
   if a.late_import:page.evaluate('async ({file,path})=>native.FS.writeFile(saveRoot+"/"+path,new Uint8Array(await(await fetch("/replay/"+file)).arrayBuffer()))',{'file':fixture.name,'path':path})
   metadata=page.evaluate('async path=>(await player.command({action:"inspect",path})).description',path)
   assert metadata['stages'] and metadata['nameBytes'] and (a.multiplayer or 'slowdown' in metadata),metadata
   if fixture.name=='demo-spell.rpy':
    assert metadata['score']==12345670 and metadata['spellNameBytes']==[0x82,0xa0] and metadata['stages'][0]['score']==12345670,metadata
    assert metadata['stages'][0]['id']==0 and 'No.' in metadata['spell'],metadata
   for entry in (list(reversed(metadata['stages'])) if a.all_stages else metadata['stages'][:1]):
    verify_stage(page,fixture,path,metadata,entry['id'])
  if a.multiplayer and a.game=='th10':
   first='replay/late-import.rpy' if a.late_import else 'replay/'+fixtures[0].name
   page.evaluate('path=>{native.FS.mkdirTree(saveRoot+"/replay/archive");native.FS.writeFile(saveRoot+"/replay/archive/demo0.rpy",native.FS.readFile(saveRoot+"/"+path));}',first)
   path='replay/archive/demo0.rpy';metadata=page.evaluate('async path=>(await player.command({action:"inspect",path})).description',path);stage=metadata['stages'][0]['id']
   page.evaluate('async ({path,stage})=>{await player.command({action:"open",path,stage});await player.command({action:"pause"});await player.command({action:"seek",value:playback.startFrame});}',{'path':path,'stage':stage})
   page.wait_for_function('!playback.preparing&&!playback.seeking||window.playbackError',timeout=60000);assert page.evaluate('!window.playbackError'),page.evaluate('playbackError')
   assert page.evaluate('playback.paused&&playback.frame===playback.startFrame'),page.evaluate('playback')
   report['folderAndPausedRestart']=page.evaluate('playback')
  assert not errors,errors
  report['passed']=True
 except Exception as error:
  report['failure']=str(error);report['pageErrors']=errors;report['failureState']=page.evaluate('({playback:window.playback,error:window.playbackError})');raise
 finally:
  a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8');browser.close();server.shutdown()
print(a.game+' local Replay native browser: PASS')
