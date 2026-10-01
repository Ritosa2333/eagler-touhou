"""Delayed real RTC input changes native Stage Clear timing; verify capture reuse."""
import argparse,json,uuid
from pathlib import Path
from playwright.sync_api import sync_playwright
p=argparse.ArgumentParser();p.add_argument('--url',required=True);p.add_argument('--relay',required=True)
p.add_argument('--players',type=int,choices=(2,3),default=2);p.add_argument('--output',type=Path,required=True)
p.add_argument('--skip-resim-visual',action='store_true')
p.add_argument('--skip-resim-geometry',action='store_true')
p.add_argument('--back-metadata-only',action='store_true')
args=p.parse_args();report={'passed':False,'players':args.players,'skipResimVisual':args.skip_resim_visual,'skipResimGeometry':args.skip_resim_geometry,'backMetadataOnly':args.back_metadata_only}
with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
    try:
        context=browser.new_context(service_workers='block');page=context.new_page()
        page.goto(args.url);page.wait_for_function('window.multiplayerSmoke!==undefined',timeout=90000)
        page.on('console',lambda m:print(m.text,flush=True) if m.text.startswith('BOUNDARY ') else None)
        result=page.evaluate("""async o=>{
          const wait=ms=>new Promise(r=>setTimeout(r,ms)),worlds=[],realms=[];
          const observe=()=>worlds.map(w=>({net:w.netStatus(),driver:w.driverStatus(),state:w.status(),native:w.nativeStatus(),canonical:w.canonical(),rng:w.rngState(),boundaryVisualRedraws:w.boundaryVisualRedraws()}));
          for(let seat=0;seat<o.players;++seat){
            const frame=document.createElement('iframe');frame.src=o.url;document.body.append(frame);
            const end=performance.now()+90000;
            while(!frame.contentWindow?.multiplayerSmoke){if(performance.now()>end)throw Error('boot');await wait(10);}
            const w=frame.contentWindow.multiplayerSmoke;
            if(o.skipResimVisual&&w.skipResimVisual(true)!==1)throw Error('skip resim visual setup');
            if(o.skipResimGeometry&&w.skipResimGeometry(true)!==1)throw Error('skip resim geometry setup');
            if(o.backMetadataOnly&&w.backMetadataOnly(true)!==1)throw Error('back metadata setup');
            await w.start(1234,Array.from({length:o.players},(_,i)=>i),seat,[12345,67890]);
            worlds.push(w);realms.push(frame.contentWindow);
          }
          for(const w of worlds)if(!w.connect(o.relay+'/?room='+o.room))throw Error('connect');
          const end=performance.now()+60000;
          while(!worlds.every(w=>w.netStatus()[2])){
            for(const w of worlds)w.pollNetwork();if(performance.now()>end)throw Error('barrier');await wait(2);
          }
          if(!worlds.every(w=>w.driverStatus()[12]===1))throw Error('Expected real RTC');
          async function advance(target,shoot){
            const end=performance.now()+20000;
            while(!worlds.every(w=>w.netStatus()[3]>target)){
              for(let seat=0;seat<worlds.length;++seat){
                const w=worlds[seat];if(!w.pollNetwork())throw Error(w.networkError());
                const n=w.netStatus();if(n[3]<=target){
                  // Existing captures are immutable. The exporter rejects an
                  // accidental second sample, while the driver reuses it.
                  if(n[3]===0&&seat===0)w.captureInput(0,0,2,8188,-8192,0);
                  else w.capture(n[3],seat===0&&shoot?1:0);
                  w.ticks(1);
                }else if(!w.reconcile())throw Error(w.networkError());
              }
              if(performance.now()>end)throw Error('advance '+target+' '+JSON.stringify(observe()));await wait(1);
            }
          }
          await advance(179,false);
          for(let count=0;;++count){
            for(const w of worlds){w.pollNetwork();w.reconcile();}
            if(worlds.every(w=>w.netStatus()[5]>=179))break;
            if(count>10000)throw Error('confirm startup');await wait(1);
          }
          for(const w of worlds)if(!w.stageClearSetup())throw Error('native clear setup');
          const held=[],state=realms[1].__th08PeerTransport,receive=state.receiveBinary;
          state.receiveBinary=function(data){
            const bytes=data instanceof realms[1].ArrayBuffer?new realms[1].Uint8Array(data):data;
            if(bytes[5]===1){held.push(new realms[1].Uint8Array(bytes));return;}
            return receive.call(this,data);
          };
          await advance(185,true);
          const before=observe();console.log('BOUNDARY delayed '+JSON.stringify(before.map(x=>x.net)));
          state.receiveBinary=receive;for(const bytes of held)receive.call(state,bytes);
          if(!held.length)throw Error('No real packets impaired');
          worlds[1].pollNetwork();
          if(!worlds[1].reconcile())throw Error(worlds[1].networkError());
          const shortened=observe();
          if(!(shortened[1].net[3]<before[1].net[3]))throw Error('Fixture did not shorten predicted timeline '+JSON.stringify(shortened));
          await advance(300,false);
          const deadline=performance.now()+20000;
          while(!worlds.every(w=>w.netStatus()[5]>=300)){
            for(const w of worlds){w.pollNetwork();w.reconcile();}
            if(performance.now()>deadline)throw Error('confirmation');await wait(1);
          }
          const after=observe();
          if(after.some(x=>x.driver[2]||x.driver[15]))throw Error('driver/channel failure');
          if(after.some(x=>JSON.stringify(x.canonical)!==JSON.stringify(after[0].canonical)))throw Error('canonical mismatch '+JSON.stringify(after));
          if(after.some(x=>JSON.stringify(x.rng)!==JSON.stringify(after[0].rng)))throw Error('RNG mismatch '+JSON.stringify(after));
          if(o.skipResimVisual&&after.every(x=>x.boundaryVisualRedraws===0))throw Error('Visual boundary redraw branch not exercised '+JSON.stringify(after));
          if(after.some(x=>x.state[6]!==1))throw Error('Did not reach stage two '+JSON.stringify(after));
          return {passed:true,heldPackets:held.length,before,shortened,after};
        }""",{'url':args.url,'relay':args.relay,'players':args.players,'skipResimVisual':args.skip_resim_visual,'skipResimGeometry':args.skip_resim_geometry,'backMetadataOnly':args.back_metadata_only,'room':'stage-'+uuid.uuid4().hex[:12]})
        report.update(result)
    except BaseException as error:report['error']=str(error);raise
    finally:
        args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2))
        browser.close();print(json.dumps({'passed':report['passed'],'output':str(args.output)}),flush=True)
