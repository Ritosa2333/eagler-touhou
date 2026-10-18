"""TH06/TH07 real-WASM spectator lifecycle regression over a private relay.

Checks repeated receiver/document entry, ordinary playback pacing, cancelling a
seek at the actual cursor, local pause, and a healthy source with no game frames.
Source RAF suspension is a test-only transport-idle fixture, not an in-game
Pause-key or public/device acceptance claim. No simulation input is rewritten.
"""
from __future__ import annotations

import hashlib
import importlib.util
import json
import os
from pathlib import Path
import subprocess
import sys
import time
import urllib.parse

from playwright.sync_api import sync_playwright

SPEC = importlib.util.spec_from_file_location(
    "stage_acceptance", Path(__file__).with_name("netplay-spectator-stage2-browser.py"))
support = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(support)

INIT = """(() => {
  globalThis.__spectatorPlaybackReport=null;
  addEventListener('message',event=>{
    const message=event.data||{};
    if(event.source===document.getElementById('runtime')?.contentWindow&&event.origin===location.origin&&
       message.protocol==='eagler-touhou/1'&&message.epoch===1&&message.event==='runtime-info'&&
       message.spectatorPlayback?.version===1)globalThis.__spectatorPlaybackReport=message.spectatorPlayback;
  });
  globalThis.__spectatorLifecycleHold = false;
  const raf = requestAnimationFrame.bind(globalThis);
  globalThis.requestAnimationFrame = callback => raf(function next(time) {
    if (globalThis.__spectatorLifecycleHold) raf(next); else callback(time);
  });
})();"""


def state(page, game):
    return page.evaluate("""game => {
      const r=document.getElementById('runtime')?.contentWindow,m=r?.Module;
      const playback=globalThis.__spectatorPlaybackReport;
      const transport=r?.__eaglerPeerTransport;
      return {playback,frame:Number(r?.__eaglerNetplayLanFrame||0),
        failed:!!(r?.__eaglerNetplayFailed||transport?.failed),
        error:r?.__eaglerNetplayError||transport?.error||'',
        open:transport?.relay?.readyState===1,
        hostFailure:globalThis.__spectatorSmokeFailure||'',
        launched:!!globalThis.__spectatorSmokeLaunched,
        runtimeDocument:r?.document?.readyState,
        runtimeStatus:r?.document?.getElementById('status')?.textContent||'',
        runtimeCalledRun:!!m?.calledRun};
    }""", game)


def wait(page, game, predicate, label, timeout=40):
    deadline=time.monotonic()+timeout
    while time.monotonic()<deadline:
        value=state(page,game)
        if value['failed'] or value['hostFailure']:
            raise AssertionError(f'{label}: {value}')
        if predicate(value):return value
        page.wait_for_timeout(40)
    raise AssertionError(f'{label} timed out: {state(page,game)}')


def command(page, game, action, frame=None):
    return support.spectator_command(page,game,state(page,game)['playback']['run'],action,frame)


def pacing(page, game, label):
    before=state(page,game)['playback'];start=time.monotonic()
    page.wait_for_timeout(1600)
    after=state(page,game);elapsed=time.monotonic()-start
    assert not after['failed'],after
    ticks=after['playback']['position']-before['position']
    assert 0<ticks<=elapsed*75+3,(label,ticks,elapsed,before,after)
    return {'seconds':elapsed,'ticks':ticks,'framesPerSecond':ticks/elapsed}


