"""Bounded, resumable full-player-Replay audit using the real SDL tick path.

Every logical tick and authored Draw runs. Only diagnostic alpha sweeps are
sampled; reports explicitly record that stride and the unobserved categories.
No input edits, difficulty edits, invulnerability or Replay fast-forward mode.
"""
import argparse
import json
import time
import hashlib
from pathlib import Path
from playwright.sync_api import sync_playwright

ROOT = Path(__file__).resolve().parents[2]
INPUT = ROOT / 'artifacts/presentation-lab/input/campaign'

INITIALIZE = r'''async ({bytes,fixture,mode,stride,startStage,sparseObserver,sampleFrames,regularScan,negativeControl}) => {
  await presentationLab.boot({replayBytes:bytes});
  const c=presentationLab.controller,core=c.core,r=c.runtime;
  for(let i=0;i<12&&core.status(r.app,9)!==4;i++)c.press('ArrowDown');
  if(core.status(r.app,9)!==4)throw Error('Replay menu not reached');
  c.press('KeyZ');c.press('KeyZ');
  if(startStage!==null){for(let i=0;i<10&&core.status(r.app,9)!==startStage;i++)c.press('ArrowDown');if(core.status(r.app,9)!==startStage)throw Error('Requested original Replay stage not present');}
  const navigation=[];
  for(let i=0;i<4&&core.status(r.app,0)!==2;i++){c.press('KeyZ');navigation.push([core.status(r.app,0),core.status(r.app,8),core.status(r.app,9)]);}
  if(core.status(r.app,0)!==2||!(c.trace()[4]&8))throw Error('Real Replay did not start: '+JSON.stringify(navigation));
  c.clearKeys();c.freeze();
  const module=await import('/analyzer.mjs');
  const scene=()=>Array.from(new Int32Array(core.memory.buffer,core.audit_scene(),16));
  const spell=()=>{const p=core.audit_spell_name(),b=new Uint8Array(core.memory.buffer,p,48);return new TextDecoder('shift-jis').decode(b.subarray(0,b.indexOf(0)<0?48:b.indexOf(0)));};
  const targeted=Array.isArray(sampleFrames)&&sampleFrames.length>0;
  regularScan=!!regularScan||!targeted;
  const sparse=(!!sparseObserver&&stride>2)||targeted;
  if(mode==='trace'||sparse)core.audit_enable(0);
  window.campaign={c,core,r,mode,stride,sparse,targeted,regularScan,negativeControl:!!negativeControl,sampleFrames:sampleFrames||[],auditOn:mode==='audit'&&!sparse,fixture,module,scene,spell,steps:0,done:false,stopReason:null,lastStage:null,lastSpell:null,lastTrace:c.trace(),lastScene:scene(),windows:0,stageMaxima:{},lastGameplayTrace:c.trace()};
  const gl=r.Module.canvas.getContext('webgl2'),ext=gl.getExtension('WEBGL_debug_renderer_info');
  return {build:presentationLab.identity,navigation,initial:scene(),renderer:ext?gl.getParameter(ext.UNMASKED_RENDERER_WEBGL):gl.getParameter(gl.RENDERER)};
}'''

