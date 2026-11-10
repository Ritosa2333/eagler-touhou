"""Original Curve/Moving extension jumps and Timed ignore behavior."""
import pathlib,json,importlib.util
ROOT=pathlib.Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('laser_jump_host',ROOT/'scripts/reverse/sample-laser-bounce.py')
host=importlib.util.module_from_spec(spec);spec.loader.exec_module(host)
rows=[]
loop=[[0,-0.0785398185,20,-999999,8,0],[0,0.0785398185,20,-999999,8,0],[-999999,-999999,0,-999999,0x400000,0]]
linear=[[.01,.3,5,0,4,0],[-.01,.3,5,0,4,0],[0,0,0,0,0x400000,0]]
forward=[[0,0,3,0,0x400000,0],[0,0,7,0,0x200,0],[0,0,0,0,0,0],[0,0,9,0,0x200,0]]
for kind,mode,program,steps in [(2,0,loop,128),(0,1,linear,80),(0,2,forward,4),(2,2,forward,4),(1,2,forward,4)]:
 row=host.experiment(kind,(0,128),.2,16,.5,0,0,width=16,steps=steps,program_records=program,x87_control_word=0x7f)
 row['mode']=mode;rows.append(row)
out=ROOT/'tests/fixtures/laser-program-jump-original.json'
out.write_text(json.dumps({'originalExeSha256':host.EXE_SHA256,'wholeGameOracle':False,'scope':'Actual native factories, manager, Curve42d5bf and Moving42903b jumps and Timed42c23d ignore path; controlled parameters and far-away player. Native ANM runs; host resource/allocation/audio boundaries only. Gameplay FPU word0x007f.','records':rows},indent=2)+'\n',encoding='utf-8')
print(json.dumps({'cases':len(rows),'frames':sum(len(r['frames']) for r in rows),'output':str(out)}))
