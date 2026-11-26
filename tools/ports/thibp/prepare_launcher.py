"""Stage the local thibp Runtime and data import for the author's Launcher."""

import argparse
import hashlib
import json
import re
import shutil
from pathlib import Path
from zipfile import ZipFile, ZIP_STORED


GAME = "thibp"
PROTOCOL = "eagler-touhou/1"
RUNTIME_URL = "./runtime/thibp/thibp.html"
COOP_RUNTIME_URL = "./runtime/thibp/multiplayer/thibp.html"
DATA_TARGET = "thibp.data"
FONT_TARGET = "msgothic.ttc"

KEYBOARD_BRIDGE = """const keyboard = new Map();
const gameKeys = new Set([
  'Escape', 'KeyZ', 'KeyX', 'KeyC',
  'ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'ShiftLeft', 'ControlLeft',
]);
function ccall(name, args = [], types = []) {
  return Module.ccall(name, null, types, args);
}
function setKey(code, down, repeat = false) {
  if (!launched || !Object.hasOwn(DIK, code)) return;
  const dik = DIK[code];
  if (down) {
    if (repeat || keyboard.has(dik)) return;
    keyboard.set(dik, diagnostics.frames);
    ccall('hsob_input_key', [dik, 1], ['number', 'number']);
    return;
  }
  if (!keyboard.has(dik)) return;
  const needsPulse = keyboard.get(dik) === diagnostics.frames;
  keyboard.delete(dik);
  ccall('hsob_input_key', [dik, 0], ['number', 'number']);
  // Preserve a tap when both events arrived before the next input poll.
  if (needsPulse) ccall('hsob_input_pulse', [dik], ['number']);
}
function clearKeyboard() {
  keyboard.clear();
  ccall('hsob_input_clear');
}
function handleGameKey(event) {
  const down = event.type === 'keydown';
  if (!launched || event.defaultPrevented || (down && (event.altKey || event.metaKey || event.isComposing))) return;
  const code = runtimeKeyboardCode(event);
  if (!gameKeys.has(code)) return;
  const held = keyboard.has(DIK[code]);
  if (!down && !held) return;
  event.preventDefault();
  if (event.repeat && !held) {
    event.stopImmediatePropagation();
    return;
  }
  // SDL also tracks physical holds when the game resets injected input on retry.
  setKey(code, down, event.repeat);
}
for (const event of ['keydown', 'keyup']) {
  window.addEventListener(event, handleGameKey, { capture: true });
}
"""


def replace(text, old, new):
    if old not in text:
        raise ValueError(f"Unexpected ph3-modern source: missing {old!r}")
    return text.replace(old, new)