CHUNK = r'''async ({count,maxTicks}) => {
  const q=campaign,{c,core,r}=q,checkpoints=[],windows=[],events=[];let advanced=0;
  for(let i=0;i<count&&!q.done;i++){
    // All authored draws still run. Capture the two consecutive reference
    // draws needed for a sweep, instead of retaining discarded observations.
    if(q.mode==='audit'&&q.targeted&&!q.auditOn){
      const before=q.scene();
      if(q.sampleFrames.some(t=>before[1]===t.stage&&before[4]>=t.frame-2&&before[4]<t.frame)){core.audit_enable(1);q.auditOn=true;}
      else if(q.regularScan&&q.sparse&&q.steps%q.stride===q.stride-2){core.audit_enable(1);q.auditOn=true;}
    }else if(q.mode==='audit'&&q.sparse&&!q.targeted&&q.steps%q.stride===q.stride-2){core.audit_enable(1);q.auditOn=true;}
    const code=core.sdl_loop_tick(r.app,1/60,17);q.steps++;advanced++;
    const s=q.scene(),trace=c.trace();
    if(s[0]===2){
      if(!s[11])q.stageMaxima[s[1]]=Math.max(q.stageMaxima[s[1]]||0,s[4]);q.lastGameplayTrace=trace;
      if(s[1]!==q.lastStage){events.push({type:'stage',step:q.steps,scene:s,trace});q.lastStage=s[1];}
      const spellId=(s[7]&1)?s[8]:-1;
      if(spellId!==q.lastSpell){events.push({type:'spell',step:q.steps,scene:s,name:spellId>=0?q.spell():'',trace});q.lastSpell=spellId;}
    }
    const checkpoint=q.steps%60===0;
    if(checkpoint)checkpoints.push({step:q.steps,scene:s,trace,state:c.state()});
    if(code||s[0]!==2){q.done=true;q.stopReason=code?'tick-result-'+code:'returned-to-scene-'+s[0];q.lastTrace=trace;q.lastScene=s;break;}
    q.lastTrace=trace;q.lastScene=s;
    const targetHit=q.targeted&&q.sampleFrames.some(t=>s[1]===t.stage&&s[4]===t.frame);
    const regularHit=q.regularScan&&q.steps%q.stride===0;
    if(q.mode==='audit'&&(targetHit||regularHit)&&core.audit_gate()&&!s[11]){
      const report=c.sweep({label:q.fixture+'/stage-'+s[1]+'/frame-'+s[4],negativeControl:q.negativeControl});q.windows++;
      const changed=JSON.stringify(report.traceBefore)!==JSON.stringify(report.traceAfter);
      const context={step:q.steps,stageIndex:s[1],stageReplayFrame:s[4],stageSeconds:s[4]/60,spellNumber:(s[7]&1)?s[8]:-1,spellName:(s[7]&1)?q.spell():'',scene:s};
      const compact=q.module.compactReport(report,{maxObjects:q.targeted?8192:8,images:false});
      if(typeof core.audit_effect_sample==='function'){
        compact.effectDiagnostics=report.objects.filter(o=>o.ownerId===7&&o.severity>=4).slice(0,4).map(o=>({
          key:o.key,anm:o.anm,script:o.script,part:o.part,
          sidecar:Array.from(new Float32Array(core.memory.buffer,core.audit_effect_sample(o.object),16)),
          calculationEpoch:core.audit_calculation_epoch?.()??0,
        }));
      }
      if(typeof core.audit_player_bomb_sample==='function'){
        compact.playerBombDiagnostics=report.objects.filter(o=>o.ownerId===16&&o.severity>=4).slice(0,4).map(o=>({
          key:o.key,anm:o.anm,script:o.script,part:o.part,
          sidecar:Array.from(new Float32Array(core.memory.buffer,core.audit_player_bomb_sample(o.object,o.part),24)),
          calculationEpoch:core.audit_calculation_epoch?.()??0,
        }));
      }
      if(typeof core.audit_enemy_sample==='function'){
        compact.enemyDiagnostics=report.objects.filter(o=>o.ownerId===8&&o.severity>=4).slice(0,4).map(o=>({
          key:o.key,anm:o.anm,script:o.script,part:o.part,
          sidecar:Array.from(new Float32Array(core.memory.buffer,core.audit_enemy_sample(o.object,o.part),28)),
          calculationEpoch:core.audit_calculation_epoch?.()??0,
        }));
      }
      if(typeof core.audit_ascii_sample==='function'){
        compact.asciiDiagnostics=report.objects.filter(o=>o.ownerId===15&&o.severity>=4).slice(0,8).map(o=>({
          key:o.key,anm:o.anm,script:o.script,part:o.part,
          sidecar:Array.from(new Float32Array(core.memory.buffer,core.audit_ascii_sample(o.object),20)),
          calculationEpoch:core.audit_calculation_epoch?.()??0,
        }));
      }
      // Identity and complete build manifest is in the session header, not
      // repeated thousands of times; full issueGroups/coverage remain intact.
      delete compact.build;compact.context=context;
      windows.push(compact);
      if(report.purityStatus==='fail'||changed){q.done=true;q.stopReason='presentation-state-change';break;}
      if(!report.valid){q.done=true;q.stopReason='invalid-audit-window';break;}
    }
    if(q.mode==='audit'&&q.sparse&&(targetHit||regularHit)){core.audit_enable(0);q.auditOn=false;}
    if(q.steps>=maxTicks){q.done=true;q.stopReason='max-ticks';}
    if(i%120===119)await new Promise(resolve=>setTimeout(resolve,0));
  }
  return {advanced,steps:q.steps,done:q.done,stopReason:q.stopReason,lastScene:q.lastScene,lastTrace:q.lastTrace,lastGameplayTrace:q.lastGameplayTrace,stageMaxima:q.stageMaxima,windows,checkpoints,events,totalWindows:q.windows};
}'''


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--url', default='http://127.0.0.1:8133/')
    parser.add_argument('--tag', required=True)
    parser.add_argument('--fixtures', nargs='+', default=['lunatic-border','lunatic-magic','extra-border'])
    parser.add_argument('--stride', type=int, default=60)
    parser.add_argument('--max-ticks', type=int, default=240000)
    parser.add_argument('--mode', choices=['audit','trace'], default='audit')
    parser.add_argument('--start-stage', type=int)
    parser.add_argument('--hardware', action='store_true')
    parser.add_argument('--resume', action='store_true')
    parser.add_argument('--sparse-observer', action='store_true')
    parser.add_argument('--sample-frame', action='append', default=[], metavar='STAGE:FRAME',
                        help='Only alpha-sweep exact Replay stage frames; may be repeated')
    parser.add_argument('--with-regular-scan', action='store_true',
                        help='With --sample-frame, also keep the ordinary stride-based audit')
    parser.add_argument('--negative-control', action='store_true',
                        help='Diagnostic only: hold interpolation at current values during sampled presentation redraws')
    parser.add_argument('--require-complete', action='store_true')
    args = parser.parse_args()
    assert 1 <= args.stride <= 600 and 1 <= args.max_ticks <= 300000
    assert args.start_stage is None or 0 <= args.start_stage <= 8
    sample_frames = []
    for value in args.sample_frame:
        try:
            stage, frame = map(int, value.split(':', 1))
        except ValueError as exc:
            raise ValueError('--sample-frame expects STAGE:FRAME') from exc
        if not 0 <= stage <= 8 or not 0 <= frame <= 300000:
            raise ValueError('sample frame outside bounded TH08 range')
        sample_frames.append({'stage': stage, 'frame': frame})
    if not args.tag.replace('-','').replace('_','').isalnum():
        raise ValueError('Safe report tag required')
    corpus = json.loads((INPUT / 'manifest.json').read_text(encoding='utf-8'))
    fixtures = {f['id']: f for f in corpus['fixtures']}
    output = ROOT / 'artifacts/presentation-lab/campaign' / args.tag
    output.mkdir(parents=True, exist_ok=True)
    with sync_playwright() as p:
        browser = p.chromium.launch(headless=True, args=['--enable-gpu','--use-gl=angle','--use-angle=d3d11'] if args.hardware else ['--enable-unsafe-swiftshader'])
        try:
            for name in args.fixtures:
                metadata = fixtures[name]
                directory = output / name
                directory.mkdir(exist_ok=True)
                if args.resume and (directory/'summary.json').exists():
                    saved = json.loads((directory/'summary.json').read_text(encoding='utf-8'))
                    if saved.get('completed'):
                        print(json.dumps({'skippedCompleted':name}),flush=True)
                        continue
                context = browser.new_context(viewport={'width':1100,'height':900}, service_workers='block')
                page = context.new_page()
                page.set_default_timeout(180000)
                errors = []
                page.on('pageerror', lambda e: errors.append(str(e)))
                started = time.monotonic()
                page.goto(args.url, wait_until='load')
                page.wait_for_function('window.presentationLab !== undefined')
                # User activation before loading the manual fixture Runtime.
                page.locator('#boot').click()
                page.wait_for_function('!!window.presentationLab?.controller')
                setup = page.evaluate(INITIALIZE, {'bytes':list((INPUT/f'{name}.rpy').read_bytes()),'fixture':name,'mode':args.mode,'stride':args.stride,'startStage':args.start_stage,'sparseObserver':args.sparse_observer,'sampleFrames':sample_frames,'regularScan':args.with_regular_scan,'negativeControl':args.negative_control})
                (directory/'identity.json').write_text(json.dumps({'fixture':metadata,'browser':browser.version,'mode':args.mode,'stride':args.stride,'sparseObserver':args.sparse_observer,'negativeControl':args.negative_control,'harnessSha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),**setup},ensure_ascii=False,indent=2),encoding='utf-8')
                stages = set()
                groups = {}
                checkpoints = 0
                last = None
                with (directory/'windows.jsonl').open('w',encoding='utf-8') as wf, (directory/'trace.jsonl').open('w',encoding='utf-8') as tf, (directory/'events.jsonl').open('w',encoding='utf-8') as ef:
                    while True:
                        batch = page.evaluate(CHUNK, {'count':600,'maxTicks':args.max_ticks})
                        last = batch
                        for event in batch['events']:
                            ef.write(json.dumps(event,ensure_ascii=False)+'\n')
                            if event['type']=='stage':
                                stages.add(event['scene'][1])
                                print(json.dumps({'fixture':name,'stageIndex':event['scene'][1],'step':event['step']},ensure_ascii=False),flush=True)
                        for row in batch['checkpoints']:
                            tf.write(json.dumps(row,separators=(',',':'))+'\n');checkpoints+=1
                        for window in batch['windows']:
                            wf.write(json.dumps(window,ensure_ascii=False,separators=(',',':'))+'\n')
                            for g in window['issueGroups']:
                                key=(window['context']['stageIndex'],g['key'])
                                entry=groups.setdefault(key,{**g,'stageIndex':key[0],'windows':0,'totalObservations':0,'first':window['context'],'examples':[]})
                                entry['windows']+=1;entry['totalObservations']+=g['observations'];entry['last']=window['context']
                                if len(entry['examples'])<3:entry['examples'].append({'context':window['context'],'key':g['exampleKey']})
                        wf.flush();tf.flush();ef.flush()
                        progress={'fixture':name,'steps':batch['steps'],'windows':batch['totalWindows'],'stageIndex':batch['lastScene'][1],'stageFrame':batch['lastScene'][4],'seconds':round(time.monotonic()-started,1),'stop':batch['stopReason']}
                        (directory/'progress.json').write_text(json.dumps(progress),encoding='utf-8')
                        if batch['steps']%6000==0 or batch['done']:print(json.dumps(progress),flush=True)
                        if batch['done']:break
                stage_info=metadata['metadata']['stageInfo']
                if args.start_stage is None:
                    expected={s['stageIndex'] for s in stage_info}
                    final_expected=stage_info[-1]['endScore']
                else:
                    selected=next((s for s in stage_info if s['stageIndex']==args.start_stage),None)
                    if selected is None: raise RuntimeError(f'{name}: selected stage is absent from Replay metadata')
                    expected={args.start_stage}
                    final_expected=selected['endScore']
                summary={'fixture':name,'steps':last['steps'],'windows':last['totalWindows'],'checkpoints':checkpoints,'stageIndices':sorted(stages),'expectedStages':sorted(expected),'lastScene':last['lastScene'],'lastTrace':last['lastTrace'],
                         'stopReason':last['stopReason'],'allExpectedStages':expected<=stages,'finalExpectedScoreInternal':final_expected,'finalScoreInternal':last['lastGameplayTrace'][13],
                         'finalScoreMatches':last['lastGameplayTrace'][13]==final_expected,
                         'lastGameplayTrace':last.get('lastGameplayTrace'), 'stageMaxima':last.get('stageMaxima',{}),
                         'completed':last['stopReason']=='returned-to-scene-1' and expected<=stages,
                         'seconds':round(time.monotonic()-started,2),'errors':errors,'issues':sorted(groups.values(),key=lambda g:(-g['severity'],-g['windows']))}
                (directory/'summary.json').write_text(json.dumps(summary,ensure_ascii=False,indent=2),encoding='utf-8')
                page.screenshot(path=str(directory/'last.png'))
                print(json.dumps({k:v for k,v in summary.items() if k not in ['issues','lastTrace']},ensure_ascii=False),flush=True)
                context.close()
                if errors or last['stopReason'] in ['presentation-state-change','invalid-audit-window']:
                    raise RuntimeError('Unsafe/incomplete audit, inspect '+str(directory))
                if args.require_complete and (not summary['completed'] or not summary['finalScoreMatches']):
                    raise RuntimeError('Full fixture completion/final score not proven, inspect '+str(directory))
        finally:
            browser.close()


if __name__ == '__main__':
    main()
