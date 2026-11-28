import json
import math
from pathlib import Path

root = Path(__file__).parent
before = json.loads((root/'release-baseline.json').read_text())
gpu = json.loads((root/'release-gpu.json').read_text())
after = json.loads((root/'release-final.json').read_text())
first = json.loads((root/'release-final-run1.json').read_text())
rate = after['environment']['refreshRate']
budget = 1000/rate
def stats(values):
    if not values:
        return None
    ordered = sorted(values)
    return {'meanMs':round(sum(values)/len(values)/1000,3),
            'p95Ms':round(ordered[math.ceil(.95*len(values))-1]/1000,3),
            'maxMs':round(ordered[-1]/1000,3),
            'overBudget':sum(v>budget*1000 for v in values)}
def case(value):
    frames = value['frames']
    return {'frames':len(frames),
            'build':stats([f['buildUs'] for f in frames]),
            'raster':stats([f['rasterUs'] for f in frames]),
            'pipeline':stats([f['totalUs'] for f in frames]),
            'rssMiB':round(value['rssEnd']/1024**2,1),
            'imageCacheMiB':round(value['imageCacheBytes']/1024**2,1),
            'physicalSize':value['physicalSize']}
result = {
 'frameBudgetMs':round(budget,3),
 'method':'Same SDK and dependencies | Release AOT | foreground window | FrameTiming, not presented FPS | production guest bootstrap and bundled catalog | Windows desktop and portrait client sizes recorded per case',
 'baselineScope':'Completed cases before the baseline harness failed to locate its portrait drawer tooltip. This was an incorrect test finder; final sample uses the real DrawerButton and passes. Initial simultaneous-build sample excluded.',
 'before':{k:case(v) for k,v in before['cases'].items()},
 'gpuOnly':{k:case(v) for k,v in gpu['cases'].items()},
 'after':{k:case(v) for k,v in after['cases'].items()},
 'finalRun1':{k:case(v) for k,v in first['cases'].items()},
 'finalRun1Status':first.get('status'),
 'afterStatus':after.get('status'),
 'environment':after['environment'],
 'bootstrapMs':{'before':before['bootstrapMs'],'gpuColdSample':gpu['bootstrapMs'],'beforeShaderWarmUp':json.loads((root/'release-after.json').read_text())['bootstrapMs'],'finalRun1':first['bootstrapMs'],'after':after['bootstrapMs']},
 'bootstrapScope':'app.main duration, not whole process launch or first contentful paint',
}
(root/'runtime-summary.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n')
print(json.dumps({k:result[k] for k in ['afterStatus','frameBudgetMs','environment','bootstrapMs']},ensure_ascii=False))
for name in ['sidebar_hover','cover_hover','detail_first_open','detail_warm_0','detail_warm_1','detail_warm_2','detail_back_0','workspace_tabs','shelf_scroll','settings_open','portrait_detail_open','portrait_drawer','portrait_tabs','idle_after_roundtrip']:
    print(name, {label:result[label].get(name) for label in ['before','after']})
