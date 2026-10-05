import {readFileSync,writeFileSync,mkdirSync} from 'node:fs';
import {resolve} from 'node:path';
import {execFileSync} from 'node:child_process';
const root=resolve(import.meta.dirname,'../..');
const source=readFileSync(resolve(root,'src/Touch.cpp'),'utf8').replaceAll('\r\n','\n');
const block=from=>{const first=source.indexOf(from);if(first<0)throw Error('Missing '+from);const open=source.indexOf('{',first);let depth=0;for(let p=open;p<source.length;++p){if(source[p]==='{')++depth;if(source[p]==='}'&&!--depth)return source.slice(first,p+1);}throw Error('Unclosed '+from);};
const helpers=source.slice(source.indexOf('bool IsEndingTouchOwner()'),source.indexOf('i32 ReplayRecordFingerId('));
const down=source.indexOf('void Touch::FingerDown');
const downOwner=source.slice(down).match(/if \(IsEndingTouchOwner\(\)\)[\s\S]*?\n    }\n/)[0];
const holdStart=source.indexOf('if (!IsDialogueTouchOwner() && g_DialogueHoldFinger.active)');
const hold=source.slice(holdStart,source.indexOf('// keep firing for a bit after release',holdStart));
const cpp=String.raw`
#include <cassert>
#include <cstdint>
#include <cstdio>
using u64=std::uint64_t;
enum{SUPERVISOR_STATE_ENDING=9,TH_BUTTON_SHOOT=1,TH_BUTTON_SKIP=256};
struct {int curState=0;} g_Supervisor;
struct Gui{bool present=false;bool HasCurrentMsgIdx(){return present;}bool IsDialogueSkippable(){return false;}bool IsWaitingForPlayerAdvance(){return false;}}g_Gui;
struct Finger{bool active=false;int id=0;float lastPxX=0,lastPxY=0;u64 start=0;}g_DialogueHoldFinger,g_MoveFinger;
bool g_MoveGestureUncaptured=false,g_DialogueTapPending=false;float g_DialogueTapStartX=0,g_DialogueTapStartY=0;
u64 now=1;u64 SDL_GetTicks(){return now;}
void AssignFinger(Finger* finger,int id,float x,float y){*finger={true,id,x,y,now};}
void ReleaseFinger(Finger* finger){finger->active=false;}
`+helpers+String.raw`
void down(){struct{int fingerID=4;}f;float px=100,py=200;
`+downOwner+String.raw`
assert(false);}
unsigned buttons(){unsigned buttons=0;
`+block('if (g_DialogueTapPending)')+hold+String.raw`
return buttons;}
int main(){g_Supervisor.curState=9;down();assert(g_DialogueHoldFinger.active&&!g_MoveFinger.active);
g_DialogueTapPending=true;assert(buttons()==TH_BUTTON_SHOOT);assert(!g_DialogueTapPending);
now=501;assert(buttons()==TH_BUTTON_SKIP);g_DialogueHoldFinger.active=false;assert(buttons()==0);
g_Supervisor.curState=1;g_DialogueTapPending=true;assert(buttons()==0);
std::puts("Ending touch ownership / tap Z / held skip / no gameplay finger: PASS");}
`;
const out=resolve(root,'artifacts/ending-touch');mkdirSync(out,{recursive:true});
const file=resolve(out,'check.cpp'),exe=resolve(out,'check.exe');writeFileSync(file,cpp);
const compiler=process.env.TH_TEST_CXX??'C:/msys64/ucrt64/bin/g++.exe';
execFileSync(compiler,['-std=c++17','-O2',file,'-o',exe],{stdio:'inherit',windowsHide:true});
execFileSync(exe,[],{stdio:'inherit',windowsHide:true,env:{...process.env,PATH:resolve(compiler,'..')+';'+process.env.PATH}});
