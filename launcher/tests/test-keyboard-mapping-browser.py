"""Exercise Launcher keyboard mapping through every real Runtime Shell in Chromium.

Only the generated WASM/Emscripten import is replaced with a native input sink.
The Runtime page, Shell initialization, identity checks, DOM listeners and
Launcher mapping implementation are served from this workspace and run as-is.
This test proves the Shell-to-JS Emscripten export sink, not native game behavior.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, urlsplit

from playwright.sync_api import sync_playwright

LAUNCHER = Path(__file__).resolve().parents[1]
DEFAULT_WORKSPACE = LAUNCHER.parent
PROTOCOL = "eagler-touhou/1"
EPOCH = 31
GAME_BYTES = bytes(range(64))
EMPTY_WASM = b"\x00asm\x01\x00\x00\x00"
FONT_FILES = {
    "th08": ["blend.bin", "cp932.bin"],
    "th09": ["cp932.bin", "blend.bin"],
    "th10": ["blend.bin", "codepages.bin"],
    "th11": ["cp932.bin"],
    "th15": ["cp932.bin"],
    "th20": [],
}

CLASSIC_MODULE = """(()=>{
const entries=new Map([['/',{kind:'dir'}]]);
const norm=p=>'/'+String(p).replaceAll('\\\\','/').split('/').filter(Boolean).join('/');
const fs={
 mkdir:p=>entries.set(norm(p),{kind:'dir'}),mkdirTree:p=>{let s='';for(const x of norm(p).split('/').filter(Boolean)){s+='/'+x;if(!entries.has(s))entries.set(s,{kind:'dir'});}},
 mount(_t,_o,p){this.mkdirTree(p);return{mount:{}};},syncfs(_p,cb){queueMicrotask(()=>cb?.(null));},
 writeFile(p,v){this.mkdirTree(norm(p).slice(0,norm(p).lastIndexOf('/'))||'/');entries.set(norm(p),{kind:'file',bytes:new Uint8Array(v)});},
 readFile:p=>entries.get(norm(p))?.bytes||new Uint8Array(),analyzePath:p=>({exists:entries.has(norm(p))}),
 stat:p=>({mode:entries.get(norm(p))?.kind==='dir'?0x4000:0x8000,size:0}),isFile:m=>(m&0xf000)===0x8000,isDir:m=>(m&0xf000)===0x4000,
 readdir:p=>['.','..',...Array.from(entries.keys()).filter(k=>k.startsWith(norm(p).replace(/\\/$/,'')+'/')).map(k=>k.slice(norm(p).replace(/\\/$/,'').length+1).split('/')[0])],
 unlink:p=>entries.delete(norm(p)),symlink:(a,b)=>entries.set(norm(b),{kind:'file',bytes:new TextEncoder().encode(a)}),rename(a,b){entries.set(norm(b),entries.get(norm(a)));entries.delete(norm(a));}
};
const m=window.Module;m.eaglerBuiltAt='2026-10-19T00:00:00.000Z';m.FS=window.FS=fs;m.IDBFS=window.IDBFS={};
window.addRunDependency=()=>{};window.removeRunDependency=()=>{};m.callMain=()=>{};
for(const callback of m.preRun||[])callback();m.onRuntimeInitialized();
})();"""

HOST_HTML = """<!doctype html><meta charset=utf-8><button id=host tabindex=0>Host focus</button><iframe id=runtime></iframe><iframe id=foreign src=/__blank.html></iframe>
<script>
window.runtimeEvents=[];
addEventListener('message',event=>{if(event.source===document.querySelector('#runtime').contentWindow)runtimeEvents.push(event.data);});
window.__eaglerPrepareManagedRuntimeDataV1=async({game})=>{const bytes=game==='th09'?1000000:64;return {buffer:new Uint8Array(bytes).buffer,bytes};};
</script>"""


def runtime_variants(workspace: Path) -> list[dict]:
    script = r'''import {existsSync} from "node:fs";
import {loadRuntimeBuildConfig} from "./launcher/lib/runtime-build-profiles.mjs";
import {PRODUCT_GAMES} from "./launcher/lib/contracts/product-catalog.mjs";
import {WORKSPACE_REPOSITORIES, workspacePath} from "./launcher/lib/workspace-layout.mjs";
const builds=await loadRuntimeBuildConfig(),rows=[];
for(const [game,entry] of Object.entries(builds.games)) {
  const product=PRODUCT_GAMES[game];
  for(const variant of ["normal","multiplayer"]) {
    if(!entry.variants[variant])continue;
    const repository=entry.variants[variant].workspaceRepository??entry.workspaceRepository;
    const classic=product.runtimeFileLayout!=="directory";
    let html,sourceEntry=null;
    if(classic)html="resources/shell.html";
    else {
      const managedHtml=workspacePath(repository,"web","sdl-runtime","managed.html");
      html=`web/sdl-runtime/${existsSync(managedHtml)?"managed.html":`${game}.html`}`;
      // TH09's packager publishes managed.mjs as Runtime shell.mjs; other
      // directory products use their declared shell.mjs source directly.
      sourceEntry=`web/sdl-runtime/${game==="th09"?"managed.mjs":"shell.mjs"}`;
      if(!existsSync(workspacePath(repository,...sourceEntry.split("/")))||
          !existsSync(workspacePath(repository,...html.split("/"))))
        throw Error(`${game}/${variant}: missing packaged Shell input`);
      if(!product.runtimeAssets.includes(`${game}.html`)||!product.runtimeAssets.includes("shell.mjs"))
        throw Error(`${game}/${variant}: Product Catalog does not declare its packaged entry`);
    }
    rows.push({id:`${game}-${variant}`,game,variant,repo:WORKSPACE_REPOSITORIES[repository],html,
      ...(classic?{classic:true}:{entry:sourceEntry})});
  }
}
process.stdout.write(JSON.stringify(rows));'''
    environment = {**os.environ, "EAGLER_WORKSPACE_ROOT": str(workspace)}
    result = subprocess.run(["node", "--input-type=module", "-e", script], cwd=workspace, env=environment,
                            capture_output=True, text=True, encoding="utf-8", check=True)
    variants = json.loads(result.stdout)
    if not variants:
        raise AssertionError("runtime-builds.json registered no normal or multiplayer Runtime variants")
    return variants


def bundle_launcher(launcher_root: Path) -> str:
    script = """import {build} from 'esbuild';