def run(game):
    root=support.WORKSPACE_ROOT/(game+'-eagler')
    build=os.environ.get(game.upper()+'_SPECTATOR_RUNTIME_BUILD','build-web-release-multiplayer')
    wasm=root/build/(game+'.wasm')
    expected=hashlib.sha256(wasm.read_bytes()).hexdigest()
    http_port,relay_port=support.free_port(),support.free_port()
    room=f'{game}mp-lifecycle-{int(time.time()*1000)}'
    relay_url=f'ws://127.0.0.1:{relay_port}'
    host=f'http://127.0.0.1:{http_port}/tests/netplay-spectator-browser-host.html'
    env={**os.environ,'EAGLER_NETPLAY_RELAY_HOST':'127.0.0.1',
      'EAGLER_NETPLAY_RELAY_PORT':str(relay_port),'EAGLER_NETPLAY_PUBLIC_NICKNAMES':'0',
      'EAGLER_NETPLAY_STUN_URLS':'','EAGLER_NETPLAY_RTC_TIMEOUT_MS':'1000'}
    hidden=subprocess.CREATE_NO_WINDOW if sys.platform=='win32' else 0
    http=subprocess.Popen([sys.executable,'-m','http.server',str(http_port),'--bind','127.0.0.1'],
      cwd=root,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,creationflags=hidden)
    relay=subprocess.Popen(['node',str(support.RELAY_SCRIPT)],cwd=support.HOST_ROOT,env=env,
      stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,creationflags=hidden)
    result={'game':game,'runtimeBuild':build,'wasmSha256':expected,'entries':[],'passed':False}
    try:
      support.wait_http(host);support.wait_relay(relay)
      with sync_playwright() as pw:
        browser=pw.chromium.launch(headless=True,args=['--disable-background-timer-throttling',
          '--disable-renderer-backgrounding','--disable-backgrounding-occluded-windows',
          '--use-gl=angle','--use-angle=d3d11','--enable-gpu','--ignore-gpu-blocklist'])
        try:
          setup=browser.new_context().new_page();setup.goto(host.rsplit('/',2)[0])
          support.admit_lobby(setup,relay_url,room,'lifecycle_viewer',2)
          sources=[]
          def open_runtime(player=None):
            context=browser.new_context();context.add_init_script('delete globalThis.RTCPeerConnection;'+INIT)
            page=context.new_page()
            page.on('pageerror',lambda error:result.setdefault('pageErrors',[]).append(str(error)))
            page.on('response',lambda response:result.setdefault('httpFailures',[]).append(
              {'url':response.url,'status':response.status}) if response.status>=400 else None)
            page.on('crash',lambda _:result.setdefault('crashedPages',[]).append(page.url))
            member=f'member_p{player+1}_00000{player+1}' if player is not None else 'member_view_0003'
            query={'runtimeBuild':build,'frames':12000,'players':2,'player':player or 0,
              'relay':f'{relay_url}/?room={room}&run=1&member={member}'}
            if player is None:query.update(spectator=1,spectatorId='lifecycle_viewer')
            page.goto(host+'?'+urllib.parse.urlencode(query),wait_until='load',timeout=60000)
            wait(page,game,lambda s:s['launched'],'Runtime launch')
            served=page.evaluate('globalThis.__spectatorRuntimeArtifact')
            assert served['wasmSha256']==expected,served
            return page
          for player in range(2):sources.append(open_runtime(player))
          wait(sources[0],game,lambda s:s['frame']>=360,'source history')
          viewer=None
          for attempt in range(3):
            if viewer is not None:
              viewer.context.close()
              # A receiver may be asynchronously retiring; explicit role
              # departure/readmission must release it before the next entry.
              setup.evaluate("""() => {const ws=__spectatorLobbySockets.at(-1);
                ws.send(JSON.stringify({type:'leave-spectator'}));ws.send(JSON.stringify({type:'spectate'}));}""")
              setup.wait_for_timeout(100)
            viewer=open_runtime()
            value=wait(viewer,game,lambda s:s['playback'] and not s['playback']['seeking'],'entry ready')
            assert not value['playback']['following'],value
            result['entries'].append({'attempt':attempt,'position':value['playback']['position'],
              'received':value['playback']['received']})
          result['initialPacing']=pacing(viewer,game,'ordinary entry')
          target=state(viewer,game)['playback']['edge']
          command(viewer,game,'seek',target);command(viewer,game,'pause')
          stopped=state(viewer,game)['playback'];assert stopped['paused'] and not stopped['seeking'],stopped
          viewer.wait_for_timeout(350)
          assert state(viewer,game)['playback']['position']==stopped['position']
          result['seekPause']={'target':target,'stoppedAt':stopped['position']}
          viewer.wait_for_timeout(17000)
          paused=state(viewer,game);assert not paused['failed'] and paused['open'],paused
          assert paused['playback']['position']==stopped['position'],paused
          result['localPauseSeconds']=17
          command(viewer,game,'play');result['resumedPacing']=pacing(viewer,game,'historical resume')
          command(viewer,game,'live')
          live=wait(viewer,game,lambda s:s['playback']['following'] and not s['playback']['seeking'],'live target',20)
          result['followingPacing']=pacing(viewer,game,'following after live')
          for source in sources:source.evaluate("document.getElementById('runtime').contentWindow.__spectatorLifecycleHold=true")
          viewer.wait_for_timeout(18000)
          idle=state(viewer,game);assert not idle['failed'] and idle['open'],idle
          result['sourceIdleSeconds']=18;result['sourceIdleState']=idle
          # Reenter after substantial history, including a real Boss bookmark
          # when the title has published one. Initial reconstruction has a
          # fixed destination and must finish before ordinary playback starts.
          node_frame=max(node['frame'] for node in idle['playback']['nodes'])
          viewer.context.close()
          setup.evaluate("""() => {const ws=__spectatorLobbySockets.at(-1);
            ws.send(JSON.stringify({type:'leave-spectator'}));ws.send(JSON.stringify({type:'spectate'}));}""")
          setup.wait_for_timeout(100)
          entry_started=time.monotonic();viewer=open_runtime()
          restored=wait(viewer,game,lambda s:s['playback'] and not s['playback']['seeking'],'historical node reentry',20)
          entry_seconds=time.monotonic()-entry_started
          assert restored['playback']['position']>=node_frame,restored
          assert not restored['playback']['following'],restored
          result['historicalNodeReentry']={'seconds':entry_seconds,'nodeFrame':node_frame,
            'position':restored['playback']['position'],'received':restored['playback']['received'],
            'pacing':pacing(viewer,game,'post-bootstrap historical playback')}
          result['passed']=True
        finally:browser.close()
    except Exception as error:
      result['error']=str(error)
      raise
    finally:
      relay.terminate();http.terminate()
      for process in [relay,http]:
        try:process.wait(timeout=5)
        except subprocess.TimeoutExpired:process.kill();process.wait()
      output=support.WORKSPACE_ROOT/'dist/audits/spectator-lifecycle-20261017'
      output.mkdir(parents=True,exist_ok=True)
      suffix=os.environ.get('SPECTATOR_AUDIT_SUFFIX','')
      if suffix and not suffix.replace('-','').isalnum():raise ValueError('invalid audit suffix')
      (output/(game+'-native'+('-'+suffix if suffix else '')+'.json')).write_text(json.dumps(result,indent=2),encoding='utf-8')
    print(json.dumps(result),flush=True)


if __name__=='__main__':
    for game in sys.argv[1:] or ['th06','th07']:
        if game not in ['th06','th07']:raise SystemExit('expected th06 or th07')
        run(game)
