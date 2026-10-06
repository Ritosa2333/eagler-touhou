"""Build local TH12 PNG atlases and metadata from supplied retail resources.

ANM v7 layout reference: thpatch/thtypes anm_types.h. Original payload is kept
outside source control and separate from the Runtime code release.
"""
import json, pathlib, struct, hashlib, shutil, math, sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT.parent/'tools/python'))
from PIL import Image
retail=ROOT/'local/retail';dest=ROOT/'site/assets';dest.mkdir(parents=True,exist_ok=True)
PINNED_EXE_SHA256='99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417'
if hashlib.sha256((ROOT.parent/'th12/th12.exe').read_bytes()).hexdigest()!=PINNED_EXE_SHA256:
    raise ValueError('Presentation preparation supports the pinned TH12 1.00b executable')
def u16(b,o):return struct.unpack_from('<H',b,o)[0]
def u32(b,o):return struct.unpack_from('<I',b,o)[0]
def anm(file):
    b=file.read_bytes();base=0;entries=[];sprites={};scripts={}
    while True:
        if u32(b,base)!=7:raise ValueError(f'{file.name}: expected v7')
        ns,nc=u16(b,base+4),u16(b,base+6)
        namepos=base+u32(b,base+16);name=b[namepos:b.index(0,namepos)].decode('shift_jis')
        td=u32(b,base+28);nxt=u32(b,base+36);eid=len(entries);image=None
        if td and u16(b,base+32):
            p=base+td;fmt,w,h,size=struct.unpack_from('<HHHI',b,p+6)
            raw=b[p+16:p+16+size];pixels=bytearray(w*h*4)
            if fmt==1:
                for i in range(w*h):pixels[i*4:i*4+4]=bytes((raw[i*4+2],raw[i*4+1],raw[i*4],raw[i*4+3]))
            elif fmt in (3,5):
                for i,(v,) in enumerate(struct.iter_unpack('<H',raw)):
                    if fmt==5:c=(((v>>8)&15)*17,((v>>4)&15)*17,(v&15)*17,((v>>12)&15)*17)
                    else:c=(((v>>11)&31)*255//31,((v>>5)&63)*255//63,(v&31)*255//31,255)
                    pixels[i*4:i*4+4]=bytes(c)
            elif fmt==7:
                for i,v in enumerate(raw):pixels[i*4:i*4+4]=bytes((255,255,255,v))
            else:raise ValueError(f'{file.name}: unsupported texture format {fmt}')
            # D3D allocates the declared texture size. Several retail chunks
            # contain only the used rectangle; retain transparent padding so
            # UVs are normalized against the original logical dimensions.
            image=f'{file.stem}-{eid}.png'
            logical=Image.new('RGBA',(max(w,u16(b,base+10)),max(h,u16(b,base+12))))
            logical.paste(Image.frombytes('RGBA',(w,h),bytes(pixels)),(0,0));logical.save(dest/image)
        entries.append({'name':name,'image':image,'width':u16(b,base+10),'height':u16(b,base+12),
          'xOffset':u16(b,base+20),'yOffset':u16(b,base+22),'textureFormat':u16(b,base+14),
          'hasData':bool(u16(b,base+32)),'sourceOffset':base})
        for i in range(ns):
            p=base+u32(b,base+64+i*4);sid,x,y,w,h=struct.unpack_from('<Iffff',b,p)
            sprites[str(len(sprites))]={'sourceId':sid,'entry':eid,'x':x,'y':y,'w':w,'h':h}
        table=base+64+ns*4
        for i in range(nc):
            sid,off=struct.unpack_from('<iI',b,table+i*8);p=base+off;commands=[]
            limit=base+(td or nxt or (len(b)-base));steps=0
            while p+8<=limit:
                op,length,time,mask=struct.unpack_from('<HHhH',b,p)
                if op==65535 or length<8:break
                if p+length>limit:raise ValueError('ANM instruction crosses entry')
                raw=b[p+8:p+length]
                floats=struct.unpack('<'+'f'*(len(raw)//4),raw)
                commands.append({'offset':p-base-off,'op':op,'time':time,'mask':mask,'ints':list(struct.unpack('<'+'i'*(len(raw)//4),raw)), 'floats':[v if math.isfinite(v) else None for v in floats]})
                p+=length;steps+=1
                if steps>100000:raise ValueError('ANM script limit')
            scripts[str(len(scripts))]={'sourceId':sid,'entry':eid,'commands':commands}
        if not nxt:break
        base+=nxt
        if base>=len(b):raise ValueError('ANM entry offset out of bounds')
    return {'entries':entries,'sprites':sprites,'scripts':scripts,'provenance':{
      'source':file.name,'sha256':hashlib.sha256(b).hexdigest(),'anmVersion':7,'originalExeSha256':PINNED_EXE_SHA256,
      'wholePresentationOracle':False}}
chosen=['pl00','pl01','pl02','bullet','enemy','front','ascii','text','title','title_v']
for stage in range(1,8):
    chosen.extend([f'stgenm{stage:02d}',f'stage{stage:02d}',f'st{stage:02d}logo'])
chosen.extend(file.stem for file in retail.glob('stgenm*m.anm'))
metadata={}
for name in chosen:
    metadata[name]=anm(retail/(name+'.anm'));print(name,len(metadata[name]['entries']),len(metadata[name]['sprites']),len(metadata[name]['scripts']))
    shutil.copyfile(retail/(name+'.anm'),dest/(name+'.anm'))
for name in ['default.ecl',*[f'stage{stage:02d}.ecl' for stage in range(1,8)],*[file.name for file in retail.glob('stage*boss.ecl')],'pl00a.sht','pl00b.sht','pl01a.sht','pl01b.sht','pl02a.sht','pl02b.sht']:
    shutil.copyfile(retail/name,dest/name)
for file in retail.glob('stage??.std'):shutil.copyfile(file,dest/file.name)
for file in retail.glob('se_*.wav'):shutil.copyfile(file,dest/file.name)
for file in retail.glob('st??_0[012][ab].msg'):shutil.copyfile(file,dest/file.name)
(dest/'atlases.json').write_text(json.dumps(metadata,ensure_ascii=False,separators=(',',':'),allow_nan=False),encoding='utf-8')
original=ROOT.parent/'th12'
identity={name:{'bytes':(original/name).stat().st_size,'sha256':hashlib.sha256((original/name).read_bytes()).hexdigest()} for name in ['th12.exe','th12.dat','thbgm.dat']}
(ROOT/'local/content-identity.json').write_text(json.dumps({'game':'th12','version':'1.00b','files':identity},indent=2),encoding='utf-8')
print('Local resource preparation complete.')
