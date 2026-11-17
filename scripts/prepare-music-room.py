"""Import original TH05 Music Room data from the user's translated PC-98 image.
Reads the HDI/FAT filesystem without modifying it. Assets are not source files.
"""
from pathlib import Path
import argparse,struct,hashlib,json
p=argparse.ArgumentParser();p.add_argument('--source',type=Path,required=True);p.add_argument('--game',type=Path,required=True);a=p.parse_args()
hdi=a.source/'thow_cn.hdi';font=a.source/'f-c1.bmp'
if not hdi.is_file() or not font.is_file():raise SystemExit('Expected thow_cn.hdi and f-c1.bmp in --source')
b=hdi.read_bytes();base=None
for at in range(4096,len(b)-512,512):
 size=struct.unpack_from('<H',b,at+11)[0]
 if size in [256,512,1024] and b[at+13] in [1,2,4,8,16,32,64] and b[at+16] in [1,2] and b[at+54:at+59]==b'FAT12':base=at;break
if base is None:raise SystemExit('No supported FAT12 partition')
u=lambda at:struct.unpack_from('<H',b,base+at)[0]
ss=u(11);spc=b[base+13];nr=u(17);fat=base+ss*u(14);root=fat+b[base+16]*u(22)*ss;data=root+((nr*32+ss-1)//ss)*ss

def chain(c):
 result=bytearray();seen=set()
 while 2<=c<0xff0:
  if c in seen:raise ValueError('FAT loop')
  seen.add(c);offset=data+(c-2)*ss*spc
  if offset+ss*spc>len(b):raise ValueError('Cluster outside image')
  result+=b[offset:offset+ss*spc];value=struct.unpack_from('<H',b,fat+c*3//2)[0];c=value>>4 if c&1 else value&4095
 return result

def find(directory,name):
 for at in range(0,len(directory),32):
  e=directory[at:at+32]
  if len(e)<32 or e[0]==0:break
  if e[0] in [0xe5,46] or e[11]&8:continue
  n=e[:8].rstrip().decode('ascii',errors='replace');ext=e[8:11].rstrip().decode('ascii',errors='replace');n+=('.'+ext if ext else '')
  if n.upper()==name:return e
 raise ValueError('Missing '+name)
folder=find(b[root:root+nr*32],'KAIKI');entry=find(chain(struct.unpack_from('<H',folder,26)[0]),'MUSIC.DAT');music=chain(struct.unpack_from('<H',entry,26)[0])[:struct.unpack_from('<I',entry,28)[0]]
a.game.mkdir(parents=True,exist_ok=True)
for name,payload in [('music.dat',music),('music-font.bmp',font.read_bytes())]:
 target=a.game/name
 if target.exists() and target.read_bytes()!=payload:raise SystemExit('Existing different resource, preserve or rename it first: '+str(target))
 target.write_bytes(payload);print(json.dumps({'file':str(target),'bytes':len(payload),'sha256':hashlib.sha256(payload).hexdigest()}))
