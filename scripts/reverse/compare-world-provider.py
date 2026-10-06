"""Compare selected original managers with a candidate active-world capture.

This is a diagnostic, never an accepted/updated golden. Frame numbers in the
report are zero-based native input ticks (candidate frame is tick + 1). Native
and candidate enemies are paired by original birth routine/occurrence; item
slots are physical pool slots. Friendly projectiles are paired by creation
ordinal inferred from native slot activation/age reset, not pointer identity.
"""
import argparse,collections,json,pathlib
TITLE=pathlib.Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser()
p.add_argument('--native',type=pathlib.Path,default=TITLE/'artifacts/reverse/world-native-active-demo2.json')
p.add_argument('--candidate',type=pathlib.Path,default=TITLE/'artifacts/reverse/world-candidate-active-demo2.json')
p.add_argument('--output',type=pathlib.Path,default=TITLE/'artifacts/reverse/world-active-comparison.json')
args=p.parse_args();native=json.loads(args.native.read_text());candidate=json.loads(args.candidate.read_text())
for flag in ['nativePlayerExecuted','nativeItemsUfoSpellMsgExecuted','activeCollisionExecuted','nativeAnmExecuted','nativeBulletUpdateExecuted']:
 if not native.get(flag):raise SystemExit('Active manager comparison requires '+flag)
first={};examples=[];counts=collections.Counter();excluded={'main','MapleEnemy','Logo','LogoEnemy','ShipShadow','ShipShadowEnemy'}
def record(tick,field,a,b,actor=None):
 row={'inputTick':tick,'candidateFrame':tick+1,'field':field,'native':a,'candidate':b}
 if actor is not None:row['actor']=actor
 counts[field]+=1
 if field not in first:first[field]=row
 if len(examples)<100:examples.append(row)
def scalar(tick,field,a,b,actor=None):
 if a!=b:record(tick,field,a,b,actor)
def vector(tick,field,a,b,actor=None,tolerance=.00004):
 if len(a)!=len(b) or any(abs(x-y)>tolerance for x,y in zip(a,b)):record(tick,field,a,b,actor)
def enemy_keys(frames,native_side):
 keys={};occurrences=collections.Counter()
 for frame in frames:
  for e in frame['enemies']:
   if e['id'] in keys:continue
   if native_side:name=e['birth']['routine']
   else:
    ctx=next((x for x in frame['ecl'] if x['enemy']==e['id']),{})
    name=(ctx.get('calls') or [ctx])[0].get('routine','')
   keys[e['id']]=(name,occurrences[name]);occurrences[name]+=1
 return keys
nk=enemy_keys(native['frames'],True);ck=enemy_keys([candidate['initial']]+candidate['ticks'],False)
friendly_native_ids={};friendly_native_previous={};friendly_candidate_ids={};nfcount=0
def friendly_candidate_fields(b):
 w=b['weapon'];offset=1 if len(w)>=13 else 0
 return {'phase':w[0],'age':b['age'],'position':b['position'],'damage':w[3+offset],'hitbox':w[4+offset:6+offset],'hitLastFrame':w[2]}
