"""Synthetic Gamepad API layouts through production input ports in three engines.

Runs the eight real JS Runtime keyboard/gamepad boundaries, without a game WASM.
No USB/Bluetooth driver, device mapping database or physical latency is covered.
"""
import argparse
import importlib.util
import json
from pathlib import Path
import subprocess
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('keyboard_boundary', ROOT / 'tests/test-keyboard-mapping-browser.py')
boundary = importlib.util.module_from_spec(spec)
spec.loader.exec_module(boundary)


def bundle_pads():
    script = """import {build} from 'esbuild';
const result=await build({entryPoints:['tests/fixtures/virtual-gamepads.mjs'],bundle:true,write:false,format:'iife',globalName:'VirtualPads',platform:'browser'});process.stdout.write(result.outputFiles[0].text);"""
    return subprocess.run(['node', '--input-type=module', '-e', script], cwd=ROOT, capture_output=True, text=True, encoding='utf-8', check=True).stdout


SCENARIOS = r"""()=>{
  const {virtualGamepadLayouts:layouts,virtualGamepad:make,virtualButton:button,virtualNeutral:neutral}=VirtualPads;
  const target=document.querySelector('iframe').contentWindow;
  let pads=[],service,model,storage,context,notify=[];
  Object.defineProperty(navigator,'getGamepads',{configurable:true,value:()=>pads});
  const assert=(condition,message)=>{if(!condition)throw Error(message);};
  const sample=seat=>target.__eaglerGamepadInputBridge.sample(seat);
  function setup(next,saved){
    service?.dispose();pads=next;const values=new Map();storage=saved??{getItem:k=>values.get(k)??null,setItem:(k,v)=>values.set(k,v)};
    model=ProductionKeyboard.createGamepadMappingModel({storage,getGamepads:()=>navigator.getGamepads()});model.refresh();notify=[];
    context={target,protocol:'eagler-touhou/1',game:window.testGame,epoch:1,targetOrigin:location.origin,launched:true,spectator:false};
    service=ProductionKeyboard.createPlayerGamepad({runtime:{getInputContext:()=>context,subscribe:fn=>{notify.push(fn);return()=>{};}},model,window,document,playerOpen:()=>true});
  }
  const results=[];
  for(const [name,type] of Object.entries(layouts)){
    const pad=make(name,3);setup([null,null,null,pad]);const key=model.getSnapshot().devices[0].key;model.select(0,key);model.setEnabled(true);sample(0);
    assert(target.__eaglerGamepadInputBridge.owned(),name+' did not take ownership');
    const shoot=type.mapping==='standard'?0:Math.min(3,pad.buttons.length-1);
    if(!type.mapping){button(pad,shoot,1);assert(sample(0)===0,name+' raw layout must start unbound');neutral(pad,name);
      model.learn(key,'shoot');model.refresh();button(pad,shoot,1);model.refresh();assert(model.getSnapshot().profiles[key].bindings.shoot[0].index===shoot,name+' button learning');
      assert(sample(0)===0,name+' learning leaked to gameplay');neutral(pad,name);sample(0);
    }
    button(pad,shoot,1);assert((sample(0)&1)!==0,name+' fire');assert(sample(1)===0,name+' unassigned seat');neutral(pad,name);assert(sample(0)===0,name+' release');
    if(pad.axes.length){
      if(!type.mapping){model.learn(key,'right');model.refresh();pad.axes[0]=1;model.refresh();neutral(pad,name);sample(0);}
      pad.axes[0]=.8;assert((sample(0)&128)!==0,name+' axis');neutral(pad,name);sample(0);
    }
    model.setThreshold(key,.8);sample(0);button(pad,shoot,.6);assert((sample(0)&1)===0,name+' analog threshold');button(pad,shoot,.9);assert((sample(0)&1)!==0,name+' analog trigger');
    for(const event of ['blur','pagehide']){
      window.dispatchEvent(new Event(event));assert(sample(0)===0,name+' '+event);assert(sample(0)===0,name+' held rearm after '+event);neutral(pad,name);sample(0);button(pad,shoot,1);assert((sample(0)&1)!==0,name+' fresh press after '+event);
    }
    context.spectator=true;assert(sample(0)===0,name+' spectator');context.spectator=false;assert(sample(0)===0,name+' held spectator exit');neutral(pad,name);sample(0);
    button(pad,shoot,1);assert((sample(0)&1)!==0,name+' reconnect setup');pad.connected=false;model.refresh();window.dispatchEvent(new Event('gamepaddisconnected'));assert(sample(0)===0,name+' disconnect');
    pad.connected=true;model.refresh();window.dispatchEvent(new Event('gamepadconnected'));assert(sample(0)===0,name+' reconnect held');neutral(pad,name);sample(0);button(pad,shoot,1);assert((sample(0)&1)!==0,name+' reconnected fresh press');
    const saved=storage;setup([null,null,null,pad],saved);assert(sample(0)===0,name+' reload must start suspended');neutral(pad,name);sample(0);button(pad,shoot,1);assert((sample(0)&1)!==0,name+' saved binding reload');neutral(pad,name);sample(0);
    results.push({layout:name,result:'PASS'});
  }
  // Identical IDs and sparse browser indices must keep seat/profile identity.
  const a=make('xbox',0),b=make('xbox',2);setup([a,null,b]);let [left,right]=model.getSnapshot().devices.map(d=>d.key);
  model.select(0,left);model.select(1,right);model.setEnabled(true);model.bind(right,'shoot',[{kind:'button',index:3}]);sample(0);sample(1);
  a.connected=false;pads=[null,null,b];model.refresh();window.dispatchEvent(new Event('gamepaddisconnected'));sample(1);
  assert(model.getSnapshot().devices[0].key===right,'identical peer was renamed');button(b,3,1);assert(sample(0)===0,'identical peer took disconnected seat');assert(sample(1)===1,'identical peer lost own custom profile');
  neutral(b,'xbox');a.connected=true;pads=[a,null,b];model.refresh();window.dispatchEvent(new Event('gamepadconnected'));sample(0);sample(1);
  button(a,0,1);button(b,3,1);assert(sample(0)===1&&sample(1)===1,'identical pair reattachment');
  // Four connected devices, two assigned, and a changed layout with the same ID.
  const four=['xbox','dualsense','arcade','pro'].map((name,n)=>make(name,n+1));setup([null,...four]);const choices=model.getSnapshot().devices;
  model.select(0,choices[1].key);model.select(1,choices[3].key);model.setEnabled(true);sample(0);sample(1);
  button(four[0],0,1);assert(sample(0)===0&&sample(1)===0,'unassigned controller leaked');button(four[1],0,1);button(four[3],1,1);assert(sample(0)===1&&sample(1)===2,'four controller isolation');
  const old=target.__eaglerGamepadMapping;context={...context,epoch:2};notify.forEach(fn=>fn());assert(old.sample(0)===0,'retired epoch retained input');
  service.dispose();
  return {layouts:results,identicalHotplug:'PASS',fourDevices:'PASS',lifecycle:'PASS',reload:'PASS'};
}"""


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--workspace',type=Path,default=ROOT.parent)
    parser.add_argument('--output',type=Path)
    args=parser.parse_args()
    launcher,pads=boundary.bundle_launcher(),bundle_pads()
    report={'scope':__doc__.strip(),'results':[]}
    with sync_playwright() as p:
        for engine in ('chromium','firefox','webkit'):
            browser=getattr(p,engine).launch(headless=True)
            try:
                for game in ('th06','th07','th08','th09','th10','th11','th15','th20'):
                    page=browser.new_page()
                    errors=[]
                    page.on('pageerror',lambda e:errors.append(str(e)))
                    page.set_content('<iframe srcdoc="<canvas tabindex=0 width=200 height=100></canvas>"></iframe>')
                    frame=page.frames[1]
                    frame.locator('canvas').wait_for()
                    frame.add_script_tag(content=boundary.runtime_source(args.workspace,game))
                    page.add_script_tag(content=launcher)
                    page.add_script_tag(content=pads)
                    page.evaluate('game=>{window.testGame=game}',game)
                    frame.locator('canvas').click()
                    result=page.evaluate(SCENARIOS)
                    assert not errors,errors
                    report['results'].append({'engine':engine,'game':game,**result})
                    print(json.dumps({'engine':engine,'game':game,'layouts':7,'result':'PASS'}),flush=True)
                    page.close()
            finally:
                browser.close()
    report['combinations']=sum(len(r['layouts']) for r in report['results'])
    if args.output:
        args.output.parent.mkdir(parents=True,exist_ok=True)
        args.output.write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps({'gamepadMatrix':'PASS','combinations':report['combinations']}))


if __name__=='__main__':
    main()
