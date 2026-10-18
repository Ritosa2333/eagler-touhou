"""Same-renderer, alternating local Update+Draw control. No network/snapshots.

This measures base gameplay cost, not rAF smoothness or physical phone speed.
Each world advances identical fixed inputs; compare canonical state every block.
"""
import argparse,json
from pathlib import Path
from playwright.sync_api import sync_playwright

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--url',required=True)
p.add_argument('--output',type=Path,required=True)
p.add_argument('--players',type=int,choices=(2,3),default=2)
p.add_argument('--cpu-rate',type=float,default=1)
p.add_argument('--bullets',type=int,default=1000)
p.add_argument('--blocks',type=int,default=40)
p.add_argument('--baseline',choices=('legacy','previous'),default='legacy',help='previous retains target filtering and broadphase')
args=p.parse_args()
report={'passed':False,'scope':__doc__,'settings':{k:str(v) if isinstance(v,Path) else v for k,v in vars(args).items()}}
with sync_playwright() as pw:
    browser=pw.chromium.launch(headless=True,channel='chromium',args=['--disable-background-timer-throttling','--disable-renderer-backgrounding'])
    try:
        page=browser.new_page();page.goto(args.url)
        page.wait_for_function('window.multiplayerSmoke!==undefined',timeout=120000)
        page.evaluate('''async o=>{
          window.worlds=[];
          for(let i=0;i<2;i++){
            const frame=document.createElement('iframe');frame.src='/';document.body.append(frame);
            const deadline=performance.now()+120000;
            while(!frame.contentWindow?.multiplayerSmoke){
              if(performance.now()>deadline)throw Error('World boot timeout');
              await new Promise(resolve=>setTimeout(resolve,50));
            }
            const w=frame.contentWindow.multiplayerSmoke;await w.start(1234,Array.from({length:o.players},(_,j)=>j));
            w.ticks(120);
            for(let n=0;n<3;n++){w.key('KeyZ',true);w.ticks(2);w.key('KeyZ',false);w.ticks(120);}
            if(!w.denseBullets(o.bullets)||!w.optimizationMode(!!i||o.previous,!!i||o.previous,!!i))throw Error('Workload setup failed');
            worlds.push(w);
          }
          if(!worlds[0].collisionProbe())throw Error('Collision oracle failed');
        }''',{'players':args.players,'bullets':args.bullets,'previous':args.baseline=='previous'})
        report['identity']=page.evaluate('worlds[0].identity()')
        page.context.new_cdp_session(page).send('Emulation.setCPUThrottlingRate',{'rate':args.cpu_rate})
        report['blocks']=page.evaluate('''async o=>{
          const rows=[];
          for(let block=0;block<o.blocks+10;block++){
            const row=[];
            for(const policy of block%2?[1,0]:[0,1]){
              const w=worlds[policy];if(!w.commit(Array(o.players).fill(0)))throw Error('Input commit failed');
              const before=w.chainProfile(),bullet=w.bulletUpdateProfile(),start=performance.now();w.ticks(6);const elapsed=performance.now()-start;
              const after=w.chainProfile();row[policy]={elapsed,calc:after.slice(0,32).reduce((s,x,i)=>s+x-before[i],0),
                bullet:w.bulletUpdateProfile().map((x,i)=>x-bullet[i])};
            }
            if(JSON.stringify(worlds[0].canonical())!==JSON.stringify(worlds[1].canonical()))throw Error('Policy divergence block '+block);
            if(block>=10)rows.push(row);
            await new Promise(resolve=>setTimeout(resolve,0));
          }
          return rows;
        }''',{'blocks':args.blocks,'players':args.players})
        report['summary']=[{'policy':name,'ticks':6*args.blocks,
            'sampledBulletMs':[sum(b[i]['bullet'][j] for b in report['blocks']) for j in range(8)],
            'updateMsPerTick':sum(b[i]['calc'] for b in report['blocks'])/(6*args.blocks),
            'updateAndDrawMsPerTick':sum(b[i]['elapsed'] for b in report['blocks'])/(6*args.blocks)}
            for i,name in enumerate((args.baseline,'optimized'))]
        report['passed']=True
    except BaseException as error:
        report['error']=str(error);raise
    finally:
        browser.close();args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,indent=2))
        print(json.dumps({k:report[k] for k in ('passed','summary','error') if k in report}),flush=True)