def adapt_shell(runtime, output, *, coop=False):
    shell = (runtime / "shell.mjs").read_text(encoding="utf-8")
    shell = replace(shell, "game='hsob'", "game='thibp'")
    shell = replace(shell, "normalizeOptions,touchSnapshot,", "normalizeOptions,touchSnapshot,prepareThibpScripts,")
    shell = replace(shell, "/savehsob", "/savethibp")
    shell = replace(shell, "path.toLowerCase().startsWith('savehsob/')", "path.toLowerCase().startsWith(saveRoot.slice(1)+'/')")
    shell = replace(shell, "path=path.slice('savehsob/'.length)", "path=path.slice(saveRoot.length)")
    shell = replace(shell, "script/th_hsob", "script/Th_IBP")
    shell = replace(shell, "Package_common_", "Th_IBP_common_")
    shell = replace(shell, "Main_replay", "Th_IBP_replay")
    shell = replace(shell, "HSoB 1.12", "东方鬼葬剑 1.10b")
    shell = replace(shell, "initial-hsob-", "initial-thibp-")
    shell = replace(shell, "KeyR:0x13,ShiftLeft", "KeyR:0x13,KeyC:0x2e,KeyV:0x2f,ShiftLeft")
    shell = replace(shell, "r:'KeyR',shift:", "r:'KeyR',c:'KeyC',v:'KeyV',shift:")
    shell = replace(shell, "return byKey[key]||'';", "return byKey[key]||(message.keyCode===27?'Escape':'');")
    start = shell.index("const keyboard=new Map();")
    end = shell.index("let touchBaselineReady=", start)
    shell = shell[:start] + KEYBOARD_BRIDGE + shell[end:]
    shell = replace(shell, "const dik=DIK[runtimeKeyboardCode(message)];if(dik!==undefined)setSource(keyboard,dik,!!message.down);",
                    "setKey(runtimeKeyboardCode(message),!!message.down,message.repeat);")
    shell = replace(shell, "clearSource(keyboard)", "clearKeyboard()")
    shell = replace(shell, "musicMode=message.music;applyTouchConfig();",
                    "musicMode=message.music;applyTouchConfig();\n"
                    " Module.ph3FollowDisplayRefresh=message.options?.limitPresentationTo60===false;")
    shell = replace(shell, "createWoo({canvas,noInitialRun:true,",
                    "createWoo({canvas,noInitialRun:true,ph3NearestPresentation:true,"
                    "ph3ScriptFpsUsesPresentation:true,ph3OnExit:exitGame,")
    shell = replace(shell, "function startHealth(){", """function exitGame(){
 launched=false;
 clearInterval(healthTimer);
 healthTimer=null;
 suspendAudio();
 queue=queue.then(async()=>{
  await sync(false);
  if(managed)emit('exit',{status:'success',code:0});
  else location.reload();
 }).catch(fail);
}

function startHealth(){""")
    # Apply the shared menu adaptation after data is mounted and before the native scripts are loaded.
    # SDL sets the browser title to the reference game's window name during startup.
    shell = replace(shell, "Module.callMain([]);",
                    "\n prepareThibpScripts(Module.FS);\n const gameTitle=document.title;\n"
                    " Module.callMain([]);\n document.title=gameTitle;")
    host = (runtime / "eagler-host.mjs").read_text(encoding="utf-8")
    host = replace(host, "game='hsob'", "game='thibp'")
    host = replace(host, r"th_hsob\/music\/", r"Th_IBP\/sound\/bgm\/")
    host += "\n" + Path(__file__).with_name("game-scripts.mjs").read_text(encoding="utf-8")
    html = (runtime / "hsob.html").read_text(encoding="utf-8")
    html = replace(html, "东方真珠岛 ～ Hollow Song of Birds", "东方鬼葬剑 ～ Infinite Blade Pavilion")
    html = replace(html, "东方真珠岛游戏画面", "东方鬼葬剑游戏画面")
    html = replace(html, "    #error{", "    @media (min-width:640px) and (min-height:480px){\n"
                   "      canvas{image-rendering:pixelated;image-rendering:crisp-edges}\n"
                   "    }\n    #error{")
    if coop:
        shell = adapt_room_shell(shell)
        host = replace(host, "const bytes=new Uint8Array(result.buffer);",
                       "const bytes=new Uint8Array(result.buffer);\n"
                       " const hash=new Uint8Array(await crypto.subtle.digest('SHA-256',bytes));")
        host = replace(host, "total:bytes.byteLength,files});\n}",
                       "total:bytes.byteLength,files});\n"
                       " return Array.from(hash,b=>b.toString(16).padStart(2,'0')).join('');\n}")
    manifest = json.loads((runtime / "manifest.json").read_text(encoding="utf-8"))
    manifest.update(game=GAME, version="thibp-1.10b-ph3-modern")
    output.mkdir(parents=True)
    for name, text in (("shell.mjs", shell), ("eagler-host.mjs", host), ("thibp.html", html)):
        (output / name).write_text(text, encoding="utf-8", newline="\n")
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    extra = ("room.mjs", "room-wire.mjs", "room-wire.wasm", "rollback-preview.mjs", "ph3-rollback.wasm") if coop else ()
    for name in ("ph3-open.mjs", "ph3-open.wasm", "build.json") + extra:
        shutil.copyfile(runtime / name, output / name)
    if coop:
        glue_path = output / "ph3-open.mjs"
        glue = glue_path.read_text(encoding="utf-8")
        glue = replace(glue, "wasmExports = await createWasm();",
                       "Module['thibpGuardImports']?.(wasmImports);\n"
                       "Module['ph3StackSave'] = stackSave;\n"
                       "Module['ph3StackRestore'] = stackRestore;\n"
                       "Object.defineProperty(Module,'thibpHeapU32',{ get:()=>HEAPU32 });\n"
                       "wasmExports = await createWasm();\n"
                       "if(wasmExports.thibp_journal_begin)Module.thibpJournal={"
                       "begin:wasmExports.thibp_journal_begin,restore:wasmExports.thibp_journal_restore,"
                       "mark:wasmExports.thibp_journal_mark,failed:wasmExports.thibp_journal_failed};")
        glue_path.write_text(glue, encoding="utf-8", newline="\n")
    files = ("thibp.html", "shell.mjs", "eagler-host.mjs", "manifest.json",
             "ph3-open.mjs", "ph3-open.wasm", "build.json") + extra
    write_json(output / "runtime-files.json", {
        "schema": "eagler-touhou/runtime-directory/1",
        "files": {name: identity(output / name) for name in files},
    })


