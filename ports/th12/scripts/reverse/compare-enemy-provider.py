"""Compare retail synthetic Enemy traces with candidate semantic snapshots.

Normalizes differing native object IDs by birth routine and occurrence; it does
not accept/update a golden. Excluded decorative actors and float tolerance are
reported explicitly. Only fields emitted by the native provider are compared.
"""
import argparse,json,pathlib,collections
TITLE=pathlib.Path(__file__).resolve().parents[2]
p=argparse.ArgumentParser();p.add_argument('--native',type=pathlib.Path,default=TITLE/'artifacts/reverse/enemy-native-passive-demo2.json');p.add_argument('--candidate',type=pathlib.Path,default=TITLE/'artifacts/reverse/enemy-candidate-passive-demo2.json');p.add_argument('--output',type=pathlib.Path,default=TITLE/'artifacts/reverse/enemy-passive-comparison.json');args=p.parse_args()
native=json.loads(args.native.read_text());candidate=json.loads(args.candidate.read_text());excluded=['main','MapleEnemy','Logo','LogoEnemy','ShipShadow','ShipShadowEnemy']
candidate_birth={};occurrences=collections.Counter();native_keys={};candidate_keys={}
for frame in [candidate['initial']]+candidate['ticks']:
 for enemy in frame['enemies']:
  if enemy['id'] in candidate_birth:continue
  contexts=[x for x in frame['ecl'] if x['enemy']==enemy['id']]
  first=contexts[0] if contexts else {};name=(first.get('calls') or [first])[0].get('routine','')
  candidate_birth[enemy['id']]=name;candidate_keys[enemy['id']]=(name,occurrences[name]);occurrences[name]+=1
occurrences.clear()
for frame in native['frames']:
 for enemy in frame['enemies']:
  if enemy['id'] in native_keys:continue
  name=enemy['birth']['routine'];native_keys[enemy['id']]=(name,occurrences[name]);occurrences[name]+=1
differences=[];first_fields={};native_emissions=collections.Counter();candidate_emissions=collections.Counter();all_candidate_bullets=set();candidate_bullet_keys={}
for event in native['events']:
 if event['kind']=='bulletCreated':native_emissions[event['frame']]+=1
for frame in candidate['ticks'][:len(native['frames'])-1]:
 for bullet in frame['bullets']:
  if not bullet['friendly'] and bullet['id'] not in all_candidate_bullets:candidate_emissions[frame['frame']-1]+=1;all_candidate_bullets.add(bullet['id']);candidate_bullet_keys[bullet['id']]=len(candidate_bullet_keys)+1
def record(frame,field,native_value,candidate_value,key=None):
 row={'frame':frame,'field':field,'native':native_value,'candidate':candidate_value}
 if key:row['actor']=key
 if field not in first_fields:first_fields[field]=row
 if len(differences)<80:differences.append(row)
for n,c in zip(native['frames'][1:],candidate['ticks']):
 frame=n['frame']
 for field,a,b in [('rngSeed',n['rngSeed'],c['rng']['seed']),('rngCalls',n['rngCalls'],c['rng']['calls']),('bulletBirths',native_emissions[frame],candidate_emissions[frame])]:
  if a!=b:record(frame,field,a,b)
 ns={native_keys[e['id']]:e for e in n['enemies'] if native_keys[e['id']][0] not in excluded};cs={candidate_keys[e['id']]:e for e in c['enemies'] if candidate_keys[e['id']][0] not in excluded}
 if ns.keys()!=cs.keys():record(frame,'actors',sorted(ns),sorted(cs))
 for key in ns.keys()&cs.keys():
  a,b=ns[key],cs[key]
  for field,av,bv in [('position',a['position'],b['position']),('absolute',[*a['absolute'],a['angle'],a['speed']],b['absolute']),('relative',[*a['relative'],a['relativeAngle'],a['relativeSpeed']],b['relative'])]:
   if any(abs(x-y)>.00002 for x,y in zip(av,bv)):record(frame,field,av,bv,key)
  native_contexts=sorted((x['context']['routine'],x['context']['instructionOffset'],x['time']) for x in a['contexts'] if x['context'])
  candidate_contexts=sorted((x['routine'],x['byteOffset'],x['time']) for x in c['ecl'] if x['enemy']==b['id'] and x['routine'])
  if native_contexts!=candidate_contexts:record(frame,'contexts',native_contexts,candidate_contexts,key)
 if native.get('nativeBulletUpdateExecuted'):
  nb={b['id']:b for b in n['bullets']};cb={candidate_bullet_keys[b['id']]:b for b in c['bullets'] if not b['friendly']}
  if nb.keys()!=cb.keys():record(frame,'bulletActors',sorted(nb),sorted(cb))
  for key in nb.keys()&cb.keys():
   a,b=nb[key],cb[key]
   for field,av,bv in [('bulletPosition',a['position'],b['position']),('bulletVelocity',a['velocity'],b['hostile'][:2]),('bulletPolar',[a['angle'],a['speed']],b['motion'])]:
    if any(abs(x-y)>.00004 for x,y in zip(av,bv)):record(frame,field,av,bv,key)
   phase={1:1,2:2,3:3}.get(a['state'],-1)
   if phase!=b['hostile'][2]:record(frame,'bulletPhase',phase,b['hostile'][2],key)
out={'schema':'th12-native-enemy-comparison/1','partial':True,'wholeGameOracle':False,'excludedBirthRoutines':excluded,'positionTolerance':.00002,'framesCompared':min(len(native['frames'])-1,len(candidate['ticks'])),'firstDifferenceByField':first_fields,'examples':differences,'nativeBulletBirths':sum(native_emissions.values()),'candidateBulletBirths':sum(candidate_emissions.values())}
args.output.write_text(json.dumps(out,indent=2)+'\n');print(json.dumps({k:v for k,v in out.items() if k!='examples'},indent=2))
