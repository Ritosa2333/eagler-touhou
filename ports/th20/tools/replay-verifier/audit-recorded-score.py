"""Compare stage-end scores to the NEXT stage's immutable recorded score.

This is a necessary checkpoint, never a proof of bullet/RNG parity. A stage's
first score is restored from the file and is not an independent validation.
"""
import argparse,json
from pathlib import Path
import importlib.util
spec=importlib.util.spec_from_file_location('inspect_replay',Path(__file__).with_name('inspect-replay.py'))
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)

def audit(replay,rows):
    fixture=module.inspect(replay);lengths={s['stage']:s['frames'] for s in fixture['stages']}
    ends={};last={};starts={}
    with rows.open(encoding='utf-8') as stream:
        for line in stream:
            w=json.loads(line)['words'];stage,frame=w[1:3]
            if frame==1:starts[stage]=w
            last[stage]=w
            if frame==lengths.get(stage):ends[stage]=w
    checkpoints=[]
    for index,stage in enumerate(fixture['stages'][:-1]):
        next_stage=fixture['stages'][index+1]
        w=ends.get(stage['stage']);expected=next_stage['score']
        actual=None if w is None else w[17]+(w[18]<<32)
        checkpoints.append(dict(stage=stage['stage'],frame=stage['frames'],
            expectedDisplayedScore=expected*10,actualDisplayedScore=None if actual is None else actual*10,
            displayedScoreDifference=None if actual is None else (actual-expected)*10,
            status='not-observed' if actual is None else ('match' if actual==expected else 'mismatch'),
            expectedLives=next_stage['lives'],actualLives=None if w is None else (w[20] if w[20]<0x80000000 else w[20]-0x100000000)))
    return dict(purpose='diagnostic-only',replaySha256=fixture['sha256'],
        status='mismatch' if any(c['status']=='mismatch' for c in checkpoints) else 'incomplete',
        checkpoints=checkpoints,finalRecordedDisplayedScore=fixture['finalScore']*10,
        firstMismatch=next((c for c in checkpoints if c['status']=='mismatch'),None),
        note='Matching restored start scores are not independent evidence; final score requires completion/result-screen validation.')

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--replay',type=Path,required=True);p.add_argument('--rows',type=Path,required=True);p.add_argument('--output',type=Path)
    a=p.parse_args();result=audit(a.replay,a.rows);text=json.dumps(result,indent=2)
    if a.output:a.output.write_text(text+'\n',encoding='utf-8')
    print(text)
