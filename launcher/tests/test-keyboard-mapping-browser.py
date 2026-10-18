"""Actual Launcher mapper and Runtime JS input boundaries in Chromium.

Native game sinks are observed, not emulated gameplay. This is not a full
game, Android/iOS, Replay, or display-to-input latency acceptance claim.
"""
import argparse
import json
from pathlib import Path
import subprocess

from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]


def bundle_launcher():
    script = """import {build} from 'esbuild';
    const result=await build({stdin:{resolveDir:process.cwd(),loader:'ts',contents:
      \"export {createPlayerKeyboard} from './app/services/player-keyboard.ts';export {createKeyboardPreferences} from './src/launcher/keyboard-preferences.mts';export {createPlayerGamepad} from './app/services/player-gamepad.ts';export {createGamepadMappingModel} from './src/launcher/gamepad-mapping.mts';export {deliverRuntimeInput,postHostedKey} from './src/launcher/touch-runtime-protocol.mts';\"},
      bundle:true,write:false,format:'iife',globalName:'ProductionKeyboard',platform:'browser'});
    process.stdout.write(result.outputFiles[0].text);"""
    return subprocess.run(["node", "--input-type=module", "-e", script], cwd=ROOT,
                          capture_output=True, text=True, encoding="utf-8", check=True).stdout


def runtime_source(workspace, game):
    repo = workspace / (game + "-eagler")
    if game in ("th06", "th07"):
        source = (repo / "resources/shell.html").read_text(encoding="utf-8")
        start = source.index("const browserKeyboardDown = new Map();")
        end = source.index('window.addEventListener("message"', start)
        return "const launched=true,protocol='eagler-touhou/1',game='" + game + "',epoch=1;window.Module={eaglerControls:{},eaglerOptions:{}};" + source[start:end] + "\nwindow.clearRuntime=clearBrowserKeyboard;window.getKeys=()=>({bits:Module.eaglerControls.keyboardBits||0,pulse:Module.eaglerControls.keyboardPulseBits||0});"
    source = (repo / (game + "_web/sdl-runtime/directory-keyboard.mjs")).read_text(encoding="utf-8")
    return source.replace("export ", "") + """
      const held=new Set();window.fixtureProfile='standard';
      const keyboard=createBrowserKeyboard({hostContext:{protocol:'eagler-touhou/1',game:'GAME',epoch:1,origin:location.origin,
        ready:()=>true,reset:()=>keyboard.clear(),profile:()=>window.fixtureProfile},
        send(code,down){if(down)held.add(code);else held.delete(code);}});
      for(const type of ['keydown','keyup'])window.addEventListener(type,e=>{
        if(keyboard.mapNative(e,type==='keydown')){e.preventDefault();e.stopImmediatePropagation();return;}
        keyboard.event(e,type==='keydown');
      },true);
      window.clearRuntime=()=>keyboard.clear();window.getKeys=()=>({keys:[...held].sort()});
    """.replace("GAME", game)


