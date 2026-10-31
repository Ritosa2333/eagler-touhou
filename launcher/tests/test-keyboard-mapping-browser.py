"""Exercise STG Launcher mapping and production Runtime JS receivers in Chromium.

Key sinks record delivered actions; virtual controllers exercise gamepad mapping.
"""
import argparse
import json
from pathlib import Path
import subprocess
import sys
import traceback

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


def runtime_source(workspace, target):
    game = target["game"]
    repo = workspace / target["directory"]
    if game in ("th06", "th07"):
        source = (repo / "resources/shell.html").read_text(encoding="utf-8")
        start = source.index("const browserKeyboardDown = new Map();")
        end = source.index('window.addEventListener("message"', start)
        return "const launched=true,protocol='eagler-touhou/1',game='" + game + "',epoch=1;window.Module={eaglerControls:{},eaglerOptions:{}};" + source[start:end] + "\nwindow.clearRuntime=clearBrowserKeyboard;window.getKeys=()=>({bits:Module.eaglerControls.keyboardBits||0,pulse:Module.eaglerControls.keyboardPulseBits||0});"
    if game == "th12":
        return managed_runtime_fixture(repo)
    source = (repo / "web/sdl-runtime/directory-keyboard.mjs").read_text(encoding="utf-8")
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


def managed_runtime_fixture(repo):
    # Exercise the production th12 input mapper and asynchronous shell receiver.
    # The sink records canonical keys; no game simulation or WASM is supplied.
    contents = """
      import {ManagedInput,keyboardCode} from 'INPUT';
      import {installManagedShell} from 'SHELL';
      const input=new ManagedInput(),held=new Set();
      const dispatch=(code,down)=>{if(down)held.add(code);else held.delete(code);};
      const key=(raw,down,source)=>{if(!input.key(raw,down,source,dispatch))dispatch(keyboardCode(raw),down);};
      window.clearRuntime=()=>{input.clear();held.clear();};
      window.getKeys=()=>({keys:[...held].sort()});
      window.managedPad=()=>input.pad();
      installManagedShell({key:(code,down,raw)=>key(raw,down,raw.inputSource==='physical'?'physical':'virtual'),clearKeyboard:clearRuntime});
      for(const type of ['keydown','keyup'])addEventListener(type,event=>{if(input.key(event,type==='keydown','physical',dispatch))event.preventDefault();},true);
    """.replace("INPUT", (repo / "src/managed-input.mjs").as_posix()).replace("SHELL", (repo / "src/managed-shell.mjs").as_posix())
    script = """import {build} from 'esbuild';const result=await build({stdin:{resolveDir:process.cwd(),contents:CONTENTS},bundle:true,write:false,format:'iife',platform:'browser'});process.stdout.write(result.outputFiles[0].text);""".replace("CONTENTS", json.dumps(contents))
    return subprocess.run(["node", "--input-type=module", "-e", script], cwd=ROOT, capture_output=True, text=True, encoding="utf-8", check=True).stdout


