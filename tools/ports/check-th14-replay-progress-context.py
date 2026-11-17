#!/usr/bin/env python3
"""Compile the actual read-only TH14 replay status method with controlled boundaries.

This checks stage/cursor ownership and end reporting, not combat equivalence.
Requires a C++17 compiler; writes temporary artifacts outside the source tree.
"""
from pathlib import Path
import argparse, hashlib, json, subprocess, tempfile
PREFIX = '#include <cstdint>\n#include <cstdio>\nusing u32=std::uint32_t;\nenum class GameSessionPhase:u32{title=0,stage=1,paused=2,loading=6};\nu32 th14_hud_load(u32 value){return value;}\nstruct Entry{u32 frames;};\nstruct Playback{u32 selected=1,cursor=80;Entry entries[2]={{100},{200}};const Entry*stage(u32 n)const{return n>=1&&n<=2?&entries[n-1]:nullptr;}u32 frame()const{return cursor;}u32 selected_stage()const{return selected;}};\nstruct PauseMenu{bool forced_exit=false;};\nstruct Battle{bool game_over_requested=false;struct{struct{bool ending_active=true;}state;}completion;struct{struct{u32 displayed_value()const{return 0;}}score;}hud;};\nstruct Session{struct{u32 stage=1;bool replay=true;GameSessionPhase phase=GameSessionPhase::stage;}state;Playback playback;PauseMenu menu;const PauseMenu*pause_menu=&menu;Battle fixture_battle;const Battle*battle=nullptr;struct{u32 score_units=0;}economy;};\nstruct Host{bool hosted_replay=true;Session session;\n'
SUFFIX = '};\nint main(){Host h;unsigned failures=0,cases=0;auto check=[&](const char*name,u32 expected){const auto*p=h.hosted_replay_status();const bool ok=p[1]==expected;std::printf("%s %u %u %s\\n",name,p[1],expected,ok?"PASS":"FAIL");++cases;failures+=!ok;};\ncheck("same-stage-live",80);\nh.session.state.stage=2;h.session.state.phase=GameSessionPhase::loading;check("next-stage-loading-old-cursor",0);\nh.session.state.phase=GameSessionPhase::stage;check("next-stage-activated-old-cursor",0);\nh.session.playback.selected=2;h.session.playback.cursor=4;check("next-stage-selected",4);\nh.session.state.phase=GameSessionPhase::paused;h.session.menu.forced_exit=true;check("natural-end-retains-declared-tail",200);\nh.session.battle=&h.session.fixture_battle;h.session.fixture_battle.game_over_requested=true;check("early-game-over-keeps-real-cursor",4);\nreturn failures?1:0;}\n'
OLD = 'words[0]=s.state.stage;words[1]=ended&&entry&&!early_game_over?entry->frames:s.playback.frame();words[2]=entry?entry->frames:0;'
NEW = 'const u32 frame=s.playback.selected_stage()==s.state.stage?s.playback.frame():0;\n        words[0]=s.state.stage;words[1]=ended&&entry&&!early_game_over?entry->frames:frame;words[2]=entry?entry->frames:0;'

def main():
 parser=argparse.ArgumentParser(description=__doc__)
 parser.add_argument('--source',type=Path,action='append',required=True,help='Application.cpp, repeat for normal and multiplayer')
 parser.add_argument('--compiler',default='c++')
 parser.add_argument('--demonstrate-old',action='store_true',help='Also require the obsolete expression to fail exactly the two ownership cases')
 args=parser.parse_args();rows=[]
 with tempfile.TemporaryDirectory(prefix='th14-replay-status-') as temp:
  out=Path(temp)
  for index,path in enumerate(args.source):
   text=path.read_text(encoding='utf8');start=text.index('    const u32* hosted_replay_status()const{');end=text.index('\n    int hosted_replay_seek_step',start);method=text[start:end]
   if method.count(NEW)!=1:raise ValueError('Expected selected-stage cursor guard in '+str(path))
   row={'source':str(path),'methodSha256':hashlib.sha256(method.encode()).hexdigest(),'cases':6}
   for label,body in [('current',method)]+([('obsolete',method.replace(NEW,OLD))] if args.demonstrate_old else []):
    cpp=out/(str(index)+'-'+label+'.cpp');exe=cpp.with_suffix('.exe');cpp.write_text(PREFIX+body+'\n'+SUFFIX,encoding='utf8')
    subprocess.run([args.compiler,'-std=c++17','-O2',str(cpp),'-o',str(exe)],check=True,timeout=60)
    run=subprocess.run([str(exe)],capture_output=True,text=True,timeout=5);lines=run.stdout.splitlines()
    failures=[line for line in lines if line.endswith(' FAIL')]
    if len(lines)!=6 or run.returncode!=(0 if label=='current' else 1):raise AssertionError(run.stdout+run.stderr)
    if label=='current' and failures:raise AssertionError(failures)
    if label=='obsolete' and [line.split()[0] for line in failures]!=['next-stage-loading-old-cursor','next-stage-activated-old-cursor']:raise AssertionError(failures)
    row[label]={'exitCode':run.returncode,'results':lines}
   rows.append(row)
 print(json.dumps({'passed':True,'actualMethodCompiled':True,'controlledSessionBoundaries':True,'rows':rows},indent=2))
if __name__=='__main__':main()
