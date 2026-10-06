"""Read-only retail presentation tables; output remains in ignored local assets."""
import pathlib,struct,json,hashlib,sys,math
ROOT=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
dest=ROOT/'site/assets'
exe=ROOT.parent/'th12/th12.exe'
identity=json.loads((ROOT/'local/content-identity.json').read_text())
PINNED_EXE_SHA256='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
if hashlib.sha256(exe.read_bytes()).hexdigest()!=PINNED_EXE_SHA256 or identity['files']['th12.exe']['sha256']!=PINNED_EXE_SHA256:
    raise ValueError('presentation table addresses require the pinned retail executable')
pe=pefile.PE(str(exe));table=pe.get_data(0x4af280-pe.OPTIONAL_HEADER.ImageBase,31*0xd0)
atlases=json.loads((dest/'atlases.json').read_text(encoding='utf-8'))
types=[]
for i in range(31):
    row=table[i*0xd0:(i+1)*0xd0]
    types.append({'script':struct.unpack_from('<i',row)[0],
      'colors':[dict(zip(['sprite','spawnSprite','cancelSprite'],struct.unpack_from('<iii',row,4+j*12))) for j in range(16)],
      'size':struct.unpack_from('<f',row,0xc4)[0],'flags':struct.unpack_from('<I',row,0xc8)[0]})
atlases['bullet']['types']=types
(dest/'atlases.json').write_text(json.dumps(atlases,ensure_ascii=False,separators=(',',':')),encoding='utf-8')

def decode_std(name):
    b=(ROOT/'local/retail'/f'{name}.std').read_bytes()
    u16=lambda o:struct.unpack_from('<H',b,o)[0]
    u32=lambda o:struct.unpack_from('<I',b,o)[0]
    vec=lambda o:list(struct.unpack_from('<fff',b,o))
    objects=[]
    for i in range(u16(0)):
        at=u32(144+i*4);quads=[];p=at+28
        while u16(p)!=0xffff:
            size=u16(p+2)
            if size<28 or p+size>len(b):raise ValueError(f'{name}: invalid STD primitive')
            quads.append({'kind':u16(p),'script':u16(p+4),'position':vec(p+8),'size':list(struct.unpack_from('<ff',b,p+20))})
            p+=size
        objects.append({'layer':b[at+2],'position':vec(at+4),'boundsSize':vec(at+16),'quads':quads})
    instances=[];p=u32(4)
    while u16(p)!=0xffff:
        object_id=u16(p)
        if object_id>=len(objects):raise ValueError(f'{name}: invalid STD instance object')
        instances.append({'object':object_id,'position':vec(p+4)});p+=16
    commands=[];start=u32(8);p=start
    while p+8<=len(b) and u16(p+6)!=0xffff:
        size=u16(p+6)
        if size<8 or p+size>len(b):raise ValueError(f'{name}: invalid STD command')
        raw=b[p+8:p+size]
        commands.append({'offset':p-start,'time':u32(p),'op':u16(p+4),
          'ints':list(struct.unpack('<'+'i'*(len(raw)//4),raw)),
          'floats':[v if math.isfinite(v) else None for v in struct.unpack('<'+'f'*(len(raw)//4),raw)]})
        p+=size
    data={'bank':name,'objects':objects,'instances':instances,'commands':commands,
      'provenance':{'source':f'{name}.std','sha256':hashlib.sha256(b).hexdigest(),'originalExeSha256':PINNED_EXE_SHA256,
        'wholePresentationOracle':False,'nativeDimensionOverride':'0x4047f8..0x404839','nativeCameraInterpreter':'0x4049a0',
        'nativeCompositor':'text.anm script81/82 via 0x431630','defaultConfigFlags':'0x431b00 OR 0x4240'},
      'viewport':[13,0,422,480],
      'near':struct.unpack('<f',pe.get_data(0x4a3e14-pe.OPTIONAL_HEADER.ImageBase,4))[0],
      'far':struct.unpack('<f',pe.get_data(0x4a3e18-pe.OPTIONAL_HEADER.ImageBase,4))[0]}
    (dest/f'{name}-background.json').write_text(json.dumps(data,separators=(',',':'),allow_nan=False),encoding='utf-8')
    return len(objects),len(instances),len(commands)
for stage in range(1,8):
    name=f'stage{stage:02d}';print(name,*decode_std(name))
bank_maps={}
for stage in range(1,8):
    name=f'stage{stage:02d}';raw=(ROOT/'local/retail'/f'{name}.ecl').read_bytes();p=raw.index(b'ANIM')+4
    count=struct.unpack_from('<I',raw,p)[0];p+=4;names=[]
    for i in range(count):
        end=raw.index(0,p);names.append(raw[p:end].decode('shift_jis'));p=end+1
    bank_maps[str(stage)]=['bullet',*[pathlib.PurePosixPath(n).stem for n in names]]
(dest/'stage-bank-map.json').write_text(json.dumps({'originalExeSha256':PINNED_EXE_SHA256,'banks':bank_maps,
  'mapping':'ECL animation bank 0 is the preloaded bullet resource; ANIM header entries occupy bank 1 onward'},separators=(',',':')),encoding='utf-8')
print('Prepared 31 retail bullet visual types and all 7 STD geometry/camera streams.')
