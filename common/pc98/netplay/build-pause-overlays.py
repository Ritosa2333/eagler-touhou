"""Build hash-pinned guest pause rendering overlays for both PC-98 co-op games."""
from pathlib import Path
import gzip,hashlib,json,os,struct,subprocess,sys,tempfile
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,os.environ.get('PC98_HUD_TOOLS',str(ROOT.parent/'tools')))
from build_native_boss_hud import Fat12,mz_parts
sha=lambda b:hashlib.sha256(b).hexdigest()
nasm=os.environ.get('NASM','nasm')
for game in (2,4):
 port=ROOT/f'ports/th{game:02}';web=Path(os.environ[f'TH{game:02}_SOURCE_WEB']);out=port/'web/netplay/native-pause-overlay.json'
 manifest=json.loads((port/'web'/('local-patch.json' if game==2 else 'patch.json')).read_text())
 result={'schema':1,'game':game,'languages':{}}
 for lang in ('jp','cn'):
  p=manifest if game==2 or lang=='cn' else json.loads((port/'web/patch-jp.json').read_text())
  name=p['disks'][lang]['file'] if game==2 else 'th04-coop'+('-jp' if lang=='jp' else '')+'.hdi.gz'
  f=Fat12(gzip.decompress((web/name).read_bytes()));exe=f.read(f.files()[('HUMAROKU' if game==2 else 'GENSO')+'/MAIN.EXE']);h,img,rel=mz_parts(exe)
  cs=p['codeSegment'] if game==2 else p['code_segment'];ds=p['dataSegment'] if game==2 else p['data_segment'];ins=(p['insert']+p['gap']) if game==2 else p['insert_image_offset']+p['insert_size'];org=ins-cs*16
  if game==4:
   start=0xb2cf;size=0x11f;state=p['native_pause_cs_offset']
   with tempfile.TemporaryDirectory() as t:
    d=Path(t);(d/'net-pause-config.inc').write_text(f'%define PATCH_ORG {start-cs*16}\n%define PAUSE_STATE {state}\n');binary=d/'overlay.bin'
    subprocess.run([nasm,'-f','bin','-I',str(d)+'/',str(ROOT/'common/pc98/netplay/th04-pause-compact.asm'),'-o',str(binary)],check=True)
    code=binary.read_bytes()
   assert len(code)<=size
   old=bytes(img[start:start+size]);assert old[:4]==bytes.fromhex('558bec56') and old[-1]==0xc3
   removed=[loc for loc in rel if start<=loc<start+size]
   result['languages'][lang]={'sourceMainSha256':sha(exe),'codeSegment':cs,'dataSegment':ds,'replace':start,'before':old.hex(),'code':code.hex(),'removeRelocations':removed}
   continue
  if game==2:
   hook=cs*16+p['entries']['pause_hook']+9;target=0xb9e2;delay=struct.unpack_from('<H',img,0xbca9)[0];state=0
   assert img[cs*16+0x10e:cs*16+0x111]==bytes.fromhex('e8ae10')
   assert img[cs*16+0x13b:cs*16+0x13f]==bytes.fromhex('1e68da02')
  else:
   state=p['native_pause_cs_offset'];hook=cs*16+state+16+6;target=0xb2cf;delay=0
  before=bytes(img[hook:hook+3]);assert before[0]==0xe8 and (hook+3+struct.unpack_from('<h',before,1)[0])&0xffff==target,(game,lang,hex(hook),before.hex())
  with tempfile.TemporaryDirectory() as t:
   d=Path(t);(d/'net-pause-config.inc').write_text(f'%define GAME {game}\n%define PATCH_ORG {org}\n%define DELAY_SEGMENT {delay}\n%define PAUSE_STATE {state}\n');binary=d/'overlay.bin'
   subprocess.run([nasm,'-f','bin','-I',str(d)+'/', '-I',str(ROOT/'common/pc98')+'/',str(ROOT/'common/pc98/netplay/net-pause.asm'),'-o',str(binary)],check=True)
   code=binary.read_bytes()
  gap=(len(code)+15)&~15;assert org+gap<65536
  extra=[]
  if game==2:
   for op in (0x1c8,0x1de):
    needle=b'\x9a'+struct.pack('<HH',op,delay);assert code.count(needle)==1;extra.append(ins+code.index(needle)+3)
  result['languages'][lang]={'sourceMainSha256':sha(exe),'codeSegment':cs,'dataSegment':ds,'insert':ins,'bytes':gap,'code':code.hex(),'hook':hook,'before':before.hex(),'extraRelocations':extra}
  if game==2:
   assert code.count(b'NPRST02!')==1
   result['languages'][lang]['restartResult']=org+code.index(b'NPRST02!')+8
 out.write_text(json.dumps(result,indent=2)+'\n');print(out,[(k,len(v['code'])//2) for k,v in result['languages'].items()])
