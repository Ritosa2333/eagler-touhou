"""Read the pinned StageInfo table selected by original40f810/4217e0."""
import pathlib,sys,json,struct
ROOT=pathlib.Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT.parent/'tools/python'))
import pefile
from native_anm_resource import check_original,EXE_SHA256
exe=ROOT.parent/'th12/th12.exe';check_original(exe);b=pefile.PE(str(exe)).get_memory_mapped_image()
def string(a):return b[a-0x400000:a-0x400000+100].split(b'\0')[0].decode('ascii')
rows=[]
for n in range(1,8):
 w=struct.unpack_from('<16I',b,0xaebf0+n*64)
 rows.append({'number':n,'std':string(w[1]),'enemyAnm':string(w[2]),'ecl':string(w[3]),'stageMusic':string(w[5]),'messages':[string(a) for a in w[6:12]],'logoAnm':string(w[12])})
out=ROOT/'tests/fixtures/stage-resource-native.json';out.write_text(json.dumps({'schema':'th12-original-stage-resource-table/1','originalExeSha256':EXE_SHA256,'wholeGameOracle':False,'source':'4aebf0 + NativeStageNumber*64;40f810/4217e0 select,421cd0 preloads field10/14','rows':rows},indent=2)+'\n')
print('7 original StageInfo resource rows recorded.')
