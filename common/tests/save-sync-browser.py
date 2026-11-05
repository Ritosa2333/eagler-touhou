"""Fault-inject a closed connection in a supplied real Emscripten runtime.

Uses an isolated browser context; never opens the user's site or saves.
Usage: python save-sync-browser.py PATH_TO_RUNTIME_DIRECTORY
"""
import json
import sys
from pathlib import Path
from playwright.sync_api import sync_playwright

runtime = Path(sys.argv[1]).resolve()
loaders = list(runtime.glob('th*-sdl.mjs')) or list(runtime.glob('th*.mjs')) or list(runtime.glob('th*.js'))
if len(loaders) != 1:
    raise ValueError('Supply a runtime directory containing one game loader')
loader = loaders[0].name
wasm = next(runtime.glob('th*.wasm')).name
helper = Path(__file__).resolve().parents[1] / 'browser/save-sync.mjs'
with sync_playwright() as playwright:
    browser = playwright.chromium.launch()
    context = browser.new_context()
    page = context.new_page()
    def route(request):
        name = request.request.url.split('/')[-1]
        if name == 'save-sync.mjs':
            request.fulfill(path=str(helper), content_type='text/javascript')
        elif name in (loader, wasm):
            request.fulfill(path=str(runtime / name), content_type='application/wasm' if name.endswith('.wasm') else 'text/javascript')
        else:
            request.fulfill(body='<!doctype html><canvas></canvas>', content_type='text/html')
    page.route('http://localhost:18763/**', route)
    page.goto('http://localhost:18763/')
    result = page.evaluate('''async({loader})=>{
      const {createSaveSync}=await import('/save-sync.mjs');
      let module;
      if(loader.endsWith('.mjs')){
        const {default:createModule}=await import('/'+loader);
        module=await createModule({canvas:document.querySelector('canvas'),print(){},printErr(){}});
      }else{
        module=await new Promise((resolve,reject)=>{
          window.Module={canvas:document.querySelector('canvas'),noInitialRun:true,
            print(){},printErr(){},getPreloadedPackage:(_,size)=>new ArrayBuffer(size),
            onRuntimeInitialized(){resolve({FS:window.FS,IDBFS:window.IDBFS});},onAbort:reject};
          const script=document.createElement('script');script.src='/'+loader;
          script.onerror=reject;document.head.append(script);
        });
      }
      const {FS,IDBFS}=module;
      const sync=createSaveSync(()=>module);
      const mp='/saves-th10-mp-probe',normal='/saves-th10-normal-probe';
      for(const path of [mp,normal]){FS.mkdirTree(path);FS.mount(IDBFS,{},path);}
      await sync(true);
      FS.writeFile(normal+'/score.dat',new Uint8Array([9,8,7]));
      FS.writeFile(mp+'/score.dat',new Uint8Array([1,2,3]));
      await sync(false);
      const healthy=IDBFS.dbs[normal],stale=IDBFS.dbs[mp];
      stale.close();
      let reproduced=false;
      await new Promise(resolve=>FS.syncfs(false,error=>{reproduced=error?.name==='InvalidStateError';resolve();}));
      if(!reproduced)throw Error('Original closed-handle fault did not reproduce');
      FS.writeFile(mp+'/score.dat',new Uint8Array([4,5,6,7]));
      await Promise.all([sync(false),sync(false)]);
      if(IDBFS.dbs[mp]===stale)throw Error('Stale connection retained');
      if(IDBFS.dbs[normal]!==healthy)throw Error('Healthy connection replaced');
      FS.writeFile(mp+'/score.dat',new Uint8Array([0]));
      await sync(true);
      const mpBytes=Array.from(FS.readFile(mp+'/score.dat'));
      const normalBytes=Array.from(FS.readFile(normal+'/score.dat'));
      if(JSON.stringify(mpBytes)!=='[4,5,6,7]'||JSON.stringify(normalBytes)!=='[9,8,7]')throw Error('Save bytes changed');
      // A closed cached connection during initial restore must recover too.
      IDBFS.dbs[mp].close();
      await sync(true);
      const restored=Array.from(FS.readFile(mp+'/score.dat'));
      if(JSON.stringify(restored)!=='[4,5,6,7]')throw Error('Restore bytes changed');
      return {originalFaultReproduced:reproduced,flushRecovered:true,restoreRecovered:true,
        queuedSyncsCompleted:true,healthyNamespacePreserved:true,mpBytes,normalBytes};
    }''', {'loader': loader})
    print(json.dumps(result, indent=2))
    context.close()
    browser.close()
