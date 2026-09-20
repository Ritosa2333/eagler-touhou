"""Print only bounded grouped diagnostic evidence from a named audit run."""
import argparse
import json
from pathlib import Path
from collections import Counter
ROOT=Path(__file__).resolve().parents[2]/'artifacts/presentation-lab/campaign'
parser=argparse.ArgumentParser()
parser.add_argument('tag')
parser.add_argument('fixture')
parser.add_argument('--limit',type=int,default=30)
args=parser.parse_args()
folder=ROOT/args.tag/args.fixture
counts=Counter();examples={};windows=0;bad=[];seen=set()
with (folder/'windows.jsonl').open(encoding='utf-8') as stream:
    for line in stream:
        try:r=json.loads(line)
        except json.JSONDecodeError:continue
        windows+=1;seen.add(r['context']['stageIndex'])
        if not r['purity'] or not r['valid']:bad.append({'context':r['context'],'changes':r['stateChanges']})
        for g in r['issueGroups']:
            key=(r['context']['stageIndex'],g['owner'],g['anm'],g['script'],g['field'],g['status'])
            counts[key]+=1;examples.setdefault(key,r['context'])
print(json.dumps({'windows':windows,'stageIndices':sorted(seen),'unsafe':bad[:2]},ensure_ascii=False))
for k,count in counts.most_common(args.limit):
    print(json.dumps({'group':k,'windows':count,'first':examples[k]},ensure_ascii=False))
