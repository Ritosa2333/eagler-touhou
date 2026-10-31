"""Probe actual retail CRT formatter after actual heap/TLS constructors.

No input/game/ANM tick, graphics or real OS call is executed. The fixture header
only bootstraps the independent selected-manager provider; it is not a replay.
"""
import contextlib,io,json,pathlib,runpy,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[2];folder=ROOT/'artifacts/reverse/crt-format';folder.mkdir(parents=True,exist_ok=True)
fixture=folder/'bootstrap-input.json';fixture.write_text(json.dumps({'character':2,'shot':1,'difficulty':3,'stage':1,'seed':4796,'initial':{'power':100,'rank':0,'pointValue':2000000,'x':0,'y':51200,'lives':2,'bombs':2},'inputs':[]})+'\n')
sys.argv=[str(ROOT/'scripts/reverse/native-enemy-provider.py'),'--frames','0','--input',str(fixture),'--native-player','--native-items','--native-bullets','--omit-animation-capture','--capture-text-output','--output',str(folder/'bootstrap.json')]
with contextlib.redirect_stdout(io.StringIO()):g=runpy.run_path(sys.argv[0])
v=g['v'];put=g['put'];get=g['get'];call=g['call'];OUT,FMT,ARGS,TEXT=0x20c0000,0x20c0100,0x20c0200,0x20c0300
v.mem_write(TEXT,'青符'.encode('cp932')+b'\0')
samples=[('%3d:%03d',struct.pack('<ii',3,7)),('%s / %d %%',struct.pack('<Ii',TEXT,140)),('*%.1f',struct.pack('<d',2.5)),('%3d%%',struct.pack('<i',123))]
rows=[];before=get(0x4ce568,'H')+get(0x4ce56c,'I')
for fmt,values in samples:
 v.mem_write(OUT,b'\0'*256);v.mem_write(FMT,fmt.encode('ascii')+b'\0');v.mem_write(ARGS,values)
 returned=call(0x46cad8,OUT,FMT,ARGS);raw=bytes(v.mem_read(OUT,256)).split(b'\0',1)[0]
 rows.append({'format':fmt,'text':raw.decode('cp932'),'bytes':len(raw),'returned':returned})
after=get(0x4ce568,'H')+get(0x4ce56c,'I');assert before==after
# Real front74 resource/ANM VM; original460800 formatter and4606b0 layout run.
# Only the audited44e660 GDI/D3D output wrapper is captured, as in the existing
# spell-title geometry fixture. No title bitmap or glyph dimensions are claimed.
from unicorn.x86_const import UC_X86_REG_EAX,UC_X86_REG_ECX,UC_X86_REG_EDX,UC_X86_REG_ESI,UC_X86_REG_EDI
v.reg_write(UC_X86_REG_EAX,23);v.reg_write(UC_X86_REG_ECX,0);call(0x4615a0,g['resource_by_name']['front.anm'],ARGS+128,74,0)
handle=get(ARGS+128,'I')[0];v.reg_write(UC_X86_REG_EDX,0x5000000);vm=call(0x461920,handle);assert vm
v.reg_write(UC_X86_REG_ESI,vm);v.reg_write(UC_X86_REG_EDI,0);before_output=get(0x4ce568,'H')+get(0x4ce56c,'I');call(0x460800,0xffffff,0,0,0,TEXT);after_output=get(0x4ce568,'H')+get(0x4ce56c,'I');assert before_output==after_output
text_outputs=[event for event in g['events'] if event['kind']=='textTextureOutput'];assert len(text_outputs)==1
out=folder/'samples.json';out.write_text(json.dumps({'originalExeSha256':g['SHA'],'provider':'Actual46ea5c/47562a constructors,46cad8 formatter and460800->4606b0 front74 title layout; bounded single-thread CPU imports and44e660 output capture.','wholeGameOracle':False,'gameTicksExecuted':0,'gpuExecuted':False,'gdiExecuted':False,'realOsExecuted':False,'rngBefore':before,'rngAfter':after,'rngBeforeTitleOutput':before_output,'rngAfterTitleOutput':after_output,'host':g['crt_host'].metadata(),'records':rows,'titleOutput':text_outputs},ensure_ascii=False,indent=2)+'\n',encoding='utf-8');print(out)