def adapt_room_shell(shell):
    shell = replace(shell, "import createWoo from './ph3-open.mjs';",
                    "import createWoo from './ph3-open.mjs';\n"
                    "import {createRoomInput,prepareRoomScripts,readRoomOptions,startRoom} from './room.mjs';\nimport {createRollbackPreview,supportsRollback} from './rollback-preview.mjs';")
    shell = replace(shell, "const saveRoot='/savethibp';",
                    "const multiplayer=query.get('runtimeVariant')==='multiplayer';\n"
                    "const saveRoot=multiplayer?'/savethibpmp':'/savethibp';\n"
                    "const roomInput=createRoomInput();\n"
                    "const prediction=(multiplayer||query.get('thibpPrediction')==='1')&&supportsRollback()?createRollbackPreview():null;\n"
                    "let roomConfig=null,room=null,roomFinished=false,contentHash=null;\n"
                    "let roomTitlePreview=false;")
    shell = replace(shell, " if (!launched || !Object.hasOwn(DIK, code)) return;",
                    " if(roomTitlePreview||diagnostics.phase==='disconnected')return;\n"
                    " if(roomConfig&&!roomFinished){if(!roomConfig.spectator&&(!down||!repeat))roomInput.key(code,down);return;}\n"
                    " if (!launched || !Object.hasOwn(DIK, code)) return;")
    shell = replace(shell, "  keyboard.clear();", "  keyboard.clear();\n  roomInput.clear();")
    shell = replace(shell, "  const held = keyboard.has(DIK[code]);",
                    "  if(roomTitlePreview||roomConfig&&!roomFinished){event.preventDefault();setKey(code,down,event.repeat);return;}\n"
                    "  const held = keyboard.has(DIK[code]);")
    shell = replace(shell, "'ShiftLeft', 'ControlLeft',", "'ShiftLeft', 'ShiftRight', 'ControlLeft', 'KeyV', 'KeyR', 'Enter',")
    shell = replace(shell, " if(launched)throw Error('Cannot configure a running game');",
                    " if(room||launched&&!multiplayer)throw Error('Cannot configure a running game');\n"
                    " if(multiplayer)roomConfig=readRoomOptions(message.options);")
    # The Launcher reads the validated LAN options through the shared Module bridge.
    shell = replace(shell, " options=normalizeOptions(message.options);musicMode=message.music;applyTouchConfig();",
                    " options=normalizeOptions(message.options);\n"
                    " Module.eaglerOptions=options;\n"
                    " musicMode=message.music;applyTouchConfig();")
    shell = replace(shell, "createWoo({canvas,noInitialRun:true,",
                    "createWoo({canvas,noInitialRun:true,locateFile:(file,base)=>base+(prediction&&file==='ph3-open.wasm'?'ph3-rollback.wasm':file),thibpGuardImports:prediction?.guardImports,thibpPrediction:prediction,")
    shell = replace(shell, " window.ph3Runtime=Module;window.FS=Module.FS;return Module;",
                    " window.Module=Module;window.ph3Runtime=Module;window.FS=Module.FS;return Module;")
    shell = replace(shell, "function launch(){", "function launchNative(){")
    shell = replace(shell, "reportedError=false;canvas.focus({preventScroll:true});",
                    "reportedError=false;if(!roomTitlePreview)canvas.focus({preventScroll:true});")
    shell = replace(shell, "performance.now()],['number','number','number','number','number']);",
                    "performance.now(),roomFinished?-1:roomConfig?.seat??-1,Number(roomInput.focusHeld())],"
                    "['number','number','number','number','number','number','number']);")
    shell = replace(shell, "async function resumeAudio(){", "async function resumeAudio(){\n"
                    " if(diagnostics.phase==='disconnected')return;")
    shell = replace(shell, "async function command(message){", """async function prepareRoom(){
 if(!multiplayer||!configured||roomConfig||room||launched&&!roomTitlePreview)
  throw Error('The THIBP room title is unavailable');
 if(!roomTitlePreview){
  roomTitlePreview=true;
  Module._thibp_coop_set_room_title_preview(1);
  launchNative();
  void resumeAudio();
 }
 // A loading frame is not the title menu. Wait for a present after the
 // original title script has created its menu.
 for(;;){
  if(diagnostics.phase==='error')throw Error(diagnostics.errors.at(-1));
  if(diagnostics.presentedFrames>Module.thibpTitleReadyAfter)return {presented:true};
  await new Promise(requestAnimationFrame);
 }
}

function startGame(){
 const fromTitle=roomTitlePreview;
 roomTitlePreview=false;
 firstFrame=false;
 launchNative();
 if(fromTitle)canvas.focus({preventScroll:true});
 void resumeAudio();
}

function stopRoom(reason){
 diagnostics.phase='disconnected';
 window.__eaglerNetplayFailed=true;
 window.__eaglerNetplayError=reason;
 clearKeyboard();
 cancelTouches();
 suspendAudio();
 emit('runtime-info',{netplayStopped:true});
}

async function launch(){
 if(!roomConfig){
  if(roomTitlePreview)Module._thibp_coop_set_room_title_preview(0);
  startGame();
  return;
 }
 if(room)return;
 if(!configured||!contentHash)throw Error('Runtime must load the local package before room launch');
 room=await startRoom({
  game:Module,config:roomConfig,options,contentHash,input:roomInput,prediction,
  onStart:startGame,onFinish:()=>{roomFinished=true;},onDisconnect:stopRoom,onError:fail,
  onStatus:state=>{diagnostics.online=state;},
  onPlayback:value=>{diagnostics.spectator=value;emit('runtime-info',{spectatorPlayback:value});},
 });
}

async function command(message){""")
    shell = replace(shell, " case 'launch':launch();return {};",
                    " case 'prepare-room':return prepareRoom();\n"
                    " case 'launch':await launch();return {};\n"
                    " case 'spectator-playback':if(!room?.control)throw Error('Spectator playback is unavailable');return room.control(message);")
    shell = replace(shell, " case 'touch-controls':applyTouchControls(message);return {};",
                    " case 'touch-controls':if(!roomTitlePreview)applyTouchControls(message);return {};")
    shell = replace(shell, " case 'direct-touch':directTouch(message);return {};",
                    " case 'direct-touch':if(!roomTitlePreview)directTouch(message);return {};")
    shell = replace(shell, "function exitGame(){", "function exitGame(){\n room?.close();\n room=null;\n roomTitlePreview=false;")
    shell = replace(shell, "window.addEventListener('pagehide',()=>{if(Module&&ready)",
                    "window.addEventListener('pagehide',()=>{room?.close();if(Module&&ready)")
    shell = replace(shell, "if(!firstFrame&&presented>0){",
                    "if(!roomTitlePreview&&!firstFrame&&presented>0&&(!roomConfig||Module._thibp_coop_frame()>0)){")
    shell = replace(shell,
                    " await mountManagedData(Module,{game,parentWindow:parent,query,emit});Module.FS.mkdirTree(saveRoot+'/replay');",
                    " contentHash=await mountManagedData(Module,{game,parentWindow:parent,query,emit});\n"
                    " if(multiplayer)prepareRoomScripts(Module.FS);\n"
                    " Module.FS.mkdirTree(saveRoot+'/replay');")
    shell = replace(shell, "startHealth();emit('ready');", "startHealth();emit('ready',{roomPreview:multiplayer});")
    return shell


