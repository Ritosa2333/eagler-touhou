"""Build local x86 import libraries from the user's existing game DLLs."""
from pathlib import Path
import argparse,re,struct,subprocess
ap=argparse.ArgumentParser();ap.add_argument('--game',type=Path,required=True);ap.add_argument('--tools',type=Path,required=True);args=ap.parse_args()
root=Path(__file__).resolve().parents[1];out=root/'lib';out.mkdir(exist_ok=True)
for name in ['glew32','PMDWin']:
 dll=args.game/(name+'.dll')
 if not dll.is_file():raise SystemExit('Missing dependency: '+str(dll))
 text=subprocess.check_output([str(args.tools/'dumpbin.exe'),'/exports',str(dll)]).decode(errors='replace')
 names=re.findall(r'^\s+\d+\s+[0-9A-F]+\s+[0-9A-F]{8}\s+(\S+)',text,re.M)
 if name=='PMDWin':
  required={'pmdwininit':4,'setpcmrate':4,'music_load2':8,'music_start':0,'music_stop':0,'getpcmdata':8,'getpartwork':4,'getopenwork':0}
  if not required.keys()<=set(names):raise SystemExit('Unsupported PMDWin DLL exports')
  names=[n+'@'+str(size) for n,size in required.items()]
 definition=out/(name+'.def');definition.write_text('LIBRARY '+name+'.dll\nEXPORTS\n'+'\n'.join(names)+'\n',encoding='ascii')
 library=out/(name+'.lib')
 subprocess.run([str(args.tools/'lib.exe'),'/nologo','/def:'+str(definition),'/machine:x86','/out:'+str(library)],check=True)
 if name=='PMDWin':
  # Preserve decorated x86 stdcall symbols for the linker, but request
  # IMPORT_OBJECT_NAME_UNDECORATE so Windows resolves the DLL's plain names.
  b=bytearray(library.read_bytes());assert b[:8]==b'!<arch>\n';at=8;count=0
  while at<len(b):
   size=int(b[at+48:at+58]);start=at+60
   if b[start:start+4]==b'\0\0\xff\xff':
    flags=struct.unpack_from('<H',b,start+18)[0];struct.pack_into('<H',b,start+18,(flags&~28)|(3<<2));count+=1
   at=start+size+(size%2)
  assert count==len(required);library.write_bytes(b)
print('Prepared x86 glew32 / PMDWin import libraries in '+str(out))