frames=min(len(native['frames'])-1,len(candidate['ticks']))
for a,b in zip(native['frames'][1:],candidate['ticks']):
 tick=a['frame'];np=a['player'];cp=b['player'];ne=a['economy'];ce=b['economy'];r=ce['resources']
 scalar(tick,'rng.seed',a['rngSeed'],b['rng']['seed']);scalar(tick,'rng.calls',a['rngCalls'],b['rng']['calls'])
 for field,av,bv in [('player.fixedPosition',np['fixedPosition'],[cp['xFixed'],cp['yFixed']]),('player.state',np['state'],cp['state']),('player.stateAge',np['stateAge'],cp['stateAge']),('player.invulnerability',np['invulnerability'],cp['invulnerability']),('player.shootAge',np['shootAge'],cp['fireFrame'])]:scalar(tick,field,av,bv)
 for name,index in [('power',0),('scoreStored',3),('pivStored',4),('maxPivStored',5),('rank',10)]:scalar(tick,'economy.'+name,ne[name],r[index])
 scalar(tick,'economy.lifeBomb',ne['lifeBombCounters'],[r[6],r[8],r[7],r[9]])
 scalar(tick,'ufo.colors',ne['ufoColors'],ce['ufoColors']);scalar(tick,'bomb.active',bool(ne['bombState']),bool(cp['bombState']['active']))
 na={nk[e['id']]:e for e in a['enemies'] if nk[e['id']][0] not in excluded};ca={ck[e['id']]:e for e in b['enemies'] if ck[e['id']][0] not in excluded}
 scalar(tick,'enemy.actors',sorted(na),sorted(ca))
 for key in na.keys()&ca.keys():
  x,y=na[key],ca[key];scalar(tick,'enemy.life',x['life'],y['life'],key);scalar(tick,'enemy.hpImmunity',x['hpImmunity'],y['immunity'],key);vector(tick,'enemy.position',x['position'],y['position'],key,tolerance=.00002)
  nc=sorted((z['context']['routine'],z['context']['instructionOffset'],z['time']) for z in x['contexts'] if z['context']);cc=sorted((z['routine'],z['byteOffset'],z['time']) for z in b['ecl'] if z['enemy']==y['id'] and z['routine']);scalar(tick,'enemy.contexts',nc,cc,key)
 ni={i['slot']:i for i in a['items']};ci={i[1]:i for i in b['items']};scalar(tick,'item.slots',sorted(ni),sorted(ci))
 for slot in ni.keys()&ci.keys():
  x,y=ni[slot],ci[slot]
  for field,av,bv in [('type',x['type'],y[2]),('state',x['state'],y[3]),('age',x['age'],y[8]),('colorTimer',x['colorTimer'],y[9]),('delay',x['delay'],y[10])]:scalar(tick,'item.'+field,av,bv,slot)
  for field,av,bv in [('position',x['position'],y[4:6]),('velocity',x['velocity'],y[6:8]),('attractionSpeed',[x['attractionSpeed']],[y[13]])]:vector(tick,'item.'+field,av,bv,slot)
 nf={}
 for shot in a['friendly']:
  slot=shot['slot'];previous=friendly_native_previous.get(slot)
  if previous is None or (shot['age']==1 and previous['age']!=0):nfcount+=1;friendly_native_ids[slot]=nfcount
  nf[friendly_native_ids[slot]]=shot
 friendly_native_previous={x['slot']:x for x in a['friendly']}
 cf={}
 for shot in b['bullets']:
  if not shot['friendly']:continue
  if shot['id'] not in friendly_candidate_ids:friendly_candidate_ids[shot['id']]=len(friendly_candidate_ids)+1
  cf[friendly_candidate_ids[shot['id']]]=friendly_candidate_fields(shot)
 scalar(tick,'friendly.count',len(nf),len(cf));scalar(tick,'friendly.actors',sorted(nf),sorted(cf))
 for key in nf.keys()&cf.keys():
  x,y=nf[key],cf[key]
  for field in ['phase','age','damage','hitLastFrame']:scalar(tick,'friendly.'+field,x[field],y[field],key)
  for field in ['position','hitbox']:vector(tick,'friendly.'+field,x[field],y[field],key)
out={'schema':'th12-native-world-comparison/1','partial':True,'wholeGameOracle':False,'framesCompared':frames,'floatTolerance':.00004,'excludedEnemyRoutines':sorted(excluded),'pairingLimitations':'Friendly creation ordinal inference can diverge after an extra/missing birth; subsequent identity-dependent differences then require a focused probe. Full animation scene, STD and hostile lasers are not compared here.','firstDifferenceByField':first,'differenceCounts':dict(counts),'examples':examples}
args.output.write_text(json.dumps(out,indent=2)+'\n');print(json.dumps({k:v for k,v in out.items() if k!='examples'},indent=2))
