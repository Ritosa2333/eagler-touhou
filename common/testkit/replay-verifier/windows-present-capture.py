"""Capture retail TH06/TH07 Demo state at the title's D3D8 Present boundary."""
import argparse,ctypes,importlib.util,json,struct,subprocess,sys,time
from ctypes import wintypes
from pathlib import Path

HERE=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('windows_process_reader',HERE/'windows-process-reader.py');module=importlib.util.module_from_spec(spec);sys.modules[spec.name]=module;spec.loader.exec_module(module)
config_spec=importlib.util.spec_from_file_location('windows_retail_config',HERE/'windows-retail-config.py');config_module=importlib.util.module_from_spec(config_spec);config_spec.loader.exec_module(config_module)
ROW=struct.Struct('<13I2fI');MAGIC=0x52565045

def foreground_process(pid):
 callback_type=ctypes.WINFUNCTYPE(wintypes.BOOL,wintypes.HWND,wintypes.LPARAM);user32=ctypes.windll.user32
 def visit(window,_):
  owner=wintypes.DWORD();user32.GetWindowThreadProcessId(window,ctypes.byref(owner))
  if owner.value==pid and user32.IsWindowVisible(window):user32.ShowWindow(window,5);user32.SetForegroundWindow(window);return False
  return True
 user32.EnumWindows(callback_type(visit),0)

def decoded(data,game):
 values=ROW.unpack(data); magic,row_game,active,frame,stage,clock,rng,backup,calls,raw,current,last_raw,last_current,x,y,player_state=values
 if magic!=MAGIC or row_game!=game:raise RuntimeError('Present observer stream identity mismatch')
 row={'frame':frame,'stage':stage,('stageFrames' if game==7 else 'gameFrames'):clock,'rng':rng,'rngBackup':backup,'rngCalls':calls,'rawInput':raw,'input':current,'lastRawInput':last_raw,'lastInput':last_current,'player':[x,y],'playerState':player_state}
 return bool(active),row

def main():
 p=argparse.ArgumentParser();p.add_argument('--game',type=int,choices=(6,7),required=True);p.add_argument('--executable',type=Path,required=True);p.add_argument('--working-directory',type=Path,required=True);p.add_argument('--target-executable-name');p.add_argument('--injector',type=Path);p.add_argument('--observer-dll',type=Path);p.add_argument('--demo-count',type=int,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--timeout',type=float,default=600);a=p.parse_args()
 if bool(a.injector)!=bool(a.observer_dll):p.error('--injector and --observer-dll must be provided together')
 cwd=a.working_directory.resolve();stream=(cwd/'replay-verifier-present.bin').resolve()
 if a.target_executable_name:raise RuntimeError('Locale-wrapper launch needs a verified child ACP/config profile before it can guarantee windowed mode')
 config_module.require_windowed(a.game,cwd)
 if stream.parent!=cwd:raise RuntimeError('Invalid Present stream path')
 stream.unlink(missing_ok=True);demos=[];active_rows=[];last_identity=None;offset=0;deadline=time.monotonic()+a.timeout
 with module.WindowsProcessReader(a.executable,cwd,target_executable_name=a.target_executable_name) as reader:
  if a.injector:
   ready='0x6c6d20' if a.game==6 else '0'
   injected=subprocess.run([str(a.injector.resolve()),str(reader.target_pid),ready,str(a.observer_dll.resolve())])
   if injected.returncode:raise RuntimeError(f'Present observer injection failed: {injected.returncode}')
   foreground_process(reader.target_pid)
  while time.monotonic()<deadline and len(demos)<a.demo_count and reader._target_is_running():
   if not stream.exists():time.sleep(.01);continue
   with stream.open('rb') as handle:handle.seek(offset);chunk=handle.read()
   complete=len(chunk)//ROW.size
   for index in range(complete):
    is_active,row=decoded(chunk[index*ROW.size:(index+1)*ROW.size],a.game);identity=(row['stage'],row['frame'])
    if is_active:
     if active_rows and row['frame']<active_rows[-1]['frame']:
      demos.append({'rotationIndex':len(demos),'rows':active_rows});active_rows=[];last_identity=None
     if identity!=last_identity:active_rows.append(row);last_identity=identity
    elif active_rows:
     demos.append({'rotationIndex':len(demos),'rows':active_rows});active_rows=[];last_identity=None
    if len(demos)>=a.demo_count:break
   offset+=complete*ROW.size
   if len(demos)<a.demo_count:time.sleep(.01)
 result={'schema':f'th0{a.game}/original-title-demo-capture/v1','complete':len(demos)==a.demo_count,'provider':f'th0{a.game}-original/d3d8-present-observer','demos':demos}
 a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_text(json.dumps(result,separators=(',',':'),allow_nan=False),encoding='utf-8')
 for demo in demos:print(json.dumps({'rotationIndex':demo['rotationIndex'],'ticks':len(demo['rows']),'last':demo['rows'][-1]}),flush=True)
 if not result['complete']:raise RuntimeError(f'Retail TH0{a.game} exited or timed out before all title Demos completed')
if __name__=='__main__':main()
