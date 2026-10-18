#!/usr/bin/env python3
from __future__ import annotations
import functools, http.server, socket, socketserver, threading, time
from pathlib import Path
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[1]
UNIFONT = ROOT.parent / "dependencies/unifont-15.1.05/unifont-15.1.05.otf"

class Handler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path.split("?", 1)[0] == "/__unifont.otf":
            data = UNIFONT.read_bytes(); self.send_response(200)
            self.send_header("Content-Type", "font/otf"); self.send_header("Content-Length", str(len(data)))
            self.end_headers(); self.wfile.write(data); return
        super().do_GET()
    def log_message(self, _format, *_args): pass

def port():
    with socket.socket() as sock: sock.bind(("127.0.0.1",0)); return sock.getsockname()[1]

def main() -> int:
    if not UNIFONT.is_file(): raise RuntimeError("unifont fixture missing")
    p = port(); handler = functools.partial(Handler, directory=str(ROOT))
    with socketserver.TCPServer(("127.0.0.1", p), handler) as server:
        threading.Thread(target=server.serve_forever, daemon=True).start()
        try:
            with sync_playwright() as pw:
                browser = pw.chromium.launch(headless=True, args=["--autoplay-policy=no-user-gesture-required"])
                page = browser.new_page(viewport={"width":960,"height":720}); errors=[]
                page.on("pageerror", lambda error: errors.append(str(error)))
                page.goto(f"http://127.0.0.1:{p}/tests/ordinary-stage-browser-host.html", wait_until="load", timeout=30000)
                deadline=time.time()+45; state={}
                while time.time()<deadline:
                    state=page.evaluate("""() => { const w=document.querySelector('#runtime')?.contentWindow; return {
                      failure:String(globalThis.__ordinaryStageFailure||''), prepared:!!w?.__eaglerOrdinaryStageVisualPrepared,
                      stage:Number(w?.__eaglerOrdinaryStageVisualStage||0), callbacks:Number(w?.__eaglerDebugRenderCallbacks||0),
                      health:(globalThis.__ordinaryStageHealth||[]).slice(-4), netplay:!!w?.__eaglerNetplayLanActive,
                      mode:String(w?.Module?.eaglerOptions?.netplayMode||'') }; }""")
                    if errors or state["failure"]: break
                    if state["prepared"] and state["stage"]==4 and state["callbacks"]>=240 and len(state["health"])>=2: break
                    time.sleep(.1)
                browser.close()
        finally: server.shutdown()
    if errors or state.get("failure") or not state.get("prepared") or state.get("stage") != 4 or state.get("callbacks",0) < 240 or state.get("netplay") or state.get("mode"):
        raise RuntimeError(f"ordinary Stage4 smoke failed: state={state} errors={errors}")
    print(f"TH07 ordinary Stage4 browser smoke: PASS callbacks={state['callbacks']} health={state['health']}")
    return 0

if __name__ == "__main__": raise SystemExit(main())
