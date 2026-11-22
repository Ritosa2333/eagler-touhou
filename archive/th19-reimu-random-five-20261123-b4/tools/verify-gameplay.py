import json,struct,sys
from pathlib import Path
root=Path('.'); web=Path(sys.argv[1]); report=json.loads((web/'report.json').read_text()); idx={(x.get('originalRound',1)-1,x.get('originalRoundFrame',x['frame'])):x['frame'] for x in report['samples']}
def native_rows():
 prev=None; pos=None
 for l in Path(report['nativePath']).open():
  d=json.loads(l); f=idx.get((d.get('segment'),d.get('frame')))
  if f is None or 'bulletPositions' not in d:continue
  if prev is not None and f!=prev:yield prev,pos
  prev,pos=f,d['bulletPositions']
 if prev is not None:yield prev,pos
f32=lambda x:struct.unpack('<f',struct.pack('<I',x&0xffffffff))[0]
def positions(fields):return [sorted((b[0],b[1],f32(b[2]),f32(b[3])) for b in field) for field in fields]
native=iter(native_rows()); row=next(native,None); maximum=0.; differences=[]; compared=0
for l in (web/'web-bullet-positions.jsonl').open():
 d=json.loads(l); f=d['frame']
 while row and row[0]<f:row=next(native,None)
 if not row or row[0]!=f:continue
 compared+=1
 a=positions(d['bulletPositions']);b=positions(row[1]);bad=False
 for aa,bb in zip(a,b):
  if len(aa)!=len(bb):bad=True;continue
  for p,q in zip(aa,bb):
   if p[:2]!=q[:2]:bad=True;continue
   error=max(abs(p[2]-q[2]),abs(p[3]-q[3]));maximum=max(maximum,error)
   if error>.001:bad=True
 if bad:differences.append(f)
samples={x['frame']:x for x in report['samples']}
missing=report['bulletPositionCoverage']['missingFrames']
transition_gaps=[f for f in missing if f and f-1 in samples and f+1 in samples and samples[f+1].get('originalRound')==samples[f-1].get('originalRound',1)+1 and samples[f+1].get('originalRoundFrame')==1]
coverage_ok=compared==report['bulletPositionCoverage']['frames'] and all(f==0 or f in transition_gaps for f in missing)
result={'report':str(web/'report.json'),'wasmSha256':report['wasmSha256'],'nativeMissingStateFrames':[f for f in missing if f], 'verifiedTransitionGaps':transition_gaps,'capturedThrough':report['capturedThrough'],'primaryRngDifference':report['firstDifferentRngByStream']['primary'],'cpuDifference':report['firstDifferentPlayerControls'],'playerDifference':report['firstDifferentPlayerPosition'],'bulletComparedFrames':compared,'bulletCoordinateTolerance':.001,'maximumBulletCoordinateDifference':maximum,'substantiveBulletDifferenceFrames':differences,'status':'PASS' if report['capturedThrough']==36000 and coverage_ok and not report.get('browserErrors') and not differences and not report['firstDifferentRngByStream']['primary'] and not report['firstDifferentPlayerControls'] and not report['firstDifferentPlayerPosition'] else 'FAIL'}
p=web/'zero-gameplay-validation.json';p.write_text(json.dumps(result,indent=2));print(json.dumps(result))
