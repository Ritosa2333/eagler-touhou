"""Actual Curve80000 paired emitter construction before BulletManager dispatch.

Retail laser factory/update, embedded ANM, and optional clear execute.40b5e0
is a declared observation boundary: records the constructed emitter and returns
zero, without actual child bullets. This verifies emission ABI/parent lifetime,
not a complete gameplay or replay result.
"""
import json,pathlib,sys
ROOT=pathlib.Path(__file__).resolve().parents[2]
source=ROOT/'scripts/reverse/sample-laser-bounce.py'
text=source.read_text(encoding='utf8').split('\nrecords =',1)[0]
# Isolate the existing provider function without executing its fixture matrix.
text=text[:text.index('\nrows =')] if '\nrows =' in text else text
text=text.replace("elif address == 0x428450:","""elif address == 0x40b5e0:
            emitter=v.reg_read(UC_X86_REG_ESI)
            events.append({'frame':frame,'kind':'emission','emitterHex':bytes(v.mem_read(emitter,0x214)).hex()})
            ret(result=0)
        elif address == 0x428450:""")
needle="    call(0x428450, [kind], {UC_X86_REG_EDI: PARAM})"
replacement="""    pbase=PARAM+{0:0x30,1:0x58,2:0x28}[kind]
    for index,record in enumerate(PROGRAM):put(pbase+index*24,'ffiiII',*record)
    call(0x428450, [kind], {UC_X86_REG_EDI: PARAM})"""
text=text.replace(needle,replacement)
scope={'__file__':str(source),'__name__':'native_laser_emission'}
exec(compile(text,str(source),'exec'),scope)
records=[]
for kind in [0,1,2]:
 for name,cancel,first,angle,speed,layers,mode in [
  ('basic',False,0,0,2,1,2),('alternate',False,2,1.2,3,2,5),
  ('cancel',True,0,-1.1,1.5,1,2)]:
  packed=(mode<<24)|(9<<16)|(4<<8)|first|(0x80000000 if cancel else 0)
  if packed>=0x80000000:packed-=0x100000000
  scope['PROGRAM']=[[speed,1,packed,3,0x80000,0],
    [angle,.2,layers,0,0x100000,0],[0,0,0,0,0,0]]
  row=scope['experiment'](kind,[20,128],.7,96,2,0,0,steps=2)
  row.update(name=f'kind{kind}-{name}',program=scope['PROGRAM'])
  records.append(row)
out=ROOT/'tests/fixtures/laser-emission-original.json'
out.write_text(json.dumps({'schema':'th12-native-laser-emission/1','originalExeSha256':scope['EXE_SHA256'],
 'wholeGameOracle':False,'gpuExecuted':False,
 'boundary':'Actual laser CPU/ANM update. Synthetic literal extension records, passive far-away Player and host heap/audio.40b5e0 records the full retail emitter then returns0; no child bullet simulation. Parent cancellation remains real.',
 'records':records},indent=2)+'\n',encoding='utf8')
print(json.dumps({'output':str(out),'cases':len(records),
 'emissions':[{ 'name':r['name'],'count':sum(e['kind']=='emission' for e in r['events'])} for r in records]}))
