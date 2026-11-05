"""Virtual controllers through production settings, navigator API and Release TH09.

Requires a local Launcher preview with current frontend + TH09 Runtime and retail
DATA. Reads the actual C++ sampling callbacks without adding input samples.
Synthetic API devices do not establish physical hardware/driver compatibility.
"""
import argparse
import importlib.util
import json
from pathlib import Path
from playwright.sync_api import sync_playwright

ROOT=Path(__file__).resolve().parents[1]
spec=importlib.util.spec_from_file_location('gamepad_matrix',ROOT/'tests/test-gamepad-matrix-browser.py')
matrix=importlib.util.module_from_spec(spec)
spec.loader.exec_module(matrix)


def controller_controls(layout):
    if layout in ('xbox','dualsense','pro'):
        return {a:{'button':n} for a,n in {'shoot':0,'bomb':1,'focus':4,'pause':9,'up':12,'down':13,'left':14,'right':15}.items()}
    if layout=='buttonOnly':
        return {a:{'button':n} for a,n in {'shoot':3,'bomb':2,'focus':1,'pause':7,'up':12,'down':13,'left':14,'right':15}.items()}
    controls={a:{'button':n} for a,n in {'shoot':3,'bomb':2,'focus':1,'pause':5}.items()}
    controls.update({a:{'axis':n,'value':v} for a,n,v in [('up',1,-1),('down',1,1),('left',0,-1),('right',0,1)]})
    if layout=='axisTriggers':controls['focus']={'axis':2,'value':1}
    return controls