def identity(path):
    with path.open("rb") as stream:
        sha = hashlib.file_digest(stream, "sha256").hexdigest()
    return {"bytes": path.stat().st_size, "sha256": sha}


def write_json(path, value):
    with path.open("x", encoding="utf-8", newline="\n") as stream:
        stream.write(json.dumps(value, indent=2) + "\n")


def package(data, font, output):
    data_identity = identity(data)
    font_identity = identity(font)
    with data.open("rb") as stream:
        if stream.read(8) != b"HSOBFS01":
            raise ValueError("Expected the ph3-modern DATA format")
        manifest_size = int.from_bytes(stream.read(4), "little")
        manifest = json.loads(stream.read(manifest_size))
    paths = [entry["path"] for entry in manifest["files"]]
    layout = "sha256-" + hashlib.sha256(json.dumps(paths, separators=(",", ":")).encode()).hexdigest()
    files = {
        "game-data": {**data_identity, "source": DATA_TARGET, "target": "/" + DATA_TARGET,
                      "revision": data_identity["sha256"][:16]},
        "shared-font": {**font_identity, "source": FONT_TARGET, "target": "/" + FONT_TARGET,
                        "revision": font_identity["sha256"][:16]},
    }
    descriptor = {
        "schema": "eagler-touhou/package/1", "game": GAME, "gameVersion": "original",
        "revision": hashlib.sha256((data_identity["sha256"] + font_identity["sha256"]).encode("ascii")).hexdigest()[:16],
        "files": files, "base": {"files": list(files)}, "components": {},
        "runtimeRequirement": {"protocol": PROTOCOL, "target": GAME, "dataFile": "game-data", "dataLayout": layout},
    }
    output.mkdir(parents=True, exist_ok=True)
    with ZipFile(output / "thibp-1.10b-zh-local.zip", "x", compression=ZIP_STORED) as archive:
        archive.writestr("package.json", json.dumps(descriptor, indent=2) + "\n")
        archive.write(data, DATA_TARGET)
        archive.write(font, FONT_TARGET)