def run(browser, launcher, workspace, target, broken=False):
    game = target["game"]
    page = browser.new_page()
    errors = []
    page.on("pageerror", lambda error: errors.append(str(error)))
    page.on("console", lambda message: errors.append(message.text) if message.type == "error" else None)
    page.route("http://keyboard.test/**", lambda route: route.fulfill(content_type="text/html", body=(
        '<canvas tabindex="0" width="200" height="100"></canvas>' if "/runtime" in route.request.url else
        '<div id="host" tabindex="0">host input</div><iframe src="/runtime?runtimeEpoch=1"></iframe>')))
    page.goto("http://keyboard.test/")
    frame = page.frames[1]
    frame.wait_for_selector("canvas")
    frame.add_script_tag(content=runtime_source(workspace, target))
    frame.evaluate("window.fixtureKeyDown=code=>typeof window.__eaglerKeyboardInputBridge?.keyDown==='function'?__eaglerKeyboardInputBridge.keyDown(code):getKeys().keys.includes(code)")
    page.add_script_tag(content=launcher)
    page.evaluate("""game=>{
      const frame=document.querySelector('iframe'),target=frame.contentWindow;
      const context={target,targetOrigin:location.origin,protocol:'eagler-touhou/1',game,epoch:1,launched:true,ready:true,spectator:false};
      window.messages=0;window.fullscreens=0;const notify=[];
      const runtime={getInputContext:()=>context,getMidiEventContext:()=>({target,document:target.document,game,epoch:1,music:'none'}),
        getSnapshot:()=>({launched:true,ready:true}),subscribe:fn=>{notify.push(fn);return()=>{};},
        postInput:(command,body)=>ProductionKeyboard.deliverRuntimeInput(context,{...body,protocol:context.protocol,game,epoch:1,command})};
      const post=target.postMessage.bind(target);target.postMessage=(...args)=>{window.messages++;post(...args);};
      window.flushInput=()=>new Promise(resolve=>{const done=event=>{if(event.data!=='fixture-input-drained')return;target.removeEventListener('message',done);resolve();};target.addEventListener('message',done);post('fixture-input-drained',location.origin);});
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
    if broken:
        frame.evaluate("delete window.__eaglerKeyboardInputBridge;delete window.__eaglerKeyboardMapping;window.__eaglerDirectInputBridge={schema:'eagler-touhou/direct-input/1',protocol:'eagler-touhou/1',game:'" + game + "',epoch:1,origin:location.origin,submit:()=>true}")
        page.evaluate("keyboardPreferences.selectLayout('wasd');hostKey('KeyJ',true)")
        state = frame.evaluate("getKeys()")
        assert state.get("bits", 0) & 1 or "KeyZ" in state.get("keys", []), "disconnected receiver did not receive shooting"
    frame.locator("canvas").click()
    frame.evaluate("window.samplePad=()=>window.managedPad?managedPad():__eaglerGamepadInputBridge.sample(0)")
    assert frame.evaluate("window.managedPad?managedPad()!==null:__eaglerGamepadInputBridge.owned()")
    assert frame.evaluate("samplePad()") == 0
    page.evaluate("pad.buttons[0]={pressed:true,value:1}")
    assert frame.evaluate("samplePad()") == 1
    page.evaluate("pad.buttons[0]={pressed:false,value:0};pad.axes[0]=.8")
    assert frame.evaluate("samplePad()") == 128
    page.evaluate("pad.axes[0]=0")
    frame.evaluate("clearRuntime()")
    page.evaluate("pad.buttons[0]={pressed:true,value:1}")
    assert frame.evaluate("samplePad()") == 0
    page.evaluate("pad.buttons[0]={pressed:false,value:0}")
    assert frame.evaluate("samplePad()") == 0


    def expect_held(code, bit, down=True):
        frame.wait_for_function("([code,bit,down])=>{const state=getKeys();return ('bits' in state?!!(state.bits&bit):state.keys.includes(code))===down}", arg=[code, bit, down], timeout=3000)

    assert page.evaluate("keyboardPreferences.getLayoutSnapshot().selected") == 'arrows'
    page.evaluate("flushInput()")
    page.keyboard.down("w")
    assert not frame.evaluate("fixtureKeyDown('ArrowUp')"), (game, "Arrow keys preset still maps W")
    page.keyboard.up("w")
    page.evaluate("async()=>{keyboardPreferences.selectLayout('wasd');await flushInput();}")
    page.keyboard.down("w")
    frame.wait_for_function("fixtureKeyDown('ArrowUp')", timeout=3000)
    page.evaluate("async()=>{keyboardPreferences.selectLayout('arrows');await flushInput();}")
    frame.wait_for_function("!fixtureKeyDown('ArrowUp')", timeout=3000)
    frame.evaluate("window.dispatchEvent(new KeyboardEvent('keydown',{code:'KeyW',key:'w',repeat:true,bubbles:true}))")
    assert not frame.evaluate("fixtureKeyDown('ArrowUp')")
    page.keyboard.up("w")
    page.evaluate("async()=>{keyboardPreferences.selectLayout('wasd');await flushInput();}")

    page.keyboard.down("w")
    expect_held("ArrowUp", 16)
    page.keyboard.down("ArrowUp")
    page.keyboard.up("w")
    expect_held("ArrowUp", 16)
    page.keyboard.up("ArrowUp")
    expect_held("ArrowUp", 16, False)
    page.keyboard.down("z")
    page.keyboard.down("j")
    page.keyboard.up("z")
    expect_held("KeyZ", 1)
    page.keyboard.up("j")
    expect_held("KeyZ", 1, False)
    for key in ("l", "Space", "ShiftLeft"):
        page.keyboard.down(key)
    for key in ("l", "ShiftLeft"):
        page.keyboard.up(key)
        expect_held("ShiftLeft", 4)
    page.keyboard.up("Space")
    expect_held("ShiftLeft", 4, False)

    page.keyboard.down("k")
    expect_held("KeyX", 2)
    page.keyboard.up("k")
    expect_held("KeyX", 2, False)

    # Touch is already canonical and must not be remapped or own a physical H.
    page.keyboard.down("h")
    frame.wait_for_function("fixtureKeyDown('KeyC')", timeout=3000)
    page.evaluate("touch(true);touch(false)")
    frame.wait_for_function("fixtureKeyDown('KeyC')", timeout=3000)
    page.keyboard.up("h")
    frame.wait_for_function("!fixtureKeyDown('KeyC')", timeout=3000)
    page.keyboard.press("Alt+Enter")
    assert page.evaluate("fullscreens") == 1
    expect_held("Enter", 4096, False)

    # Host focus uses the same production receiver as native-window focus.
    page.evaluate("hostKey('KeyJ',true)")
    expect_held("KeyZ", 1)
    page.evaluate("hostKey('KeyJ',false)")
    expect_held("KeyZ", 1, False)
    if game != "th12":
        assert page.evaluate("messages") == 0, (game, "keyboard fell back to queued postMessage")
    else:
        assert page.evaluate("messages") > 0, "th12 production shell uses postMessage"

    page.keyboard.down("w")
    expect_held("ArrowUp", 16)
    frame.evaluate("clearRuntime()")
    frame.evaluate("window.dispatchEvent(new KeyboardEvent('keydown',{code:'KeyW',key:'w',keyCode:87,repeat:true,bubbles:true,cancelable:true}))")
    expect_held("ArrowUp", 16, False)
    page.keyboard.up("w")
    expect_held("ArrowUp", 16, False)
    page.keyboard.down("w")
    expect_held("ArrowUp", 16)
    page.evaluate("window.dispatchEvent(new Event('blur'))")
    expect_held("ArrowUp", 16, False)
    page.keyboard.up("w")
    if game == "th09":
        for profile, expected in (("th09-left", "KeyT"), ("th09-right", "Numpad8")):
            frame.evaluate("p=>{clearRuntime();fixtureProfile=p}", profile)
            page.keyboard.down("w")
            expect_held(expected, 16)
            page.keyboard.up("w")
    page.evaluate("owner.dispose();gamepadOwner.dispose()")
    assert errors == [], errors
    page.close()
    return {"game": game, "variant": target["variant"], "keyboardBoundary": "PASS", "transport": "postMessage" if game == "th12" else "synchronous"}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--workspace", type=Path, default=ROOT.parent, help="Monorepo root")
    parser.add_argument("--game", help="Comma-separated STG products")
    parser.add_argument("--variant", choices=["normal", "multiplayer", "all"], default="all")
    args = parser.parse_args()
    command = ["node", str(ROOT.parent / "tools/ports/runtime-targets.mjs"), "--input-profile=standard", "--variant=" + args.variant]
    if args.game is not None:
        command.append("--game=" + args.game)
    targets = json.loads(subprocess.run(command, capture_output=True, text=True, encoding="utf-8", check=True).stdout)
    launcher = bundle_launcher()
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True)
        try:
            results = []
            for target in targets:
                try:
                    results.append(run(browser, launcher, args.workspace, target))
                except Exception:
                    results.append({"game": target["game"], "variant": target["variant"], "keyboardBoundary": "FAIL", "error": traceback.format_exc()})
                finally:
                    for context in browser.contexts:
                        context.close()
            # Known broken delivery must fail the same shooting assertion.
            try:
                run(browser, launcher, args.workspace, targets[0], broken=True)
            except AssertionError as error:
                assert str(error) == "disconnected receiver did not receive shooting", str(error)
            else:
                raise AssertionError("Known disconnected receiver passed")
        finally:
            browser.close()
    print(json.dumps({"scope": "STG Launcher mapping to production JS receiver and recorded key sink", "required": len(targets), "executed": len(results), "negativeControl": "PASS", "keyboardMappingBrowser": results}, ensure_ascii=False))
    if any(row["keyboardBoundary"] != "PASS" for row in results):
        sys.exit(1)


if __name__ == "__main__":
    main()