def run(browser,url,out,fixture_bundle,case):
    context=browser.new_context(viewport={'width':1440,'height':1000})
    page=context.new_page()
    errors=[]
    page.on('pageerror',lambda e:errors.append(str(e)))
    controllers=case['pads']
    initialize=fixture_bundle+"\n"+r"""
if(window===top){
  window.virtualPads=[];window.virtualRests={};
  for(const [type,index] of CONTROLLERS){virtualPads[index]=VirtualPads.virtualGamepad(type,index);virtualRests[index]=[...virtualPads[index].axes];}
  window.virtualSet=(index,control,down)=>{const pad=virtualPads[index];if('button' in control)VirtualPads.virtualButton(pad,control.button,down?1:0);else {pad.axes[control.axis]=down?control.value:virtualRests[index][control.axis];pad.timestamp++;}};
  window.virtualHotplug=(index,connected)=>{const pad=window.savedVirtualPads?.[index]??virtualPads[index];window.savedVirtualPads??={};savedVirtualPads[index]=pad;pad.connected=connected;virtualPads[index]=connected?pad:null;
    for(const win of [window,...[...document.querySelectorAll('iframe')].map(f=>f.contentWindow)]){const event=new win.Event(connected?'gamepadconnected':'gamepaddisconnected');Object.defineProperty(event,'gamepad',{value:pad});win.dispatchEvent(event);}
  };
}
Object.defineProperty(navigator,'getGamepads',{configurable:true,value:()=>top.virtualPads});
localStorage.setItem('eagler-touhou-first-use-notice-seen-v1','1');
""".replace('CONTROLLERS',json.dumps(controllers))
    context.add_init_script(initialize)
    page.goto(url,wait_until='domcontentloaded')
    page.locator('.input-settings').wait_for(timeout=20000)

    def change_pad(index,control,down):page.evaluate('a=>virtualSet(...a)',[index,control,down])
    def learn(index,action,control):
        row=page.locator(f'.gamepad-settings tr[data-action="{action}"] .input-binding-button')
        row.click();page.wait_for_timeout(80);change_pad(index,control,True)
        row.locator('kbd').first.wait_for(timeout=5000)
        change_pad(index,control,False);page.wait_for_timeout(70)
    def choose_pad(position):page.locator('select[aria-label="手柄设备"]').select_option(index=position+1,force=True)
    def setup_profile(kind,index):
        controls=controller_controls(kind)
        if kind not in ('xbox','dualsense','pro'):
            for action,control in controls.items():learn(index,action,control)
        return controls

    # General controller settings must also navigate menus before local versus.
    page.get_by_role('button',name='手柄改键',exact=True).click()
    page.get_by_role('checkbox',name='启用手柄改键',exact=True).check()
    choose_pad(0)
    general=setup_profile(*controllers[0])
    page.locator('select[aria-label="配置场景"]').select_option('local',force=True)
    page.get_by_role('checkbox',name='启用本地双人配置',exact=True).check()
    controls=[]
    for seat,assignment in enumerate(case['players']):
        page.get_by_role('tab',name=f'玩家 {seat+1}',exact=True).click()
        if assignment=='keyboard':
            page.locator('select[aria-label="双人键位布局"]').select_option('wasd',force=True)
            controls.append({'shoot':'f','bomb':'g','focus':'ShiftLeft','pause':'Escape','up':'w','down':'s','left':'a','right':'d'})
        else:
            kind,index=controllers[assignment]
            page.locator('select[aria-label="玩家输入设备"]').select_option('gamepad',force=True)
            choose_pad(assignment)
            control=controller_controls(kind)
            if assignment!=0:control=setup_profile(kind,index)
            if case.get('identical') and seat==1:
                learn(index,'shoot',{'button':3});control['shoot']={'button':3}
            controls.append(control)
    page.locator('.input-settings-dialog').screenshot(path=str(out/(case['name']+'-settings.png')))
    page.get_by_role('button',name='关闭输入设置',exact=True).click()
    page.locator('.input-settings-dialog').wait_for(state='detached')
    if case.get('reload'):
        page.reload(wait_until='domcontentloaded');page.locator('.input-settings').wait_for()
        saved=page.evaluate("JSON.parse(localStorage.getItem('eagler-touhou-th09-local-input-v1'))")
        assert saved['enabled'] and all(p['device']=='gamepad' for p in saved['players']),saved
    page.locator('#musicSelect').select_option('none',force=True)
    if page.locator('#languageSelect option[value="ja"]').count():page.locator('#languageSelect').select_option('ja',force=True)
    page.get_by_role('button',name='启动游戏',exact=True).click()
    page.get_by_role('button',name='仍要启动',exact=True).click()
    page.wait_for_function("[...document.querySelectorAll('iframe')].some(f=>f.contentWindow?.Module?._th09_keyboard_profile)",timeout=60000)
    frame=next(f for f in page.frames if 'input-runtime/normal/' in f.url)
    frame.wait_for_function("Module.HEAP32[Module._th09_title_status()/4+3]===1",timeout=60000)
    frame.locator('canvas').click()
    frame.evaluate(r"""()=>{
      window.padTrace=[];window.padLast=[-1,-1];window.keyLast=[-1,-1];
      for(const [owner,name,last] of [[__eaglerGamepadMapping,'sample',padLast],[__eaglerKeyboardMapping,'sampleLocal',keyLast]]){
        const original=owner[name];owner[name]=seat=>{const bits=original(seat);last[seat]=bits;if(padTrace.length<10000)padTrace.push([name,seat,bits]);return bits;};
      }
      window.titleState=()=>Array.from(Module.HEAP32.subarray(Module._th09_title_status()/4,Module._th09_title_status()/4+8));
      window.sessionState=()=>Array.from(Module.HEAP32.subarray(Module._th09_session_status()/4,Module._th09_session_status()/4+8));
      window.inputLast=()=>padLast.map((bits,n)=>Math.max(0,bits)|Math.max(0,keyLast[n]));
    }""")
    def title():return frame.evaluate('titleState()')
    def set_action(seat,action,down):
        assignment=case['players'][seat];control=controls[seat][action]
        if assignment=='keyboard':getattr(page.keyboard,'down' if down else 'up')(control)
        else:change_pad(controllers[assignment][1],control,down)
    def press(seat,action):set_action(seat,action,True);page.wait_for_timeout(85);set_action(seat,action,False);page.wait_for_timeout(110)
    def general_press(action):
        change_pad(controllers[0][1],general[action],True);page.wait_for_timeout(85);change_pad(controllers[0][1],general[action],False);page.wait_for_timeout(110)
    for _ in range(4):
        if title()[4]==2:break
        general_press('down')
    assert title()[4]==2,(case['name'],'controller menu navigation',title())
    general_press('shoot');frame.wait_for_function('titleState()[2]===6&&titleState()[3]===1')
    for _ in range(5):
        if title()[4]==0:break
        general_press('up')
    assert title()[4]==0,title()
    general_press('shoot');frame.wait_for_function('titleState()[2]===7&&titleState()[3]===1')
    press(0,'shoot');frame.wait_for_function('titleState()[2]===8&&titleState()[3]===1')
    set_action(0,'right',True);page.wait_for_timeout(100);assert frame.evaluate('inputLast()')==[128,0],frame.evaluate('inputLast()')
    set_action(1,'up',True);page.wait_for_timeout(100);assert frame.evaluate('inputLast()')==[128,16],frame.evaluate('inputLast()')
    set_action(0,'right',False);page.wait_for_timeout(70);assert frame.evaluate('inputLast()')==[0,16]
    set_action(1,'up',False);page.wait_for_timeout(70);assert frame.evaluate('inputLast()')==[0,0]
    press(0,'shoot');assert title()[2]==8,'One player confirmed both characters'
    press(1,'shoot');frame.wait_for_function('titleState()[2]===16&&titleState()[3]===1')
    press(0,'shoot');frame.wait_for_function('titleState()[1]===0')
    page.wait_for_timeout(1800)
    for action,bits in [('shoot',1),('focus',4),('right',128)]:
        set_action(0,action,True);set_action(1,action,True);page.wait_for_timeout(100)
        assert frame.evaluate('inputLast()')==[bits,bits],(case['name'],action,frame.evaluate('inputLast()'))
        set_action(0,action,False);page.wait_for_timeout(70);assert frame.evaluate('inputLast()')==[0,bits]
        set_action(1,action,False);page.wait_for_timeout(70);assert frame.evaluate('inputLast()')==[0,0]
    press(0,'pause');before=frame.evaluate('sessionState()[1]');page.wait_for_timeout(200);assert frame.evaluate('sessionState()[1]')==before,'Pause did not stop simulation'
    press(1,'pause');page.wait_for_timeout(180);assert frame.evaluate('sessionState()[1]')>before,'Other controller could not resume'
    if case.get('identical'):
        first_index=controllers[0][1]
        page.evaluate('index=>virtualHotplug(index,false)',first_index);page.wait_for_timeout(100)
        set_action(1,'shoot',True);page.wait_for_timeout(100);assert frame.evaluate('inputLast()')==[0,1],'Surviving identical controller changed seats'
        set_action(1,'shoot',False);page.evaluate('index=>virtualHotplug(index,true)',first_index);page.wait_for_timeout(100)
        set_action(0,'shoot',True);set_action(1,'shoot',True);page.wait_for_timeout(100);assert frame.evaluate('inputLast()')==[1,1],'Identical pair failed after reattachment'
        set_action(0,'shoot',False);set_action(1,'shoot',False);page.wait_for_timeout(70)
    frame.locator('canvas').screenshot(path=str(out/(case['name']+'-game.png')))
    assert not errors,errors
    result={'case':case,'result':'PASS','menu':'controller only','twoPlayerSelection':'PASS','gameplay':'shoot/focus/movement isolation PASS','pauseResume':'PASS','trace':frame.evaluate('padTrace'),'errors':errors}
    (out/(case['name']+'.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    print(json.dumps({'case':case['name'],'result':'PASS'}),flush=True)
    context.close()
    return {k:v for k,v in result.items() if k!='trace'}


def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--url',default='http://127.0.0.1:38982/?game=th09')
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();args.output.mkdir(parents=True,exist_ok=True)
    cases=[{'name':'standard-pair','pads':[['xbox',0],['dualsense',3]],'players':[0,1],'reload':True},
           {'name':'raw-stick-trigger-pair','pads':[['arcade',0],['axisTriggers',3]],'players':[0,1]},
           {'name':'identical-hotplug','pads':[['xbox',0],['xbox',3]],'players':[0,1],'identical':True},
           {'name':'keyboard-button-pad','pads':[['buttonOnly',3]],'players':['keyboard',0]}]
    with sync_playwright() as p:
        browser=p.chromium.launch(headless=True)
        try:results=[run(browser,args.url,args.output,matrix.bundle_pads(),case) for case in cases]
        finally:browser.close()
    (args.output/'report.json').write_text(json.dumps({'scope':__doc__.strip(),'results':results},ensure_ascii=False,indent=2),encoding='utf-8')


if __name__=='__main__':main()
