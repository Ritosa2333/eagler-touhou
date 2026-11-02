"""Original Last Spell ECL with all seats, both rule sets and death-window overlap."""
import argparse,json
from pathlib import Path
from playwright.sync_api import sync_playwright
parser=argparse.ArgumentParser()
parser.add_argument('--url',required=True)
parser.add_argument('--output',type=Path,required=True)
args=parser.parse_args()
report={'passed':False,'scope':'Actual native ECL, Player and Bomb owners; fixture setup; no network transport claim','cases':[]}
with sync_playwright() as pw:
 browser=pw.chromium.launch(headless=True,args=['--enable-unsafe-swiftshader'])
 try:
  for count in [2,3]:
   page=browser.new_page(viewport={'width':640,'height':480});errors=[]
   page.on('pageerror',lambda error:errors.append(str(error)))
   page.goto(args.url);page.wait_for_function('window.multiplayerSmoke',timeout=90000)
   page.evaluate('n=>multiplayerSmoke.start(1234,Array.from({length:n},(_,s)=>s))',count)
   page.evaluate("multiplayerSmoke.ticks(120)")
   for _ in range(3):page.evaluate("multiplayerSmoke.key('KeyZ',true);multiplayerSmoke.ticks(2);multiplayerSmoke.key('KeyZ',false);multiplayerSmoke.ticks(120)")
   cases=[(spell,challenge,story,victim,0) for spell in [144,172] for challenge in [False,True] for story in [False,True] for victim in range(count)]
   cases += [(172,challenge,True,count-1,host) for challenge in [False,True] for host in [1,2,3]]
   for spell,challenge,story,victim,host in cases:
    row={'count':count,'spell':spell,'challenge':challenge,'story':story,'victim':victim,'hostState':host,'passed':False};report['cases'].append(row)
    assert page.evaluate('v=>multiplayerSmoke.lastSpellSetup(...v)',[spell,host,challenge,story])==1
    result=page.evaluate('''o=>{
      const w=multiplayerSmoke,read=()=>({state:w.lastSpellState(),faults:w.diagnostics()});
      let ready=false;
      for(let n=0;n<600;n++){w.ticks(1);const s=w.lastSpellState();if((s[0]&0x180)&&s[2]===o.spell&&s[5]>120){ready=true;break;}}
      const before=read();if(!ready)return {ready,before};
      const inputs=Array(o.count).fill(0);inputs[o.victim]=2;
      w.commit(inputs);w.ticks(2);const manual=read();w.commit(Array(o.count).fill(0));w.ticks(1);
      const hit=w.fixtureGrazedBulletHit(o.victim),afterHit=read();let started=false,finished=false;
      for(let n=0;n<360;n++){w.ticks(1);const s=w.lastSpellState();if(s[8+o.victim*8+3]&&s[8+o.victim*8+4]===4)started=true;if(started&&!(s[0]&0x180)){finished=true;break;}}
      return {ready,before,manual,hit,afterHit,started,finished,final:read()};
    }''',{'spell':spell,'victim':victim,'count':count})
    row.update(result)
    assert result['ready'],row
    assert not result['manual']['state'][8+victim*8+3],row
    assert result['hit'] and result['started'] and result['finished'],row
    assert not(result['final']['state'][0]&0x580),row
    # The native automatic release is not a life/bomb-consuming player bomb.
    assert result['before']['state'][40+victim*2:42+victim*2]==result['final']['state'][40+victim*2:42+victim*2],row
    assert not errors,errors
    row['passed']=True
    print(json.dumps({key:row[key] for key in ['count','spell','challenge','story','victim','hostState','passed']}),flush=True)
   page.close()
  report['passed']=True
 finally:
  browser.close();args.output.parent.mkdir(parents=True,exist_ok=True);args.output.write_text(json.dumps(report,indent=2),encoding='utf-8')