def run(browser, launcher, workspace, game):
    page = browser.new_page()
    page.set_content('<div id="host" tabindex="0">host input</div><iframe srcdoc="<canvas tabindex=0 width=200 height=100></canvas>"></iframe>')
    frame = page.frames[1]
    frame.wait_for_selector("canvas")
    frame.add_script_tag(content=runtime_source(workspace, game))
    page.add_script_tag(content=launcher)
    page.evaluate("""game=>{
      const frame=document.querySelector('iframe'),target=frame.contentWindow;
      const context={target,targetOrigin:location.origin,protocol:'eagler-touhou/1',game,epoch:1,launched:true,ready:true,spectator:false};
      window.messages=0;window.fullscreens=0;const notify=[];
      const runtime={getInputContext:()=>context,getMidiEventContext:()=>({target,document:target.document,game,epoch:1,music:'none'}),
        getSnapshot:()=>({launched:true,ready:true}),subscribe:fn=>{notify.push(fn);return()=>{};},
        postInput:(command,body)=>ProductionKeyboard.deliverRuntimeInput(context,{...body,protocol:context.protocol,game,epoch:1,command})};
      target.postMessage=()=>{window.messages++;};
      window.keyboardPreferences=ProductionKeyboard.createKeyboardPreferences(null);
      window.owner=ProductionKeyboard.createPlayerKeyboard({runtime,window,document,frame,preferences:keyboardPreferences,playerOpen:()=>true,
        launcherOwnsTarget:t=>!!t?.closest?.('input,textarea,select,button'),releaseHeldTouchFire(){},cancelTouchFunction(){},
        toggleFullscreen(){window.fullscreens++;},isPlayerFullscreen:()=>true,android:false});
      window.pad={id:'Input boundary fixture',index:0,connected:true,mapping:'standard',buttons:Array.from({length:17},()=>({pressed:false,value:0})),axes:[0,0,0,0]};
      window.gamepadModel=ProductionKeyboard.createGamepadMappingModel({storage:null,getGamepads:()=>[window.pad]});
      gamepadModel.refresh();gamepadModel.select(0,gamepadModel.getSnapshot().devices[0].key);gamepadModel.setEnabled(true);
      window.gamepadOwner=ProductionKeyboard.createPlayerGamepad({runtime,model:gamepadModel,window,document,playerOpen:()=>true});
      window.touch=down=>ProductionKeyboard.postHostedKey(context,{code:'KeyC',key:'c',keyCode:67},down);
      window.hostKey=(code,down)=>document.querySelector('#host').dispatchEvent(new KeyboardEvent(down?'keydown':'keyup',{code,key:'',keyCode:0,bubbles:true,cancelable:true}));
    }""", game)
    frame.locator("canvas").click()
    assert frame.evaluate("__eaglerGamepadInputBridge.owned()")
    assert frame.evaluate("__eaglerGamepadInputBridge.sample(0)") == 0
    page.evaluate("pad.buttons[0]={pressed:true,value:1}")
    assert frame.evaluate("__eaglerGamepadInputBridge.sample(0)") == 1
    page.evaluate("pad.buttons[0]={pressed:false,value:0};pad.axes[0]=.8")
    assert frame.evaluate("__eaglerGamepadInputBridge.sample(0)") == 128
    page.evaluate("pad.axes[0]=0")
    frame.evaluate("clearRuntime()")
    page.evaluate("pad.buttons[0]={pressed:true,value:1}")
    assert frame.evaluate("__eaglerGamepadInputBridge.sample(0)") == 0
    page.evaluate("pad.buttons[0]={pressed:false,value:0}")
    assert frame.evaluate("__eaglerGamepadInputBridge.sample(0)") == 0


    def held(code, bit):
        state = frame.evaluate("getKeys()")
        return bool(state["bits"] & bit) if "bits" in state else code in state["keys"]

    assert page.evaluate("keyboardPreferences.getLayoutSnapshot().selected") == 'arrows'
    page.keyboard.down("w")
    assert not frame.evaluate("__eaglerKeyboardInputBridge.keyDown('ArrowUp')"), (game, "Arrow keys preset still maps W")
    page.keyboard.up("w")
    page.evaluate("keyboardPreferences.selectLayout('wasd')")
    page.keyboard.down("w")
    assert frame.evaluate("__eaglerKeyboardInputBridge.keyDown('ArrowUp')")
    page.evaluate("keyboardPreferences.selectLayout('arrows')")
    assert not frame.evaluate("__eaglerKeyboardInputBridge.keyDown('ArrowUp')"), (game, "switching layout retained the old key hold")
    frame.evaluate("window.dispatchEvent(new KeyboardEvent('keydown',{code:'KeyW',key:'w',repeat:true,bubbles:true}))")
    assert not frame.evaluate("__eaglerKeyboardInputBridge.keyDown('ArrowUp')")
    page.keyboard.up("w")
    page.evaluate("keyboardPreferences.selectLayout('wasd')")

    page.keyboard.down("w")
    assert held("ArrowUp", 16), (game, "W did not reach the current canonical input")
    page.keyboard.down("ArrowUp")
    page.keyboard.up("w")
    assert held("ArrowUp", 16), (game, "alias release cancelled a held original key")
    page.keyboard.up("ArrowUp")
    assert not held("ArrowUp", 16)
    page.keyboard.down("z")
    page.keyboard.down("j")
    page.keyboard.up("z")
    assert held("KeyZ", 1)
    page.keyboard.up("j")
    assert not held("KeyZ", 1)
    for key in ("l", "Space", "ShiftLeft"):
        page.keyboard.down(key)
    for key in ("l", "ShiftLeft"):
        page.keyboard.up(key)
        assert held("ShiftLeft", 4)
    page.keyboard.up("Space")
    assert not held("ShiftLeft", 4)

    page.keyboard.down("k")
    assert held("KeyX", 2)
    page.keyboard.up("k")
    assert not held("KeyX", 2)

    # Touch is already canonical and must not be remapped or own a physical H.
    page.keyboard.down("h")
    page.evaluate("touch(true);touch(false)")
    assert frame.evaluate("__eaglerKeyboardInputBridge.keyDown('KeyC')")
    page.keyboard.up("h")
    assert not frame.evaluate("__eaglerKeyboardInputBridge.keyDown('KeyC')")
    page.keyboard.press("Alt+Enter")
    assert page.evaluate("fullscreens") == 1
    assert not held("Enter", 4096)

    # The actual host service must use the same synchronous Runtime receiver.
    page.evaluate("hostKey('KeyJ',true)")
    assert held("KeyZ", 1)
    page.evaluate("hostKey('KeyJ',false)")
    assert not held("KeyZ", 1)
    assert page.evaluate("messages") == 0, (game, "keyboard fell back to queued postMessage")

    page.keyboard.down("w")
    frame.evaluate("clearRuntime()")
    frame.evaluate("window.dispatchEvent(new KeyboardEvent('keydown',{code:'KeyW',key:'w',keyCode:87,repeat:true,bubbles:true,cancelable:true}))")
    assert not held("ArrowUp", 16)
    page.keyboard.up("w")
    assert not held("ArrowUp", 16)
    page.keyboard.down("w")
    assert held("ArrowUp", 16)
    page.evaluate("window.dispatchEvent(new Event('blur'))")
    assert not held("ArrowUp", 16)
    page.keyboard.up("w")
    if game == "th09":
        for profile, expected in (("th09-left", "KeyT"), ("th09-right", "Numpad8")):
            frame.evaluate("p=>{clearRuntime();fixtureProfile=p}", profile)
            page.keyboard.down("w")
            assert held(expected, 16)
            page.keyboard.up("w")
    page.evaluate("owner.dispose()")
    page.close()
    return {"game": game, "keyboardBoundary": "PASS", "transport": "synchronous"}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--workspace", type=Path, default=ROOT.parent)
    args = parser.parse_args()
    launcher = bundle_launcher()
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        try:
            results = [run(browser, launcher, args.workspace, game) for game in ("th06", "th07", "th08", "th09", "th10", "th11", "th15", "th20")]
        finally:
            browser.close()
    print(json.dumps({"keyboardMappingBrowser": results}))


if __name__ == "__main__":
    main()
