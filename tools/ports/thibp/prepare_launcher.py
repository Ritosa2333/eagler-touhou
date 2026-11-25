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


def adapt_shell(runtime, output):
    shell = (runtime / "shell.mjs").read_text(encoding="utf-8")
    shell = replace(shell, "game='hsob'", "game='thibp'")
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
    # SDL sets the browser title to the reference game's window name during startup.
    shell = replace(shell, "Module.callMain([]);",
                    "\n const gameTitle=document.title;\n Module.callMain([]);\n document.title=gameTitle;")
    host = (runtime / "eagler-host.mjs").read_text(encoding="utf-8")
    host = replace(host, "game='hsob'", "game='thibp'")
    host = replace(host, r"th_hsob\/music\/", r"Th_IBP\/sound\/bgm\/")
    html = (runtime / "hsob.html").read_text(encoding="utf-8")
    html = replace(html, "东方真珠岛 ～ Hollow Song of Birds", "东方鬼葬剑 ～ Infinite Blade Pavilion")
    html = replace(html, "东方真珠岛游戏画面", "东方鬼葬剑游戏画面")
    html = replace(html, "    #error{", "    @media (min-width:640px) and (min-height:480px){\n"
                   "      canvas{image-rendering:pixelated;image-rendering:crisp-edges}\n"
                   "    }\n    #error{")
    manifest = json.loads((runtime / "manifest.json").read_text(encoding="utf-8"))
    manifest.update(game=GAME, version="thibp-1.10b-ph3-modern")
    output.mkdir(parents=True)
    for name, text in (("shell.mjs", shell), ("eagler-host.mjs", host), ("thibp.html", html)):
        (output / name).write_text(text, encoding="utf-8", newline="\n")
    (output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    for name in ("ph3-open.mjs", "ph3-open.wasm", "build.json"):
        shutil.copyfile(runtime / name, output / name)
    files = ("thibp.html", "shell.mjs", "eagler-host.mjs", "manifest.json",
             "ph3-open.mjs", "ph3-open.wasm", "build.json")
    write_json(output / "runtime-files.json", {
        "schema": "eagler-touhou/runtime-directory/1",
        "files": {name: identity(output / name) for name in files},
    })


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


def site_manifest(package_path, output):
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
    args = parser.parse_args()
    if args.command == "package":
        package(args.data, args.font, args.output)
    else:
        site_manifest(args.package, args.output)


if __name__ == "__main__":
    main()
