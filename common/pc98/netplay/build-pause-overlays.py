"""Build the hash-pinned TH04 guest pause overlay."""
from pathlib import Path
import gzip,hashlib,json,os,subprocess,sys,tempfile

ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,os.environ.get('PC98_HUD_TOOLS',str(ROOT.parent/'tools')))
from build_native_boss_hud import Fat12,mz_parts

sha=lambda b:hashlib.sha256(b).hexdigest()
nasm=os.environ.get('NASM','nasm')
port=ROOT/'ports/th04';web=Path(os.environ['TH04_SOURCE_WEB']);out=port/'web/netplay/native-pause-overlay.json'
result={'schema':1,'game':4,'languages':{}}
for lang in ('jp','cn'):
 p=json.loads((port/'web'/('patch-jp.json' if lang=='jp' else 'patch.json')).read_text())
 name='th04-coop'+('-jp' if lang=='jp' else '')+'.hdi.gz'
 f=Fat12(gzip.decompress((web/name).read_bytes()));exe=f.read(f.files()['GENSO/MAIN.EXE']);_,img,rel=mz_parts(exe)
 cs,ds=p['code_segment'],p['data_segment']
 start,size,state=0xb2cf,0x11f,p['native_pause_cs_offset']
 with tempfile.TemporaryDirectory() as t:
  d=Path(t);(d/'net-pause-config.inc').write_text(f'%define PATCH_ORG {start-cs*16}\n%define PAUSE_STATE {state}\n');binary=d/'overlay.bin'
  subprocess.run([nasm,'-f','bin','-I',str(d)+'/',str(ROOT/'common/pc98/netplay/th04-pause-compact.asm'),'-o',str(binary)],check=True)
  code=binary.read_bytes()
 assert len(code)<=size
 old=bytes(img[start:start+size]);assert old[:4]==bytes.fromhex('558bec56') and old[-1]==0xc3
 removed=[loc for loc in rel if start<=loc<start+size]
 result['languages'][lang]={'sourceMainSha256':sha(exe),'codeSegment':cs,'dataSegment':ds,'replace':start,'before':old.hex(),'code':code.hex(),'removeRelocations':removed}
out.write_text(json.dumps(result,indent=2)+'\n');print(out,[(k,len(v['code'])//2) for k,v in result['languages'].items()])