def site_manifest(package_path, output, *, coop=False):
    with ZipFile(package_path) as archive:
        descriptor = json.loads(archive.read("package.json"))
        if descriptor["schema"] != "eagler-touhou/package/1" or descriptor["game"] != GAME:
            raise ValueError("Expected the THIBP offline game package")
        requirement = descriptor["runtimeRequirement"]
        if (requirement["protocol"], requirement["target"], requirement["dataFile"]) != (PROTOCOL, GAME, "game-data"):
            raise ValueError("Unexpected THIBP Runtime requirement")
        layout = requirement["dataLayout"]
        for key, target in (("game-data", DATA_TARGET), ("shared-font", FONT_TARGET)):
            file = descriptor["files"][key]
            if (file["target"] != "/" + target or archive.getinfo(file["source"]).file_size != file["bytes"]
                    or not re.fullmatch(r"[0-9a-f]{64}", file["sha256"])):
                raise ValueError(f"Invalid package file: {key}")
    if not re.fullmatch(r"sha256-[0-9a-f]{64}", layout):
        raise ValueError("Invalid THIBP data layout")
    data = descriptor["files"]["game-data"]
    data_identity = {"bytes": data["bytes"], "sha256": data["sha256"]}
    compatibility = {
        "schema": "eagler-touhou/offline-game-pack/1",
        "runtimeCompatibility": {"protocol": PROTOCOL, "dataLayout": layout, "versionSource": "offline-pack"},
        "requiredShared": ["/" + FONT_TARGET],
        "languages": {"source": "offline-pack", "baseline": ["ja"]},
    }
    site = {
        "schema": "eagler-touhou/site-manifest/1",
        "protocol": PROTOCOL,
        "profile": "web-development",
        "shared": {"resourceMode": "import", "testBuild": True},
        "games": {
            GAME: {
                "runtime": RUNTIME_URL,
                **({"multiplayerRuntime": COOP_RUNTIME_URL} if coop else {}),
                "gameData": {**data_identity, "path": DATA_TARGET, "layout": layout,
                             "version": "sha256-" + data_identity["sha256"]},
                "music": {"midi": {"files": [], "supported": False}},
                "offlineCompatibility": compatibility,
            }
        },
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    write_json(output, site)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    pack = commands.add_parser("package", help="build a private offline import package")
    pack.add_argument("data", type=Path)
    pack.add_argument("font", type=Path)
    pack.add_argument("output", type=Path)
    manifest = commands.add_parser("site-manifest", help="describe an existing offline package for local preview")
    manifest.add_argument("package", type=Path)
    manifest.add_argument("output", type=Path)
    manifest.add_argument("--coop-engine", action="store_true")
    args = parser.parse_args()
    if args.command == "package":
        package(args.data, args.font, args.output)
    else:
        site_manifest(args.package, args.output, coop=args.coop_engine)


if __name__ == "__main__":
    main()
