"""Compare actual replay checkpoints and observed property groups, not FPS."""
import argparse
import json
from itertools import zip_longest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2] / 'artifacts/presentation-lab/campaign'


def load(path):
    return json.loads(path.read_text(encoding='utf-8'))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('before')
    parser.add_argument('after')
    parser.add_argument('--fixtures', nargs='+', default=['extra-border','lunatic-border','lunatic-magic','lunatic-ghost'])
    parser.add_argument('--partial', action='store_true')
    args = parser.parse_args()
    report = {'schema':'th08/campaign-comparison/1','before':args.before,'after':args.after,'fixtures':[],'passed':True}
    for fixture in args.fixtures:
        a, b = ROOT/args.before/fixture, ROOT/args.after/fixture
        before, after = load(a/'summary.json'), load(b/'summary.json')
        divergence=[];count=0
        with (a/'trace.jsonl').open() as af, (b/'trace.jsonl').open() as bf:
            for left,right in zip_longest(af,bf):
                if args.partial and right is None:break
                if left is None or right is None:
                    divergence.append({'kind':'checkpoint-count','at':count});break
                x,y=json.loads(left),json.loads(right);count+=1
                differences={name:[i for i,(l,r) in enumerate(zip(x[name],y[name])) if l!=r] for name in ['scene','trace','state']}
                if x['step']!=y['step'] or any(differences.values()):
                    divergence.append({'step':y['step'],'fields':differences,'before':x,'after':y})
                    if len(divergence)>=3:break
        key=lambda g:(g['stageIndex'],g['key'])
        old={key(g):g for g in before['issues'] if g['severity']>=4}
        new={key(g):g for g in after['issues'] if g['severity']>=4}
        resolved=[{k:v for k,v in g.items() if k in ['owner','ownerId','anm','script','field','status','stageIndex','windows','first','last']} for k,g in old.items() if k not in new]
        remaining=[{k:v for k,v in g.items() if k in ['owner','ownerId','anm','script','field','status','stageIndex','windows','first','last']} for g in new.values()]
        passed=not divergence and not after['errors'] and (args.partial or before['completed'] and after['completed'])
        row={'fixture':fixture,'passed':passed,'checkpointCount':count,'logicalStepsBefore':before['steps'],'logicalStepsAfter':after['steps'],
             'allRecordedStages':after['allExpectedStages'],'stageIndices':after['stageIndices'],'windowsBefore':before['windows'],'windowsAfter':after['windows'],
             'beforeWasm':load(a/'identity.json')['build']['wasm'],'afterWasm':load(b/'identity.json')['build']['wasm'],
             'divergence':divergence,'resolvedGroups':resolved,'remainingGroups':remaining,
             'evidenceScope':'same input file; 68-word gameplay trace and 9 named hashes every 60 host logical ticks; not a full-state serializer or pixel oracle'}
        report['fixtures'].append(row);report['passed'] &= passed
        print(json.dumps({k:v for k,v in row.items() if k not in ['resolvedGroups','remainingGroups','divergence']},ensure_ascii=False),flush=True)
        print(json.dumps({'resolvedGroupCount':len(resolved),'remainingGroupCount':len(remaining),'firstDivergence':divergence[:1]},ensure_ascii=False),flush=True)
    target=ROOT/args.after/'comparison.json'
    target.write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
    if not report['passed']:raise SystemExit('Checkpoint/completion comparison failed; inspect '+str(target))


if __name__=='__main__':main()
