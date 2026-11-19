"""Execute the unchanged retail stage-entry practice-unlock block with declared score/control memory."""
from pathlib import Path
import argparse,hashlib,importlib,json,struct,sys
from unicorn import Uc,UC_ARCH_X86,UC_MODE_32
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EIP

def main():
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--decoded',type=Path,required=True);parser.add_argument('--output',type=Path,required=True);a=parser.parse_args()
 root=Path(__file__).resolve().parents[1];out=a.output.resolve();assert out.is_relative_to(root/'artifacts/th14') and not out.exists()
 sys.path.insert(0,str(root/'tools'));img=importlib.import_module('th-anm-handlers').handlers(14)[0];sha=lambda b:hashlib.sha256(b).hexdigest();assert sha(img.data)=='f08e885e28d2e0fc8c579e88034dd7de8f2a648d8f08ce781cf08eadc9ffe353'
 raw=a.decoded.read_bytes();assert len(raw)==149076;m=Uc(UC_ARCH_X86,UC_MODE_32)
 for _,address,size,offset,length in img.sections:
  first=address&~4095;m.mem_map(first,((address+size+4095)&~4095)-first);m.mem_write(address,img.data[offset:offset+length])
 control,score=0x70001000,0x70010000;m.mem_map(0x70000000,0x40000)
 def put(p,v):m.mem_write(p,struct.pack('<I',v))
 put(0x4db68c,score);cases=[]
 for selection in range(6):
  for difficulty,stage in [(d,s) for d in range(4) for s in range(1,7)]+[(4,7)]:
   for mode in [0,1,2]:
    m.mem_write(score+8,raw);put(control+0x94,mode);put(0x4f5828,selection//2);put(0x4f582c,selection%2);put(0x4f5834,difficulty);put(0x4f58a4,stage);m.reg_write(UC_X86_REG_EAX,control);m.emu_start(0x435b6c,0x435ba4,count=40);assert m.reg_read(UC_X86_REG_EIP)==0x435ba4
    actual=bytes(m.mem_read(score+8,len(raw)));expected=bytearray(raw);at=selection*0x5298+0x7f5+(stage+difficulty*8)*8
    if mode==0:expected[at]=1
    assert actual==expected,(selection,difficulty,stage,mode)
    cases.append({'selection':selection,'difficulty':difficulty,'stage':stage,'controlMode':mode,'enteredFlag':actual[at],'changedDecodedBytes':sum(x!=y for x,y in zip(raw,actual))})
 q={'schema':'th14/practice-unlock-native/1','passed':True,'caseCount':len(cases),'ordinaryWrites':150,'nonzeroControlNoWrites':300,'executableSha256':sha(img.data),'instructionAddress':'0x435b6c','instructionBytes':0x38,'instructionSha256':sha(img.read(0x435b6c,0x38)),'decodedInputSha256':sha(raw),'cases':cases,'onlyExpectedEnteredFlagChanged':True,'stageCompletedFlagNotWritten':True,'originalInstructionsModified':False,'originalProcessLaunched':False,'virtualScoreAndControlPorts':True,'browserAccepted':False,'completeGame':False}
 out.parent.mkdir(parents=True,exist_ok=True);out.write_text(json.dumps(q,indent=2)+'\n');print(json.dumps({k:v for k,v in q.items() if k!='cases'}))
if __name__=='__main__':main()
