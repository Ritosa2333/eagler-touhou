"""Native Last Spell stage scripts with a dead host and challenge cancellation."""
import argparse,json
from pathlib import Path
from playwright.sync_api import sync_playwright
parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
parser.add_argument('--case',choices=['all','kaguya','elixir'],default='all')
parser.add_argument('--observe',action='store_true')
args=parser.parse_args();report={'passed':False,'cases':[],'scope':'Original ECL and SDL game owners; diagnostic initial conditions; no transport acceptance'}
with sync_playwright() as pw:
 browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
 try:
  cases=[]
  if args.case in ['all','kaguya']:
   cases += [(count,172,host,False,0) for count in [2,3] for host in [1,2]]
  if args.case in ['all','elixir']:
   cases += [(count,144,0,challenge,victim) for count in [2,3] for challenge in [False,True] for victim in [0,count-1]]
  for count,spell,host,challenge,victim in cases:
   page=browser.new_page(viewport={'width':640,'height':480});errors=[];page.on('pageerror',lambda e:errors.append(str(e)))
   row={'count':count,'spell':spell,'deadHost':host,'challenge':challenge,'victim':victim,'passed':False};report['cases'].append(row)
   try:
    page.goto(args.url);page.wait_for_function('window.multiplayerSmoke',timeout=90000)
    page.evaluate('seats=>multiplayerSmoke.start(1234,seats)',list(range(count)))
    page.evaluate("multiplayerSmoke.ticks(120)")
    for _ in range(3):page.evaluate("multiplayerSmoke.key('KeyZ',true);multiplayerSmoke.ticks(2);multiplayerSmoke.key('KeyZ',false);multiplayerSmoke.ticks(120)")
    row['setup']=page.evaluate('v=>multiplayerSmoke.lastSpellSetup(...v)',[spell,host,challenge]);assert row['setup']==1,row
    result=page.evaluate("""o=>{
      const w=multiplayerSmoke,read=()=>({state:w.lastSpellState(),native:w.nativeStatus(),faults:w.diagnostics()});
      const result={samples:[],ready:false};
      for(let frame=0;frame<360;++frame){
        w.ticks(1);const value=read();
        if(frame%30===0)result.samples.push({frame,...value});
        if(value.state[0]&0x180&&value.state[2]===o.spell){result.ready=true;result.entry=value;break;}
      }
      if(o.spell===172){
        for(let frame=0;frame<240;++frame){w.ticks(1);if(frame%30===0)result.samples.push({frame:360+frame,...read()});}
        result.final=read();return result;
      }
      if(!result.ready){result.final=read();return result;}
      result.hit=w.fixtureGrazedBulletHit(o.victim);result.afterHit=read();
      result.cancelStarted=false;result.cancelFinished=false;
      for(let frame=0;frame<240;++frame){
        w.ticks(1);const value=read(),offset=8+o.victim*8;
        if(value.state[offset+3]&&value.state[offset+4]===4)result.cancelStarted=true;
        if(frame%10===0)result.samples.push({frame:360+frame,...value});
        if(result.cancelStarted&&!(value.state[0]&0x180)){result.cancelFinished=true;break;}
      }
      result.final=read();return result;
    }""",{'spell':spell,'victim':victim})
    row.update(result);row['errors']=errors
    if spell==172:
     row['passed']=result['ready'] and result['final']['state'][5]>120 and not errors
    else:row['passed']=result['ready'] and result.get('hit',False) and result.get('cancelStarted',False) and result.get('cancelFinished',False) and not errors
    print(json.dumps({k:row[k] for k in ['count','spell','deadHost','challenge','victim','passed']}),flush=True)
    if not args.observe:assert row['passed'],row
   finally:page.close()
  report['passed']=all(c['passed'] for c in report['cases'])
 finally:
  browser.close();args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2),encoding='utf8')