const result=await build({stdin:{resolveDir:process.cwd(),loader:'ts',contents:
  \"export {createPlayerKeyboard} from './app/services/player-keyboard.ts';export {createKeyboardPreferences} from './src/launcher/keyboard-preferences.mts';export {deliverRuntimeInput} from './src/launcher/touch-runtime-protocol.mts';\"},
  bundle:true,write:false,format:'iife',globalName:'ProductionKeyboard',platform:'browser'});
process.stdout.write(result.outputFiles[0].text);"""
    return subprocess.run(["node", "--input-type=module", "-e", script], cwd=launcher_root,
                          capture_output=True, text=True, encoding="utf-8", check=True).stdout


class RuntimeHandler(BaseHTTPRequestHandler):
    workspace: Path
    variant: dict
    native_module: str
    negative_control = False
    missing: list[str] = []

    def log_message(self, _format, *_args):
        pass

    def respond(self, body: bytes, content_type: str, status: int = 200):
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Cache-Control", "no-store")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        parsed = urlsplit(self.path)
        path = parsed.path.lstrip("/")
        if path == "__host.html":
            return self.respond(HOST_HTML.encode(), "text/html; charset=utf-8")
        if path == "__blank.html":
            return self.respond(b"<!doctype html><meta charset=utf-8>", "text/html; charset=utf-8")
        if path == "__test__/classic-module.js":
            return self.respond(CLASSIC_MODULE.encode(), "text/javascript; charset=utf-8")
        if path == "__test__/font.bin":
            return self.respond(GAME_BYTES, "application/octet-stream")

        parts = Path(path).parts
        owner_prefix = self.variant["repo"].replace("\\", "/") + "/"
        variant = self.variant if path.startswith(owner_prefix) else None
        if path.endswith("/__native-keyboard-sink.mjs") and variant:
            return self.respond(self.native_module.encode(), "text/javascript; charset=utf-8")
        if path.endswith((".wasm", ".data", ".bin", ".ttf", ".otf", ".ttc", ".ogg")):
            return self.respond(EMPTY_WASM if path.endswith(".wasm") else GAME_BYTES, "application/wasm" if path.endswith(".wasm") else "application/octet-stream")
        if path.endswith("/manifest.json") or path.endswith("/version.json"):
            entry = variant or {}
            game = entry.get("game", "th08")
            item = {"game": game, "protocol": PROTOCOL, "adapter": "sdl3-eagler", "profile": "multiplayer" if entry.get("variant") == "multiplayer" else "production",
                    "product": game + "mp" if entry.get("variant") == "multiplayer" else game,
                    "variant": entry.get("variant", "normal"), "builtAt": "2026-10-19T00:00:00.000Z", "version": "1.0.0-test",
                    "features": {"multiplayer": entry.get("variant") == "multiplayer", "thprac": False, "languages": False, "focusHitbox": False},
                    "execution": {"kind": "test-native-sink", "sha256": "a" * 64, "loaderSha256": "b" * 64, "architecture": "wasm32"},
                    "buildIdentity": "c" * 64}
            return self.respond(json.dumps(item).encode(), "application/json")
        if path.endswith("/resources.json"):
            game = (variant or {}).get("game", "th08")
            resources = [{"path": "/fonts/" + name, "url": "/__test__/font.bin", "bytes": len(GAME_BYTES)}
                         for name in FONT_FILES.get(game, [])]
            payload = {"schema": "eagler-sdl-resources/1", "game": game, "resources": resources}
            return self.respond(json.dumps(payload).encode(), "application/json")

        if not path or ".." in parts:
            return self.respond(b"not found", "text/plain", 404)
        source = self.workspace.joinpath(*parts)
        if not source.is_file() and len(parts) >= 3 and parts[-2] == "sdl-runtime":
            generated_candidates = [self.workspace / variant["repo"] / "portable" / "browser" / parts[-1],
                                    self.workspace / "common" / "browser" / parts[-1]] if variant else []
            source = next((candidate for candidate in generated_candidates if candidate.is_file()), source)
        if not source.is_file():
            self.missing.append(path)
            return self.respond(b"not found: " + path.encode(), "text/plain", 404)
        body = source.read_bytes()
        suffix = source.suffix.lower()
        if suffix in (".html",) and variant:
            text = body.decode("utf-8")
            if variant.get("classic"):
                # Use the Shell's own Emscripten template/bootstrap. The fixture
                # only supplies the generated runtime callback normally provided
                # by the compiled Emscripten JavaScript/WASM package.
                text = text.replace("{{{ SCRIPT }}}", '<script src="/__test__/classic-module.js"></script>', 1)
            if variant["game"] == "th09":
                text = text.replace('content="normal"', f'content="{variant["variant"]}"', 1)
            if variant["game"] == "th15":
                text = text.replace('src="managed.mjs"', 'src="shell.mjs"', 1)
            return self.respond(text.encode(), "text/html; charset=utf-8")
        is_packaged_entry = bool(variant and not variant.get("classic") and
                                 path == f"{variant['repo']}/web/sdl-runtime/shell.mjs")
        if is_packaged_entry:
            source = self.workspace / variant["repo"] / variant["entry"]
            body = source.read_bytes()
            text = body.decode("utf-8")
            imports = re.findall(r"import\s+createModule\s+from\s+['\"]\./[^'\"]+\.mjs['\"]\s*;", text)
            if len(imports) != 1:
                return self.respond(f"expected one native module import in {source}, found {len(imports)}".encode(), "text/plain", 500)
            replacement = "import createModule from './__native-keyboard-sink.mjs';"
            text = text.replace(imports[0], replacement, 1)
            if variant["game"] in ("th11", "th15"):
                marker = "/*TH11_BUILD_INFO*/" if variant["game"] == "th11" else "/*TH15_BUILD_INFO*/"
                start = text.find(marker)
                if start < 0:
                    return self.respond(f"missing {marker} in {source}".encode(), "text/plain", 500)
                end = text.find(";", start)
                build = {"version": "1.0.0-test", "completeGame": True}
                if variant["game"] == "th11":
                    build["multiplayer"] = variant["variant"] == "multiplayer"
                else:
                    build["multiplayer"] = variant["variant"] == "multiplayer"
                text = text[:start] + marker + json.dumps(build, separators=(",", ":")) + text[end:]
            if self.negative_control and variant["id"] == "th10-multiplayer":
                needle = "if(keyboard.mapNative(event,name==='keydown'))"
                if text.count(needle) != 1:
                    return self.respond(b"negative control could not find the Shell native-mapping branch exactly once", "text/plain", 500)
                text = text.replace(needle, "if(false)", 1)
            return self.respond(text.encode(), "text/javascript; charset=utf-8")

        if suffix == ".mjs":
            return self.respond(body, "text/javascript; charset=utf-8")
        if suffix == ".css":
            return self.respond(body, "text/css; charset=utf-8")
        if suffix == ".json":
            return self.respond(body, "application/json")
        return self.respond(body, "application/octet-stream")


def launch_payload(variant: dict, origin: str) -> dict:
    options = {"replayViewer": variant["variant"] == "multiplayer" and variant["game"] != "th09"}
    shared = [{"path": path, "url": "/__test__/font.bin", "bytes": len(GAME_BYTES)}
              for path in ("/msgothic.ttc", "/unifont.otf")]
    return {"protocol": PROTOCOL, "game": variant["game"], "epoch": EPOCH,
            "command": "configure", "language": "lang_ja", "music": "none", "options": options,
            "sharedResources": shared, "runtimeResources": [], "resources": []}


def run_variant(browser, launcher_bundle: str, workspace: Path, launcher_root: Path,
                variant: dict, negative_control=False) -> dict:
    RuntimeHandler.workspace = workspace
    RuntimeHandler.variant = variant
    RuntimeHandler.native_module = (launcher_root / "tests" / "browser" / "keyboard-native-sink.mjs").read_text(encoding="utf-8")
    RuntimeHandler.negative_control = negative_control
    RuntimeHandler.missing = []
    server = ThreadingHTTPServer(("127.0.0.1", 0), RuntimeHandler)
    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()
    origin = f"http://127.0.0.1:{server.server_port}"
    page = browser.new_page()
    page_errors = []
    browser_errors = []
    page.on("pageerror", lambda error: page_errors.append(str(error)))
    page.on("console", lambda message: browser_errors.append(message.text) if message.type == "error" else None)
    try:
        page.goto(origin + "/__host.html")
        page.add_script_tag(content=launcher_bundle)
        html_path = f"/{variant['repo']}/{variant['html']}"
        params = f"runtimeEpoch={EPOCH}&runtimeVariant={variant['variant']}&managedData=1&gameGeneration=test-generation&hosted=1"
        page.evaluate("url=>document.querySelector('#runtime').src=url", html_path + "?" + params)
        frame = page.frame_locator("#runtime")
        try:
            page.wait_for_function("() => runtimeEvents.some(message => message?.event === 'ready')", timeout=20000)
        except Exception as error:
            frames = [{"url": item.url, "body": item.locator("body").inner_text(timeout=1000) if item.url != "about:blank" else ""} for item in page.frames]
            raise AssertionError(f"{variant['id']} Shell did not become ready: {error}; pageErrors={page_errors}; consoleErrors={browser_errors}; missing={RuntimeHandler.missing}; frames={frames}") from error
        if variant["game"] == "th09":
            actual_variant = frame.locator('meta[name="eagler-runtime-variant"]').get_attribute("content")
            assert actual_variant == variant["variant"], (variant["id"], "TH09 packaged Shell variant identity mismatch", actual_variant)
        page.evaluate("""game=>{
          const frame=document.querySelector('#runtime'),target=frame.contentWindow,epoch=Number(new URL(target.location.href).searchParams.get('runtimeEpoch'));
          const context={target,targetOrigin:location.origin,protocol:'eagler-touhou/1',game,epoch,launched:true,ready:true,spectator:false};
          const runtime={getInputContext:()=>context,getMidiEventContext:()=>({target,document:target.document,game,epoch,music:'none'}),
            getSnapshot:()=>({launched:true,ready:true,firstFrame:false}),subscribe:()=>()=>{},
            postInput:(command,body)=>ProductionKeyboard.deliverRuntimeInput(context,{...body,protocol:context.protocol,game,epoch,command})};
          window.keyboardPreferences=ProductionKeyboard.createKeyboardPreferences(null);
          window.keyboardOwner=ProductionKeyboard.createPlayerKeyboard({runtime,window,document,frame,preferences:keyboardPreferences,playerOpen:()=>true,
            launcherOwnsTarget:target=>!!target?.closest?.('input,textarea,select,button'),releaseHeldTouchFire(){},cancelTouchFunction(){},
            toggleFullscreen(){},isPlayerFullscreen:()=>true,android:false});
          window.__runtimeContext=context;
        }""", variant["game"])

        command_id = f"{variant['id']}-configure"
        page.evaluate("""({payload,request})=>{
          const frame=document.querySelector('#runtime');frame.contentWindow.postMessage({...payload,request},location.origin);
        }""", {"payload": launch_payload(variant, origin), "request": command_id})
        try:
            page.wait_for_function("request => runtimeEvents.some(message => message?.request === request && message.ok === true)", arg=command_id, timeout=15000)
        except Exception as error:
            raise AssertionError(f"{variant['id']} configure command failed: events={page.evaluate('runtimeEvents')}; pageErrors={page_errors}; consoleErrors={browser_errors}") from error
        command_id = f"{variant['id']}-launch"
        page.evaluate("""({game,request})=>{
          const c=window.__runtimeContext; c.target.postMessage({protocol:c.protocol,game,epoch:c.epoch,command:'launch',request},location.origin);
        }""", {"game": variant["game"], "request": command_id})
        try:
            page.wait_for_function("request => runtimeEvents.some(message => message?.request === request && message.ok === true)", arg=command_id, timeout=30000)
        except Exception as error:
            raise AssertionError(f"{variant['id']} launch command failed: events={page.evaluate('runtimeEvents')}; pageErrors={page_errors}; consoleErrors={browser_errors}") from error

        # Parent messages with a bad product identity and a foreign source must
        # be rejected by the real Shell listener before reaching native input.
        page.evaluate("""game=>{
          const c=window.__runtimeContext;for(const [suffix,patch] of [['wrong-game',{game:game+'-wrong'}],['wrong-epoch',{epoch:c.epoch+1}],['wrong-protocol',{protocol:'wrong/1'}]])
            c.target.postMessage({protocol:c.protocol,game:c.game,epoch:c.epoch,command:'keyboard',code:'KeyZ',key:'z',keyCode:90,down:true,request:'reject-'+suffix,...patch},location.origin);
          const foreign=document.querySelector('#foreign').contentWindow;
          foreign.postMessage({protocol:c.protocol,game:c.game,epoch:c.epoch,command:'keyboard',code:'KeyZ',key:'z',keyCode:90,down:true,request:'reject-foreign'},location.origin);
        }""", variant["game"])
        page.wait_for_timeout(80)
        rejections = page.evaluate("""()=>runtimeEvents.filter(message=>String(message?.request||'').startsWith('reject-')).map(message=>message.request)""")
        assert not rejections, (variant["id"], "Shell acknowledged an invalid protocol/game/epoch/source", rejections)
        assert read_held(frame, variant) == [], (variant["id"], "invalid identity changed the native input sink")

        # Physical Chromium input goes to the focused real child document. The
        # Shell listener must synchronously call the Launcher-owned map port.
        frame.locator("canvas").click()
        page.keyboard.down("w")
        if negative_control:
            assert_native_held(frame, variant, ["ArrowUp"], "WASD W must reach the native ArrowUp sink")
        else:
            assert_native_held(frame, variant, [] if variant.get("classic") else ["KeyW"], "default arrows layout leaves W unmapped")
            page.keyboard.up("w")
            assert_native_held(frame, variant, [], "physical W release must reach the native sink")

            page.evaluate("keyboardPreferences.selectLayout('wasd')")
            page.keyboard.down("w")
            assert_native_held(frame, variant, ["ArrowUp"], "WASD W must reach the native ArrowUp sink")
            page.keyboard.up("w")
            assert_native_held(frame, variant, [], "remapped physical W release must reach the native sink")

            page.keyboard.down("j")
            assert_native_held(frame, variant, ["KeyZ"], "Launcher J remapping must reach the native KeyZ sink")
            page.keyboard.up("j")
            assert_native_held(frame, variant, [], "Launcher remapped key-up must clear native KeyZ")

            page.keyboard.down("k")
            assert_native_held(frame, variant, ["KeyX"], "Launcher K remapping must reach the native KeyX sink")
            page.evaluate("keyboardPreferences.selectLayout('arrows')")
            assert_native_held(frame, variant, [], "changing layout while held must clear the native sink")
            page.keyboard.up("k")

            page.evaluate("keyboardPreferences.selectLayout('wasd')")
            page.keyboard.down("w")
            assert_native_held(frame, variant, ["ArrowUp"], "WASD mapping must recover after layout reset")
            page.locator("#host").focus()
            page.wait_for_function("() => document.querySelector('#runtime').contentWindow.__eaglerNativeKeyboardSink?.held().length === 0 || document.querySelector('#runtime').contentWindow.Module?.eaglerControls?.keyboardBits === 0", timeout=3000)
            assert_native_held(frame, variant, [], "real focus loss must clear the held native key")
            page.keyboard.up("w")
        assert not page_errors, (variant["id"], "Runtime Shell raised browser page errors", page_errors)
        result = {"variant": variant["id"], "shell": "production-source", "browser": "Chromium", "identityRejection": "PASS",
                  "launcherMapping": "PASS" if not negative_control else "EXPECTED_FAILURE", "jsExportSink": "PASS" if not negative_control else "EXPECTED_FAILURE"}
        return result
    finally:
        page.close()
        server.shutdown()
        server.server_close()
        thread.join(timeout=2)
        if page_errors:
            # Do not hide initialization/launch exceptions when a later browser
            # assertion also fails.
            print(json.dumps({"variant": variant["id"], "pageErrors": page_errors}))


def read_held(frame, variant):
    if variant.get("classic"):
        bits = frame.locator("body").evaluate("() => Module.eaglerControls.keyboardBits || 0")
        keys = []
        if bits & 16: keys.append("ArrowUp")
        if bits & 32: keys.append("ArrowDown")
        if bits & 64: keys.append("ArrowLeft")
        if bits & 128: keys.append("ArrowRight")
        if bits & 1: keys.append("KeyZ")
        if bits & 2: keys.append("KeyX")
        if bits & 4: keys.append("ShiftLeft")
        if bits & 8: keys.append("KeyC")
        return sorted(keys)
    return frame.locator("body").evaluate("() => window.__eaglerNativeKeyboardSink?.held() ?? []")


def assert_native_held(frame, variant, expected, message):
    actual = read_held(frame, variant)
    assert actual == sorted(expected), (variant["id"], message, {"expected": sorted(expected), "actual": actual})


def main() -> int:
    parser = argparse.ArgumentParser(description="Run real Runtime Shell keyboard behavior in Chromium.")
    parser.add_argument("--workspace", type=Path, default=DEFAULT_WORKSPACE, help="monorepo root containing ports/ and launcher/")
    parser.add_argument("--variants", nargs="+", help="diagnostic subset; the default runs every registered normal/MP variant")
    parser.add_argument("--negative-control", action="store_true", help="sever TH10 MP Shell mapNative wiring and require the WASD assertion to fail")
    args = parser.parse_args()
    workspace = args.workspace.resolve()
    launcher_root = workspace / "launcher"
    if not (workspace / "ports" / "th08").is_dir() or not (launcher_root / "package.json").is_file():
        parser.error(f"workspace does not look like the monorepo root: {workspace}")
    variants = runtime_variants(workspace)
    by_id = {item["id"]: item for item in variants}
    if len(by_id) != len(variants):
        parser.error("runtime-builds.json produced duplicate variant identities")
    unknown = sorted(set(args.variants or []) - set(by_id))
    if unknown:
        parser.error(f"unknown Runtime variant(s): {', '.join(unknown)}")
    selected = [by_id[name] for name in args.variants] if args.variants else variants
    if args.negative_control:
        if "th10-multiplayer" not in by_id:
            parser.error("TH10 multiplayer variant is required for the Shell wiring negative control")
        selected = [by_id["th10-multiplayer"]]
    launcher = bundle_launcher(launcher_root)
    results = []
    with sync_playwright() as playwright:
        browser = playwright.chromium.launch(headless=True)
        try:
            for variant in selected:
                try:
                    results.append(run_variant(browser, launcher, workspace, launcher_root, variant, args.negative_control))
                except AssertionError as error:
                    if args.negative_control and "WASD W must reach the native ArrowUp sink" in str(error):
                        print(json.dumps({"keyboardMappingBrowser": results, "negativeControl": "EXPECTED_FAILURE", "evidence": str(error)}))
                        return 0
                    raise
        finally:
            browser.close()
    if args.negative_control:
        raise AssertionError("negative control unexpectedly passed; Shell disconnect did not break the browser assertion")
    print(json.dumps({"keyboardMappingBrowser": results, "variantsPassed": len(results), "variantsRequired": len(variants)}))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
